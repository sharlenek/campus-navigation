# Project Documentation — Time Complexity and Reflections

This document summarizes the worst-case time complexity (Big O) for the primary commands implemented by `CampusCompass` and the helper methods they call. Use the following notation:
- V: number of vertices (locations) in the graph
- E: number of edges in the graph
- S: number of students in `StudentManager`
- s: number of classes for a particular student
- k: number of classes being iterated/processed (often equal to `s`)
- n: number of toggle operations provided to a command

**ParseCSV**: Parses `edges.csv` and `classes.csv`, calling `Graph::addLocation`, `Graph::addEdge`, and `StudentManager::addClassInfo`.
- Time: O(E + C)
  - E = number of lines/edges in `edges.csv` (each edge leads to constant-time work to insert into unordered_map/vector)
  - C = number of class entries in `classes.csv` (each `addClassInfo` is an unordered_map assignment)

Command time complexities (worst-case):

- **insert**: Insert a student with `n` classes.
  - Calls: `Graph::hasLocation` (O(1) average), `StudentManager::insertStudent` which validates name/id and checks each class via `classExists` and inserts `n` class codes into an unordered_set.
  - Time: O(n) (dominant cost is validating/inserting the student's class list)

- **remove**: Remove a student by id.
  - Calls: `StudentManager::removeStudent` (unordered_map erase).
  - Time: O(1) average

- **dropClass**: Drop a single class for a student.
  - Calls: `StudentManager::dropClass` (find student O(1), erase from unordered_set O(1)).
  - Time: O(1) average

- **replaceClass**: Replace one class with another for a student.
  - Calls: lookups in unordered_maps/sets and two set operations (erase/insert).
  - Time: O(1) average

- **removeClass**: Remove a class from all students.
  - Calls: `StudentManager::removeClassFromAll` iterates every student and erases the class from their set if present.
  - Time: O(S) in the number of students (each student's class-set erase is average O(1))

- **toggleEdgesClosure**: Toggle a list of edges open/closed (n pairs provided).
  - Calls: `Graph::toggleEdge` for each pair. `toggleEdge` scans the adjacency vector for node `a` and node `b` to find the corresponding `Edge` entry and flips `open`.
  - Time: O(n * d_max) worst-case, where `d_max` is the maximum degree of the involved vertices (each toggle scans adjacency lists of the two incident vertices). If you prefer global notation: O(n * (E/V)) as an average-degree estimate.

- **checkEdgeStatus**: Report whether a specific edge exists and whether it is open.
  - Calls: `Graph::edgeStatus` which scans the adjacency list for `a` to find `b`.
  - Time: O(d_a) worst-case (degree of vertex `a`); average-case O(E/V)

- **isConnected**: Check whether two vertices are connected via open edges.
  - Calls: `Graph::isConnected` which performs a BFS/queue over vertices using open edges.
  - Time: O(V + E) worst-case (visits vertices and edges reachable via open edges)

- **printShortestEdges**: For a student id, print total-time (shortest path) to each class.
  - Calls: `StudentManager::getSortedClasses` (copies student's class set and sorts them: O(s log s)), then for each of the student's `k` classes calls `Graph::shortestPath` (Dijkstra).
  - Dijkstra Time: O(E log V) (using a binary heap/priority queue and scanning adjacency lists)
  - Total Time: O(s log s + k * (E log V)) — typically k = s, so O(s log s + s * E log V)

- **printStudentZone**: Compute student's zone of vertices from shortest paths and then compute an MST cost over those vertices.
  - Calls: for each of the student's `k` classes, `Graph::shortestPathWithRoute` (Dijkstra with parent tracking): O(E log V) each. All returned routes' vertices are inserted into an unordered_set (amortized O(1) per insertion). Then `Graph::mstCost` runs Prim's-like algorithm restricted to the selected vertices.
  - MST Time: In the worst case the chosen vertex set is the full vertex set and the implementation behaves like Prim's algorithm: O(E log V).
  - Total Time: O(k * (E log V) + E log V) = O((k + 1) * E log V) in the worst case (plus the cost of assembling the vertex set). If k is small relative to the graph, the dominating cost is MST or the repeated Dijkstra runs depending on sizes.

- **verifySchedule**: Check pairwise adjacent classes in a student's schedule for feasibility.
  - Calls: build `classSchedule` (O(s)), sort it (O(s log s)), then for each adjacent pair (up to s-1 pairs) call `Graph::shortestPath` (Dijkstra) to compute travel time.
  - Total Time: O(s log s + s * (E log V))

Notes on Graph algorithms and constants:
- `Graph::shortestPath` and `Graph::shortestPathWithRoute` use Dijkstra with a priority queue. The implementation initializes a `dist` entry for every vertex in `adj` (O(V)), and then processes edges with a standard PQ-based loop: worst-case O(E log V).
- `Graph::isConnected` is BFS-based: O(V + E).
- `Graph::mstCost` implements a Prim-like algorithm using a PQ and considering only the subset of `vertices` passed; in the worst case the subset equals the whole graph so worst-case O(E log V).

Extra credit:
- No extra-credit methods were added beyond what exists in the repository. If extra-credit functionality had been implemented, describe it here (algorithm, complexity, and trade-offs).

What I learned and what I'd do differently
- Learned: Working across graph algorithms (BFS, Dijkstra, Prim) together with hashed containers (`unordered_map`, `unordered_set`) helped solidify how algorithmic choices interact with average-case cost assumptions. Implementing route reconstruction for Dijkstra (`shortestPathWithRoute`) reinforced parent-tracking and the idea that returning paths can add linear (in path length) overhead to a shortest-path call.
- Do differently: I would centralize repeated Dijkstra calls when possible (e.g., multi-target Dijkstra or building a single-source shortest-path tree once and reusing distances/routes) to avoid repeated full-graph searches for multiple class destinations. That would reduce costs in `printShortestEdges`, `printStudentZone`, and `verifySchedule` when many queries originate from the same source.

Project difficulty, intuition, and overall thoughts (2–4 sentences)
- The project was moderately challenging: integrating data parsing, student bookkeeping, and several graph algorithms required careful API boundaries between `CampusCompass`, `StudentManager`, and `Graph`.
- Intuitively, the hardest part is balancing correctness and efficiency when multiple features require shortest-path computations; reuse (caching or single-source expansions) would simplify and accelerate many operations.

If you want, I can:
- Add this `DOCUMENTATION.md` to the repository (already done),
- Add micro-benchmarks or tests that measure actual runtime for large graphs,
- Or refactor `printStudentZone`/`printShortestEdges` to reuse Dijkstra results (multi-target or single-source reuse).

---
File created by the project assistant to accompany the submission and grading.
