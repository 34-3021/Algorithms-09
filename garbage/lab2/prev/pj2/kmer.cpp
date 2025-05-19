#include <cstdio>
#include <cstring>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include "kmer.h"
#include "hashing.h"

const int kmer_size = 30;

std::unordered_map<coordinate, match*, hash_fn, equal_fn> matches_start, matches_end;
std::unordered_map<coordinate, match*, hash_fn, equal_fn> matches_i_start, matches_i_end;
std::unordered_map<hash_t, std::vector<int>> kmer_hashes, kmer_hashes_i;

std::unordered_map<coordinate, match*, hash_fn, equal_fn> m_start, m_end;
std::unordered_map<coordinate, match*, hash_fn, equal_fn> m_i_start, m_i_end;

FILE* ref_file;
FILE* query_file;
FILE* plot_file;
int main(int argc, char* argv[]) {
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

    input(ref_file, query_file);
    size_t reference_len = reference.size() - 1, query_len = query.size() - 1;
    initialize(std::max(reference_len, query_len) + 1);
    hash(0, reference_len, reference, hash_ref);
    hash(0, query_len, query, hash_query);
    hash_inv(reference, hash_refrc);
    for (int i = 1; i + kmer_size - 1 <= reference_len; i++) {
        hash_t hash_r = partial_hash(i, i + kmer_size - 1, hash_ref);
        hash_t hash_r_inv = partial_hash_inv(i, i + kmer_size - 1, reference_len, hash_refrc);
        kmer_hashes[hash_r].push_back(i);
        kmer_hashes_i[hash_r_inv].push_back(i);
    }
    for (int j = 1; j + kmer_size - 1 <= query_len; j++) {
        hash_t hash_q = partial_hash(j, j + kmer_size - 1, hash_query);
        auto it = kmer_hashes.find(hash_q);
        if (it != kmer_hashes.end()) {
            for (int i : it->second) {
                match* m = new match{i, i + kmer_size - 1, j, j + kmer_size - 1, false};
                matches_start[{i, j}] = m;
                matches_end[{i + kmer_size - 1, j + kmer_size - 1}] = m;
            }
        }
        auto it_i = kmer_hashes_i.find(hash_q);
        if (it_i != kmer_hashes_i.end()) {
            for (int i : it_i->second) {
                match* m = new match{i, i + kmer_size - 1, j, j + kmer_size - 1, false};
                matches_i_start[{i, j}] = m;
                matches_i_end[{i + kmer_size - 1, j + kmer_size - 1}] = m;
            }
        }
    }
    for (auto& [coord, m] : matches_start) {
        if (m->vis) continue;
        m->vis = true;
        int curstart_i = m->i_start, curstart_j = m->j_start;
        int curend_i = m->i_end, curend_j = m->j_end;
        for (int bke = 0; curend_i + bke <= reference_len && curend_j + bke <= query_len; bke++) {
            auto it = matches_end.find({curend_i + bke, curend_j + bke});
            if (it != matches_end.end()) {
                match* m2 = it->second;
                if (!m2->vis) {
                    m2->vis = true;
                    curend_i = m2->i_end;
                    curend_j = m2->j_end;
                }
            } else {
                break;
            }
        }
        for (int fwe = 0; curstart_i - fwe >= 1 && curstart_j - fwe >= 1; fwe++) {
            auto it = matches_i_start.find({curstart_i - fwe, curstart_j - fwe});
            if (it != matches_i_start.end()) {
                match* m2 = it->second;
                if (!m2->vis) {
                    m2->vis = true;
                    curstart_i = m2->i_start;
                    curstart_j = m2->j_start;
                }
            } else {
                break;
            }
        }
        match* full_match = new match{curstart_i, curend_i, curstart_j, curend_j, true};
        m_start[{curstart_i, curstart_j}] = full_match;
        m_end[{curend_i, curend_j}] = full_match;
    }
    for (auto& [coord, m] : matches_i_start) {
        if (m->vis) continue;
        m->vis = true;
        int curstart_i = m->i_start, curstart_j = m->j_start;
        int curend_i = m->i_end, curend_j = m->j_end;
        for (int bke = 0; curstart_i - bke >= 1 && curend_j + bke <= query_len; bke++) {
            auto it = matches_i_end.find({curend_i - bke, curend_j + bke});
            if (it != matches_i_end.end()) {
                match* m2 = it->second;
                if (!m2->vis) {
                    m2->vis = true;
                    curend_i = m2->i_end;
                    curend_j = m2->j_end;
                }
            } else {
                break;
            }
        }
        for (int fwe = 0; curend_i + fwe <= reference_len && curstart_j - fwe >= 1; fwe++) {
            auto it = matches_start.find({curstart_i + fwe, curstart_j - fwe});
            if (it != matches_start.end()) {
                match* m2 = it->second;
                if (!m2->vis) {
                    m2->vis = true;
                    curstart_i = m2->i_start;
                    curstart_j = m2->j_start;
                }
            } else {
                break;
            }
        }
        match* full_match = new match{curstart_i, curend_i, curstart_j, curend_j, true};
        m_i_start[{curstart_i, curstart_j}] = full_match;
        m_i_end[{curend_i, curend_j}] = full_match;
    }
    for (auto& [coord, m] : m_start) {
        m->vis = true;
        fprintf(plot_file, "%d,%d,%d,%d\n", m->j_start, m->j_end, m->i_start, m->i_end);
    }
    for (auto& [coord, m] : m_i_start) {
        m->vis = true;
        fprintf(plot_file, "%d,%d,%d,%d\n", m->j_start, m->j_end, m->i_end, m->i_start);
    }
}