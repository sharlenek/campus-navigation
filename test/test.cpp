// Unit tests for Graph
#include <catch2/catch_test_macros.hpp>
#include "../src/Graph.h"
#include "../src/StudentManager.h"
#include "../src/CampusCompass.h"
#include <sstream>

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

TEST_CASE("Graph hasLocation works correctly", "[graph]") {
    Graph g;
    g.addLocation(1, "A");
    g.addLocation(2, "B");
    
    REQUIRE(g.hasLocation(1) == true);
    REQUIRE(g.hasLocation(2) == true);
    REQUIRE(g.hasLocation(99) == false);
}

TEST_CASE("Graph shortestPathWithRoute returns correct path", "[graph]") {
    Graph g;
    g.addLocation(1, "A");
    g.addLocation(2, "B");
    g.addLocation(3, "C");
    g.addEdge(1,2,5);
    g.addEdge(2,3,3);
    
    std::vector<int> route;
    int dist = g.shortestPathWithRoute(1, 3, route);
    
    REQUIRE(dist == 8);
    REQUIRE(route.size() == 3);
    REQUIRE(route[0] == 1);
    REQUIRE(route[1] == 2);
    REQUIRE(route[2] == 3);
}

TEST_CASE("Graph edgeStatus returns DNE for non-existent edge", "[graph]") {
    Graph g;
    g.addLocation(1, "A");
    g.addLocation(2, "B");
    
    REQUIRE(g.edgeStatus(1, 2) == "DNE");
    REQUIRE(g.edgeStatus(1, 99) == "DNE");
}

TEST_CASE("StudentManager validates UFID correctly", "[student]") {
    StudentManager sm;
    sm.addClassInfo("COP3530", 14, 640, 690);
    
    // Valid 8-digit ID
    REQUIRE(sm.insertStudent("John Doe", "12345678", 1, {"COP3530"}) == true);
    
    // Invalid: too short
    REQUIRE(sm.insertStudent("Jane Doe", "1234567", 1, {"COP3530"}) == false);
    
    // Invalid: too long
    REQUIRE(sm.insertStudent("Jack Doe", "123456789", 1, {"COP3530"}) == false);
    
    // Invalid: contains letters
    REQUIRE(sm.insertStudent("Jill Doe", "1234567A", 1, {"COP3530"}) == false);
}

TEST_CASE("StudentManager validates name correctly", "[student]") {
    StudentManager sm;
    sm.addClassInfo("COP3530", 14, 640, 690);
    
    // Valid name with space
    REQUIRE(sm.insertStudent("John Doe", "12345678", 1, {"COP3530"}) == true);
    
    // Invalid: contains numbers
    REQUIRE(sm.insertStudent("John123", "87654321", 1, {"COP3530"}) == false);
    
    // Invalid: contains special characters
    REQUIRE(sm.insertStudent("John-Doe", "11111111", 1, {"COP3530"}) == false);
    
    // Invalid: empty name
    REQUIRE(sm.insertStudent("", "22222222", 1, {"COP3530"}) == false);
}

TEST_CASE("StudentManager rejects duplicate UFID", "[student]") {
    StudentManager sm;
    sm.addClassInfo("COP3530", 14, 640, 690);
    sm.addClassInfo("COP3502", 23, 575, 625);
    
    REQUIRE(sm.insertStudent("John Doe", "12345678", 1, {"COP3530"}) == true);
    REQUIRE(sm.insertStudent("Jane Doe", "12345678", 1, {"COP3502"}) == false);
}

TEST_CASE("StudentManager dropClass removes student when no classes remain", "[student]") {
    StudentManager sm;
    sm.addClassInfo("COP3530", 14, 640, 690);
    
    sm.insertStudent("John Doe", "12345678", 1, {"COP3530"});
    
    // Drop the only class
    REQUIRE(sm.dropClass("12345678", "COP3530") == true);
    
    // Student should be gone
    REQUIRE(sm.getStudent("12345678") == nullptr);
}

TEST_CASE("StudentManager dropClass keeps student with remaining classes", "[student]") {
    StudentManager sm;
    sm.addClassInfo("COP3530", 14, 640, 690);
    sm.addClassInfo("COP3502", 23, 575, 625);
    
    sm.insertStudent("John Doe", "12345678", 1, {"COP3530", "COP3502"});
    
    // Drop one class
    REQUIRE(sm.dropClass("12345678", "COP3530") == true);
    
    // Student should still exist
    const Student* s = sm.getStudent("12345678");
    REQUIRE(s != nullptr);
    REQUIRE(s->classes.size() == 1);
    REQUIRE(s->classes.count("COP3502") == 1);
}

TEST_CASE("StudentManager replaceClass validates conditions", "[student]") {
    StudentManager sm;
    sm.addClassInfo("COP3530", 14, 640, 690);
    sm.addClassInfo("COP3502", 23, 575, 625);
    sm.addClassInfo("CDA3101", 14, 705, 755);
    
    sm.insertStudent("John Doe", "12345678", 1, {"COP3530", "COP3502"});
    
    // Valid replacement
    REQUIRE(sm.replaceClass("12345678", "COP3530", "CDA3101") == true);
    
    // Can't replace with class already enrolled in
    REQUIRE(sm.replaceClass("12345678", "CDA3101", "COP3502") == false);
    
    // Can't replace class student doesn't have
    REQUIRE(sm.replaceClass("12345678", "COP3530", "COP3502") == false);
    
    // Can't replace with non-existent class
    REQUIRE(sm.replaceClass("12345678", "COP3502", "FAKE999") == false);
}

TEST_CASE("StudentManager removeClassFromAll counts correctly", "[student]") {
    StudentManager sm;
    sm.addClassInfo("COP3530", 14, 640, 690);
    sm.addClassInfo("COP3502", 23, 575, 625);
    
    sm.insertStudent("Student A", "11111111", 1, {"COP3530", "COP3502"});
    sm.insertStudent("Student B", "22222222", 1, {"COP3530"});
    sm.insertStudent("Student C", "33333333", 1, {"COP3502"});
    
    // Remove COP3530 should affect 2 students
    REQUIRE(sm.removeClassFromAll("COP3530") == 2);
    
    // Student B should be removed (only had COP3530)
    REQUIRE(sm.getStudent("22222222") == nullptr);
    
    // Student A should still exist with COP3502
    const Student* sA = sm.getStudent("11111111");
    REQUIRE(sA != nullptr);
    REQUIRE(sA->classes.size() == 1);
    
    // Student C unaffected
    REQUIRE(sm.getStudent("33333333") != nullptr);
}

TEST_CASE("StudentManager getSortedClasses returns alphabetical order", "[student]") {
    StudentManager sm;
    sm.addClassInfo("COP3530", 14, 640, 690);
    sm.addClassInfo("CDA3101", 14, 705, 755);
    sm.addClassInfo("MAC2311", 18, 575, 625);
    
    sm.insertStudent("John Doe", "12345678", 1, {"MAC2311", "CDA3101", "COP3530"});
    
    auto classes = sm.getSortedClasses("12345678");
    REQUIRE(classes.size() == 3);
    REQUIRE(classes[0] == "CDA3101");
    REQUIRE(classes[1] == "COP3530");
    REQUIRE(classes[2] == "MAC2311");
}

TEST_CASE("CampusCompass ParseCSV loads data correctly", "[integration]") {
    CampusCompass compass;
    
    // Assuming the data files exist
    bool result = compass.ParseCSV("../data/edges.csv", "../data/classes.csv");
    REQUIRE(result == true);
}

TEST_CASE("Graph MST with closed edges", "[graph]") {
    Graph g;
    g.addLocation(1, "A");
    g.addLocation(2, "B");
    g.addLocation(3, "C");
    g.addLocation(4, "D");
    
    g.addEdge(1, 2, 1);
    g.addEdge(2, 3, 2);
    g.addEdge(3, 4, 3);
    g.addEdge(1, 4, 10);
    
    // Close an edge
    g.toggleEdge(2, 3);
    
    std::unordered_set<int> verts = {1, 2, 3, 4};
    int cost = g.mstCost(verts);
    
    // MST should use edges 1-2 (1), 1-4 (10), 3-4 (3) = 14
    // (can't use 2-3 because it's closed)
    REQUIRE(cost == 14);
}

TEST_CASE("Graph shortest path respects closed edges", "[graph]") {
    Graph g;
    g.addLocation(1, "A");
    g.addLocation(2, "B");
    g.addLocation(3, "C");
    
    g.addEdge(1, 2, 5);
    g.addEdge(2, 3, 5);
    g.addEdge(1, 3, 20);
    
    // Normal path is 1->2->3 = 10
    REQUIRE(g.shortestPath(1, 3) == 10);
    
    // Close edge 2-3
    g.toggleEdge(2, 3);
    
    // Now must use direct edge 1->3 = 20
    REQUIRE(g.shortestPath(1, 3) == 20);
}
