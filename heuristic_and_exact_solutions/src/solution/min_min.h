/**
 * \file src/solution/min_min_algorithm.h
 * \brief Contains the \c MinMin class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2021
 *
 * This header file contains the \c MinMin class that handles different execution modes.
 */

#ifndef APPROXIMATE_SOLUTIONS_SRC_SOLUTION_MIN_MIN_ALGORITHM_H_
#define APPROXIMATE_SOLUTIONS_SRC_SOLUTION_MIN_MIN_ALGORITHM_H_

#include <list>
#include "src/solution/algorithm.h"

/**
 * \class MinMin min_min_algorithm.h "src/solution/min_min_algorithm.h"
 * \brief Implements the business logic for the execution of the heuristic
 */
class MinMin : public Algorithm {
 public:
  /// Default constructor
  MinMin() = default;

  /// Schedule the avail task, one-by-one
  void ScheduleAvailTasks(std::list<std::shared_ptr<Activation>>, Solution&);

  ///
  [[nodiscard]] std::string GetName() const override { return name_; }

  /// Create variables; initialize variable; schedule tasks in parts; print solution
  void Run() override;

 private:
  std::string name_ = "Heft";
};

#endif  // APPROXIMATE_SOLUTIONS_SRC_SOLUTION_MIN_MIN_ALGORITHM_H_
