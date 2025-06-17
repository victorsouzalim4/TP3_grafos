#include <gtest/gtest.h>
#include "Undirected_Graph.h"
#include <algorithm>

TEST(UndirectedGraphTest, AddEdgeSuccessfullyCreatesBidirectionalLink) {
    UndirectedGraph g;
    g.addVertex("A");
    g.addVertex("B");

    g.addEdge("A", "B", 1.5);

    auto neighborsA = g.getNeighbors("A");
    auto neighborsB = g.getNeighbors("B");

    EXPECT_EQ(neighborsA.size(), 1);
    EXPECT_EQ(neighborsB.size(), 1);
    EXPECT_EQ(neighborsA[0], "B");
    EXPECT_EQ(neighborsB[0], "A");
}

TEST(UndirectedGraphTest, AddEdgeThrowsIfVertexMissing) {
    UndirectedGraph g;
    g.addVertex("A");

    EXPECT_THROW(g.addEdge("A", "B", 1.0), std::invalid_argument);
    EXPECT_THROW(g.addEdge("B", "A", 1.0), std::invalid_argument);
    EXPECT_THROW(g.addEdge("X", "Y", 1.0), std::invalid_argument);
}

TEST(UndirectedGraphTest, RemoveEdgeSuccessfullyRemovesBidirectionalLink) {
    UndirectedGraph g;
    g.addVertex("A");
    g.addVertex("B");
    g.addEdge("A", "B", 2.0);

    ASSERT_NO_THROW(g.removeEdge("A", "B"));

    auto neighborsA = g.getNeighbors("A");
    auto neighborsB = g.getNeighbors("B");

    EXPECT_TRUE(neighborsA.empty());
    EXPECT_TRUE(neighborsB.empty());
}

TEST(UndirectedGraphTest, RemoveEdgeThrowsIfVertexMissing) {
    UndirectedGraph g;
    g.addVertex("A");

    EXPECT_THROW(g.removeEdge("A", "B"), std::invalid_argument);
    EXPECT_THROW(g.removeEdge("X", "A"), std::invalid_argument);
}

TEST(UndirectedGraphTest, RemoveEdgeThrowsIfEdgeDoesNotExist) {
    UndirectedGraph g;
    g.addVertex("A");
    g.addVertex("B");

    EXPECT_THROW(g.removeEdge("A", "B"), std::runtime_error);

    g.addEdge("A", "B", 1.0);
    g.removeEdge("A", "B");

    // Edge already removed — should throw again
    EXPECT_THROW(g.removeEdge("A", "B"), std::runtime_error);
}

