#include <cstdio>
#include <vector>
#include <queue>
#include <functional>
#include <limits>
#include <algorithm>
using namespace std;

using ull = unsigned long long;
const ull INF = numeric_limits<ull>::max();

ull add(ull a, ull b) {
    return a == INF || b == INF || a > INF - b ? INF : a + b;
}

class Input {
    char buf[1 << 16];
    size_t pos = 0, len = 0;

    int get() {
        if (pos == len) {
            len = fread(buf, 1, sizeof(buf), stdin);
            pos = 0;
            if (!len) return EOF;
        }
        return buf[pos++];
    }

public:
    ull read() {
        int c;
        do {
            c = get();
        } while (c <= 32 && c != EOF);

        ull x = 0;
        while (c > 32 && c != EOF) {
            x = x * 10 + c - '0';
            c = get();
        }
        return x;
    }
};

struct Arc {
    ull w;
    int to, next;
};

int main() {
    Input in;
    int n = (int)in.read(), m = (int)in.read();
    int s = (int)in.read() - 1, t = (int)in.read() - 1;
    int u = (int)in.read() - 1, v = (int)in.read() - 1;

    vector<int> head(n, -1);
    vector<Arc> edges;
    edges.reserve(2 * m);

    auto insert = [&](int a, int b, ull w) {
        edges.push_back({w, b, head[a]});
        head[a] = (int)edges.size() - 1;
    };

    for (int i = 0; i < m; ++i) {
        int a = (int)in.read() - 1, b = (int)in.read() - 1;
        ull w = in.read();
        insert(a, b, w);
        insert(b, a, w);
    }

    vector<int> order;
    order.reserve(n);

    auto dijkstra = [&](int source, bool save) {
        vector<ull> d(n, INF);
        vector<unsigned char> settled(n, 0);
        using State = pair<ull, int>;
        priority_queue<State, vector<State>, greater<State>> q;

        d[source] = 0;
        q.push({0, source});

        while (!q.empty()) {
            auto [dist, a] = q.top();
            q.pop();
            if (dist != d[a]) continue;

            settled[a] = 1;
            if (save) order.push_back(a);

            for (int i = head[a]; i != -1; i = edges[i].next) {
                const auto &e = edges[i];
                ull nd = add(dist, e.w);

                if (nd < d[e.to]) {
                    d[e.to] = nd;
                    q.push({nd, e.to});

                    // Acota la cola incluso con muchas aristas paralelas.
                    if (q.size() > 2 * static_cast<size_t>(n)) {
                        vector<State> fresh;
                        fresh.reserve(n);
                        for (int b = 0; b < n; ++b) {
                            if (!settled[b] && d[b] != INF)
                                fresh.push_back({d[b], b});
                        }
                        q = decltype(q)(greater<State>(), move(fresh));
                    }
                }
            }
        }
        return d;
    };

    auto du = dijkstra(u, false);
    if (du[v] == INF) {
        puts("-1");
        return 0;
    }

    auto ds = dijkstra(s, true);
    if (ds[t] == INF) {
        printf("%llu\n", du[v]);
        return 0;
    }

    auto dt = dijkstra(t, false);
    auto dv = dijkstra(v, false);
    vector<ull> best = du;
    ull answer = du[v];

    for (int a : order) {
        if (add(ds[a], dt[a]) != ds[t]) continue;

        answer = min(answer, add(best[a], dv[a]));

        for (int i = head[a]; i != -1; i = edges[i].next) {
            const auto &e = edges[i];
            if (add(ds[a], e.w) == ds[e.to] &&
                add(ds[e.to], dt[e.to]) == ds[t]) {
                best[e.to] = min(best[e.to], best[a]);
            }
        }
    }

    printf("%llu\n", answer);
}