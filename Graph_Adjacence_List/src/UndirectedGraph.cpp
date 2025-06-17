#include "undirected_Graph.h"
#include "Edge.h"

#include <string>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

void UndirectedGraph::addEdge(const string& from, const string& to, double weight) {
    if (!labelToIndex.count(from)) {
        throw invalid_argument("Origin vertex'" + from + "' does not exists.");
    }
    if (!labelToIndex.count(to)) {
        throw invalid_argument("Destiny vertex '" + to + "' does not exists.");
    }

    int indexFrom = labelToIndex[from];
    int indexTo = labelToIndex[to];
    Edge e1(indexTo, weight);
    Edge e2(indexFrom, weight);

    adjList[indexFrom].push_back(e1);
    adjList[indexTo].push_back(e2);

}

void UndirectedGraph::removeEdge(const string& from, const string& to){
    if (!labelToIndex.count(from)) {
        throw invalid_argument("Origin vertex'" + from + "' does not exists.");
    }
    if (!labelToIndex.count(to)) {
        throw invalid_argument("Destiny vertex '" + to + "' does not exists.");
    }

    int indexFrom = labelToIndex[from];
    int indexTo = labelToIndex[to];

    vector<Edge>& eFrom = adjList[indexFrom];
    size_t beforeFrom = eFrom.size();

    eFrom.erase(
        remove_if(eFrom.begin(), eFrom.end(),
            [indexTo](const Edge& eFrom) { return eFrom.to == indexTo; }),
        eFrom.end());

    if(eFrom.size() == beforeFrom){
        throw runtime_error("Edge " + from + " <-> " + to + " not found" );
    }

    vector<Edge>& eTo = adjList[indexTo];
    size_t beforeTo = eTo.size();

    eTo.erase(
        remove_if(eTo.begin(), eTo.end(),
            [indexFrom](const Edge& eTo) { return eTo.to == indexFrom; }),
        eTo.end());

    if(eTo.size() == beforeTo){
        throw runtime_error("Edge " + from + " <-> " + to + " not found" );
    }
}