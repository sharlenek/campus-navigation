#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <unordered_set>

struct Edge {
    int to;
    int weight;
    bool open;
};

class Graph {
private:
    std::unordered_map<int, std::vector<Edge>> adj;
    std::unordered_map<int, std::string> names;

public:
    Graph() = default;

    void addLocation(int id, const std::string& name);
    void addEdge(int a, int b, int weight);

    void toggleEdge(int a, int b);
    std::string edgeStatus(int a, int b) const;

    bool isConnected(int a, int b) const;
    int shortestPath(int src, int dst) const;

    int mstCost(const std::unordered_set<int>& vertices) const;

    const std::string& getLocationName(int id) const { return names.at(id); }
    bool hasLocation(int id) const { return names.find(id) != names.end(); }
    int shortestPathWithRoute(int src, int dst, std::vector<int>& route) const;
};
