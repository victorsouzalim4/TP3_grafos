#include <iostream>
#include "directed_Graph.h"
#include "Undirected_Graph.h"
#include "Utils/segmentation.h"
#include "vertex.h"

using namespace std;

int main(){

    UndirectedGraph g;
    Segmentation s;

    g.addVertex("A");
    g.addVertex("B");
    g.addVertex("C");
    g.addVertex("D");
    g.addVertex("E");


    try {
        g.addEdge("A", "B", 3.5);
        g.addEdge("B", "C");
        g.addEdge("B", "D", 800.0);
        g.addEdge("C", "D", 2000.0);
        g.addEdge("E", "D", 5.0);
        g.addEdge("C", "E", 4000.0);
        s.segmentGraph(g, 5);
        // auto path = g.DFS("A", "E");
        // cout << path.second << endl;
        // for(string v : path.first){
        //     cout << v << "-> ";
        // }

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }



    return 0;
}