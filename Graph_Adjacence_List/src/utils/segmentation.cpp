#include "Utils/segmentation.h"
#include <iostream>
#include <algorithm>

vector<int> Segmentation::segmentGraph(UndirectedGraph& graph, double k, int min_size) {
    int n = graph.getVertices().size();
    UnionFind ds(n);

    // Construir todas as arestas do grafo
    std::vector<std::tuple<double, int, int>> edges;
    for (int u = 0; u < n; u++) {
        std::vector<Edge> neighbors = graph.getNeighborsInternal(u);
        for (Edge e : neighbors) {
            if (u < e.to) {
                edges.push_back({e.weight, u, e.to});
            }
        }
    }

    std::sort(edges.begin(), edges.end());

    // Segmentação principal
    for (const auto& edge : edges) {
        double weight = std::get<0>(edge);
        int u = std::get<1>(edge);
        int v = std::get<2>(edge);
        ds.join(u, v, weight, k);
    }

    // Segunda passagem: aplicar min_size
    for (const auto& edge : edges) {
        int u = std::get<1>(edge);
        int v = std::get<2>(edge);
        int comp_u = ds.find(u);
        int comp_v = ds.find(v);

        if (comp_u != comp_v) {
            int size_u = ds.getSize(comp_u);
            int size_v = ds.getSize(comp_v);

            if (size_u < min_size || size_v < min_size) {
                ds.forceJoin(comp_u, comp_v);  // novo método que une componentes incondicionalmente
            }
        }
    }

    // Agrupar rótulos finais
    std::unordered_map<int, std::vector<std::string>> components;
    std::vector<int> componentIds(n);
    for (int i = 0; i < n; i++) {
        int root = ds.find(i);
        componentIds[i] = root;
        components[root].push_back(graph.getVertices()[i].label);
    }

    std::cout << "Segmentação final:\n";
    for (auto& [root, comp] : components) {
        std::cout << "Componente:";
        for (const auto& label : comp)
            std::cout << " " << label;
        std::cout << std::endl;
    }

    std::cout << "Quantidade de componentes: " << components.size() << std::endl;

    return componentIds;
}

