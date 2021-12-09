/**
 * \file src/solution/greedy_randomized_constructive_heuristic.h
 * \brief Contains the \c Grasp class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2021
 *
 * This header file contains the \c Grasp class that construct
 * several \c Solution objects by selecting tasks randomly inside a restrict candidate list.
 */

#ifndef APPROXIMATE_SOLUTIONS_SRC_SOLUTION_GRASP_H_
#define APPROXIMATE_SOLUTIONS_SRC_SOLUTION_GRASP_H_

#include "src/solution/algorithm.h"

class Grasp : public Algorithm {
 public:
  ///
  Grasp() = default;

  /// Default destructor
  ~Grasp() override = default;

  /// Schedule the avail task, one-by-one
  void ScheduleAvailTasks(std::list<std::shared_ptr<Activation>>, Solution&);

  ///
  [[nodiscard]] std::string GetName() const override { return name_; }

  ///
  void Run() override;

 private:

  std::string name_ = "grasp";
};  // end of class Grasp

#endif  // APPROXIMATE_SOLUTIONS_SRC_SOLUTION_GRASP_H_
