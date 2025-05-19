#include <cstdio>
#include <cstring>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <vector>
#include <algorithm>
#ifdef DEBUG
#include "lib/hashing.h"
#else
#include "hashing.h"
#endif

const int kmer_size = 6;
const int INF = 1e8;

FILE* ref_file;
FILE* query_file;
FILE* plot_file;
FILE* dbg_file;

std::unordered_map<int, std::unordered_map<hash_t, int>> kmer_hashes, kmer_hashes_i;  // length k,k+1,...,2k-1 has hash? where?

struct result {
    int weight;
    int ref_start, ref_end;
};

result edgeweight(int u, int v) {
    if (u == v) return {0, 0, 0};
    if (v - u >= 2 * kmer_size) return {INF, 0, 0};
    if (v - u >= kmer_size) {
        if (kmer_hashes[v - u].contains(partial_hash(u + 1, v, hash_query))) {
            int ref_start = kmer_hashes[v - u][partial_hash(u + 1, v, hash_query)];
            return {0, ref_start, ref_start + v - u - 1};
        }
        if (kmer_hashes_i[v - u].contains(partial_hash(u + 1, v, hash_query))) {
            int ref_start = kmer_hashes_i[v - u][partial_hash(u + 1, v, hash_query)];
            return {0, ref_start, ref_start + v - u - 1};
        }
        //not perfect match then
        //evaluate query[u+1,v] and every 1~v-u+kmer_size substrings of reference
    }
    return {v - u, 0, 0};  //get no point if a short match
}

int dijkstra(int start, int end) {
    std::vector<int> dist(query.size() + 5, INF);
    std::vector<int> prev(query.size() + 5, -1);
    std::vector<std::pair<int, int>> in_ref(query.size() + 5, {0, 0});
    std::priority_queue<std::pair<int, int>> pq;
    pq.push({0, start});
    dist[start] = 0;

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();
        if (u == end) break;

        for (int v = u + 1; v <= std::min(static_cast<int>(query.size()), u + 2 * kmer_size - 1); ++v) {
            result weight = edgeweight(u, v);
            if (dist[u] + weight.weight < dist[v]) {
                dist[v] = dist[u] + weight.weight;
                in_ref[v] = {weight.ref_start, weight.ref_end};
                prev[v] = u;
                pq.push({-dist[v], v});
            }
        }
    }
    //traverse the path
    int cur = end;
    while (prev[cur] != -1) {
        if (dist[cur] != dist[prev[cur]]) {
            fprintf(dbg_file, "PEN u %d v %d: %d\n", cur, prev[cur], dist[cur] - dist[prev[cur]]);
        }
        if (in_ref[cur].first != 0)
            fprintf(plot_file, "(%d,%d,%d,%d),\n", prev[cur] + 1, cur, in_ref[cur].first, in_ref[cur].second);
        cur = prev[cur];
    }
    return dist[end];
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

    input(ref_file, query_file);
    size_t reference_len = reference.size() - 1, query_len = query.size() - 1;
    initialize(std::max(reference_len, query_len) + 1);
    hash(0, reference_len, reference, hash_ref);
    hash(0, query_len, query, hash_query);
    hash_inv(reference, hash_refrc);
    for (int len = kmer_size; len <= 2 * kmer_size - 1; len++) {
        kmer_hashes[len].clear();
        kmer_hashes_i[len].clear();
        for (int i = 1; i + len - 1 <= static_cast<int>(reference_len); i++) {
            hash_t hash_r = partial_hash(i, i + len - 1, hash_ref);

            hash_t hash_r_inv = partial_hash_inv(i, i + len - 1, reference_len, hash_refrc);
            kmer_hashes[len][hash_r] = i;
            kmer_hashes_i[len][hash_r_inv] = i;
        }
    }
    int min_penalty = dijkstra(0, query_len);
    if (min_penalty == INF) {
        printf("No match found\n");
    } else {
        printf("Minimum penalty: %d\n", min_penalty);
    }
#ifdef DEBUG
    getchar();
#endif
}