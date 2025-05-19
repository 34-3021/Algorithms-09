#include <cstdio>
#include <cstring>
#include <queue>
#include <unordered_map>

using hb = unsigned long long;

const int MAXN = 7005;

bool f[MAXN][MAXN];
int j_times[MAXN][MAXN];

int imjp[MAXN][MAXN];  // how long can go i- j+ = 1+arr[i+1][j-1]
int ipjp[MAXN][MAXN];  // how long can go i+ j+ = 1+arr[i-1][j-1]

char s1[MAXN], s2[MAXN];

char inv(char c) {
    switch (c) {
        case 'A':
            return 'T';
        case 'T':
            return 'A';
        case 'C':
            return 'G';
        case 'G':
            return 'C';
        default:
            return ' ';
    }
}

void pre(int n, int m) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (s1[i] == s2[j]) {
                ipjp[i][j] = 1 + (i > 0 && j > 0 ? ipjp[i - 1][j - 1] : 0);
            } else {
                ipjp[i][j] = 0;
            }
        }
    }
    for (int i = 0; i < n; i++) {
        for (int j = m - 1; j >= 0; j--) {
            if (s1[i] == inv(s2[j])) {
                imjp[i][j] = 1 + (j < m - 1 && i > 0 ? imjp[i - 1][j + 1] : 0);
            } else {
                imjp[i][j] = 0;
            }
        }
    }
}

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

FILE* plotFile;
FILE* inputFile;
FILE* logFile;

struct q_elem {
    int jinit;  //initiated from
    int j_times;
    int max_j_covered;
};

int main() {
    // freopen("week2/cases.txt", "r", stdin);
    // freopen("week2/result.txt", "w", stdout);
    // as plotFile instead of stdout
    plotFile = fopen("week2/result.txt", "w");
    inputFile = fopen("week2/cases.txt", "r");
    logFile = fopen("week2/log.txt", "w");

    //printf("Reference:\n");
    fscanf(inputFile, "%s", s1);
    //printf("Query:\n");
    fscanf(inputFile, "%s", s2);
    int n = strlen(s1), m = strlen(s2);
    pre(n, m);

    // Main part
    f[0][0] = true;
    j_times[0][0] = 0;
    for (int i = 0; i < n; ++i) {
        int cur_j_to_all_ok = 0;
        int all_ok_from = 0;
        int cur_max_j_ok = 0;
        std::deque<q_elem> q;  // 保存最大可以管到的

        // 如果当前：能管到的范围大于 q.back()：先 pop back 直到 当前的 j_times > 队尾的 j_times ，然后放到队尾
        // 否则 pop front 若当前管到的范围大于 q.front() 且 j_times < 队头的 j_times
        // 然后判断 此时 j_times < 队头的 j_times？若是，

        for (int j = 1; j < m; ++j) {
            //first judge if f[i][j] is ok
            if (j <= cur_j_to_all_ok) {
                if ((!f[i][j]) || (f[i][j] && (j_times[i][j] > j_times[i][j - all_ok_from] + 1))) {
                    fprintf(logFile, "(%d,%d) -> (%d,%d) INV \n", i, all_ok_from, i, j);
                    f[i][j] = true;
                    f_from[i][j] = {i, all_ok_from, true, j - all_ok_from};
                    j_times[i][j] = j_times[i][all_ok_from] + 1;
                }
            }
            if (i == 399)
                fprintf(logFile, "--(%d,%d) with max reverse %d\n", i, j, ipjp[i][j]);

            if (cur_max_j_ok >= j - ipjp[i][j] && f[i][cur_max_j_ok]) {
                if ((!f[i][j]) || (f[i][j] && (j_times[i][j] > j_times[i][cur_max_j_ok] + 1))) {
                    fprintf(logFile, "(%d,%d) -> (%d,%d) POS \n", i, cur_max_j_ok, i, j);
                    f[i][j] = true;
                    f_from[i][j] = {i, cur_max_j_ok, false, j - cur_max_j_ok};
                    j_times[i][j] = j_times[i][cur_max_j_ok] + 1;
                }
            }

            if (f[i][j])
                cur_max_j_ok = j;
            else
                continue;

            if (i == 399)
                fprintf(logFile, "--(%d,%d) with max pos %d\n", i, j + 1, imjp[i][j + 1]);

            int len = imjp[i][j + 1];  //from j+1 to j+imjp[i][j+1] is ok
            if (cur_j_to_all_ok < j + len) {
                cur_j_to_all_ok = j + len;
                all_ok_from = j;
            }
        }
        if (i != n - 1) {
            for (int j = 0; j < m - 1; ++j) {
                if (s1[i + 1] == s2[j + 1] && f[i][j]) {
                    fprintf(logFile, "(%d,%d) -> (%d,%d)\n", i, j, i + 1, j + 1);
                    f[i + 1][j + 1] = true;
                    f_from[i + 1][j + 1] = {i, j, false, 1};
                    j_times[i + 1][j + 1] = j_times[i][j];
                }
            }
        }
    }

    // End Main part

    // DUMP f
    //
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
        if (len <= 7) {
            int i;
            for (i = 0; i < len; i++) {
                pattern[i] = s1[it->first.endIndexInS1 - len + 1 + i];
            }
            for (; i < 7; i++) {
                pattern[i] = ' ';
            }
            pattern[8] = '\0';
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