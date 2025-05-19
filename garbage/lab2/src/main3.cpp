#include <cstdio>
#include <vector>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <queue>

struct NodeEntry {
    char type;
    unsigned short i, j;
    bool operator==(const NodeEntry& other) const {
        return type == other.type && i == other.i && j == other.j;
    }
    bool operator<(const NodeEntry& other) const {
        return i < other.i || (i == other.i && j < other.j);
    }
};

namespace std {
template <>
struct hash<NodeEntry> {
    size_t operator()(const NodeEntry& entry) const {
        return entry.i << 16 | entry.j;
    }
};
}  // namespace std

FILE* ref_file;
FILE* query_file;
FILE* plot_file;
FILE* dbg_file;

struct dis_t {
    int dis;
    NodeEntry entry;
};

std::vector<char> reference, reference_r, query;
std::unordered_map<NodeEntry, dis_t, std::hash<NodeEntry>> dis;
std::unordered_set<NodeEntry, std::hash<NodeEntry>> vis;
//std::deque<NodeEntry> queue;
std::priority_queue<std::pair<int, NodeEntry>> queue_pq;

char tr(char c) {
    if (c == 'A') return 'T';
    if (c == 'T') return 'A';
    if (c == 'C') return 'G';
    if (c == 'G') return 'C';
    return c;
}

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
    reference_r.clear();
    query.clear();
    reference.push_back('X');
    query.push_back('X');
    reference_r.push_back('Y');
    char c;
    while (true) {
        c = fgetc(ref_file);
        if (c == EOF) break;
        if (c == '\n') break;
        reference.push_back(c);
        reference_r.push_back(tr(c));
    }
    while (true) {
        c = fgetc(query_file);
        if (c == EOF) break;
        if (c == '\n') break;
        query.push_back(c);
    }
    dis[NodeEntry{
        'C',
        0,
        0}] = {0, NodeEntry{'C', 0, 0}};
    queue_pq.push({0, NodeEntry{'C', 0, 0}});
    //visited_set.insert(NodeEntry{'C', 0, 0, 0});
    unsigned short tgt_j = query.size() - 1;

    while (!queue_pq.empty()) {
        NodeEntry entry = queue_pq.top().second;
        [[unlikely]]
        if (entry.j == tgt_j) {
            printf("Found a path with distance %d\n", dis[entry].dis);
            break;
        }
        queue_pq.pop();
        if (vis.find(entry) != vis.end()) {
            continue;
        }
        //fprintf(dbg_file, "%c %d %d %d\n", entry.type, entry.i, entry.j, dis[entry]);
        [[unlikely]] vis.insert(entry);
        int distance = dis[entry].dis;
        // if (vis.size() % 100000 == 0) {
        //     printf("Visited %d nodes, dis %d\n", vis.size(), distance);
        // }
        if (entry.type == 'C') {
            for (int i = 0; i <= reference.size(); i++) {
                NodeEntry next_entry{'A', i, entry.j};
                if (!dis.contains(next_entry) || dis[next_entry].dis > distance) {
                    dis[next_entry] = {distance, entry};
                    queue_pq.push({-distance, next_entry});
                }
                NodeEntry next_entry2{'B', i, entry.j};
                if (!dis.contains(next_entry2) || dis[next_entry2].dis > distance) {
                    dis[next_entry2] = {distance, entry};
                    queue_pq.push({-distance, next_entry});
                }
            }
            printf("Reach query %d, distance: %d\n", entry.j, distance);
        } else if (entry.type == 'A') {
            unsigned short& i = entry.i;
            unsigned short& j = entry.j;
            // Process A type nodes
            [[likely]]
            if (i + 1 <= reference.size() - 1 && j + 1 <= query.size() - 1) {
                NodeEntry next_entry{'A', i + 1, j + 1};
                int new_distance = (reference[i + 1] == query[j + 1]) ? 0 : 1;
                if (!dis.contains(next_entry) || dis[next_entry].dis > distance + new_distance) {
                    dis[next_entry] = {distance + new_distance, entry};
                    if (new_distance == 0) {
                        queue_pq.push({-(distance + new_distance), next_entry});
                    } else {
                        queue_pq.push({-(distance + new_distance), next_entry});
                    }
                }
            }
            if (i + 1 <= reference.size() - 1) {
                NodeEntry next_entry{'A', i + 1, j};
                if (!dis.contains(next_entry) || dis[next_entry].dis > distance + 1) {
                    dis[next_entry] = {distance + 1, entry};
                    queue_pq.push({-(distance + 1), next_entry});
                }
            }
            if (j + 1 <= query.size() - 1) {
                NodeEntry next_entry{'A', i, j + 1};
                if (!dis.contains(next_entry) || dis[next_entry].dis > distance + 1) {
                    dis[next_entry] = {distance + 1, entry};
                    queue_pq.push({-(distance + 1), next_entry});
                }
            }
            NodeEntry next_entry{'C', 0, j};
            if (!dis.contains(next_entry) || dis[next_entry].dis > distance + 2) {
                dis[next_entry] = {distance + 2, entry};
                queue_pq.push({-(distance + 2), next_entry});
            }
        } else if (entry.type == 'B') {
            unsigned short& i = entry.i;
            unsigned short& j = entry.j;
            // Process B type nodes
            [[likely]]
            if (i >= 1 && j + 1 <= query.size() - 1) {
                NodeEntry next_entry{'B', i - 1, j + 1};
                int new_distance = (reference_r[i - 1] == query[j + 1]) ? 0 : 1;
                if (!dis.contains(next_entry) || dis[next_entry].dis > distance + new_distance) {
                    dis[next_entry] = {distance + new_distance, entry};
                    if (new_distance == 0) {
                        queue_pq.push({-(distance + new_distance), next_entry});
                    } else {
                        queue_pq.push({-(distance + new_distance), next_entry});
                    }
                }
            }
            if (i >= 1) {
                NodeEntry next_entry{'B', i - 1, j};
                if (!dis.contains(next_entry) || dis[next_entry].dis > distance + 1) {
                    dis[next_entry] = {distance + 1, entry};
                    queue_pq.push({-(distance + 1), next_entry});
                }
            }
            if (j + 1 <= query.size() - 1) {
                NodeEntry next_entry{'B', i, j + 1};
                if (!dis.contains(next_entry) || dis[next_entry].dis > distance + 1) {
                    dis[next_entry] = {distance + 1, entry};
                    queue_pq.push({-(distance + 1), next_entry});
                }
            }
            NodeEntry next_entry{'C', 0, j};
            if (!dis.contains(next_entry) || dis[next_entry].dis > distance + 2) {
                dis[next_entry] = {distance + 2, entry};
                queue_pq.push({-(distance + 2), next_entry});
            }
        }
    }
    // Print the path
    NodeEntry entry = queue_pq.top().second;
    int seg_end_i = entry.i, seg_end_j = entry.j;
    while (entry.type != 'C' || entry.i != 0 || entry.j != 0) {
        if (entry.type == 'A' && entry.type != dis[entry].entry.type) {
            fprintf(plot_file, "(%d,%d,%d,%d),\n", entry.j + 1, seg_end_j, entry.i + 1, seg_end_i);
            fflush(plot_file);
        }
        if (entry.type == 'B' && entry.type != dis[entry].entry.type) {
            fprintf(plot_file, "(%d,%d,%d,%d),\n", entry.j + 1, seg_end_j, seg_end_i, entry.i - 1);
            fflush(plot_file);
        }
        if (entry.type == 'C' && entry.type != dis[entry].entry.type) {
            seg_end_i = dis[entry].entry.i;
            seg_end_j = dis[entry].entry.j;
        }
        entry = dis[entry].entry;
    }
}