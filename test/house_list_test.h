#include <stddef.h>
#include <stdlib.h>
#include "../src/house_list.h"
#include "utils.h"

/* ------------------------------------------------------------------ */
/*  Helper: build a small in-memory list                               */
/* ------------------------------------------------------------------ */

static HouseList *make_sample(void) {
    HouseList *l = house_list_create();
    house_list_add(l, "Carrer de Roc Boronat", 138, 41.403981, 2.193255);
    house_list_add(l, "Carrer de Roc Boronat", 140, 41.403990, 2.193300);
    house_list_add(l, "Carrer de Mallorca",     50, 41.390000, 2.160000);
    house_list_add(l, "Avinguda Diagonal",        1, 41.395000, 2.170000);
    return l;
}

/* ------------------------------------------------------------------ */
/*  Tests                                                               */
/* ------------------------------------------------------------------ */

static void test_create(void) {
    runningtest("test_create");
    {
        HouseList *l = house_list_create();
        assertEqualsInt(l != NULL, 1);
        assertEqualsInt(l->size, 0);
        assertEqualsInt(l->head == NULL, 1);
        house_list_free(l);
    }
    successtest();
}

static void test_add(void) {
    runningtest("test_add");
    {
        HouseList *l = house_list_create();
        int r = house_list_add(l, "Carrer de Roc Boronat", 138, 41.4, 2.19);
        assertEqualsInt(r, 0);
        assertEqualsInt(l->size, 1);
        house_list_add(l, "Carrer de Mallorca", 50, 41.39, 2.16);
        assertEqualsInt(l->size, 2);
        house_list_free(l);
    }
    successtest();
}

static void test_find_exact(void) {
    runningtest("test_find_exact");
    {
        HouseList *l = make_sample();

        House *h = house_list_find(l, "Carrer de Roc Boronat", 138);
        assertEqualsInt(h != NULL, 1);
        assertEqualsInt(h->number, 138);

        House *missing_num = house_list_find(l, "Carrer de Roc Boronat", 999);
        assertEqualsInt(missing_num == NULL, 1);

        House *missing_street = house_list_find(l, "Carrer Inexistent", 1);
        assertEqualsInt(missing_street == NULL, 1);

        house_list_free(l);
    }
    successtest();
}

static void test_find_case_insensitive(void) {
    runningtest("test_find_case_insensitive");
    {
        HouseList *l = make_sample();

        House *lower = house_list_find(l, "carrer de roc boronat", 138);
        assertEqualsInt(lower != NULL, 1);

        House *upper = house_list_find(l, "CARRER DE ROC BORONAT", 138);
        assertEqualsInt(upper != NULL, 1);

        house_list_free(l);
    }
    successtest();
}

static void test_find_abbreviation(void) {
    runningtest("test_find_abbreviation");
    {
        HouseList *l = make_sample();

        House *abbrev = house_list_find(l, "C. de Roc Boronat", 138);
        assertEqualsInt(abbrev != NULL, 1);

        House *av = house_list_find(l, "Av. Diagonal", 1);
        assertEqualsInt(av != NULL, 1);

        house_list_free(l);
    }
    successtest();
}

static void test_numbers_on_street(void) {
    runningtest("test_numbers_on_street");
    {
        HouseList *l = make_sample();

        int  cnt  = 0;
        int *nums = house_list_numbers_on_street(l, "Carrer de Roc Boronat", &cnt);
        assertEqualsInt(cnt, 2);
        assertEqualsInt(nums[0], 138);
        assertEqualsInt(nums[1], 140);
        free(nums);

        int  cnt2  = 0;
        int *nums2 = house_list_numbers_on_street(l, "Carrer Inexistent", &cnt2);
        assertEqualsInt(cnt2, 0);
        assertEqualsInt(nums2 == NULL, 1);

        house_list_free(l);
    }
    successtest();
}

static void test_similar_streets(void) {
    runningtest("test_similar_streets");
    {
        HouseList *l = make_sample();

        int    cnt = 0;
        char **sim = house_list_similar_streets(
            l, "Carrer de Roc Voronat", 3, &cnt);
        assertEqualsInt(cnt > 0, 1);
        assertEquals(sim[0], "Carrer de Roc Boronat");
        free(sim);

        house_list_free(l);
    }
    successtest();
}

/* ------------------------------------------------------------------ */
/*  Module entry point called from test/test.c                         */
/* ------------------------------------------------------------------ */

void house_list_test(void) {
    running("house_list_test");
    {
        test_create();
        test_add();
        test_find_exact();
        test_find_case_insensitive();
        test_find_abbreviation();
        test_numbers_on_street();
        test_similar_streets();
    }
    success();
}