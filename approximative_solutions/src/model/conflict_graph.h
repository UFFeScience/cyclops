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

class ConflictGraph {
 public:
  /// Parametrised constructor
  // explicit ConflictGraph(const int size) {
  //   for (int i = 0; i < size; ++i) {
  //     std::vector<int> row(static_cast<long unsigned int>(size), -1);
  //     _conflicts.push_back(row);
  //   }
  // }

  void initialize(const int size) {
    for (int i = 0; i < size; ++i) {
      std::vector<int> row(static_cast<long unsigned int>(size), -1);
      _conflicts.push_back(row);
    }
  }

  /// Getter for _id
  int getConflict(const int i, const int j) const {
    return _conflicts[static_cast<long unsigned int>(i)][static_cast<long unsigned int>(j)];
  }

  /// Setter for _id
  void setConflict(const int i, const int j, const int value) {
    _conflicts[static_cast<long unsigned int>(i)][static_cast<long unsigned int>(j)] = value;
  }

 private:
  std::vector<std::vector<int>> _conflicts;
};  // end of class ConflictGraph

#endif  // SRC_MODEL_CONFLICT_GRAPH_H_
