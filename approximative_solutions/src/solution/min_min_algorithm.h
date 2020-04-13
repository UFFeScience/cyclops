/**
 * \file src/solution/MinMin.h
 * \brief Contains the \c MinMin class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2020
 *
 * This header file contains the \c MinMin class that handles different execution modes.
 */

#ifndef APPROXIMATIVE_SOLUTIONS_SRC_SOLUTION_MIN_MIN_ALGORITHM_H_
#define APPROXIMATIVE_SOLUTIONS_SRC_SOLUTION_MIN_MIN_ALGORITHM_H_

#include <list>
#include "src/solution/algorithm.h"

/**
 * \class MinMinAlgorithm min_min_algorithm.h "src/solution/min_min_algorithm.h"
 * \brief Implements the business logic for the execution of the heuristic
 */
class MinMinAlgorithm : public Algorithm {
 public:
  /// Default constructor
  MinMinAlgorithm() = default;

  /// Default destructor
  ~MinMinAlgorithm() = default;

  /// Schedule the avail task, one-by-one
  void ScheduleAvailTasks(std::list<Task> avail_tasks, Solution& solution);

  /// Create variables; initialize variable; schedule tasks in parts; print solution
  void Run(void);

 private:
};  // end of class MinMinAlgorithm

#endif  // APPROXIMATIVE_SOLUTIONS_SRC_SOLUTION_MIN_MIN_ALGORITHM_H_
