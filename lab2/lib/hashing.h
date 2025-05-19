
#include <cstdio>
#include <vector>

typedef unsigned long long hash_t;
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
void input(FILE* ref_file, FILE* query_file) {
    reference.clear();
    query.clear();
    char c;
    reference.push_back('A');
    query.push_back('A');
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
}
void hash(int l, int r, std::vector<char>& s, std::vector<hash_t>& ht) {
    hash_t h = 0;
    for (int i = l; i <= r; i++) {
        h = h * 131 + s[i];
        ht[i] = h;
    }
}
void hash_inv(std::vector<char>& s, std::vector<hash_t>& ht) {
    int n = s.size();
    reference_rc.resize(n + 2);
    for (int i = 0; i < n; i++) {
        reference_rc[i] = s[n - i - 1];
    }
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
hash_t partial_hash(int l, int r, std::vector<hash_t>& ht) {
    if (l == 0) {
        return ht[r];
    } else {
        return ht[r] - ht[l - 1] * pow131[r - l + 1];
    }
}
hash_t partial_hash_inv(int l, int r, int n, std::vector<hash_t>& htinv) {
    return partial_hash(n - r, n - l, htinv);
}