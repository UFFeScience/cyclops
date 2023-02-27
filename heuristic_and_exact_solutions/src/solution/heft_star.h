/**
 * \file src/solution/heft.h
 * \brief Contains the \c Grch class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2021
 *
 * This header file contains the \c Grch class that construct
 * several \c Solution objects by selecting tasks randomly inside a restrict candidate list.
 */

#ifndef APPROXIMATE_SOLUTIONS_SRC_SOLUTION_HEFT_STAR_ALGORITHM_H_
#define APPROXIMATE_SOLUTIONS_SRC_SOLUTION_HEFT_STAR_ALGORITHM_H_

#include "src/solution/algorithm.h"
#include <list>

class HeftStar : public Algorithm {
 public:
  ///
  HeftStar() = default;

  /// Default destructor
//  ~HeftStar() override = default;

  /// Schedule the avail task, one-by-one
  Solution ScheduleAvailTasks(std::list<std::shared_ptr<Activation>> avail_activations, Solution& solution);

  ///
  [[nodiscard]] std::string GetName() const override { return name_; }

  ///
  void Run() override;

 private:

  std::string name_ = "HeftStar";
};  // end of class GreedyAlgorithm

#endif  // APPROXIMATE_SOLUTIONS_SRC_SOLUTION_HEFT_STAR_ALGORITHM_H_
