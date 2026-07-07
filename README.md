# CS203: Smart Transportation Network Planning and Resilience Analysis
Course Project on Graph Theory and Algorithmic Optimization

Developed by **Sumanth Reddy Kamjula** (Roll No: `240510`), B.Tech CSE, Indian Institute of Technology Kanpur (IITK).

---

## 📌 Project Overview
This project implements a large-scale smart transportation network simulation connecting 500 cities (modeled as a sparse, connected graph $G = (V, E)$). It features optimizations, strategic hub suggestions, disaster recovery routing, traffic-aware pathfinding, and critical point failure diagnoses.

The network is initialized by placing cities at random 2D coordinates within a $1000 \times 1000$ grid. The physical distances are Euclidean, and the graph is kept sparse by connecting nodes to their $k$-nearest neighbors. Disjoint components are resolved dynamically using Union-Find to guarantee a fully connected grid.

---

## 🛠️ Project Structure
*   **`main.cpp`**: Core implementation of the transportation graph, routing algorithms, and CLI menu.
*   **`bits/stdc++.h`**: Local header file resolving GCC-specific headers on macOS systems.
*   **`Graph_Theory_Project_Report.pdf`**: The formal project report detailing requirements, mathematical models, and console results.
*   **`.gitignore`**: Excludes system-specific and compilation output files (e.g., binaries, `.DS_Store`).

---

## 🚀 Tasks Implemented

### Task 1: MST Optimization (Kruskal's Algorithm)
*   **Objective:** Find the minimum construction cost layout that keeps all 500 cities connected.
*   **Complexity:** $\mathcal{O}(E \log E)$ due to edge sorting. Uses Disjoint Set Union (DSU) with path compression.

### Task 2: Strategic City Identification (Degree Centrality)
*   **Objective:** Rank cities based on physical degree connection centrality to allocate key infrastructural projects (e.g., International Airport, Logistics Hub, Railway Junction).
*   **Complexity:** $\mathcal{O}(V + E)$.

### Task 3: Disaster Recovery Routing (Dijkstra's Algorithm)
*   **Objective:** Dynamic path calculations bypassing failed nodes (destroyed cities) or failed edges (destroyed roads) in real-time.
*   **Complexity:** $\mathcal{O}((V + E) \log V)$ using a heap-based priority queue.

### Task 4: Traffic-Aware Smart Routing (Modified Dijkstra)
*   **Objective:** Avoid traffic-congested roads by scaling weights (multiplier parameters: Low `1.0x`, Medium `1.5x`, High `2.5x`) dynamically.
*   **Complexity:** $\mathcal{O}((V + E) \log V)$.

### Task 5: Critical Infrastructure Analysis (BFS Diagnostics)
*   **Objective:** Identify the single point of failure (cut-vertices and bridges) that splits the grid into the maximum number of disconnected components if removed.
*   **Complexity:** $\mathcal{O}((V + E)^2)$.

---

## 💻 Compilation and Execution

Since macOS Clang does not natively support `<bits/stdc++.h>`, we include a local copy in `./bits/stdc++.h`. You must include the current folder in the compile search path using `-I.`.

### Using standard `g++` (GCC/Clang):
```bash
# Compile the C++ program (using C++17)
g++ -std=c++17 -I. main.cpp -o main

# Run the executable
./main
```

---

## 📊 Console Outputs & Reports
For details about the algorithm benchmarks, console logs, and implementation reviews, please check the [Graph_Theory_Project_Report.pdf](file:///Users/kamjulasumanthreddy/Downloads/graphtheory%20final/Graph_Theory_Project_Report.pdf) file.
