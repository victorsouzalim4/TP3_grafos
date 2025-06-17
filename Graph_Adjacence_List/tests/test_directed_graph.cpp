
#include <gtest/gtest.h>
#include "Directed_Graph.h"

//DirectedGraph

TEST(DirectedGraphTest, AddEdgeSuccessfully) {
    DirectedGraph g;
    g.addVertex("A");
    g.addVertex("B");

    ASSERT_NO_THROW(g.addEdge("A", "B", 2.5));

    auto neighbors = g.getNeighbors("A");
    ASSERT_EQ(neighbors.size(), 1);
    EXPECT_EQ(neighbors[0], "B");
}

TEST(DirectedGraphTest, AddEdgeFromNonexistentVertexThrows) {
    DirectedGraph g;
    g.addVertex("B");

    EXPECT_THROW(g.addEdge("A", "B", 2.5), std::invalid_argument);
}

TEST(DirectedGraphTest, AddEdgeToNonexistentVertexThrows) {
    DirectedGraph g;
    g.addVertex("A");

    EXPECT_THROW(g.addEdge("A", "B", 2.5), std::invalid_argument);
}

TEST(DirectedGraphTest, RemoveEdgeSuccessfully) {
    DirectedGraph g;
    g.addVertex("A");
    g.addVertex("B");
    g.addEdge("A", "B", 3.0);

    ASSERT_NO_THROW(g.removeEdge("A", "B"));

    auto neighbors = g.getNeighbors("A");
    EXPECT_TRUE(neighbors.empty());
}

TEST(DirectedGraphTest, RemoveNonexistentEdgeThrows) {
    DirectedGraph g;
    g.addVertex("A");
    g.addVertex("B");

    EXPECT_THROW(g.removeEdge("A", "B"), std::runtime_error);
}

TEST(DirectedGraphTest, RemoveEdgeFromNonexistentVertexThrows) {
    DirectedGraph g;
    g.addVertex("B");

    EXPECT_THROW(g.removeEdge("A", "B"), std::invalid_argument);
}

TEST(DirectedGraphTest, RemoveEdgeToNonexistentVertexThrows) {
    DirectedGraph g;
    g.addVertex("A");

    EXPECT_THROW(g.removeEdge("A", "B"), std::invalid_argument);
}
