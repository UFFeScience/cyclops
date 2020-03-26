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

  ~MinMinAlgorithm();
  // /**
  //  * \brief Parametrised constructor.
  //  */
  // GreedyAlgorithm(const std::string workflow_file_name,
  //                 const std::string cluster_file_name,
  //                 const std::string conflict_graph_file_name)
  //   : Algorithm(workflow_file_name, cluster_file_name, conflict_graph_file_name) { }
  double ComputeStartTime(size_t task,
                          size_t vm,
                          std::vector<double> ft_vector,
                          std::vector<double> queue);

  double ComputeFitness(Task task,
                        VirtualMachine vm,
                        std::vector<double>& ft_vector,
                        std::vector<double>& queue,
                        std::vector<size_t>& file_place,
                        double lambda);

  void schedule(std::list<Task> avail_tasks,
                std::vector<double>& ft_vector,
                std::vector<double>& queue,
                std::vector<size_t>& file_place,
                std::list<size_t>& task_ordering,
                double lambda);

  /// Builds the required geometric representation depending on the simulation
  void Run(void);

 private:
};  // end of class GreedyAlgorithm

#endif  // SRC_SOLUTION_MIN_MIN_H_
