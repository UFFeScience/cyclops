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


  double ComputeFileWriteTime(File* file,
                              VirtualMachine vm,
                              std::vector<size_t>& allocation_output_files);

  double ComputeTaskReadTime(Task& task,
                             VirtualMachine& vm,
                             std::vector<size_t>& allocation_output_files);

  double ComputeTaskWriteTime(Task& task,
                             VirtualMachine& vm,
                             std::vector<size_t>& allocation_output_files);

  double ComputeTaskStartTime(size_t task,
                              size_t vm,
                              std::vector<double> ft_vector,
                              std::vector<double> queue);

  double ComputeTaskFitness(Task task,
                            VirtualMachine vm,
                            std::vector<double>& ft_vector,
                            std::vector<double>& queue,
                            std::vector<size_t>& allocation);

  double CalculateObjectiveFunction();

  void ScheduleAvailTasks(std::list<Task> avail_tasks,
                       std::vector<double>& ft_vector,
                       std::vector<double>& queue,
                       std::vector<size_t>& file_place,
                       std::list<size_t>& task_ordering);

  /// Builds the required geometric representation depending on the simulation
  void Run(void);

 private:
};  // end of class GreedyAlgorithm

#endif  // SRC_SOLUTION_MIN_MIN_H_
