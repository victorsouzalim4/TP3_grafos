#ifndef VERTEX_H
#define VERTEX_H

#include <string>

using namespace std;

class Vertex {
    public:
        string label;
        double heuristicWeight;
        bool active;

        Vertex(string l);
        Vertex(string l, double hw);
};

#endif