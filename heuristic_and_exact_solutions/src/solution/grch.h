/**
 * \file src/solution/greedy_randomized_constructive_heuristic.h
 * \brief Contains the \c Grch class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2020
 *
 * This header file contains the \c Grch class that construct
 * several \c Solution objects by selecting tasks randomly inside a restrict candidate list.
 */

#ifndef APPROXIMATIVE_SOLUTIONS_SRC_SOLUTION_GRCH_H_
#define APPROXIMATIVE_SOLUTIONS_SRC_SOLUTION_GRCH_H_

#include "src/solution/algorithm.h"

class Grch : public Algorithm {
 public:
  ///
  Grch() = default;

  /// Default destructor
  ~Grch() override = default;

  /// Schedule the avail task, one-by-one
  void ScheduleAvailTasks(std::list<Task*> avail_tasks, Solution& solution);

  ///
  void Run();

 private:
};  // end of class GreedyAlgorithm

#endif  // APPROXIMATIVE_SOLUTIONS_SRC_SOLUTION_GRCH_H_
