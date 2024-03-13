/**
 * \file src/model/conflict_graph.h
 * \brief Contains the \c ConflictGraph class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2020
 *
 * This header file contains the \c ConflictGraph class.
 */

#ifndef APPROXIMATE_SOLUTIONS_SRC_MODEL_CONFLICT_GRAPH_H_
#define APPROXIMATE_SOLUTIONS_SRC_MODEL_CONFLICT_GRAPH_H_

#include <vector>
#include <utility>
#include "src/data_structure/matrix.h"

/**
 * \class ConflictGraph conflict_graph.h "src/model/conflict_graph.h"
 * \brief Represents the Conflict Graph; store the conflict value between files for all pair of files
 */
class ConflictGraph {
public:
    /// Default constructor
    ConflictGraph() = default;

    /// Parameterized constructor with full member initialisation
    // ConflictGraph(size_t size) : conflicts_(size, size, -1) { }

    /// Default constructor
    ~ConflictGraph() = default;

    /// Getter for the accumulated values of soft constraints
    [[nodiscard]] size_t get_maximum_of_soft_constraints() const { return maximum_of_soft_constraints; }

    /// Redefine the size of the conflict graph
    void Redefine(const int size) {
        conflicts_.redefine(size, size, 0);
    }

    /// Return the conflict value between the file with ID \c i and file with ID \c j
    int ReturnConflict(size_t i, size_t j) {
        return conflicts_(static_cast<int>(i), static_cast<int>(j));
    }

    /// Add a new conflict value between the file with ID \c i and file with ID \c j
    void AddConflict(const size_t i, const size_t j, const int value) {
        conflicts_(static_cast<int>(i), static_cast<int>(j)) = value;
        conflicts_(static_cast<int>(j), static_cast<int>(i)) = value;

        if (value > 0) {
            maximum_of_soft_constraints += static_cast<size_t>(value);
        }
    }

private:
    /// The matrix containing all conflict values
    Matrix<int> conflicts_;

    /// A positive integer that contain the sum of all soft conflict value
    size_t maximum_of_soft_constraints = 0ul;
};

#endif  // APPROXIMATE_SOLUTIONS_SRC_MODEL_CONFLICT_GRAPH_H_
