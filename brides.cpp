#include <iostream>
#include <vector>
#include <cassert>
#include <set>
#include <algorithm>
#include <tuple>

using std::vector;
using std::min;
using std::pair;
using std::set;
using std::tie;
using std::cin;
using std::cout;

static constexpr int INF = std::numeric_limits<int>::max() / 2;

struct Edge {
    int flow = 0;
    int capacity = 0;
    int from = -1;
    int to = -1;
    int weight = INF;
    int id = -1;

    Edge() = default;

    Edge(int fl, int c, int fr, int t, int w = 0, int i = -1) : flow(fl), capacity(c), from(fr), to(t), weight(w),
                                                                id(i) {}

    Edge &operator++() {
        ++flow;
        assert(flow <= capacity);
        return *this;
    }


    Edge &operator--() {
        --flow;
        return *this;
    }


    Edge &operator+=(int fl) {
        flow += fl;
        assert(flow <= capacity);
        return *this;
    }

    Edge &operator-=(int fl) {
        flow -= fl;
        return *this;
    }
};


struct Graph {
    int n = 0, m = 0;
    vector<vector<int>> gr;
    vector<Edge> edges;

    Graph() = default;

    Graph(int n_, int m_) : n(n_), m(m_) {
        gr.resize(n);
        edges.reserve(m);
    }

    void resize(int n_) {
        n = n_;
        gr.resize(n);
    }

    void clear() {
        gr.clear();
        edges.clear();
    }

    void add_edge(int u, const Edge &e) {
        gr[u].push_back(edges.size());
        edges.push_back(e);
        ++m;
    }
};

int k;
int cc = 0;
long double sum = 0;
Graph graph;
vector<int> potential;
vector<int> parents; // number of edge
vector<int> used;
vector<int> way;

void set_potentials(int start) {
    potential.assign(graph.n, INF);
    potential[start] = 0;
    bool was_update = true;
    while (was_update) {
        was_update = false;
        for (const auto &e : graph.edges) {
            assert(e.flow <= e.capacity);
            if (e.flow == e.capacity) {
                continue;
            }
            if (potential[e.from] + e.weight < potential[e.to]) {
                was_update = true;
                potential[e.to] = potential[e.from] + e.weight;
            }
        }
    }
}

// weight of edge e: u -> v := e.weight + (potential[u] - potential[v])
bool find_and_push_flow(int start, int finish) {
    /*cerr << "pot: ";
    for (int i = 0; i < n; ++i) {
        cerr << potential[i] << " ";
    }
    cerr << "\n";*/

    parents.assign(graph.n, -1);
    vector<int> dist(graph.n, INF);

    dist[start] = 0;
    set<pair<int, int>> s;
    s.insert({dist[start], start});

    while (!s.empty()) {
        int u, d;
        tie(d, u) = *s.begin();
        s.erase(s.begin());

        for (int index : graph.gr[u]) {
            Edge e = graph.edges[index];
            assert(e.flow <= e.capacity);
            if (e.flow == e.capacity) {
                continue;
            }
            int v = e.to;
            if (dist[v] > dist[u] + e.weight + (potential[u] - potential[v])) {
                assert(e.weight + (potential[u] - potential[v]) >= 0);
                s.erase({dist[v], v});
                dist[v] = dist[u] + e.weight + (potential[u] - potential[v]);
                s.insert({dist[v], v});
                parents[v] = index;
            }
        }
    }
    if (dist[finish] == INF) {
        return false;
    }

    vector<int> way;
    int cur = finish;
    while (parents[cur] != -1) {
        way.push_back(parents[cur]);
        cur = graph.edges[parents[cur]].from;
    }
    reverse(way.begin(), way.end());

    for (auto index : way) {
        ++graph.edges[index];
        --graph.edges[index ^ 1];
    }
    for (int i = 0; i < graph.n; ++i) {
        potential[i] += dist[i];
    }
    return true;
}

bool dfs(int v, int par, int finish) {
    used[v] = cc;
    if (v == finish) {
        return true;
    }
    for (const auto &e : graph.gr[v]) {
        int to = graph.edges[e].to;
        if (to == par) {
            continue;
        }
        if (graph.edges[e].flow > 0 && (used[to] != cc && dfs(to, v, finish))) {
            --graph.edges[e];
            ++graph.edges[e ^ 1];
            way.push_back(graph.edges[e].id);
            sum += graph.edges[e].weight;
            return true;
        }
    }
    return false;
}


int main() {
    int n, m;
    cout.precision(30);
    cin >> n >> m >> k;

    graph.resize(n);
    graph.edges.reserve(4 * m);
    potential.resize(n);
    parents.resize(n);
    used.resize(n, -1);

    for (int i = 0; i < m; ++i) {
        int u, v, w;
        cin >> u >> v >> w;
        u--, v--;

        Edge u_v(0, 1, u, v, w, i + 1);   // u -> v
        Edge u_v_(0, 0, v, u, -w, -1); // back for u -> v
        Edge v_u(0, 1, v, u, w, i + 1);   // v -> u
        Edge v_u_(0, 0, u, v, -w, -1); // back for v -> u

        graph.add_edge(u, u_v);
        graph.add_edge(v, u_v_);
        graph.add_edge(v, v_u);
        graph.add_edge(u, v_u_);
    }

    // set_potentials(0);
    int cnt = 0;
    for (int i = 0; i < k; ++i) {
        if (find_and_push_flow(0, n - 1)) {
            cnt++;
        } else {
            break;
        }
    }

    if (cnt != k) {
        cout << "-1\n";
        return 0;
    }

    vector<vector<int>> ways(k);
    for (int i = 0; i < k; ++i) {
        cc++;
        way.resize(0);
        dfs(0, -1, n - 1);
        reverse(way.begin(), way.end());
        ways[i] = way;
    }
    cout << sum / k << "\n";
    for (int i = 0; i < k; ++i) {
        cout << ways[i].size() << " ";
        for (auto x : ways[i]) {
            cout << x << " ";
        }
        cout << "\n";
    }

    return 0;
}
