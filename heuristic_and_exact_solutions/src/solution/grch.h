/**
 * \file src/solution/grch.h
 * \brief Contains the \c Heft class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2021
 *
 * This header file contains the \c Heft class that construct
 * several \c Solution objects by selecting tasks randomly inside a restrict candidate list.
 */

#ifndef APPROXIMATE_SOLUTIONS_SRC_SOLUTION_GRCH_H_
#define APPROXIMATE_SOLUTIONS_SRC_SOLUTION_GRCH_H_

#include "src/solution/algorithm.h"
#include <list>
#include <algorithm>
#include <vector>       // std::vector

class Grch : public Algorithm {
 public:
  ///
  Grch() = default;

  /// Default destructor
//  ~Grch() override = default;

  /// Schedule the avail task, one-by-one
  Solution ScheduleAvailTasks(std::vector<std::shared_ptr<Activation>> avail_activations, Solution& solution);

  ///
  [[nodiscard]] std::string GetName() const override { return name_; }

  ///
  void Run() override;

 private:

  std::string name_ = "grch";
};

#endif  // APPROXIMATE_SOLUTIONS_SRC_SOLUTION_GRCH_H_
