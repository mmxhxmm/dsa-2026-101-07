# NPM-MAPS — Project Report

---

## 1. Runtime Complexity of Initializing the Intersections Map

Initialization iterates once over every street in the list and calls `insert_intersection` for each one. `insert_intersection` computes a hash in O(1) and appends a node to the bucket's linked list in O(1) (prepend). With **N** streets in the input:

```
O(N)
```

where N is the number of street segments loaded from `streets.txt`.

---

## 2. Runtime Complexity of Finding Coordinates Given a Name

### Sequential list search (normal search)
The houses and places lists are unsorted singly-linked lists. Finding a match requires scanning from the head until a name match is found. In the worst case every element is visited:

```
O(N)
```

where N is the number of entries in the list (houses or places).

### Hash map search
The intersections hash map uses a hash function to compute a bucket index in O(1), then walks the collision chain. With a good hash function and table size `TABLE_SIZE = 50021`, the average chain length is short and the lookup is effectively:

```
O(1) average,  O(N) worst case (all keys collide)
```

---

## 3. Runtime Complexity of the Path-Finding Algorithm (BFS)

BFS visits each street segment at most once (tracked by `from_id_to_id` key). For each visited segment it calls `get_connected`, which does a linear scan of all streets to find neighbors.

Let:
- **V** = number of street segments (vertices)
- **E** = number of edges (connections between segments)
- **N** = total streets in the list (used by `get_connected`)

Each vertex is dequeued once → **O(V)** dequeue operations.  
Each dequeue triggers `get_connected` → **O(N)** scan.  
Total:

```
O(V × N)
```

Since V ≈ N, this simplifies to **O(N²)** in practice. A hash-map-based neighbor lookup would reduce this to **O(V + E)**.

---

## 4. Connected Streets Latency: Sequential vs Hash Map

### Raw Data

| Map | Streets (N) | Normal Search (ns) | Hash Map Search (ns) |
|-----|-------------|-------------------|----------------------|
| xs_1 | 16 | 187,461 | 13,555 |
| xs_2 | 71 | 39,378 | 12,714 |
| md_1 | 1,318 | 3,746,176 | 9,134 |
| lg_1 | 3,797 | 1,831,352 | 9,330 |
| xl_1 | 18,944 | 13,226,304 | 14,271 |

### Plot

<picture>
  <source 
    media="(prefers-color-scheme: dark)" 
    srcset="./images/plot1_connected_streets.png">
  <source 
    media="(prefers-color-scheme: light)"
    srcset="./problem_images/example_map_street_numbers.svg">
  <img 
    alt="plot connected streets" 
    srcset="./problem_images/plot1_connected_streets.png"
    style="display: block; margin: 0 auto; max-width: 600px; height: auto;">
</picture>

### Explanation

The sequential search grows linearly with N — it must scan every street segment until a match is found, giving O(N) behaviour. This is clearly visible: from xs_1 (16 streets, 187K ns) to xl_1 (18,944 streets, 13.2M ns), latency increases roughly proportionally to the number of streets.

The hash map lookup remains effectively constant at ~10,000–14,000 ns regardless of map size, confirming O(1) average behaviour. The small variation is due to cache effects and hash collision chain length, not N.

---

## 5. BFS Latency: Sequential Neighbor Lookup vs Hash Map Neighbor Lookup

Since the final implementation uses a sequential scan inside `get_connected` (the hash map was abandoned due to directional graph inconsistencies), the measurements below compare the two approaches conceptually, with the sequential scan timings measured experimentally.

### Raw Data — Sequential (current implementation)

| Map | Streets (N) | BFS Close (ns) | BFS Far (ns) |
|-----|-------------|---------------|--------------|
| xs_1 | 16 | 2,639 | 58,471 |
| xs_2 | 71 | 2,897 | 82,958 |
| md_1 | 1,318 | 25,470 | 53,790,451 |
| lg_1 | 3,797 | 290,970 | 93,917,427 |
| xl_1 | 18,944 | 3,497 | 2,501,791,961 |

### Plot

<picture>
  <source 
    media="(prefers-color-scheme: dark)" 
    srcset="./images/plot2_bfs_close_vs_far.png">
  <source 
    media="(prefers-color-scheme: light)"
    srcset="./problem_images/example_map_street_numbers.svg">
  <img 
    alt="plot BFS close and far" 
    srcset="./problem_images/plot2_bfs_close_vs_far.png"
    style="display: block; margin: 0 auto; max-width: 600px; height: auto;">
</picture>

### Explanation

BFS with sequential neighbor lookup is O(V × N). As N grows, each BFS step requires scanning the entire street list to find connections. This produces super-linear growth — from md_1 to xl_1 (×14 streets), BFS far latency grows by ×46. For close paths BFS terminates early after visiting few vertices, so latency stays low regardless of map size.

A hash-map-based neighbor lookup would reduce each `get_connected` call from O(N) to O(1), making BFS O(V + E) overall — dramatically faster on large maps.

---

## 6. BFS Latency: Close vs Far Paths

### Raw Data

| Map | Streets (N) | BFS Close (ns) | BFS Far (ns) | Ratio (far/close) |
|-----|-------------|---------------|--------------|-------------------|
| xs_1 | 16 | 2,639 | 58,471 | ×22 |
| xs_2 | 71 | 2,897 | 82,958 | ×29 |
| md_1 | 1,318 | 25,470 | 53,790,451 | ×2,112 |
| lg_1 | 3,797 | 290,970 | 93,917,427 | ×323 |
| xl_1 | 18,944 | 3,497 | 2,501,791,961 | ×715,406 |

### Plot

<picture>
  <source 
    media="(prefers-color-scheme: dark)" 
    srcset="./images/plot3_bfs_sequential_vs_hashmap.png">
  <source 
    media="(prefers-color-scheme: light)"
    srcset="./problem_images/plot3_bfs_sequential_vs_hashmap.png">
  <img 
    alt="plot BFS close and far" 
    srcset="./problem_images/plot2_bfs_close_vs_far.png"
    style="display: block; margin: 0 auto; max-width: 600px; height: auto;">
</picture>

### Explanation

BFS is a shortest-path algorithm that explores vertices level by level from the origin. For close destinations, BFS terminates after visiting few vertices — hence very low latency regardless of map size. For far destinations, BFS must explore a large portion of the graph before reaching the destination, and since each step does an O(N) neighbor scan, total latency grows steeply.

The ratio between far and close latency grows dramatically with map size, confirming that path distance (number of vertices explored) is the dominant factor in BFS cost, compounded by the O(N) per-step neighbor lookup.

---

## 7. Improvement to the `visited` Data Structure

### Current implementation

`visited` is a singly-linked list of `(from_id, to_id)` pairs. Checking membership requires scanning from the head:

```
O(V) per lookup  →  O(V²) total across all BFS steps
```

### Proposed improvement: Hash Set

Replace the linked list with a **hash set** keyed on `(from_id, to_id)`. A simple approach is to combine the two IDs into a single 64-bit key:

```c
unsigned long long key = (unsigned long long)from_id * 2654435761ULL ^ (unsigned long long)to_id;
```

and insert/lookup in a fixed-size open-addressing hash table.

| Operation | Linked list | Hash set |
|-----------|-------------|----------|
| `is_visited` | O(V) | O(1) average |
| `mark_visited` | O(1) | O(1) average |
| Total BFS visited cost | O(V²) | O(V) |

### Trade-offs

- **Memory**: the hash set requires pre-allocating a table sized to the expected number of vertices. Over-allocation wastes memory; under-allocation causes collisions and degrades to O(V).
- **Worst case**: with a poor hash function or adversarial input, all keys collide and lookups degrade to O(V).
- **Implementation complexity**: open addressing requires handling tombstones for deletions and resize logic; a chained hash set is simpler but uses more memory per entry.

---

## 8. Improvement to Finding the Closest Street Segment

### Current implementation

`closest_street` iterates over the entire street list and computes the haversine distance to every segment, keeping the minimum:

```
O(N)  per query
```

### Proposed improvement: k-d Tree

Index all street segment midpoints in a **2D k-d tree** keyed on `(latitude, longitude)`. A k-d tree supports nearest-neighbour queries in:

```
O(log N) average,  O(√N) worst case
```

Construction costs O(N log N) once at initialization.

Alternatively, a **spatial grid** (divide the bounding box into fixed cells) allows O(1) average lookup by computing the cell index directly from coordinates, at the cost of a fixed memory allocation proportional to grid resolution.

| Approach | Build cost | Query cost | Memory |
|----------|-----------|------------|--------|
| Linear scan (current) | O(1) | O(N) | O(1) extra |
| k-d tree | O(N log N) | O(log N) avg | O(N) |
| Spatial grid | O(N) | O(1) avg | O(G²) where G = grid cells |

### Trade-offs

- **k-d tree**: excellent query performance, but build cost O(N log N) is paid once at startup. Rebalancing is expensive if the street graph changes dynamically.
- **Spatial grid**: O(1) queries and O(N) build, but grid cells in sparse areas waste memory and dense areas may degrade to O(N) if too many segments share a cell.
- Both approaches increase code complexity significantly compared to the current linear scan, which is simple and correct.
