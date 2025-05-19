#include <unordered_map>

struct coordinate {
    int x, y;
};

struct hash_fn {
    std::size_t operator()(const coordinate& coord) const {
        return (std::hash<int>()(coord.x * 17) * 17) ^ std::hash<int>()(coord.y);
    }
};

struct equal_fn {
    bool operator()(const coordinate& lhs, const coordinate& rhs) const {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }
};

struct match {
    int i_start, i_end;
    int j_start, j_end;
    bool vis;
};