#include <cstdio>
#include <unordered_map>
#include <algorithm>
#include <cstring>

using hb = unsigned long long;
const int MAXN = 30015;

char reference[MAXN], query[MAXN], reference_rc[MAXN];
int reference_len, query_len;
hb hash_ref[MAXN], hash_query[MAXN], hash_refrc[MAXN];
hb pow131[MAXN];

int f[MAXN];
struct Repeat_Entry {
    int endIndexInS1;
    int len;
    bool inverted;
    Repeat_Entry(int endIndexInS1, int len, bool inverted) : endIndexInS1(endIndexInS1), len(len), inverted(inverted) {}
};

template <typename T>
T min(T a, T b) {
    return a < b ? a : b;
}

struct hash_fn {
    std::size_t operator()(const Repeat_Entry& entry) const {
        return entry.endIndexInS1 * 1000000 + entry.len * 2 + entry.inverted;
    }
};
struct equal_fn {
    bool operator()(const Repeat_Entry& lhs, const Repeat_Entry& rhs) const {
        return lhs.endIndexInS1 == rhs.endIndexInS1 && lhs.len == rhs.len && lhs.inverted == rhs.inverted;
    }
};

std::unordered_map<Repeat_Entry, int, hash_fn, equal_fn> repeatCount;

void hash(int l, int r, char* s, hb* ht) {
    hb h = 0;
    for (int i = l; i < r; i++) {
        h = h * 131 + s[i];
        ht[i] = h;
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
hb partial_hash(int l, int r, hb* ht) {
    if (l == 0) {
        return ht[r];
    }
    return ht[r] - ht[l - 1] * pow131[r - l + 1];
}
hb partial_hash_inv(int l, int r, int n, hb* htinv) {
    return partial_hash(n - r - 1, n - l - 1, htinv);
}

FILE* plotFile;
FILE* inputFile;
FILE* logFile;
FILE* ansFile;

struct segment_entry {
    int begin;
    bool inverted;
};
struct from {
    int j;
    int pos_of_reference;
    bool inverted;
} f_from[MAXN];

std::unordered_map<hb, segment_entry> substrings[1005];

int main() {
    plotFile = fopen("week2/result.txt", "w");
    inputFile = fopen("week2/cases2.txt", "r");
    logFile = fopen("week2/log.txt", "w");
    ansFile = fopen("week2/ans.txt", "w");
    pow131[0] = 1;
    for (int i = 1; i < MAXN; i++) {
        pow131[i] = pow131[i - 1] * 131;
    }
    //printf("Reference:\n");
    fscanf(inputFile, "%s", reference);
    //printf("Query:\n");
    fscanf(inputFile, "%s", query);
    reference_len = strlen(reference), query_len = strlen(query);
    hash(0, reference_len, reference, hash_ref);
    hash(0, query_len, query, hash_query);
    hash_inv(0, reference_len, reference, hash_refrc);

    printf("Evaluating substrings...\n");
    for (int i = 0; i < reference_len; i++) {
        // for (int j = i; j < min(reference_len, i + 1000); j++) {
        {
            int j = min(i + 29, reference_len - 1);
            hb h1 = partial_hash(i, j, hash_ref);
            hb h2 = partial_hash_inv(i, j, reference_len, hash_refrc);
            if (!substrings[j - i + 1].contains(h1)) {
                substrings[j - i + 1][h1] = {i, false};
            }
            if (!substrings[j - i + 1].contains(h2)) {
                substrings[j - i + 1][h2] = {i, true};
            }
        }
        printf("\rEvaluating substrings... %d%%", (i + 1) * 100 / reference_len);
    }
    printf("\rEvaluating substrings... 100%%\n");
    printf("Substrings evaluated.\n");

    memset(f, 0x3f, sizeof(f));

    f[query_len] = 0;

    printf("Finding matches...\n");
    for (int i = query_len - 1; i >= 0; i--) {
        for (int j = i; j < min(query_len, i + 1000); j++) {
            hb h = partial_hash(i, j, hash_query);
            if (substrings[j - i + 1].contains(h)) {
                int jump_from = j + 1;
                if (f[i] > 1 + f[jump_from]) {
                    f[i] = 1 + f[jump_from];
                    int begin = substrings[j - i + 1][h].begin;
                    bool inverted = substrings[j - i + 1][h].inverted;
                    f_from[i] = {jump_from, begin, inverted};
                }
            }
        }
        printf("\rFinding matches... %d%%", (query_len - i) * 100 / query_len);
    }
    if (f[0] == 0x3f3f3f3f) {
        printf("Not Reachable\n");
#ifdef DEBUG
        getchar();
        getchar();
#endif
        return 0;
    }

    int j = 0;
    //fprintf(plotFile, "%d, %d, %d\n", 0, j, 0);
    while (j < query_len) {
        int nxt = f_from[j].j;
        Repeat_Entry entry(f_from[j].pos_of_reference + nxt - j - 1, nxt - j, f_from[j].inverted);
        if (!f_from[j].inverted)
            fprintf(plotFile, "%d, %d, %d, %d\n", f_from[j].pos_of_reference, j, f_from[j].pos_of_reference + nxt - j - 1, nxt - 1);
        else
            fprintf(plotFile, "%d, %d, %d, %d\n", f_from[j].pos_of_reference + nxt - j - 1, j, f_from[j].pos_of_reference, nxt - 1);
        fprintf(ansFile, "(%d, %d, %d, %d), ", j, nxt - 1, f_from[j].pos_of_reference, f_from[j].pos_of_reference + nxt - j - 1);
        //fprintf(plotFile, "%d, %d, %d\n", f_from[j].pos_of_reference, nxt, f_from[j].inverted);
        if (!repeatCount.contains(entry)) {
            repeatCount[entry] = 1;
        } else {
            repeatCount[entry]++;
        }
        j = f_from[j].j;
    }

    //fprintf(plotFile, "%d, %d, %d\n", i, j, next.inverted);

    printf("|");
    for (int i = 0; i < 50; i++) {
        printf("-");
    }
    printf("|\n|");
    for (int i = 0; i < 20; i++) {
        printf(" ");
    }
    printf("Statistics");
    for (int i = 0; i < 20; i++) {
        printf(" ");
    }
    printf("|\n|");
    for (int i = 0; i < 50; i++) {
        printf("-");
    }
    printf("|\n");
    printf("| Pos in Ref | Pattern | Length | Inverted | Count |\n");
    printf("|");
    for (int i = 0; i < 50; i++) {
        printf("-");
    }
    printf("|\n");
    for (auto it = repeatCount.begin(); it != repeatCount.end(); it++) {
        //if (it->second == 1) continue;
        char pattern[10];
        int len = it->first.len;
        if (len <= 7) {
            int i;
            for (i = 0; i < len; i++) {
                pattern[i] = reference[it->first.endIndexInS1 - len + 1 + i];
            }
            for (; i < 7; i++) {
                pattern[i] = ' ';
            }
            pattern[8] = '\0';
        } else {
            int i;
            for (i = 0; i < 2; i++) {
                pattern[i] = reference[it->first.endIndexInS1 - len + 1 + i];
            }
            pattern[i++] = '.';
            pattern[i++] = '.';
            pattern[i++] = '.';
            pattern[i++] = reference[it->first.endIndexInS1];
            pattern[i++] = reference[it->first.endIndexInS1 - 1];
            pattern[i] = '\0';
        }
        printf("| %3d - %4d | %s | %6d | %8s | %5d |\n", it->first.endIndexInS1 - it->first.len + 1, it->first.endIndexInS1, pattern, it->first.len, it->first.inverted ? "True" : "False", it->second);
    }
    printf("|");
    for (int i = 0; i < 50; i++) {
        printf("-");
    }
    printf("|\n");
#ifdef DEBUG
    getchar();
    getchar();
#endif
    return 0;
}