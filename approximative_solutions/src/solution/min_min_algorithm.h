/**
 * \file src/solution/MinMin.h
 * \brief Contains the \c MinMin class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \date 2020
 *
 * This header file contains the \c MinMin class that handles different execution modes.
 */

#ifndef SRC_SOLUTION_MIN_MIN_H_
#define SRC_SOLUTION_MIN_MIN_H_

#include <list>
#include "src/solution/algorithm.h"

class MinMinAlgorithm : public Algorithm {
public:
  MinMinAlgorithm() = default;

  ~MinMinAlgorithm() = default;

  void ScheduleAvailTasks(std::list<Task> avail_tasks, Solution& solution);

  /// Builds the required geometric representation depending on the simulation
  void Run(void);
private:
};  // end of class GreedyAlgorithm

#endif  // SRC_SOLUTION_MIN_MIN_H_
