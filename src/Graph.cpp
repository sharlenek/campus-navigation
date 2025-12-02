#include "Graph.h"
#include <queue>
#include <limits>
#include <algorithm>
#include <functional>
#include <stdexcept>

using namespace std;

void Graph::addLocation(int id, const std::string &name) {
    // don't overwrite existing name if already present
    if (names.find(id) == names.end()) {
        names[id] = name;
    }
}

void Graph::addEdge(int a, int b, int weight) {
    // add nodes if missing
    if (adj.find(a) == adj.end()) adj[a] = {};
    if (adj.find(b) == adj.end()) adj[b] = {};

    // add edge a->b
    Edge e1;
    e1.to = b;
    e1.weight = weight;
    e1.open = true;
    adj[a].push_back(e1);

    // add edge b->a
    Edge e2;
    e2.to = a;
    e2.weight = weight;
    e2.open = true;
    adj[b].push_back(e2);
}

void Graph::toggleEdge(int a, int b) {
    auto it = adj.find(a);
    if (it != adj.end()) {
        for (auto &e : it->second) {
            if (e.to == b) {
                e.open = !e.open;
                break;
            }
        }
    }
    auto it2 = adj.find(b);
    if (it2 != adj.end()) {
        for (auto &e : it2->second) {
            if (e.to == a) {
                e.open = !e.open;
                break;
            }
        }
    }
}

std::string Graph::edgeStatus(int a, int b) const {
    auto it = adj.find(a);
    if (it == adj.end()) return "DNE";
    for (const auto &e : it->second) {
        if (e.to == b) {
            return e.open ? "open" : "closed";
        }
    }
    return "DNE";
}

bool Graph::isConnected(int a, int b) const {
    if (adj.find(a) == adj.end() || adj.find(b) == adj.end()) return false;
    unordered_set<int> visited;
    queue<int> q;
    q.push(a);
    visited.insert(a);
    while (!q.empty()) {
        int u = q.front(); q.pop();
        if (u == b) return true;
        auto it = adj.find(u);
        if (it == adj.end()) continue;
        for (const auto &e : it->second) {
            if (!e.open) continue;
            if (visited.find(e.to) == visited.end()) {
                visited.insert(e.to);
                q.push(e.to);
            }
        }
    }
    return false;
}

int Graph::shortestPath(int src, int dst) const {
    if (adj.find(src) == adj.end() || adj.find(dst) == adj.end()) return -1;
    const int INF = std::numeric_limits<int>::max();
    unordered_map<int, int> dist;
    // min-heap (dist, node)
    using P = pair<int,int>;
    priority_queue<P, vector<P>, greater<P>> pq;

    for (const auto &kv : adj) dist[kv.first] = INF;
    dist[src] = 0;
    pq.push({0, src});

    while (!pq.empty()) {
        auto [d,u] = pq.top(); pq.pop();
        if (d != dist[u]) continue;
        if (u == dst) return d;
        for (const auto &e : adj.at(u)) {
            if (!e.open) continue;
            int nd = d + e.weight;
            if (nd < dist[e.to]) {
                dist[e.to] = nd;
                pq.push({nd, e.to});
            }
        }
    }

    return (dist[dst] == INF) ? -1 : dist[dst];
}

int Graph::mstCost(const std::unordered_set<int> &vertices) const {
    if (vertices.empty()) return 0;
    // Prim's algorithm restricted to `vertices` and only open edges
    unordered_set<int> V = vertices;
    // pick a start vertex
    int start = *V.begin();
    unordered_set<int> visited;
    using EdgeItem = pair<int, pair<int,int>>; // weight, (u,v)
    priority_queue<EdgeItem, vector<EdgeItem>, greater<EdgeItem>> pq;

    visited.insert(start);
    // push all edges from start that lead to vertices set
    auto pushEdgesFrom = [&](int u){
        auto it = adj.find(u);
        if (it == adj.end()) return;
        for (const auto &e : it->second) {
            if (!e.open) continue;
            if (V.find(e.to) == V.end()) continue;
            pq.push({e.weight, {u, e.to}});
        }
    };

    pushEdgesFrom(start);
    int total = 0;
    while (!pq.empty() && visited.size() < V.size()) {
        auto item = pq.top(); pq.pop();
        int w = item.first;
        int u = item.second.first;
        int v = item.second.second;
        if (visited.find(v) != visited.end()) continue;
        // add v
        visited.insert(v);
        total += w;
        pushEdgesFrom(v);
    }

    if (visited.size() != V.size()) {
        // graph not fully connected within vertices (shouldn't happen for tested cases)
        return -1;
    }
    return total;
}
