#include <cstdio>
#include <vector>
#include <deque>
#include <unordered_map>
#include <unordered_set>

struct NodeEntry {
    unsigned short i, j;
    char type;
    bool operator==(const NodeEntry& other) const {
        return type == other.type && i == other.i && j == other.j;
    }
    NodeEntry(char t, unsigned short i, unsigned short j)
        : type(t), i(i), j(j) {}
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
    NodeEntry entry;
    int dis;
    int ref_counter;
    dis_t(int d, NodeEntry e) : dis(d), entry(e), ref_counter(0) {}
    dis_t() : dis(0), entry(NodeEntry{'C', 0, 0}), ref_counter(0) {}
};

std::vector<char> reference, reference_r, query;
std::unordered_map<NodeEntry, dis_t, std::hash<NodeEntry>> dis;
std::unordered_set<NodeEntry, std::hash<NodeEntry>> vis;
std::deque<NodeEntry> queue;

void check_dis_cnt(NodeEntry entry) {
    if (dis[entry].ref_counter > 0) return;
    dis[dis[entry].entry].ref_counter--;
    check_dis_cnt(dis[entry].entry);
    dis.erase(entry);
}

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
    queue.push_back(NodeEntry{'C', 0, 0});
    //visited_set.insert(NodeEntry{'C', 0, 0, 0});
    unsigned short tgt_j = query.size() - 1;

    while (!queue.empty()) {
        NodeEntry entry = queue.front();
        [[unlikely]]
        if (entry.j == tgt_j) {
            printf("Found a path with distance %d\n", dis[entry].dis);
            break;
        }
        queue.pop_front();
        if (vis.find(entry) != vis.end()) {
            continue;
        }
        //fprintf(dbg_file, "%c %d %d %d\n", entry.type, entry.i, entry.j, dis[entry]);
        vis.insert(entry);
        int distance = dis[entry].dis;
        // if (vis.size() % 100000 == 0) {
        //     printf("Visited %d nodes, dis %d, \n", vis.size(), distance);
        // }
        if (entry.type == 'C') {
            for (int i = 0; i <= reference.size(); i++) {
                NodeEntry next_entry{'A', i, entry.j};
                if (!dis.contains(next_entry)) {
                    dis[next_entry] = {distance, entry};
                    dis[entry].ref_counter++;
                    queue.push_front(next_entry);
                } else if (dis[next_entry].dis > distance) {
                    dis[dis[next_entry].entry].ref_counter--;
                    check_dis_cnt(dis[next_entry].entry);
                    dis[next_entry] = {distance, entry};
                    dis[entry].ref_counter++;
                    queue.push_front(next_entry);
                }
                NodeEntry next_entry2{'B', i, entry.j};
                if (!dis.contains(next_entry2)) {
                    dis[next_entry2] = {distance, entry};
                    dis[entry].ref_counter++;
                    queue.push_front(next_entry2);
                } else if (dis[next_entry2].dis > distance) {
                    dis[dis[next_entry2].entry].ref_counter--;
                    check_dis_cnt(dis[next_entry2].entry);
                    dis[next_entry2] = {distance, entry};
                    dis[entry].ref_counter++;
                    queue.push_front(next_entry2);
                }
            }
            printf("Reach query %d, distance: %d, memload: %.2lf%%\n", entry.j, distance, (double)100 * dis.size() / vis.size());
        } else if (entry.type == 'A') {
            unsigned short& i = entry.i;
            unsigned short& j = entry.j;
            // Process A type nodes
            [[likely]]
            if (i + 1 <= reference.size() - 1 && j + 1 <= query.size() - 1) {
                NodeEntry next_entry{'A', i + 1, j + 1};
                int new_distance = (reference[i + 1] == query[j + 1]) ? 0 : 1;
                if (!dis.contains(next_entry)) {
                    dis[next_entry] = {distance + new_distance, entry};
                    dis[entry].ref_counter++;
                    if (new_distance == 0) {
                        queue.push_front(next_entry);
                    } else {
                        queue.push_back(next_entry);
                    }
                } else if (dis[next_entry].dis > distance + new_distance) {
                    dis[dis[next_entry].entry].ref_counter--;
                    check_dis_cnt(dis[next_entry].entry);
                    dis[next_entry] = {distance + new_distance, entry};
                    dis[entry].ref_counter++;
                    if (new_distance == 0) {
                        queue.push_front(next_entry);
                    } else {
                        queue.push_back(next_entry);
                    }
                }
            }
            if (i + 1 <= reference.size() - 1) {
                NodeEntry next_entry{'A', i + 1, j};
                if (!dis.contains(next_entry)) {
                    dis[next_entry] = {distance + 1, entry};
                    dis[entry].ref_counter++;
                    queue.push_back(next_entry);
                } else if (dis[next_entry].dis > distance + 1) {
                    dis[dis[next_entry].entry].ref_counter--;
                    check_dis_cnt(dis[next_entry].entry);
                    dis[next_entry] = {distance + 1, entry};
                    dis[entry].ref_counter++;
                    queue.push_back(next_entry);
                }
            }
            if (j + 1 <= query.size() - 1) {
                NodeEntry next_entry{'A', i, j + 1};
                if (!dis.contains(next_entry)) {
                    dis[next_entry] = {distance + 1, entry};
                    dis[entry].ref_counter++;
                    queue.push_back(next_entry);
                } else if (dis[next_entry].dis > distance + 1) {
                    dis[dis[next_entry].entry].ref_counter--;
                    check_dis_cnt(dis[next_entry].entry);
                    dis[next_entry] = {distance + 1, entry};
                    dis[entry].ref_counter++;
                    queue.push_back(next_entry);
                }
            }
            NodeEntry next_entry{'C', 0, j};
            if (!dis.contains(next_entry)) {
                dis[next_entry] = {distance + 1, entry};
                dis[entry].ref_counter++;
                queue.push_back(next_entry);
            } else if (dis[next_entry].dis > distance + 1) {
                dis[dis[next_entry].entry].ref_counter--;
                check_dis_cnt(dis[next_entry].entry);
                dis[next_entry] = {distance + 1, entry};
                dis[entry].ref_counter++;
                queue.push_back(next_entry);
            }
        } else if (entry.type == 'B') {
            unsigned short& i = entry.i;
            unsigned short& j = entry.j;
            // Process B type nodes
            [[likely]]
            if (i >= 1 && j + 1 <= query.size() - 1) {
                NodeEntry next_entry{'B', i - 1, j + 1};
                int new_distance = (reference_r[i - 1] == query[j + 1]) ? 0 : 1;
                if (!dis.contains(next_entry)) {
                    dis[next_entry] = {distance + new_distance, entry};
                    dis[entry].ref_counter++;
                    if (new_distance == 0) {
                        queue.push_front(next_entry);
                    } else {
                        queue.push_back(next_entry);
                    }
                } else if (dis[next_entry].dis > distance + new_distance) {
                    dis[dis[next_entry].entry].ref_counter--;
                    check_dis_cnt(dis[next_entry].entry);
                    dis[next_entry] = {distance + new_distance, entry};
                    dis[entry].ref_counter++;
                    if (new_distance == 0) {
                        queue.push_front(next_entry);
                    } else {
                        queue.push_back(next_entry);
                    }
                }
            }
            if (i >= 1) {
                NodeEntry next_entry{'B', i - 1, j};
                if (!dis.contains(next_entry)) {
                    dis[next_entry] = {distance + 1, entry};
                    dis[entry].ref_counter++;
                    queue.push_back(next_entry);
                } else if (dis[next_entry].dis > distance + 1) {
                    dis[dis[next_entry].entry].ref_counter--;
                    check_dis_cnt(dis[next_entry].entry);
                    dis[next_entry] = {distance + 1, entry};
                    dis[entry].ref_counter++;
                    queue.push_back(next_entry);
                }
            }
            if (j + 1 <= query.size() - 1) {
                NodeEntry next_entry{'B', i, j + 1};
                if (!dis.contains(next_entry)) {
                    dis[next_entry] = {distance + 1, entry};
                    dis[entry].ref_counter++;
                    queue.push_back(next_entry);
                } else if (dis[next_entry].dis > distance + 1) {
                    dis[dis[next_entry].entry].ref_counter--;
                    check_dis_cnt(dis[next_entry].entry);
                    dis[next_entry] = {distance + 1, entry};
                    dis[entry].ref_counter++;
                    queue.push_back(next_entry);
                }
            }
            NodeEntry next_entry{'C', 0, j};
            if (!dis.contains(next_entry)) {
                dis[next_entry] = {distance + 1, entry};
                dis[entry].ref_counter++;
                queue.push_back(next_entry);
            } else if (dis[next_entry].dis > distance + 1) {
                dis[dis[next_entry].entry].ref_counter--;
                check_dis_cnt(dis[next_entry].entry);
                dis[next_entry] = {distance + 1, entry};
                dis[entry].ref_counter++;
                queue.push_back(next_entry);
            }
        }
    }
    // Print the path
    NodeEntry entry = queue.front();
    int seg_end_i = entry.i, seg_end_j = entry.j;
    while (entry.type != 'C' || entry.i != 0 || entry.j != 0) {
        if (entry.type == 'A' && entry.type != dis[entry].entry.type) {
            fprintf(plot_file, "(%d,%d,%d,%d),\n", entry.j, seg_end_j, entry.i, seg_end_i);
            fflush(plot_file);
        }
        if (entry.type == 'B' && entry.type != dis[entry].entry.type) {
            fprintf(plot_file, "(%d,%d,%d,%d),\n", entry.j, seg_end_j, seg_end_i, entry.i);
            fflush(plot_file);
        }
        if (entry.type == 'C' && entry.type != dis[entry].entry.type) {
            seg_end_i = dis[entry].entry.i;
            seg_end_j = dis[entry].entry.j;
        }
        entry = dis[entry].entry;
    }
}