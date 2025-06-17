#ifndef DIRECTED_GRAPH_H
#define DIRECTED_GRAPH_H
#define GRAPH_INTERNAL_ACCESS

#include "Graph.h"
#include <string>

class DirectedGraph : public Graph {
   public:
    void addEdge(const string& from, const string& to, double weight = 1.0);
    void removeEdge(const string& from, const string& to);
};

#endif