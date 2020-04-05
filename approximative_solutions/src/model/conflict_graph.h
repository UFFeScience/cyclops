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

class ConflictGraph {
public:
  ConflictGraph() = default;
  /// Parametrised constructor
  // explicit ConflictGraph(const int size) {
  //   for (int i = 0; i < size; ++i) {
  //     std::vector<int> row(static_cast<long unsigned int>(size), -1);
  //     _conflicts.push_back(row);
  //   }
  // }

  ConflictGraph(size_t size) {
    for (size_t i = 0; i < size; ++i) {
      conflicts_.push_back(std::vector<std::pair<size_t, int>>());
    }
  }

  ~ConflictGraph() = default;

  void Redefine(const int size) {
    // for (int i = 0; i < size; ++i) {
    //   std::vector<int> row(size, -1);
    //   _conflicts.push_back(row);
    // }
    for (int i = 0; i < size; ++i) {
      conflicts_.push_back(std::vector<std::pair<size_t, int>>());
    }
  }

  /// Getter for _id
  std::vector<std::pair<size_t, int>>& ReturnConflictVector(const size_t i) {
    return conflicts_[i];
  }

  /// Setter for _id
  void AddConflict(const size_t i, const size_t j, const int value) {
    conflicts_[i].push_back(std::make_pair(j, value));
    conflicts_[j].push_back(std::make_pair(i, value));;
  }

private:
  std::vector<std::vector<std::pair<size_t, int>>> conflicts_;
};  // end of class ConflictGraph

#endif  // SRC_MODEL_CONFLICT_GRAPH_H_
