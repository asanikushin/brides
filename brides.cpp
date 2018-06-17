#include <bits/stdc++.h>

using namespace std;

const int INF = 1000 * 1000 * 1000;

struct Edge {
    int flow, capacity;
    int from, to;
    int weight;
    int id;

    Edge() {
        flow = 0;
        capacity = 0;
        from = -1;
        to = -1;
        weight = INF;
        id = -1;
    }

    Edge(int fl, int c, int fr, int t, int w = 0, int i = -1) {
        flow = fl;
        capacity = c;
        from = fr;
        to = t;
        weight = w;
        id = i;
    }

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


int n, m, k;
int cc = 0;
long double sum = 0;
vector<vector<int>> graph;
vector<Edge> edges;
vector<int> potential;
vector<int> parents; // number of edge
vector<int> used;
vector<int> way;

void set_potentials(int start) {
    potential.assign(n, INF);
    potential[start] = 0;
    bool was_update = true;
    while (was_update) {
        was_update = false;
        for (auto e : edges) {
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

    parents.assign(n, -1);
    vector<int> dist(n, INF);

    dist[start] = 0;
    set<pair<int, int>> s;
    s.insert({dist[start], start});

    while (!s.empty()) {
        int u, d;
        tie(d, u) = *s.begin();
        s.erase(s.begin());

        for (int index : graph[u]) {
            Edge e = edges[index];
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
        cur = edges[parents[cur]].from;
    }
    reverse(way.begin(), way.end());

    for (auto index : way) {
        ++edges[index];
        --edges[index ^ 1];
    }
    for (int i = 0; i < n; ++i) {
        potential[i] += dist[i];
    }
    return true;
}

bool dfs(int v, int par, int finish) {
    used[v] = cc;
    if (v == finish) {
        return true;
    }
    for (auto e : graph[v]) {
        int to = edges[e].to;
        if (to == par) {
            continue;
        }
        if (edges[e].flow > 0 && (used[to] != cc && dfs(to, v, finish))) {
            --edges[e];
            ++edges[e ^ 1];
            way.push_back(edges[e].id);
            sum += edges[e].weight;
            return true;
        }
    }
    return false;
}


int main() {
    cout.precision(30);
    cin >> n >> m >> k;

    graph.resize(n);
    potential.resize(n);
    parents.resize(n);
    used.resize(n, -1);
    edges.reserve(4 * m);

    for (int i = 0; i < m; ++i) {
        int u, v, w;
        cin >> u >> v >> w;
        u--, v--;

        Edge u_v(0, 1, u, v, w, i + 1);   // u -> v
        Edge u_v_(0, 0, v, u, -w, -1); // back for u -> v
        Edge v_u(0, 1, v, u, w, i + 1);   // v -> u
        Edge v_u_(0, 0, u, v, -w, -1); // back for v -> u

        graph[u].push_back(edges.size());
        edges.push_back(u_v);

        graph[v].push_back(edges.size());
        edges.push_back(u_v_);

        graph[v].push_back(edges.size());
        edges.push_back(v_u);

        graph[u].push_back(edges.size());
        edges.push_back(v_u_);

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
    cout << (sum + .0) / k << "\n";
    for (int i = 0; i < k; ++i) {
        cout << ways[i].size() << " ";
        for (auto x : ways[i]) {
            cout << x << " ";
        }
        cout << "\n";
    }

    return 0;
}
