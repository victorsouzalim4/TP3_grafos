#include <gtest/gtest.h>
#include "Directed_Graph.h"
#include <algorithm> 

TEST(GraphTest, AddVertexIncreasesLength) {
    DirectedGraph g;

    EXPECT_EQ(g.getLenght(), 0);
    g.addVertex("A");
    EXPECT_EQ(g.getLenght(), 1);
    g.addVertex("B");
    EXPECT_EQ(g.getLenght(), 2);
}

TEST(GraphTest, AddDuplicateVertexDoesNotIncreaseLength) {
    DirectedGraph g;

    g.addVertex("A");
    g.addVertex("B");
    int lengthBefore = g.getLenght();

    g.addVertex("A");  // Duplicate
    EXPECT_EQ(g.getLenght(), lengthBefore);
}

TEST(GraphTest, GetNeighborsReturnsCorrectLabels) {
    DirectedGraph g;
    g.addVertex("A");
    g.addVertex("B");
    g.addVertex("C");

    g.addEdge("A", "B", 3.5);
    g.addEdge("A", "C", 1.0);

    auto neighbors = g.getNeighbors("A");
    ASSERT_EQ(neighbors.size(), 2);
    EXPECT_NE(std::find(neighbors.begin(), neighbors.end(), "B"), neighbors.end());
    EXPECT_NE(std::find(neighbors.begin(), neighbors.end(), "C"), neighbors.end());
}

TEST(GraphTest, GetNeighborsThrowsForInvalidVertex) {
    DirectedGraph g;
    g.addVertex("A");

    EXPECT_THROW(g.getNeighbors("Z"), std::invalid_argument);
}

TEST(GraphTest, RemoveVertexReducesLengthAndClearsEdges) {
    DirectedGraph g;
    g.addVertex("A");
    g.addVertex("B");

    g.addEdge("A", "B", 2.0);
    EXPECT_EQ(g.getLenght(), 2);

    g.removeVertex("B");

    EXPECT_EQ(g.getLenght(), 1);
    auto neighbors = g.getNeighbors("A");
    EXPECT_TRUE(neighbors.empty());
}

TEST(GraphTest, RemoveVertex_DoesNotThrowInitially) {
    DirectedGraph g;
    g.addVertex("A");
    g.addVertex("B");
    g.addEdge("A", "B", 5);

    // Deve remover "B" sem lançar exceção
    ASSERT_NO_THROW(g.removeVertex("B"));
}

TEST(GraphTest, RemoveVertex_ThrowsWhenAlreadyRemoved) {
    DirectedGraph g;
    g.addVertex("A");
    g.addVertex("B");
    g.addEdge("A", "B", 5);

    g.removeVertex("B");

    // Segunda remoção deve lançar exceção
    EXPECT_THROW(g.removeVertex("B"), std::invalid_argument);
}

TEST(GraphTest, RemoveVertex_RemovesOutgoingEdges) {
    DirectedGraph g;
    g.addVertex("A");
    g.addVertex("B");
    g.addEdge("A", "B", 5);

    g.removeVertex("B");

    auto neighbors = g.getNeighbors("A");
    EXPECT_TRUE(neighbors.empty());
}

TEST(GraphTest, AddAfterRemoveWithSameLabelCreatesNewEntry) {
    DirectedGraph g;
    g.addVertex("A");
    g.removeVertex("A");
    int newIdx = g.addVertex("A");

    EXPECT_EQ(g.getLenght(), 1);
    auto neighbors = g.getNeighbors("A");
    EXPECT_TRUE(neighbors.empty());
}

