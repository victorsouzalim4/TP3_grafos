#include "edge.h"

using namespace std;

Edge::Edge(int to, double weight) : to(to), weight(weight) {}

void Edge::print() const{
    cout << "(" << to << " ," << weight << "); ";
}