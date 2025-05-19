#ifndef ALGO_H
#define ALGO_H
#include <vector>
#include <unordered_map>

struct coord {
    int i;
    int j;
};

enum EdgeType {
    REF_BLANK,       //sij to sij+1
    QUERY_BLANK,     //sij to si+1j
    MATCH,           //sij to si+1j+1
    REPEAT,          //sij-k to sij
    INVERTED_REPEAT  //sij to sij+k
};

class Node;

class Edge {
public:
    EdgeType type;
    Node* from;
    Node* to;
    int weight;
};
class Node {
public:
    int i;
    int j;
    int rating;
    bool processed;
    Node(int i, int j, int _rating) : i(i), j(j), rating(_rating), processed(false) {}
};

#endif