#include "Utils/segmentation.h"
#include <iostream>
#include <algorithm>

void Segmentation::segmentGraph(UndirectedGraph& graph, double k) {
    int n = graph.getVertices().size();
    UnionFind ds(n);

    // Construir todas as arestas do grafo
    std::vector<std::tuple<double, int, int>> edges;

    for (int u = 0; u < n; u++) {
        std::vector<Edge> neighbors = graph.getNeighborsInternal(u);
        for (Edge e : neighbors) {
            if (u < e.to) { // Evitar duplicatas no grafo não direcionado
                edges.push_back({e.weight, u, e.to});
            }
        }
    }

    // Ordenar as arestas pelo peso (dissimilaridade)
    std::sort(edges.begin(), edges.end());

    // Executar o algoritmo de segmentação
    for (const auto& edge : edges) {
        double weight = std::get<0>(edge);
        int u = std::get<1>(edge);
        int v = std::get<2>(edge);
        ds.join(u, v, weight, k);
    }

    // Exibir resultado final da segmentação
    std::unordered_map<int, std::vector<std::string>> components;
    for (int i = 0; i < n; i++) {
        int root = ds.find(i);
        components[root].push_back(graph.getVertices()[i].label);
    }

    std::cout << "Segmentação final:\n";
    for (auto it = components.begin(); it != components.end(); ++it) {
        int root = it->first;
        std::vector<std::string>& comp = it->second;

        std::cout << "Componente:";
        for (const auto& label : comp)
            std::cout << " " << label;
        std::cout << std::endl;
    }
}
