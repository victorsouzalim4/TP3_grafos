#include "directed_Graph.h"

#include <string>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "Edge.h"
#include <algorithm>

using namespace std;

void DirectedGraph::addEdge(const string& from, const string& to, double weight) {
    if (!labelToIndex.count(from)) {
        throw invalid_argument("Origin vertex'" + from + "' does not exists.");
    }
    if (!labelToIndex.count(to)) {
        throw invalid_argument("Destiny vertex '" + to + "' does not exists.");
    }

    int indexFrom = labelToIndex[from];
    int indexTo = labelToIndex[to];
    Edge e(indexTo, weight);

    adjList[indexFrom].push_back(e);

}

void DirectedGraph::removeEdge(const string& from, const string& to){
    if (!labelToIndex.count(from)) {
        throw invalid_argument("Origin vertex'" + from + "' does not exists.");
    }
    if (!labelToIndex.count(to)) {
        throw invalid_argument("Destiny vertex '" + to + "' does not exists.");
    }

    int indexFrom = labelToIndex[from];
    int indexTo = labelToIndex[to];

    vector<Edge>& e = adjList[indexFrom];
    size_t before = e.size();

    e.erase(
        remove_if(e.begin(), e.end(),
            [indexTo](const Edge& e) { return e.to == indexTo; }),
        e.end());

    if(e.size() == before){
        throw runtime_error("Edge " + from + " -> " + to + " not found" );
    }
}
