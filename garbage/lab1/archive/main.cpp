#include <cstdio>
#include <cstring>
#include <unordered_map>

using hb = unsigned long long;

char s1[200005], s2[200005], s3[200005];
hb hash_table1[200005];
hb hash_table2[200005];
hb hash_table3[200005];
hb pow131[200005];

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

void hash(int l, int r, char* s, hb* ht) {
    hb h = 0;
    for (int i = l; i < r; i++) {
        h = h * 131 + s[i];
        ht[i] = h;
    }
}
void hash_rev(int l, int r, char* s, hb* ht) {
    //s3 is REV and REP of s
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
    // ATCGATCG
    // 0      7 len 8
    // REV
    // GCTAGCTA
    // REP
    // CGATCGAT
    // 0      7
    // find partial_hash_rev (1,3) of ATCGATCG
    // is equal to partial_hash (4,6) of CGATCGAT

    return partial_hash(n - r - 1, n - l - 1, htrev);
}

int main() {
    pow131[0] = 1;
    for (int i = 1; i < 200005; i++) {
        pow131[i] = pow131[i - 1] * 131;
    }
    printf("Reference:\n");
    scanf("%s", s1);
    printf("Query:\n");  // *****CBBBCBAAA****
                         // *****CBBBCBBBCBBBCBAAA
    scanf("%s", s2);

    int n = strlen(s1), m = strlen(s2);
    hash(0, n, s1, hash_table1);
    hash(0, m, s2, hash_table2);

    hash_rev(0, n, s1, hash_table3);

    int i = 0, j = 0;
    while (i < n && j < m) {
        if (s1[i] == s2[j]) {
            i++;
            j++;
            continue;
        }
        bool inverted = false;
        int maxl = 0;
        for (int k = 2; j + k - 1 < m; ++k) {  // try s2[i-1-k+1 : i-1] == s2[j : j+k-1]
            if (partial_hash_rev(i - k, i - 1, n, hash_table3) == partial_hash(j, j + k - 1, hash_table2)) {
                maxl = k;
                inverted = true;
                printf("(INV)i: %d j: %d k: %d\n", i, j, k);
            }
            if (partial_hash(i - k, i - 1, hash_table1) == partial_hash(j, j + k - 1, hash_table2)) {
                maxl = k;
                inverted = false;
                printf("i: %d j: %d k: %d\n", i, j, k);
            }
        }
        if (maxl == 0) {
            printf("EXCEPTION at i j: %d %d\n", i, j);
            break;
        }
        Repeat_Entry entry(i - 1, maxl, inverted);
        if (repeatCount.find(entry) == repeatCount.end()) {
            repeatCount[entry] = 1;
        } else {
            repeatCount[entry]++;
        }
        j += maxl;
    }
    printf("OK\n");
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
            pattern[7] = '\0';
        } else {
            int i;
            for (i = 0; i < 2; i++) {
                pattern[i] = s1[it->first.endIndexInS1 - len + 1 + i];
            }
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