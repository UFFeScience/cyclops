/**
 * \file src/solution/cplex.h
 * \brief Contains the \c Cplex class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2020
 *
 * This header file contains the \c Cplex class.
 */

#ifndef APPROXIMATIVE_SOLUTIONS_SRC_CPLEX_H_
#define APPROXIMATIVE_SOLUTIONS_SRC_CPLEX_H_

#include "src/solution/algorithm.h"

class Cplex : public Algorithm {
 public:
  ///
  Cplex() = default;

  /// Default destructor
  ~Cplex() = default;

  /// Schedule the avail task, one-by-one
  void ScheduleAvailTasks(std::list<Task*> avail_tasks, Solution& solution);

  ///
  void Run(void);

 private:
};  // end of class GreedyAlgorithm

#endif  // APPROXIMATIVE_SOLUTIONS_SRC_CPLEX_H_

