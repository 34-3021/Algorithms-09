#include <vector>
#include <unordered_map>

class Node;
class Edge;

struct NodeEntry {
    char type;
    int i, j, k;
    bool operator==(const NodeEntry& other) const {
        return type == other.type && i == other.i && j == other.j && k == other.k;
    }
};

namespace std {
template <>
struct hash<NodeEntry> {
    size_t operator()(const NodeEntry& entry) const {
        size_t h1 = hash<char>{}(entry.type);
        size_t h2 = hash<int>{}(entry.i);
        size_t h3 = hash<int>{}(entry.j);
        size_t h4 = hash<int>{}(entry.k);
        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
    }
};
}  // namespace std

class Edge {
public:
    Node* from;
    NodeEntry to;
    int weight;

    Edge(Node* from, NodeEntry to, int weight) : from(from), to(to), weight(weight) {}
};

class Node {
public:
    std::vector<Edge*> edges;
    int distance;
    NodeEntry entry;
    NodeEntry prev;
    Node() : distance(0x3f3f3f3f) {}
};

class GraphGenerator {
private:
    std::vector<char> reference;
    int ref_len;
    std::vector<char> query;
    int query_len;
    std::unordered_map<NodeEntry, Node*, std::hash<NodeEntry>> nodes;
    bool match(char a, char b) {
        if (a == 'A' && b == 'T') return true;
        if (a == 'T' && b == 'A') return true;
        if (a == 'C' && b == 'G') return true;
        if (a == 'G' && b == 'C') return true;
        return false;
    }
    void initTypeANode(NodeEntry _entry) {
        Node* node = new Node();
        node->entry = _entry;
        //         对于 A 类点为起点的边：

        // - $NodeA(i,j) \to NodeA(i+1,j+1)$ ，当 $ref[i+1]$==$query[j+1]$ 边权为 0，否则边权为 1。
        // - $NodeA(i,j) \to NodeA(i+1,j)$ ，边权为 1。
        // - $NodeA(i,j) \to NodeA(i,j+1)$ ，边权为 1。
        // - $NodeA(i,j) \to NodeC(j)$ ，边权为 1。

        nodes[_entry] = node;
        // 1. $NodeA(i,j) \to NodeA(i+1,j+1)$
        if (_entry.i + 1 <= ref_len && _entry.j + 1 <= query_len) {
            if (reference[_entry.i + 1] == query[_entry.j + 1]) {
                node->edges.push_back(new Edge(node, NodeEntry{'A', _entry.i + 1, _entry.j + 1}, 0));
            } else {
                node->edges.push_back(new Edge(node, NodeEntry{'A', _entry.i + 1, _entry.j + 1}, 1));
            }
        }
        // 2. $NodeA(i,j) \to NodeA(i+1,j)$
        if (_entry.i + 1 <= ref_len) {
            node->edges.push_back(new Edge(node, NodeEntry{'A', _entry.i + 1, _entry.j}, 1));
        }
        // 3. $NodeA(i,j) \to NodeA(i,j+1)$
        if (_entry.j + 1 <= query_len) {
            node->edges.push_back(new Edge(node, NodeEntry{'A', _entry.i, _entry.j + 1}, 1));
        }
        // 4. $NodeA(i,j) \to NodeC(j)$
        if (_entry.j <= query_len) {
            node->edges.push_back(new Edge(node, NodeEntry{'C', 0, _entry.j, 0}, 1));
        }
    }
    void initTypeBNode(NodeEntry _entry) {
        Node* node = new Node();
        node->entry = _entry;
        //         对于 B 类点为起点的边：

        // - $NodeB(i,j) \to NodeB(i-1,j+1)$ ，当 $ref[i-1]$==$query[j+1]$ 边权为 0，否则边权为 1。
        // - $NodeB(i,j) \to NodeB(i-1,j)$ ，边权为 1。
        // - $NodeB(i,j) \to NodeB(i,j+1)$ ，边权为 1。
        // - $NodeB(i,j) \to NodeC(j)$ ，边权为 1。

        nodes[_entry] = node;
        // 1. $NodeB(i,j) \to NodeB(i-1,j+1)$
        if (_entry.i - 1 >= 0 && _entry.j + 1 <= query_len) {
            if (match(reference[_entry.i - 1], query[_entry.j + 1])) {
                node->edges.push_back(new Edge(node, NodeEntry{'B', _entry.i - 1, _entry.j + 1}, 0));
            } else {
                node->edges.push_back(new Edge(node, NodeEntry{'B', _entry.i - 1, _entry.j + 1}, 1));
            }
        }
        // 2. $NodeB(i,j) \to NodeB(i-1,j)$
        if (_entry.i - 1 >= 0) {
            node->edges.push_back(new Edge(node, NodeEntry{'B', _entry.i - 1, _entry.j}, 1));
        }
        // 3. $NodeB(i,j) \to NodeB(i,j+1)$
        if (_entry.j + 1 <= query_len) {
            node->edges.push_back(new Edge(node, NodeEntry{'B', _entry.i, _entry.j + 1}, 1));
        }
        // 4. $NodeB(i,j) \to NodeC(j)$
        if (_entry.j <= query_len) {
            node->edges.push_back(new Edge(node, NodeEntry{'C', 0, _entry.j, 0}, 1));
        }
    }
    void initTypeCNode(NodeEntry _entry) {
        Node* node = new Node();
        node->entry = _entry;
        //        对于 C 类点为起点的边：

        // - $NodeC(j) \to NodeA(i,j)$ ，边权为 0。
        // - $NodeC(j) \to NodeB(i,j)$ ，边权为 0。

        nodes[_entry] = node;
        // 1. $NodeC(j) \to NodeA(i,j)$
        for (int i = 0; i <= ref_len; i++) {
            node->edges.push_back(new Edge(node, NodeEntry{'A', i, _entry.j, 0}, 0));
        }
        // 2. $NodeC(j) \to NodeB(i,j)$
        for (int i = 0; i <= ref_len; i++) {
            node->edges.push_back(new Edge(node, NodeEntry{'B', i, _entry.j, 0}, 0));
        }
    }

public:
    Node* getNode(NodeEntry _entry) {
        if (!nodes.contains(_entry)) {
            if (_entry.type == 'A') {
                initTypeANode(_entry);
            } else if (_entry.type == 'B') {
                initTypeBNode(_entry);
            } else if (_entry.type == 'C') {
                initTypeCNode(_entry);
            }
        }
        return nodes[_entry];
    }
    GraphGenerator(std::vector<char> _reference, std::vector<char> _query) {
        reference = {'X'};
        reference.insert(reference.end(), _reference.begin(), _reference.end());
        query = {'X'};
        query.insert(query.end(), _query.begin(), _query.end());
        ref_len = reference.size() - 1;
        query_len = query.size() - 1;
    }
};