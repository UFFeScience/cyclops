/**
 * \file src/model/conflict_graph.h
 * \brief Contains the \c ConflictGraph class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \date 2020
 *
 * This header file contains the \c ConflictGraph class.
 */

#ifndef SRC_MODEL_CONFLICT_GRAPH_H_
#define SRC_MODEL_CONFLICT_GRAPH_H_

#include <vector>
#include <utility>
#include "src/data_structure/matrix.h"

class ConflictGraph {
public:
  ConflictGraph() = default;
  // ConflictGraph() = delete;
  /// Parametrised constructor
  // explicit ConflictGraph(const int size) {
  //   for (int i = 0; i < size; ++i) {
  //     std::vector<int> row(static_cast<long unsigned int>(size), -1);
  //     _conflicts.push_back(row);
  //   }
  // }

  ConflictGraph(size_t size) : conflicts_(size, size, -1) {
    // for (size_t i = 0; i < size; ++i) {
    //   conflicts_.push_back(std::vector<std::pair<size_t, int>>());
    // }

  }

  ~ConflictGraph() = default;

  /// Getter for _id
  size_t get_number_of_soft_constraints() const { return number_of_soft_constraints; }

  void Redefine(const int size) {
    conflicts_.redefine(size, size, -1);
    // for (int i = 0; i < size; ++i) {
    //   std::vector<int> row(size, -1);
    //   _conflicts.push_back(row);
    // }

    // for (int i = 0; i < size; ++i) {
    //   conflicts_.push_back(std::vector<std::pair<size_t, int>>());
    // }
  }

  int ReturnConflict(size_t i, size_t j) {
    return conflicts_(i, j);
  }

  /// Getter for _id
  // std::vector<std::pair<size_t, int>>& ReturnConflictVector(const size_t i) {
  //   return conflicts_[i];
  // }

  /// Setter for _id
  void AddConflict(const size_t i, const size_t j, const int value) {
    // conflicts_[i].push_back(std::make_pair(j, value));
    // conflicts_[j].push_back(std::make_pair(i, value));;
    conflicts_(i, j) = value;
    conflicts_(j, i) = value;

    if (value == 0) {
      number_of_soft_constraints += 1;
    }
  }

private:
  // std::vector<std::vector<std::pair<size_t, int>>> conflicts_;
  Matrix<int> conflicts_;

  size_t number_of_soft_constraints = 0ul;
};  // end of class ConflictGraph

#endif  // SRC_MODEL_CONFLICT_GRAPH_H_
