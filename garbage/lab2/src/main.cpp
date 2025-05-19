#include <cstdio>
#include "graph.h"
#include <vector>
#include <deque>
#include <unordered_set>

FILE* ref_file;
FILE* query_file;
FILE* plot_file;
FILE* dbg_file;

std::vector<char> reference, query;
std::deque<Node*> queue;
std::unordered_set<int> visited;
std::unordered_set<Node*> visited_set;

int main(int argc, char* argv[]) {
#ifdef DEBUG
    ref_file = fopen("pj3/reference.txt", "r");
    query_file = fopen("pj3/query.txt", "r");
    plot_file = fopen("pj3/plot.txt", "w");
    dbg_file = fopen("pj3/debug.txt", "w");
#else
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <reference_file> <query_file> \n", argv[0]);
        return 1;
    }
    ref_file = fopen(argv[1], "r");
    if (ref_file == NULL) {
        fprintf(stderr, "Error opening reference file\n");
        return 1;
    }
    query_file = fopen(argv[2], "r");
    if (query_file == NULL) {
        fprintf(stderr, "Error opening query file\n");
        fclose(ref_file);
        return 1;
    }
    plot_file = fopen("plot.txt", "w");
    if (plot_file == NULL) {
        fprintf(stderr, "Error opening plot file\n");
        fclose(ref_file);
        fclose(query_file);
        return 1;
    }
    dbg_file = fopen("debug.txt", "w");
    if (dbg_file == NULL) {
        fprintf(stderr, "Error opening debug file\n");
        fclose(ref_file);
        fclose(query_file);
        fclose(plot_file);
        return 1;
    }
#endif
    reference.clear();
    query.clear();
    char c;
    while (true) {
        c = fgetc(ref_file);
        if (c == EOF) break;
        if (c == '\n') break;
        reference.push_back(c);
    }
    while (true) {
        c = fgetc(query_file);
        if (c == EOF) break;
        if (c == '\n') break;
        query.push_back(c);
    }
    GraphGenerator GG(reference, query);
    Node* begin = GG.getNode(NodeEntry{'C', 0, 0, 0});
    begin->distance = 0;
    //Node* end = GG.getNode(NodeEntry{'C', 0, static_cast<int>(query.size()), 0});
    //fprintf(dbg_file, "Init ok, begin: %p, end: %p\n", begin, end);

    //dijkstra(actually bfs as only 0 and 1 weights)
    queue.push_back(begin);
    //visited_set.insert(NodeEntry{'C', 0, 0, 0});
    int distance = -1;
    while (!queue.empty()) {
        Node* node = queue.front();
        queue.pop_front();

        if (visited_set.contains(node)) {
            continue;
        }
        visited_set.insert(node);

        if (visited_set.size() % 100000 == 0) {
            printf("Visited: %d\n", visited_set.size());
        }

        //fprintf(dbg_file, "Pop node: %c %d %d %d\n", node_entry.type, node_entry.i, node_entry.j, node->distance);
        if (node->distance != distance) {
            //clear visited
            visited.clear();
            distance = node->distance;
            printf("Distance: %d\n", distance);
        }
        if (!visited.contains(node->entry.j)) {
            visited.insert(node->entry.j);
            fprintf(dbg_file, "Pop node: %c %d %d %d\n", node->entry.type, node->entry.i, node->entry.j, node->distance);
            fflush(dbg_file);
            printf("Reach query %d, distance: %d\n", node->entry.j, node->distance);
        }
        if (node->entry.j == query.size()) {
            fprintf(dbg_file, "Find end node\n");
            break;
        }
        for (Edge* edge : node->edges) {
            // if (visited_set.contains(edge->to)) {
            //     continue;
            // }
            Node* next_node = GG.getNode(edge->to);
            if (next_node->distance > node->distance + edge->weight) {
                next_node->distance = node->distance + edge->weight;
                next_node->prev = node->entry;
                if (edge->weight == 0)
                    queue.push_front(next_node);
                else
                    queue.push_back(next_node);
                //visited_set.insert(edge->to);
            }
        }
    }
}