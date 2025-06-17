#include "vertex.h"
#include <string>

using namespace std;

Vertex::Vertex(string l) : label(l), heuristicWeight(0.0), active(true) {}
Vertex::Vertex(string l, double hw) : label(l), heuristicWeight(hw), active(true) {}
