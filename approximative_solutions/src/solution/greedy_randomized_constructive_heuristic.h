/**
 * \file src/solution/greedy_randomized_constructive_heuristic.h
 * \brief Contains the \c GreedyRandomizedConstructiveHeuristic class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2020
 *
 * This header file contains the \c GreedyRandomizedConstructiveHeuristic class that construct
 * several \c Solution objects by selecting tasks randomly inside a restrict candidate list.
 */

#ifndef APPROXIMATIVE_SOLUTIONS_SRC_SOLUTION_GREEDY_ALGORITHM_H_
#define APPROXIMATIVE_SOLUTIONS_SRC_SOLUTION_GREEDY_ALGORITHM_H_

#include "src/solution/algorithm.h"

class GreedyRandomizedConstructiveHeuristic : public Algorithm {
 public:
  ///
  GreedyRandomizedConstructiveHeuristic() = default;

  /// Default destructor
  ~GreedyRandomizedConstructiveHeuristic() = default;

  /// Schedule the avail task, one-by-one
  void ScheduleAvailTasks(std::list<Task*> avail_tasks, Solution& solution);

  ///
  void Run(void);

 private:
};  // end of class GreedyAlgorithm

#endif  // APPROXIMATIVE_SOLUTIONS_SRC_SOLUTION_GREEDY_ALGORITHM_H_
