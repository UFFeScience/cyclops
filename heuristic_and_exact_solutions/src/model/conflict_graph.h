/**
 * \file src/model/conflict_graph.h
 * \brief Contains the \c ConflictGraph class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2024
 *
 * This header file contains the \c ConflictGraph class.
 */

#ifndef APPROXIMATE_SOLUTIONS_SRC_MODEL_CONFLICT_GRAPH_H_
#define APPROXIMATE_SOLUTIONS_SRC_MODEL_CONFLICT_GRAPH_H_


#include <vector>
#include <utility>

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
    void Redefine(size_t size) {
        files_size_ = size;
        conflicts_.resize(size * size, 0);
    }

    /// Return the conflict value between the file with ID \c line and file with ID \c column
    int ReturnConflict(size_t line, size_t column) {
        return conflicts_[(line * files_size_) + column];
    }

    /// Add a new conflict value between the file with ID \c line and file with ID \c column
    void AddConflict(const size_t line, const size_t column, const int value) {

        if (value == 0) {  // Hard constraint
            conflicts_[(line * files_size_) + column] = -1;
            conflicts_[(column * files_size_) + line] = -1;
        } else {
            conflicts_[(line * files_size_) + column] = value;
            conflicts_[(column * files_size_) + line] = value;
        }

        if (value > 0) {
            maximum_of_soft_constraints += static_cast<size_t>(value);
        }
    }

private:
    ///
    size_t files_size_;

    /// The matrix containing all conflict values
    std::vector<int> conflicts_;

    /// A positive integer that contain the sum of all soft conflict value
    size_t maximum_of_soft_constraints = 0ul;
};


#endif  // APPROXIMATE_SOLUTIONS_SRC_MODEL_CONFLICT_GRAPH_H_
