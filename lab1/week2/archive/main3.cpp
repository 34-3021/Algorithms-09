#include <cstdio>
#include <cstring>
#include <queue>
#include <unordered_map>

using hb = unsigned long long;

const int MAXN = 9005;

bool f[MAXN][MAXN];
struct from {
    int i, j;
    bool inverted;
    int jl;
} f_from[MAXN][MAXN];

struct to {
    int i, j;
};

struct Repeat_Entry {
    int endIndexInS1;
    int len;
    bool inverted;
    Repeat_Entry(int endIndexInS1, int len, bool inverted) : endIndexInS1(endIndexInS1), len(len), inverted(inverted) {}
};

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

std::queue<to> q;

// starting from 0
// f[i][j] means 0~i in s1 and 0~j in s2 is valid
// f[i][j] to f[i+1][j+1] if s1[i+1]==s2[j+1]
// f[i][j] to f[i][j+n] if s1[i-n+1:i] == s2[j+1:j+n] or invr(s1[i-n+1:i]) == s2[j+1:j+n]

char s1[MAXN], s2[MAXN], s3[MAXN];
hb hash_table1[MAXN];
hb hash_table2[MAXN];
hb hash_table3[MAXN];
hb pow131[MAXN];

void hash(int l, int r, char* s, hb* ht) {
    hb h = 0;
    for (int i = l; i < r; i++) {
        h = h * 131 + s[i];
        ht[i] = h;
    }
}
void hash_rev(int l, int r, char* s, hb* ht) {
    int n = strlen(s);
    for (int i = 0; i < n; i++) {
        s3[i] = s[n - i - 1];
    }
    s3[n] = '\0';
    for (int i = 0; i < n; i++) {
        switch (s3[i]) {
            case 'A':
                s3[i] = 'T';
                break;
            case 'T':
                s3[i] = 'A';
                break;
            case 'C':
                s3[i] = 'G';
                break;
            case 'G':
                s3[i] = 'C';
                break;
        }
    }
    hash(0, n, s3, ht);
}
hb partial_hash(int l, int r, hb* ht) {
    if (l == 0) {
        return ht[r];
    }
    return ht[r] - ht[l - 1] * pow131[r - l + 1];
}
hb partial_hash_rev(int l, int r, int n, hb* htrev) {
    return partial_hash(n - r - 1, n - l - 1, htrev);
}

FILE* plotFile;
FILE* inputFile;
FILE* logFile;

int main() {
    // freopen("week2/cases.txt", "r", stdin);
    //freopen("week2/result.txt", "w", stdout);
    // as plotFile instead of stdout
    plotFile = fopen("week2/result.txt", "w");
    inputFile = fopen("week2/cases.txt", "r");
    logFile = fopen("week2/log.txt", "w");
    pow131[0] = 1;
    for (int i = 1; i < MAXN; i++) {
        pow131[i] = pow131[i - 1] * 131;
    }
    //printf("Reference:\n");
    fscanf(inputFile, "%s", s1);
    //printf("Query:\n");
    fscanf(inputFile, "%s", s2);
    int n = strlen(s1), m = strlen(s2);
    hash(0, n, s1, hash_table1);
    hash(0, m, s2, hash_table2);
    hash_rev(0, n, s1, hash_table3);

    f[0][0] = true;
    f[1][1] = true;
    f_from[1][1] = {0, 0, false, 1};

    // starting from 0
    // f[i][j] means 0~i in s1 and 0~j in s2 is valid
    // f[i][j] to f[i+1][j+1] if s1[i+1]==s2[j+1]
    // f[i][j] to f[i][j+n] if s1[i-n+1:i] == s2[j+1:j+n] or invr(s1[i-n+1:i]) == s2[j+1:j+n]
    for (int i = 1; i < n - 1; i++) {
        //for each i, find possible j
        for (int j = i; j < m - 1;) {
            if (f[i][j]) {
                int k = 2;
                while (j + k < m) {
                    if (partial_hash(i - k + 1, i, hash_table1) == partial_hash(j + 1, j + k, hash_table2)) {
                        if (f[i][j + k] == true && f_from[i][j + k].jl <= k) {
                            f_from[i][j + k] = {i, j, false, k};
                        }
                        if (f[i][j + k] == false) {
                            f[i][j + k] = true;
                            f_from[i][j + k] = {i, j, false, k};
                            q.push({i, j + k});
                        }
                        fprintf(logFile, "(%d,%d) -> (%d,%d)\n", i, j, i, j + k);
                    }
                    if (partial_hash_rev(i - k + 1, i, n, hash_table3) == partial_hash(j + 1, j + k, hash_table2)) {
                        if (f[i][j + k] == true && f_from[i][j + k].jl <= k) {
                            f_from[i][j + k] = {i, j, true, k};
                        }
                        if (f[i][j + k] == false) {
                            f[i][j + k] = true;
                            f_from[i][j + k] = {i, j, true, k};
                            q.push({i, j + k});
                        }
                        fprintf(logFile, "(%d,%d) -> (%d,%d)\n", i, j, i, j + k);
                    }
                    if (f[i][j + k]) {
                        j = j + k;
                        break;
                    }
                    k++;
                }
                if (j + k >= m) {
                    j++;
                }
            } else {
                j++;
            }
        }

        for (int j = i; j < m - 1; j++) {
            if (f[i][j]) {  // f[i][j] to f[i+1][j+1] if s1[i+1]==s2[j+1]
                if (s1[i + 1] == s2[j + 1]) {
                    f[i + 1][j + 1] = true;
                    f_from[i + 1][j + 1] = {i, j, false, 1};
                    //fprintf(logFile, "(%d,%d) -> (%d,%d)\n", i, j, i + 1, j + 1);
                }
            }
        }
    }

    //dump f to f.txt

    FILE* fFile = fopen("week2/f.txt", "w");

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (f[i][j]) {
                fprintf(fFile, "%d,%d\n", i, j);
            }
        }
    }

    if (f[n - 1][m - 1] == false) {
        printf("Not Reachable\n");
#ifdef DEBUG
        getchar();
        getchar();
#endif
        return 0;
    }
    //printf("OK\n");
    //printf("ROUTE TRACE:\n");
    int i = n - 1, j = m - 1;
    fprintf(plotFile, "%d, %d, 0\n", i, j);
    while (i > 0 || j > 0) {
        from next = f_from[i][j];
        fprintf(logFile, "(%d,%d) <- (%d,%d)\n", i, j, next.i, next.j);
        if (next.i != i - 1 || next.j != j - 1) {
            Repeat_Entry entry(i, j - next.j, next.inverted);
            if (repeatCount.find(entry) == repeatCount.end()) {
                repeatCount[entry] = 1;
            } else {
                repeatCount[entry]++;
            }
        }
        i = next.i;
        j = next.j;
        fprintf(plotFile, "%d, %d, %d\n", i, j, next.inverted);
    }

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
        char pattern[10];
        int len = it->first.len;
        if (len <= 6) {
            int i;
            for (i = 0; i < len; i++) {
                pattern[i] = s1[it->first.endIndexInS1 - len + 1 + i];
            }
            for (; i < 6; i++) {
                pattern[i] = ' ';
            }
            pattern[7] = '\0';
        } else {
            int i;
            for (i = 0; i < 2; i++) {
                pattern[i] = s1[it->first.endIndexInS1 - len + 1 + i];
            }
            pattern[i++] = '.';
            pattern[i++] = '.';
            pattern[i++] = '.';
            pattern[i++] = s1[it->first.endIndexInS1];
            pattern[i++] = s1[it->first.endIndexInS1 - 1];
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