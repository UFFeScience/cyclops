/**
 * \file src/solution/greedy_randomized_constructive_heuristic.h
 * \brief Contains the \c GreedyRandomizedConstructiveHeuristic class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2021
 *
 * This header file contains the \c GreedyRandomizedConstructiveHeuristic class that construct
 * several \c Solution objects by selecting tasks randomly inside a restrict candidate list.
 */

#ifndef APPROXIMATE_SOLUTIONS_SRC_SOLUTION_GREEDY_ALGORITHM_H_
#define APPROXIMATE_SOLUTIONS_SRC_SOLUTION_GREEDY_ALGORITHM_H_

#include "src/solution/algorithm.h"
#include <list>

class GreedyRandomizedConstructiveHeuristic : public Algorithm {
 public:
  ///
  GreedyRandomizedConstructiveHeuristic() = default;

  /// Default destructor
  ~GreedyRandomizedConstructiveHeuristic() override = default;

  /// Schedule the avail task, one-by-one
  void ScheduleAvailTasks(std::list<std::shared_ptr<Activation>> avail_tasks, Solution& solution);

  ///
  [[nodiscard]] std::string GetName() const override { return name_; }

  ///
  void Run() override;

 private:

  std::string name_ = "GreedyRandomizedConstructiveHeuristic";
};  // end of class GreedyAlgorithm

#endif  // APPROXIMATE_SOLUTIONS_SRC_SOLUTION_GREEDY_ALGORITHM_H_
