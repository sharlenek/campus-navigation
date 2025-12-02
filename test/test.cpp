// Unit tests for Graph
#include <catch2/catch_test_macros.hpp>
#include "../src/Graph.h"

TEST_CASE("Graph basic connectivity and shortest path", "[graph]") {
    Graph g;
    g.addLocation(1, "A");
    g.addLocation(2, "B");
    g.addLocation(3, "C");

    g.addEdge(1,2,5);
    g.addEdge(2,3,7);
    g.addEdge(1,3,20);

    REQUIRE(g.edgeStatus(1,2) == "open");
    REQUIRE(g.isConnected(1,3) == true);
    REQUIRE(g.shortestPath(1,3) == 12); // 1->2->3

    g.toggleEdge(1,2);
    REQUIRE(g.edgeStatus(1,2) == "closed");
    REQUIRE(g.isConnected(1,2) == false);
    REQUIRE(g.shortestPath(1,3) == 20); // now direct edge

    g.toggleEdge(1,2);
    REQUIRE(g.edgeStatus(1,2) == "open");
}

TEST_CASE("Graph MST cost on triangle", "[graph]") {
    Graph g;
    g.addLocation(1, "A");
    g.addLocation(2, "B");
    g.addLocation(3, "C");

    g.addEdge(1,2,4);
    g.addEdge(2,3,3);
    g.addEdge(1,3,10);

    std::unordered_set<int> verts = {1,2,3};
    int cost = g.mstCost(verts);
    // MST should pick edges weight 3 and 4 = 7
    REQUIRE(cost == 7);
}

TEST_CASE("Shortest path unreachable returns -1", "[graph]") {
    Graph g;
    g.addLocation(1, "A");
    g.addLocation(2, "B");
    g.addLocation(4, "D");
    g.addEdge(1,2,5);
    REQUIRE(g.shortestPath(1,4) == -1);
}
