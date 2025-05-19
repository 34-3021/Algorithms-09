#include <cstdio>
#include <cstring>
#include <queue>
#include <unordered_map>

using hb = unsigned long long;

const int MAXN = 9005;

int f[MAXN][MAXN];

char s1[MAXN], s2[MAXN];

FILE* plotFile;
FILE* inputFile;
FILE* logFile;
FILE* tableFile;

template <typename T>
T max(T a, T b) {
    return a > b ? a : b;
}

int main() {
    // freopen("week2/cases.txt", "r", stdin);
    // freopen("week2/result.txt", "w", stdout);
    // as plotFile instead of stdout
    //plotFile = fopen("week2/result.txt", "w");
    inputFile = fopen("week2/cases.txt", "r");
    //logFile = fopen("week2/log.txt", "w");

    tableFile = fopen("week2/table.txt", "w");
    //printf("Reference:\n");
    fscanf(inputFile, "%s", s1 + 1);
    //printf("Query:\n");
    fscanf(inputFile, "%s", s2 + 1);
    int n = strlen(s1 + 1), m = strlen(s2 + 1);

    for (int i = 0; i <= n; i++) {
        f[i][0] = -i;
    }
    for (int j = 0; j <= m; j++) {
        f[0][j] = -j;
    }
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            if (s1[i] == s2[j]) {
                f[i][j] = f[i - 1][j - 1] + 1;
            } else {
                f[i][j] = f[i - 1][j - 1] - 1;
            }
            f[i][j] = max(f[i][j], f[i - 1][j] - 1);
            f[i][j] = max(f[i][j], f[i][j - 1] - 1);
        }
    }

    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= m; j++) {
            fprintf(tableFile, "%d,", f[i][j]);
        }
        fprintf(tableFile, "\n");
    }

#ifdef DEBUG
    getchar();
    getchar();
#endif
    return 0;
}