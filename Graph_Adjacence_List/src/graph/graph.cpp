#define GRAPH_INTERNAL_ACCESS

#include "graph.h"
#include "vertex.h"
#include "edge.h"
#include "Utils/utils.h"

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <utility>
#include <limits>
#include <functional>
#include <queue>
#include <iomanip>
#include <stack>

using namespace std;

Graph::Graph(): len(0) {}

int Graph::addVertex(const string& label, double heuristicWeight){
    if(labelToIndex.count(label)){
        return labelToIndex[label];
    }

    int index = vertices.size();

    Vertex v(label, heuristicWeight);
    vertices.push_back(v);
    labelToIndex[label] = index;
    adjList.emplace_back();

    len++;

    return index;
}

void Graph::removeVertex(const string& label){
    if(!labelToIndex.count(label)){
        throw invalid_argument("Vertex '" + label + "' does not exists.");
    }

    int index = labelToIndex[label];
    vertices[index].active = false;
    adjList[index].clear();

    for (auto& edges : adjList) {
        edges.erase(
            remove_if(edges.begin(), edges.end(),
                [index](const Edge& e) { return e.to == index; }),
            edges.end());
    }

    labelToIndex.erase(label);

    len--;
}

int Graph::getLenght(){
    return len;
}

vector<string> Graph::getNeighbors(const string& label){
    if(!labelToIndex.count(label)){
        throw invalid_argument("Vertex '" + label + "' does not exists.");
    }

    int index = labelToIndex[label];
    vector<string> neighbors;

    for(Edge neighbor : adjList[index]){
        string neighborLabel = vertices[neighbor.to].label;
        neighbors.push_back(neighborLabel);
    }

    return neighbors;
}

vector<Edge> Graph::getNeighborsInternal(int vertex){
    if (vertex < 0) {
        throw std::invalid_argument("Vertex index cannot be negative.");
    }
    
    if (vertex >= vertices.size()) {
        throw std::invalid_argument("Vertex index '" + std::to_string(vertex) + "' is out of bounds.");
    }

    if (!vertices[vertex].active) {
        throw std::invalid_argument("Vertex at position '" + std::to_string(vertex) + "' is inactive.");
    }
    
    return adjList[vertex];
}

unordered_map<string, int> Graph::getLabeltoIndex(){
    return labelToIndex;
}

vector<Vertex> Graph::getVertices(){
    return vertices;
}

void Graph::print() const {
    for(int i = 0; i < int(adjList.size()); i++){

        if(vertices[i].active){
            
            cout << vertices[i].label << ": ";

            for(int j = 0; j < int(adjList[i].size()); j++ ){
                cout << "(" << vertices[adjList[i][j].to].label << ", " << adjList[i][j].weight << "); ";
            }
            cout << "\n";
        }
    }
}

pair<vector<string>, double> Graph::dijkstra(const string& from, const string& to) {
    vector<tuple<double, int, int>> d(adjList.size(), make_tuple(numeric_limits<double>::max(), -1, 0));
    vector<bool> visited(vertices.size(), false);

    for (int i = 0; i < vertices.size(); i++) {
        get<1>(d[i]) = i;
    }

    priority_queue<tuple<double, int, int>, vector<tuple<double, int, int>>, greater<>> minHeap;

    int indexFrom = labelToIndex[from];
    int indexTo = labelToIndex[to];

    get<0>(d[indexFrom]) = 0.0;
    get<2>(d[indexFrom]) = indexFrom;

    minHeap.push({0.0, indexFrom, indexFrom});

    while (!minHeap.empty()) {
        auto top = minHeap.top();
        double dist = get<0>(top);
        int u = get<1>(top);
        minHeap.pop();

        visited[u] = true;

        if (u == indexTo) break;

        for (Edge neighbor : adjList[u]) {
            if (!visited[neighbor.to] && get<0>(d[neighbor.to]) > dist + neighbor.weight) {
                get<0>(d[neighbor.to]) = dist + neighbor.weight;
                get<2>(d[neighbor.to]) = u;
                minHeap.push({get<0>(d[neighbor.to]), neighbor.to, u});
            }
        }
    }

    if (!visited[indexTo]) {
        return {{}, numeric_limits<double>::max()};
    }

    return Utils::reconstructPath(*this, d, indexFrom, indexTo);
}

pair<vector<string>, int> Graph::DFS(const string& from, const string& to){

    stack<pair<int, int>> stack;
    vector<bool> visited(vertices.size(), false);
    vector<tuple<double, int, int>> d(int(adjList.size()), make_tuple(0, -1, 0));
    
    for(int i = 0; i < vertices.size(); i++) get<1>(d[i]) = i;

    int indexFrom = labelToIndex[from];
    int indexTo = labelToIndex[to];

    stack.push(make_pair(indexFrom, indexFrom));

    while(!stack.empty()){

        auto top = stack.top();
        int u = top.first;
        visited[u] = true;
        stack.pop();

        if(u == indexTo) break;

        for(Edge neighbor : adjList[u]){
            if(!visited[neighbor.to]){
                stack.push(make_pair(neighbor.to, u));
                get<0>(d[neighbor.to]) = get<0>(d[u]) + 1;
                get<2>(d[neighbor.to]) = u;
            }
        }
    }

    if (!visited[indexTo]) {
        return { {}, numeric_limits<int>::max()};
    }

    for(tuple<double, int, int> value : d){
        cout << get<0>(value) << " " << get<1>(value) << " " <<get<2>(value) << endl;
    }

    return Utils::reconstructPath(*this, d, indexFrom, indexTo); 
}

pair<vector<string>, int> Graph::BFS(const string& from, const string& to){

    queue<pair<int, int>> queue;
    vector<bool> visited(vertices.size(), false);
    vector<tuple<double, int, int>> d(int(adjList.size()), make_tuple(0, -1, 0));
    
    for(int i = 0; i < vertices.size(); i++) get<1>(d[i]) = i;

    int indexFrom = labelToIndex[from];
    int indexTo = labelToIndex[to];

    queue.push(make_pair(indexFrom, indexFrom));

    while(!queue.empty()){

        auto front = queue.front();
        int u = front.first;
        visited[u] = true;
        queue.pop();

        if(u == indexTo) break;

        for(Edge neighbor : adjList[u]){
            if(!visited[neighbor.to]){
                queue.push(make_pair(neighbor.to, u));
                get<0>(d[neighbor.to]) = get<0>(d[u]) + 1;
                get<2>(d[neighbor.to]) = u;
            }
        }
    }

    if (!visited[indexTo]) {
        return { {}, numeric_limits<int>::max()};
    }

    for(tuple<double, int, int> value : d){
        cout << get<0>(value) << " " << get<1>(value) << " " <<get<2>(value) << endl;
    }

    return Utils::reconstructPath(*this, d, indexFrom, indexTo); 
}