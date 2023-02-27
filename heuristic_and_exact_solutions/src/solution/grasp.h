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

#include <src/statistic/write_to_local_search_file.h>
#include <src/statistic/write_to_ttt_file.h>
#include <src/statistic/write_to_iteration.h>
#include "src/solution/algorithm.h"
#include "grch.h"

class Grasp : public Grch {
 public:
  ///
  Grasp() = default;

  ///
  [[nodiscard]] std::string GetName() const override { return name_; }

  ///
  void localSearch(Solution&);

  ///
  void Run() override;

 private:

  std::string name_ = "grasp";

  WriteToTttFile tttFile{"grasp"};

  WriteToIteration iterationFile{"grasp"};

  WriteToLocalSearchFile lsFile1{"grasp", 1};
  WriteToLocalSearchFile lsFile2{"grasp", 2};
  WriteToLocalSearchFile lsFile3{"grasp", 3};
};

#endif  // APPROXIMATE_SOLUTIONS_SRC_SOLUTION_GRASP_H_
