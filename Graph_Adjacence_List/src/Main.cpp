#include <iostream>
#include "directed_Graph.h"
#include "Undirected_Graph.h"
#include "vertex.h"

using namespace std;

int main(){

    UndirectedGraph g;

    g.addVertex("A");
    g.addVertex("B");
    g.addVertex("C");
    g.addVertex("D");
    g.addVertex("E");


    try {
        g.addEdge("A", "B", 3.5);
        g.addEdge("B", "C");
        g.addEdge("B", "D", 2.0);
        g.addEdge("C", "D", 4.0);
        g.addEdge("C", "E", 3.0);
        auto path = g.DFS("A", "E");
        cout << path.second << endl;
        for(string v : path.first){
            cout << v << "-> ";
        }

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }



    return 0;
}