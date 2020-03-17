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

#include "src/solution/algorithm.h"
#include "src/solution/min_min.h"

class MinMin : public Algorithm {
 public:
  // /**
  //  * \brief Parametrised constructor.
  //  */
  // GreedyAlgorithm(const std::string workflow_file_name,
  //                 const std::string cluster_file_name,
  //                 const std::string conflict_graph_file_name)
  //   : Algorithm(workflow_file_name, cluster_file_name, conflict_graph_file_name) { }

  /// Builds the required geometric representation depending on the simulation
  void run(void);

 private:
};  // end of class GreedyAlgorithm

#endif  // SRC_SOLUTION_MIN_MIN_H_
