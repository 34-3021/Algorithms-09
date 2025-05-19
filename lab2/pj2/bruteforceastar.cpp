#include <cstdio>
#include <cstring>
#include <algorithm>
#include <vector>
#include <queue>

using hb = unsigned long long;
const int MAXN = 30015;

char reference[MAXN], query[MAXN], reference_rc[MAXN];
int reference_len, query_len;
hb hash_ref[MAXN], hash_query[MAXN], hash_refrc[MAXN];
hb pow131[MAXN];

int* f[MAXN];
bool* vis[MAXN];

struct from {
    int i, j, ri;
};

from* f_from[MAXN];

struct heap_ele {
    int i, j, score;
    bool operator<(const heap_ele& other) const {
        return score < other.score;
    }
};

std::priority_queue<heap_ele> pq;

const int RATE_MAT[5][5] = {
    {5, -2, -4, -2, -6},
    {-2, 5, -6, -4, -8},
    {-4, -6, 5, -4, -4},
    {-2, -4, -4, 5, -2},
    {-6, -8, -4, -2, 0}};

const int WORST_GAP_MATCH = -4;
const int WORST_MISMATCH = -4;
const int BEST_MATCH = 5;

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
    return RATE_MAT[static_cast<int>(a)][static_cast<int>(b)];
}
void hash(int l, int r, char* s, hb* ht) {
    hb h = 0;
    ht[0] = 0;
    for (int i = l; i < r; i++) {
        h = h * 131 + s[i];
        ht[i + 1] = h;
    }
}
void hash_inv(int l, int r, char* s, hb* ht) {
    int n = strlen(s);
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

inline hb partial_hash(int l, int r, hb* ht) {
    return ht[r + 1] - ht[l] * pow131[r - l + 1];
}
inline hb partial_hash_inv(int l, int r, int n, hb* htinv) {
    return partial_hash(n - r - 1, n - l - 1, htinv);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s <reference_file> <query_file>\n", argv[0]);
        return 1;
    }

    FILE* referenceFile = fopen(argv[1], "r");
    FILE* queryFile = fopen(argv[2], "r");
    if (!referenceFile || !queryFile) {
        printf("Error opening files.\n");
        return 1;
    }
    FILE* answerFile = fopen("result.txt", "w");
    FILE* logFile = fopen("log.txt", "w");
    FILE* plotFile = fopen("plot.txt", "w");

    pow131[0] = 1;
    for (int i = 1; i < MAXN; i++) {
        pow131[i] = pow131[i - 1] * 131;
    }

    fscanf(referenceFile, "%s", reference);
    fscanf(queryFile, "%s", query);

    reference_len = strlen(reference);
    query_len = strlen(query);

    printf("Allocating memory...\n");
    //initialize f, f_from, vis
    for (int i = 0; i < reference_len + 5; i++) {
        f[i] = new int[query_len + 5];
        f_from[i] = new from[query_len + 5];
        vis[i] = new bool[query_len + 5];
        for (int j = 0; j < query_len + 5; j++) {
            f[i][j] = -1e9;
            vis[i][j] = false;
        }
    }
    printf("Memory allocated.\n");

    printf("Doing preprocessing... of length %d and %d\n", reference_len, query_len);
    hash(0, reference_len, reference, hash_ref);
    hash(0, query_len, query, hash_query);
    hash_inv(0, reference_len, reference, hash_refrc);
    printf("Preprocessing done.\n");

    int estimated_worst_ans = std::abs(reference_len - query_len) * WORST_GAP_MATCH +
                              std::min(reference_len, query_len) * WORST_MISMATCH;
    f[0][0] = 0;
    printf("Initializing f\n");
    for (int i = 0; i < reference_len; i++) {
        f[i + 1][0] = rate(reference[i], ' ');
    }
    for (int j = 0; j < query_len; j++) {
        f[0][j + 1] = rate(' ', query[j]);
    }
    printf("Initialization done.\n");
    pq.push({0, 0, 0});
    int cnt = 0;
    while (!pq.empty()) {
        cnt++;
        heap_ele cur = pq.top();
        pq.pop();
        if (vis[cur.i][cur.j]) {
            continue;
        }
        vis[cur.i][cur.j] = true;
        int i = cur.i, j = cur.j;
        if (cnt % 100000 == 0) {
            printf("Processing %d,%d with score %d\n", i, j, cur.score);
        }
        // if (true) {
        //     fprintf(logFile, "Processing %d,%d with score %d\n", i, j, cur.score);
        //     fflush(logFile);
        // }
        if (i == reference_len && j == query_len) {
            printf("Found the answer!\n");
            break;
        }
        //to i+1, j+1
        //f[i + 1][j + 1] = std::max(f[i + 1][j + 1], f[i][j] + rate(reference[i], query[j]));
        int as1;
        if (i < reference_len && j < query_len) {
            as1 = f[i][j] + rate(reference[i], query[j]);
            if (as1 > f[i + 1][j + 1]) {
                f[i + 1][j + 1] = as1;
                f_from[i + 1][j + 1].i = i;
                f_from[i + 1][j + 1].j = j;
                f_from[i + 1][j + 1].ri = -1;
                pq.push({i + 1, j + 1, as1});
            }
        }
        //to i+1, j
        //f[i + 1][j] = std::max(f[i + 1][j], f[i][j] + rate(reference[i], ' '));
        if (i < reference_len && j <= query_len) {
            as1 = f[i][j] + rate(reference[i], ' ');
            if (as1 > f[i + 1][j]) {
                f[i + 1][j] = as1;
                f_from[i + 1][j].i = i;
                f_from[i + 1][j].j = j;
                f_from[i + 1][j].ri = -1;
                pq.push({i + 1, j, as1});
            }
        }
        //to i, j+1
        //f[i][j + 1] = std::max(f[i][j + 1], f[i][j] + rate(' ', query[j]));
        if (i <= reference_len && j < query_len) {
            as1 = f[i][j] + rate(' ', query[j]);
            if (as1 > f[i][j + 1]) {
                f[i][j + 1] = as1;
                f_from[i][j + 1].i = i;
                f_from[i][j + 1].j = j;
                f_from[i][j + 1].ri = -1;
                pq.push({i, j + 1, as1});
            }
        }
        //to i, j+k (ref[i-k...i-1]=query[j,j+k-1] or inverted)
        for (int k = 1; k <= query_len - j && k <= i; k++) {
            [[unlikely]]
            if (partial_hash(i - k, i - 1, hash_ref) == partial_hash(j, j + k - 1, hash_query)) {
                as1 = f[i][j] + k - 15;
                if (as1 > f[i][j + k]) {
                    f[i][j + k] = as1;
                    f_from[i][j + k].i = i;
                    f_from[i][j + k].j = j;
                    f_from[i][j + k].ri = i - k;
                    pq.push({i, j + k, as1});
                }
            }
            [[unlikely]]
            if (partial_hash_inv(i - k, i - 1, reference_len, hash_refrc) == partial_hash(j, j + k - 1, hash_query)) {
                as1 = f[i][j] + k - 15;
                if (as1 > f[i][j + k]) {
                    f[i][j + k] = as1;
                    f_from[i][j + k].i = i;
                    f_from[i][j + k].j = j;
                    f_from[i][j + k].ri = i - k;
                    pq.push({i, j + k, as1});
                }
            }
        }
    }

    int cur_segment_end_i = reference_len - 1, cur_segment_end_j = query_len - 1;
    int last_i = reference_len - 1, last_j = query_len - 1;
    int i = reference_len, j = query_len;
    bool flag = false;
    while (i > 0 || j > 0) {
        if (f_from[i][j].ri == -1) {
            last_i = i;
            last_j = j;
            i = f_from[i][j].i;
            j = f_from[i][j].j;
            flag = true;
            continue;
        }
        if (flag && cur_segment_end_i != -1 && cur_segment_end_j != -1) fprintf(answerFile, "(%d,%d,%d,%d),", last_j - 1, cur_segment_end_j, last_i - 1, cur_segment_end_i);
        if (flag && cur_segment_end_i != -1 && cur_segment_end_j != -1) fprintf(plotFile, "%d,%d,%d,%d,DIR\n", last_j - 1, cur_segment_end_j, last_i - 1, cur_segment_end_i);
        flag = false;
        if (i != 0 && j != 0) fprintf(answerFile, "(%d,%d,%d,%d),", f_from[i][j].j, j - 1, f_from[i][j].ri, i - 1);
        if (i != 0 && j != 0) fprintf(plotFile, "%d,%d,%d,%d,REP\n", f_from[i][j].j, j - 1, f_from[i][j].ri, i - 1);
        last_i = i;
        last_j = j;
        i = f_from[i][j].i;
        j = f_from[i][j].j;
        cur_segment_end_i = i - 1;
        cur_segment_end_j = j - 1;
    }
    if (flag && cur_segment_end_i != -1 && cur_segment_end_j != -1) fprintf(answerFile, "(%d,%d,%d,%d),", 0, cur_segment_end_j, 0, cur_segment_end_i);
    if (flag && cur_segment_end_i != -1 && cur_segment_end_j != -1) fprintf(plotFile, "%d,%d,%d,%d,DIR\n", 0, cur_segment_end_j, 0, cur_segment_end_i);
    // remove the last comma
    fseek(answerFile, -1, SEEK_END);
    return 0;
}