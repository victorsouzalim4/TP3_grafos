#define GRAPH_INTERNAL_ACCESS

#include "Utils/utils.h"

#include <string>
#include <vector>
#include <utility>
#include <algorithm>


pair<vector<string>, double> Utils::reconstructPath(Graph g, vector<tuple<double, int, int>> d, int from, int to){

    vector<Vertex> vertices = g.getVertices();
    int u = to;
    vector<string> path;

    while(u != get<2>(d[from])){

        path.push_back(vertices[u].label);
        u = get<2>(d[u]);
    }

    path.push_back(vertices[u].label);

    reverse(path.begin(), path.end());

    return make_pair(path, get<0>(d[to]));
}