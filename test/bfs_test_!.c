#include "../hdr/bfs.h"
#include "../hdr/streets.h"
#include "../hdr/common.h"
#include "../hdr/init.h"
#include "utils.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── Helpers ────────────────────────────────────────────────────────── */

static t_street make_street(long long from_id, long long to_id,
                             const char *name) {
  t_street s;
  memset(&s, 0, sizeof(t_street));
  s.from_id  = from_id;
  s.to_id    = to_id;
  s.from_lat = 0.0;
  s.from_lon = 0.0;
  s.to_lat   = 0.0;
  s.to_lon   = 0.0;
  strncpy(s.st_name, name, sizeof(s.st_name) - 1);
  return s;
}

static t_streets *build_list(t_street *arr, int n) {
  t_streets *list = NULL;
  for (int i = 0; i < n; i++)
    add_street_to_list(&list, arr[i]);
  return list;
}

/* ── Tests ──────────────────────────────────────────────────────────── */

/*
 * Graph:  A──►B──►C
 * Expect: path found, first segment is Street AB
 */
void test_bfs_direct_path() {
  runningtest("test_bfs_direct_path");
  {
    t_street arr[] = {
      make_street(1, 2, "Street AB"),
      make_street(2, 3, "Street BC"),
    };
    t_streets *all  = build_list(arr, 2);
    t_streets *path = bfs(NULL, &arr[0], &arr[1], all);

    if (path == NULL) {
      fprintf(stderr, "\033[0;31m    Expected a path but got NULL\033[0m\n\n");
      free_streets(all);
      assert(0);
    }
    assertEquals(path->street.st_name, "Street AB");
    free_streets(path);
    free_streets(all);
  }
  successtest();
}

/*
 * Graph:  A──►B──►C──►D
 * Expect: path found and last segment touches node 4
 */
void test_bfs_multi_hop() {
  runningtest("test_bfs_multi_hop");
  {
    t_street arr[] = {
      make_street(1, 2, "Street AB"),
      make_street(2, 3, "Street BC"),
      make_street(3, 4, "Street CD"),
    };
    t_streets *all  = build_list(arr, 3);
    t_streets *path = bfs(NULL, &arr[0], &arr[2], all);

    if (path == NULL) {
      fprintf(stderr, "\033[0;31m    Expected a path but got NULL\033[0m\n\n");
      free_streets(all);
      assert(0);
    }
    t_streets *cur = path;
    while (cur->next) cur = cur->next;
    int reached = (cur->street.to_id == 4 || cur->street.from_id == 4);
    if (!reached) {
      fprintf(stderr, "\033[0;31m    Last segment does not reach node 4\033[0m\n\n");
      free_streets(path);
      free_streets(all);
      assert(0);
    }
    free_streets(path);
    free_streets(all);
  }
  successtest();
}

/*
 * Graph:  A──►B    C──►D  (disconnected)
 * Expect: NULL
 */
void test_bfs_no_path() {
  runningtest("test_bfs_no_path");
  {
    t_street arr[] = {
      make_street(1, 2, "Street AB"),
      make_street(3, 4, "Street CD"),
    };
    t_streets *all  = build_list(arr, 2);
    t_streets *path = bfs(NULL, &arr[0], &arr[1], all);

    if (path != NULL) {
      fprintf(stderr, "\033[0;31m    Expected NULL but got a path\033[0m\n\n");
      free_streets(path);
      free_streets(all);
      assert(0);
    }
    free_streets(all);
  }
  successtest();
}

/*
 * from == to (same segment)
 * Expect: path returned immediately
 */
void test_bfs_same_origin_destination() {
  runningtest("test_bfs_same_origin_destination");
  {
    t_street arr[] = { make_street(1, 2, "Street AB") };
    t_streets *all  = build_list(arr, 1);
    t_streets *path = bfs(NULL, &arr[0], &arr[0], all);

    if (path == NULL) {
      fprintf(stderr, "\033[0;31m    Expected a path but got NULL\033[0m\n\n");
      free_streets(all);
      assert(0);
    }
    free_streets(path);
    free_streets(all);
  }
  successtest();
}

/*
 * NULL arguments must return NULL without crashing
 */
void test_bfs_null_arguments() {
  runningtest("test_bfs_null_arguments");
  {
    t_street   s   = make_street(1, 2, "Street AB");
    t_streets *all = NULL;
    add_street_to_list(&all, s);

    assertNull(bfs(NULL, NULL, &s, all));
    assertNull(bfs(NULL, &s, NULL, all));
    assertNull(bfs(NULL, &s, &s, NULL));

    free_streets(all);
  }
  successtest();
}

/*
 * Graph with a fork:
 *      ┌──► B ──► D
 *  A ──┤
 *      └──► C ──► D
 * Expect: a valid path found
 */
void test_bfs_fork_path() {
  runningtest("test_bfs_fork_path");
  {
    t_street arr[] = {
      make_street(1, 2, "Street AB"),
      make_street(1, 3, "Street AC"),
      make_street(2, 4, "Street BD"),
      make_street(3, 4, "Street CD"),
    };
    t_streets *all  = build_list(arr, 4);
    t_street   dest = make_street(2, 4, "Street BD");
    t_streets *path = bfs(NULL, &arr[0], &dest, all);

    if (path == NULL) {
      fprintf(stderr, "\033[0;31m    Expected a path but got NULL\033[0m\n\n");
      free_streets(all);
      assert(0);
    }
    free_streets(path);
    free_streets(all);
  }
  successtest();
}

/*
 * Graph with a cycle: A──►B──►C──►A
 * Expect: terminates and finds B (no infinite loop)
 */
void test_bfs_cycle_no_infinite_loop() {
  runningtest("test_bfs_cycle_no_infinite_loop");
  {
    t_street arr[] = {
      make_street(1, 2, "Street AB"),
      make_street(2, 3, "Street BC"),
      make_street(3, 1, "Street CA"),
    };
    t_streets *all  = build_list(arr, 3);
    t_streets *path = bfs(NULL, &arr[0], &arr[1], all);

    if (path == NULL) {
      fprintf(stderr, "\033[0;31m    Expected a path but got NULL\033[0m\n\n");
      free_streets(all);
      assert(0);
    }
    free_streets(path);
    free_streets(all);
  }
  successtest();
}

/*
 * Graph with a cycle: A-->B<--C-->D
 * Expect: terminates and finds B (no infinite loop)
 */
void test_bfs_diff_directions() {
  runningtest("test_bfs_diff_directions");
  {
    t_street arr[] = {
      make_street(1, 2, "Street AB"),
      make_street(3, 2, "Street CB"),
      make_street(3, 4, "Street CD"),
    };
    t_streets *all  = build_list(arr, 3);
    t_street   dest = make_street(3, 4, "Street CD");
    t_streets *path = bfs(NULL, &arr[0], &dest, all);

    if (path != NULL) {
      fprintf(stderr, S_RED"\tExpected NULL but got a path\n\n"RESET);
      free_streets(path);
      free_streets(all);
      assert(0);
    }

    free_streets(all);
  }
  successtest();
}

/* ── Module entry point ─────────────────────────────────────────────── */

void bfs_test()
{
  running("bfs_test");
  {
    test_bfs_direct_path();
    test_bfs_multi_hop();
    test_bfs_no_path();
    test_bfs_same_origin_destination();
    test_bfs_null_arguments();
    test_bfs_fork_path();
    test_bfs_cycle_no_infinite_loop();
    test_bfs_diff_directions();
  }
  success();
}
