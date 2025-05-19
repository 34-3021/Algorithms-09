#include <cstdio>
#include <vector>
#include <unordered_map>
#include <queue>
#include "algo.h"

using hash_t = unsigned long long;

const int RATE_MAT[5][5] = {
    {5, -1, -2, -1, -3},
    {-1, 5, -3, -2, -4},
    {-2, -3, 5, -2, -2},
    {-1, -2, -2, 5, -1},
    {-3, -4, -2, -1, 0}};

int rate(char a, char b) {
    if (a == 'A')
        a = 0;
    else if (a == 'C')
        a = 1;
    else if (a == 'G')
        a = 2;
    else if (a == 'T')
        a = 3;
    else
        a = 4;  // gap
    if (b == 'A')
        b = 0;
    else if (b == 'C')
        b = 1;
    else if (b == 'G')
        b = 2;
    else if (b == 'T')
        b = 3;
    else
        b = 4;  // gap
    return RATE_MAT[a][b];
}

std::vector<char> reference, query, reference_rc;
std::vector<hash_t> hash_ref, hash_query, hash_refrc;
std::vector<hash_t> pow131;

void initialize(size_t max_size) {
    hash_ref.resize(max_size);
    hash_query.resize(max_size);
    hash_refrc.resize(max_size);
    pow131.resize(max_size, 1);

    for (size_t i = 1; i < max_size; ++i) {
        pow131[i] = pow131[i - 1] * 131;
    }
}
void input() {
    char c;
    while (true) {
        c = getchar();
        if (c == '\n') break;
        reference.push_back(c);
    }
    while (true) {
        c = getchar();
        if (c == '\n') break;
        query.push_back(c);
    }
}
void hash(int l, int r, std::vector<char> s, std::vector<hash_t> ht) {
    hash_t h = 0;
    for (int i = l; i < r; i++) {
        h = h * 131 + s[i];
        ht[i] = h;
    }
}
void hash_inv(int l, int r, std::vector<char> s, std::vector<hash_t> ht) {
    int n = s.size();
    for (int i = 0; i < n; i++) {
        reference_rc[i] = s[n - i - 1];
    }
    reference_rc[n] = '\0';
    for (int i = 0; i < n; i++) {
        switch (reference_rc[i]) {
            case 'A':
                reference_rc[i] = 'T';
                break;
            case 'T':
                reference_rc[i] = 'A';
                break;
            case 'C':
                reference_rc[i] = 'G';
                break;
            case 'G':
                reference_rc[i] = 'C';
                break;
        }
    }
    hash(0, n, reference_rc, ht);
}
hash_t partial_hash(int l, int r, std::vector<hash_t> ht) {
    if (l == 0) {
        return ht[r];
    } else {
        return ht[r] - ht[l - 1] * pow131[r - l + 1];
    }
}
hash_t partial_hash_inv(int l, int r, int n, std::vector<hash_t> htinv) {
    return partial_hash(n - r - 1, n - l - 1, htinv);
}

struct cmp {
    bool operator()(const Node* a, const Node* b) const {
        return a->rating < b->rating;
    }
};

struct segment_entry {
    int begin;
    int end;
    bool inverted;
};

std::unordered_map<coord, Node*> coord_to_node;

std::priority_queue<Node*, std::vector<Node*>, cmp> node_queue;

void processNode(Node* node) {
}

using substring_container = std::unordered_map<hash_t, segment_entry*>;

std::vector<substring_container> substrings;  //of length k

int main() {
    input();
    size_t reference_len = reference.size(), query_len = query.size();
    initialize(reference_len + 1);
    hash(0, reference_len, reference, hash_ref);
    hash(0, query_len, query, hash_query);
    hash_inv(0, reference_len, reference, hash_refrc);

    for (int i = 0; i < reference_len; i++) {
        for (int j = i; j < reference_len; j++) {
            hash_t h1 = partial_hash(i, j, hash_ref);
            hash_t h2 = partial_hash_inv(i, j, reference_len, hash_refrc);
            if (!substrings[j - i + 1].contains(h1)) {
                segment_entry* entry = new segment_entry;
                entry->begin = i;
                entry->end = j;
                entry->inverted = false;
                substrings[j - i + 1][h1] = entry;
            }
            if (!substrings[j - i + 1].contains(h2)) {
                segment_entry* entry = new segment_entry;
                entry->begin = i;
                entry->end = j;
                entry->inverted = true;
                substrings[j - i + 1][h2] = entry;
            }
        }
    }

    //initialize 0,0 node
    Node* start_node = new Node(0, 0, 0);
    coord_to_node[{0, 0}] = start_node;
    node_queue.push(start_node);
    start_node->processed = true;

    while (!node_queue.empty()) {
        Node* node = node_queue.top();
        node_queue.pop();
        processNode(node);
        for (int i = 0; i < query_len; i++) {
            for (int j = i; j < query_len; j++) {
                hash_t h = partial_hash(i, j, hash_query);
                if (substrings[j - i + 1].contains(h)) {
                    int jump_from = j + 1;
                    if (node->rating > 1 + node->rating) {
                        node->rating = 1 + node->rating;
                        coord_to_node[{jump_from, substrings[j - i + 1][h]->begin}] = new Node(jump_from, substrings[j - i + 1][h]->begin, node->rating);
                        node_queue.push(coord_to_node[{jump_from, substrings[j - i + 1][h]->begin}]);
                    }
                }
            }
        }
    }

#ifdef DEBUG
    getchar();
    getchar();
#endif
    return 0;
}