#include <bits/stdc++.h>

struct Point {
    int x;
    int y;
};

struct Edge {
    int u;
    int v;
    int weight;

    // Operator for sorting edges in Kruskal's algorithm
    bool operator<(const Edge& other) const {
        return weight < other.weight;
    }
};

class Graph {
public:
    int num_nodes;
    int k_neighbors;
    int seed;
    std::vector<Point> nodes;
    std::vector<std::vector<std::pair<int, int>>> adj; // Adjacency list: adj[u] = {v, weight}
    std::map<std::pair<int, int>, std::string> traffic_states; // Edge -> Traffic level ('Low', 'Medium', 'High')

    Graph(int n = 500, int k = 5, int s = 42) : num_nodes(n), k_neighbors(k), seed(s) {
        adj.resize(num_nodes);
        generate_network();
    }

    // Helper to generate a connected 500-node graph using nearest neighbors
    void generate_network() {
        std::mt19937 rng(seed);
        std::uniform_int_distribution<int> coord_dist(10, 990);

        // 1. Generate random x, y coordinates
        nodes.clear();
        for (int i = 0; i < num_nodes; ++i) {
            nodes.push_back({coord_dist(rng), coord_dist(rng)});
        }

        auto get_dist = [this](int u, int v) {
            double dx = nodes[u].x - nodes[v].x;
            double dy = nodes[u].y - nodes[v].y;
            return static_cast<int>(std::round(std::hypot(dx, dy)));
        };

        // 2. Add edges to the k nearest neighbors for each node
        for (int u = 0; u < num_nodes; ++u) {
            std::vector<std::pair<int, int>> dists;
            for (int v = 0; v < num_nodes; ++v) {
                if (u != v) {
                    dists.push_back({get_dist(u, v), v});
                }
            }
            std::sort(dists.begin(), dists.end());
            int limit = std::min(k_neighbors, static_cast<int>(dists.size()));
            for (int rank = 0; rank < limit; ++rank) {
                int d = dists[rank].first;
                int v = dists[rank].second;
                
                bool exists = false;
                for (const auto& edge : adj[u]) {
                    if (edge.first == v) { exists = true; break; }
                }
                if (!exists) {
                    adj[u].push_back({v, d});
                    adj[v].push_back({u, d});
                }
            }
        }

        // 3. Connect components using Union-Find to ensure graph is fully connected
        std::vector<int> parent(num_nodes);
        for (int i = 0; i < num_nodes; ++i) parent[i] = i;

        auto find = [&parent](auto& self, int i) -> int {
            if (parent[i] == i) return i;
            return parent[i] = self(self, parent[i]);
        };

        auto union_nodes = [&find, &parent](int i, int j) -> bool {
            int root_i = find(find, i);
            int root_j = find(find, j);
            if (root_i != root_j) {
                parent[root_i] = root_j;
                return true;
            }
            return false;
        };

        for (int u = 0; u < num_nodes; ++u) {
            for (const auto& edge : adj[u]) {
                union_nodes(u, edge.first);
            }
        }

        std::map<int, std::vector<int>> components;
        for (int i = 0; i < num_nodes; ++i) {
            components[find(find, i)].push_back(i);
        }

        while (components.size() > 1) {
            auto it = components.begin();
            std::vector<int>& comp0 = it->second;

            int best_d = std::numeric_limits<int>::max();
            int best_u = -1;
            int best_v = -1;

            for (auto other_it = std::next(it); other_it != components.end(); ++other_it) {
                std::vector<int>& comp_other = other_it->second;
                for (int u : comp0) {
                    for (int v : comp_other) {
                        int d = get_dist(u, v);
                        if (d < best_d) {
                            best_d = d;
                            best_u = u;
                            best_v = v;
                        }
                    }
                }
            }

            adj[best_u].push_back({best_v, best_d});
            adj[best_v].push_back({best_u, best_d});
            union_nodes(best_u, best_v);

            for (int i = 0; i < num_nodes; ++i) parent[i] = i;
            for (int u = 0; u < num_nodes; ++u) {
                for (const auto& edge : adj[u]) {
                    union_nodes(u, edge.first);
                }
            }
            components.clear();
            for (int i = 0; i < num_nodes; ++i) {
                components[find(find, i)].push_back(i);
            }
        }

        // 4. Assign dynamic traffic states (70% Low, 20% Medium, 10% High)
        std::uniform_real_distribution<double> rand_dist(0.0, 1.0);
        traffic_states.clear();
        for (int u = 0; u < num_nodes; ++u) {
            for (const auto& edge : adj[u]) {
                int v = edge.first;
                if (u < v) {
                    double r = rand_dist(rng);
                    std::string state = "Low";
                    if (r >= 0.70 && r < 0.90) state = "Medium";
                    else if (r >= 0.90) state = "High";
                    traffic_states[{u, v}] = state;
                }
            }
        }
    }

    std::vector<Edge> get_all_edges() const {
        std::vector<Edge> edges;
        for (int u = 0; u < num_nodes; ++u) {
            for (const auto& edge : adj[u]) {
                int v = edge.first;
                int w = edge.second;
                if (u < v) edges.push_back({u, v, w});
            }
        }
        return edges;
    }

    double get_traffic_multiplier(int u, int v) const {
        std::pair<int, int> edge = (u < v) ? std::make_pair(u, v) : std::make_pair(v, u);
        auto it = traffic_states.find(edge);
        if (it != traffic_states.end()) {
            if (it->second == "Medium") return 1.5;
            if (it->second == "High") return 2.5;
        }
        return 1.0;
    }
};

// Task 1: MST Optimization using Kruskal's algorithm
std::vector<Edge> solve_task1_mst(const Graph& graph) {
    std::cout << "\n--------------------------------------------\n";
    std::cout << "--- Task 1: MST Optimization ---\n";
    std::cout << "--------------------------------------------\n";

    std::vector<Edge> all_edges = graph.get_all_edges();
    long long original_cost = 0;
    for (const auto& edge : all_edges) {
        original_cost += edge.weight;
    }

    std::sort(all_edges.begin(), all_edges.end());
    std::vector<int> parent(graph.num_nodes);
    for (int i = 0; i < graph.num_nodes; ++i) parent[i] = i;

    auto find = [&parent](auto& self, int i) -> int {
        if (parent[i] == i) return i;
        return parent[i] = self(self, parent[i]);
    };

    auto union_nodes = [&find, &parent](int i, int j) -> bool {
        int root_i = find(find, i);
        int root_j = find(find, j);
        if (root_i != root_j) {
            parent[root_i] = root_j;
            return true;
        }
        return false;
    };

    std::vector<Edge> mst_edges;
    long long mst_cost = 0;

    for (const auto& edge : all_edges) {
        if (union_nodes(edge.u, edge.v)) {
            mst_edges.push_back(edge);
            mst_cost += edge.weight;
            if (mst_edges.size() == static_cast<size_t>(graph.num_nodes - 1)) {
                break;
            }
        }
    }

    double savings_pct = (static_cast<double>(original_cost - mst_cost) / original_cost) * 100.0;

    std::cout << "Original Network Cost = " << original_cost << " km\n";
    std::cout << "Optimized MST Cost    = " << mst_cost << " km\n";
    std::cout << "Total Cost Saved      = " << std::fixed << std::setprecision(1) << savings_pct << " %\n\n";

    std::cout << "Selected Roads (First 10):\n";
    for (size_t i = 0; i < std::min(static_cast<size_t>(10), mst_edges.size()); ++i) {
        std::cout << "  Road " << i + 1 << ": City " << mst_edges[i].u + 1 
                  << " to City " << mst_edges[i].v + 1 
                  << " (" << mst_edges[i].weight << " km)\n";
    }
    std::cout << "... and " << mst_edges.size() - 10 << " more roads.\n\n";
    std::cout << "[All " << mst_edges.size() << " structural spans successfully written to path buffer]\n";

    return mst_edges;
}

// Task 2: Strategic City Identification using degree centrality
void solve_task2_centrality(const Graph& graph) {
    std::cout << "\n--------------------------------------------\n";
    std::cout << "--- Task 2: Strategic City Identification ---\n";
    std::cout << "--------------------------------------------\n";

    std::vector<std::pair<int, int>> centrality;
    for (int u = 0; u < graph.num_nodes; ++u) {
        centrality.push_back({static_cast<int>(graph.adj[u].size()), u});
    }

    std::sort(centrality.begin(), centrality.end(), [](const auto& a, const auto& b) {
        if (a.first != b.first) return a.first > b.first;
        return a.second < b.second;
    });

    std::cout << "Top 10 Cities by Degree Centrality:\n";
    for (int rank = 0; rank < 10; ++rank) {
        std::cout << "  Rank " << rank + 1 << ": City " << centrality[rank].second + 1 
                  << " (Degree: " << centrality[rank].first << ")\n";
    }

    std::cout << "\nTop Candidates:\n";
    std::vector<std::string> roles = {"Airport", "Logistics Hub", "Railway Junction"};
    for (int i = 0; i < 3; ++i) {
        std::cout << "  - " << roles[i] << ": City " << centrality[i].second + 1 << "\n";
    }

    std::cout << "\nRecommendation: City " << centrality[0].second + 1 
              << " is a major hub and is recommended for an international airport.\n";
}

// Helper to run Dijkstra's algorithm skipping deleted elements
std::pair<double, std::vector<int>> run_dijkstra(
    const Graph& graph, 
    int start, 
    int end, 
    const std::set<int>& removed_nodes, 
    const std::set<std::pair<int, int>>& removed_edges
) {
    if (removed_nodes.count(start) || removed_nodes.count(end)) {
        return {std::numeric_limits<double>::infinity(), {}};
    }

    std::vector<double> dist(graph.num_nodes, std::numeric_limits<double>::infinity());
    std::vector<int> parent(graph.num_nodes, -1);
    dist[start] = 0.0;

    typedef std::pair<double, int> pq_elem;
    std::priority_queue<pq_elem, std::vector<pq_elem>, std::greater<pq_elem>> pq;
    pq.push({0.0, start});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        if (u == end) break;
        if (d > dist[u]) continue;

        for (const auto& edge : graph.adj[u]) {
            int v = edge.first;
            double w = edge.second;

            if (removed_nodes.count(v)) continue;

            std::pair<int, int> current_edge = (u < v) ? std::make_pair(u, v) : std::make_pair(v, u);
            if (removed_edges.count(current_edge)) continue;

            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                parent[v] = u;
                pq.push({dist[v], v});
            }
        }
    }

    if (dist[end] == std::numeric_limits<double>::infinity()) {
        return {std::numeric_limits<double>::infinity(), {}};
    }

    std::vector<int> path;
    int curr = end;
    while (curr != -1) {
        path.push_back(curr);
        curr = parent[curr];
    }
    std::reverse(path.begin(), path.end());

    return {dist[end], path};
}

// Task 3: Disaster Recovery Routing
void solve_task3_disaster(const Graph& graph) {
    std::cout << "\n--------------------------------------------\n";
    std::cout << "--- Task 3: Disaster Recovery Routing ---\n";
    std::cout << "--------------------------------------------\n";

    std::cout << "Disaster Disruption Configuration:\n";
    std::cout << "Enter 'demo' for default scenario, or 'custom' to input values: ";
    std::string mode;
    std::cin >> mode;

    std::set<int> removed_nodes;
    std::set<std::pair<int, int>> removed_edges;
    int src_a = 0;
    int src_b = 99;
    int dest_d = 202;

    if (mode == "custom") {
        std::cout << "Enter number of cities to destroy: ";
        int n_destroy;
        if (std::cin >> n_destroy) {
            for (int i = 0; i < n_destroy; ++i) {
                std::cout << "  Destroyed City ID: ";
                int u;
                std::cin >> u;
                removed_nodes.insert(u - 1);
            }
        }

        std::cout << "Enter number of roads to destroy: ";
        int e_destroy;
        if (std::cin >> e_destroy) {
            for (int i = 0; i < e_destroy; ++i) {
                std::cout << "  Road endpoint A: ";
                int u;
                std::cin >> u;
                std::cout << "  Road endpoint B: ";
                int v;
                std::cin >> v;
                u--; v--;
                removed_edges.insert((u < v) ? std::make_pair(u, v) : std::make_pair(v, u));
            }
        }

        std::cout << "Enter Source City A: ";
        std::cin >> src_a; src_a--;
        std::cout << "Enter Source City B: ";
        std::cin >> src_b; src_b--;
        std::cout << "Enter Destination City D: ";
        std::cin >> dest_d; dest_d--;
    } else {
        removed_nodes = {11, 54}; // City 12 and City 55
        removed_edges = {{11, 90}, {43, 66}}; // Roads 12-91 and 44-67
        src_a = 0;      // City 1
        src_b = 99;     // City 100
        dest_d = 202;   // City 203

        std::cout << "\n[Demo Disruption Config]\n";
        std::cout << "  Destroying Cities: 12, 55\n";
        std::cout << "  Destroying Roads : 12-91, 44-67\n";
        std::cout << "  Route Query: A = City " << src_a + 1 
                  << ", B = City " << src_b + 1 << " to D = City " << dest_d + 1 << "\n\n";
    }

    auto [dist_a, path_a] = run_dijkstra(graph, src_a, dest_d, removed_nodes, removed_edges);
    auto [dist_b, path_b] = run_dijkstra(graph, src_b, dest_d, removed_nodes, removed_edges);

    if (dist_a != std::numeric_limits<double>::infinity()) {
        std::cout << "Route from City " << src_a + 1 << " to City " << dest_d + 1 
                  << ": Distance = " << static_cast<int>(dist_a) << " km\n  Path: ";
        for (size_t i = 0; i < std::min(static_cast<size_t>(5), path_a.size()); ++i) {
            std::cout << "City " << path_a[i] + 1 << (i + 1 < path_a.size() ? " -> " : "");
        }
        if (path_a.size() > 5) {
            std::cout << "... (" << path_a.size() - 5 << " more) -> City " << path_a.back() + 1;
        }
        std::cout << "\n";
    } else {
        std::cout << "Route from City " << src_a + 1 << " to City " << dest_d + 1 
                  << ": Fallback state: \"No valid route available after disaster.\"\n";
    }

    if (dist_b != std::numeric_limits<double>::infinity()) {
        std::cout << "Route from City " << src_b + 1 << " to City " << dest_d + 1 
                  << ": Distance = " << static_cast<int>(dist_b) << " km\n  Path: ";
        for (size_t i = 0; i < std::min(static_cast<size_t>(5), path_b.size()); ++i) {
            std::cout << "City " << path_b[i] + 1 << (i + 1 < path_b.size() ? " -> " : "");
        }
        if (path_b.size() > 5) {
            std::cout << "... (" << path_b.size() - 5 << " more) -> City " << path_b.back() + 1;
        }
        std::cout << "\n";
    } else {
        std::cout << "Route from City " << src_b + 1 << " to City " << dest_d + 1 
                  << ": Fallback state: \"No valid route available after disaster.\"\n";
    }
}

// Helper to run Dijkstra's incorporating traffic penalties
std::pair<double, std::vector<int>> run_dijkstra_traffic(const Graph& graph, int start, int end) {
    std::vector<double> dist(graph.num_nodes, std::numeric_limits<double>::infinity());
    std::vector<int> parent(graph.num_nodes, -1);
    dist[start] = 0.0;

    typedef std::pair<double, int> pq_elem;
    std::priority_queue<pq_elem, std::vector<pq_elem>, std::greater<pq_elem>> pq;
    pq.push({0.0, start});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        if (u == end) break;
        if (d > dist[u]) continue;

        for (const auto& edge : graph.adj[u]) {
            int v = edge.first;
            double base_w = edge.second;
            double mult = graph.get_traffic_multiplier(u, v);
            double traffic_w = base_w * mult;

            if (dist[u] + traffic_w < dist[v]) {
                dist[v] = dist[u] + traffic_w;
                parent[v] = u;
                pq.push({dist[v], v});
            }
        }
    }

    if (dist[end] == std::numeric_limits<double>::infinity()) {
        return {std::numeric_limits<double>::infinity(), {}};
    }

    std::vector<int> path;
    int curr = end;
    while (curr != -1) {
        path.push_back(curr);
        curr = parent[curr];
    }
    std::reverse(path.begin(), path.end());

    return {dist[end], path};
}

int get_path_physical_dist(const Graph& graph, const std::vector<int>& path) {
    int dist = 0;
    for (size_t i = 0; i + 1 < path.size(); ++i) {
        int u = path[i];
        int v = path[i+1];
        for (const auto& edge : graph.adj[u]) {
            if (edge.first == v) {
                dist += edge.second;
                break;
            }
        }
    }
    return dist;
}

// Task 4: Traffic-Aware Smart Routing
void solve_task4_traffic(const Graph& graph) {
    std::cout << "\n--------------------------------------------\n";
    std::cout << "--- Task 4: Traffic-Aware Smart Routing ---\n";
    std::cout << "--------------------------------------------\n";

    std::cout << "Enter 'demo' for default scenario, or 'custom' to input cities: ";
    std::string mode;
    std::cin >> mode;

    int src = 0;
    int dest = 249;

    if (mode == "custom") {
        std::cout << "Enter Source City: ";
        std::cin >> src; src--;
        std::cout << "Enter Destination City: ";
        std::cin >> dest; dest--;
    } else {
        std::cout << "\n[Demo Routing Query] Source: City " << src + 1 
                  << " to Destination: City " << dest + 1 << "\n\n";
    }

    auto [normal_cost, normal_path] = run_dijkstra(graph, src, dest, {}, {});
    auto [traffic_eff_cost, traffic_path] = run_dijkstra_traffic(graph, src, dest);
    int traffic_phys_dist = get_path_physical_dist(graph, traffic_path);

    double delay_pct = ((traffic_eff_cost - normal_cost) / normal_cost) * 100.0;

    std::cout << "Normal Route (no traffic):\n";
    std::cout << "  Distance = " << static_cast<int>(normal_cost) << " km\n";
    std::cout << "  Path     = ";
    for (size_t i = 0; i < std::min(static_cast<size_t>(5), normal_path.size()); ++i) {
        std::cout << "City " << normal_path[i] + 1 << (i + 1 < normal_path.size() ? " -> " : "");
    }
    if (normal_path.size() > 5) {
        std::cout << "... (" << normal_path.size() - 5 << " more) -> City " << normal_path.back() + 1;
    }
    std::cout << "\n\n";

    std::cout << "Traffic-Aware Route:\n";
    std::cout << "  Distance = " << traffic_phys_dist << " km\n";
    std::cout << "  Effective Travel Cost = " << std::fixed << std::setprecision(1) << traffic_eff_cost 
              << " km (adjusted for traffic)\n";
    std::cout << "  Path     = ";
    for (size_t i = 0; i < std::min(static_cast<size_t>(5), traffic_path.size()); ++i) {
        std::cout << "City " << traffic_path[i] + 1 << (i + 1 < traffic_path.size() ? " -> " : "");
    }
    if (traffic_path.size() > 5) {
        std::cout << "... (" << traffic_path.size() - 5 << " more) -> City " << traffic_path.back() + 1;
    }
    std::cout << "\n\n";

    std::cout << "Delay = " << std::fixed << std::setprecision(1) << delay_pct << "%\n";
}

// Helper to count components using BFS
int count_connected_components(const Graph& graph, int removed_node = -1, std::pair<int, int> removed_edge = {-1, -1}) {
    std::vector<bool> visited(graph.num_nodes, false);
    int count = 0;

    for (int start = 0; start < graph.num_nodes; ++start) {
        if (start == removed_node) continue;
        if (!visited[start]) {
            count++;
            std::vector<int> q;
            q.push_back(start);
            visited[start] = true;
            size_t head = 0;
            while (head < q.size()) {
                int u = q[head++];
                for (const auto& edge : graph.adj[u]) {
                    int v = edge.first;
                    if (v == removed_node) continue;
                    
                    if ((u == removed_edge.first && v == removed_edge.second) ||
                        (u == removed_edge.second && v == removed_edge.first)) {
                        continue;
                    }

                    if (!visited[v]) {
                        visited[v] = true;
                        q.push_back(v);
                    }
                }
            }
        }
    }
    return count;
}

// Task 5: Critical Infrastructure Analysis
void solve_task5_criticality(const Graph& graph) {
    std::cout << "\n--------------------------------------------\n";
    std::cout << "--- Task 5: Critical Infrastructure Analysis ---\n";
    std::cout << "--------------------------------------------\n";
    std::cout << "Analyzing graph connectivity (" << graph.num_nodes << " nodes)... Please wait...\n";

    int base_components = count_connected_components(graph);
    std::cout << "Base Component Count = " << base_components << "\n";

    int max_node_components = base_components;
    int most_critical_node = -1;

    for (int u = 0; u < graph.num_nodes; ++u) {
        int comps = count_connected_components(graph, u);
        if (comps > max_node_components) {
            max_node_components = comps;
            most_critical_node = u;
        }
    }

    int max_edge_components = base_components;
    std::pair<int, int> most_critical_edge = {-1, -1};
    std::vector<Edge> all_edges = graph.get_all_edges();

    for (const auto& edge : all_edges) {
        int comps = count_connected_components(graph, -1, {edge.u, edge.v});
        if (comps > max_edge_components) {
            max_edge_components = comps;
            most_critical_edge = {edge.u, edge.v};
        }
    }

    std::cout << "\nCritical Diagnostic Output:\n";
    if (most_critical_node != -1) {
        std::cout << "  Most Critical City: City " << most_critical_node + 1 
                  << " (Splits graph into " << max_node_components << " components)\n";
    } else {
        std::cout << "  Most Critical City: None (No single city splits the graph)\n";
    }

    if (most_critical_edge.first != -1) {
        std::cout << "  Most Critical Road: City " << most_critical_edge.first + 1 
                  << " - City " << most_critical_edge.second + 1 
                  << " (Splits graph into " << max_edge_components << " components)\n";
    } else {
        std::cout << "  Most Critical Road: None (No single road splits the graph)\n";
    }
}

void display_menu() {
    std::cout << "\n============================================\n";
    std::cout << "  GRAPH THEORY LABORATORY - SUBMISSION SUITE\n";
    std::cout << "============================================\n";
    std::cout << " 1. Run Task 1: MST Optimization\n";
    std::cout << " 2. Run Task 2: Strategic City Identification\n";
    std::cout << " 3. Run Task 3: Disaster Recovery Routing\n";
    std::cout << " 4. Run Task 4: Traffic-Aware Smart Routing\n";
    std::cout << " 5. Run Task 5: Critical Infrastructure Analysis\n";
    std::cout << " 6. Run All Tasks\n";
    std::cout << " 7. Re-generate Graph\n";
    std::cout << " 8. Exit\n";
    std::cout << "============================================\n";
    std::cout << "Enter choice: ";
}

int main() {
    std::cout << "Initializing 500-City Transportation Graph...\n";
    Graph g(500, 5, 42);
    std::cout << "Nodes: " << g.num_nodes << ", Edges: " << g.get_all_edges().size() << "\n";

    while (true) {
        display_menu();
        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        if (choice == 1) solve_task1_mst(g);
        else if (choice == 2) solve_task2_centrality(g);
        else if (choice == 3) solve_task3_disaster(g);
        else if (choice == 4) solve_task4_traffic(g);
        else if (choice == 5) solve_task5_criticality(g);
        else if (choice == 6) {
            solve_task1_mst(g);
            solve_task2_centrality(g);
            solve_task3_disaster(g);
            solve_task4_traffic(g);
            solve_task5_criticality(g);
        } else if (choice == 7) {
            int n, k, s;
            std::cout << "Enter number of cities (default 500): ";
            std::cin >> n;
            std::cout << "Enter nearest neighbors k (default 5): ";
            std::cin >> k;
            std::cout << "Enter seed (default 42): ";
            std::cin >> s;
            g = Graph(n, k, s);
            std::cout << "Re-generated graph with " << g.num_nodes << " nodes.\n";
        } else if (choice == 8) {
            std::cout << "Exiting. Thank you!\n";
            break;
        } else {
            std::cout << "Invalid choice.\n";
        }
    }
    return 0;
}
