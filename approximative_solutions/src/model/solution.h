/**
 * \file src/model/solution.h
 * \brief Contains the \c Solution class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \date 2020
 *
 * This header file contains the \c Solution class.
 */

#ifndef SRC_MODEL_SOLUTION_H_
#define SRC_MODEL_SOLUTION_H_

#include <iostream>
#include <vector>
#include <list>
#include <limits>
#include <algorithm>
#include <random>

#include "src/solution/algorithm.h"

class Algorithm;

struct vm_time {
  std::vector<std::pair<double, double>> life;
  double last;
  double static_vm = false;
  double storage = 0.0;
  int count_file = 0;
};

class Solution {

public:
  Solution(Algorithm* algorithm);

  Solution(const Solution &other)
      : algorithm_(other.algorithm_),
        allocation_(other.allocation_),
        ordering_(other.ordering_),
        height_soft_(other.height_soft_),
        time_vector_(other.time_vector_),
        queue_(other.queue_),
        start_time_vector_(other.start_time_vector_),
        makespan_(other.makespan_),
        lambda_(other.lambda_),
        transfer_size_(other.transfer_size_),
        scheduler_(other.scheduler_),
        vm_queue_(other.vm_queue_) {}  // copy constructor


  //virtual ~Solution();
  ~Solution() { algorithm_ = nullptr; }

  /// Getter for time_
  double get_fitness() const { return makespan_; }

  /// Getter for allocation_
  std::vector<size_t> get_allocation() const { return allocation_; }

  /// Getter for allocation_
  std::vector<size_t> get_ordering() const { return ordering_; }

  /// Add a Vm or a Storage to a task or file
  void SetAllocation(size_t position, size_t storage_id) {
    allocation_[position] = storage_id;
  }

  /// Set a task id to a specific position in this vector containing the order of execution
  void SetOrdering(size_t position, size_t task_id) {
    ordering_[position] = task_id;
  }

  /// Add a task id to the end of the ordering vector
  void AddOrdering(size_t task_id) {
    ordering_.push_back(task_id);
  }

  double AllocateTask(Task task, VirtualMachine vm);

  double AllocateOneOutputFileGreedily(File* file,
                                       VirtualMachine vm);

  double ComputeTaskReadTime(Task& task,
                             VirtualMachine& vm);

  double AllocateOutputFiles(Task& task,
                             VirtualMachine& vm);

  double ComputeTaskStartTime(size_t task,
                              size_t vm);

  // double ComputeTaskFitness(Task task,
  //                           VirtualMachine vm,
  //                           std::vector<double>& ft_vector,
  //                           std::vector<double>& queue,
  //                           std::vector<size_t>& allocation);

  // double CalculateObjectiveFunction();

  // void InsertOrdering(std::list<size_t>& list) {
  //   ordering_.clear();
  //   ordering_.insert(ordering_.end(), list.begin(), list.end());
  // }

  /// Compute the makespan
  double ComputeMakespan(bool check_storage = true, bool check_sequence = false);

  /// Compute the cost
  double ComputeCost();

  /// Calculate de ObjectiveFunction using the algorithm's \c phi_, \c time_max and \c cost_max values
  double CalculateObjectiveFunction();

  inline bool CheckTaskSequence(size_t);

  inline bool checkFiles();

  double MakespanOfAllocatedTask(Task task, VirtualMachine vm);

  double CalculateSecurityExposure();

  double CalculateCost();

  std::ostream& write(std::ostream& os) const;

  Solution& operator=(const Solution&) = default;

  friend std::ostream& operator<<(std::ostream& os, const Solution& a) {
    return a.write(os);
  }

private:

  // int ComputeHeightSoft(size_t node);

  // void Encode();

  // inline double TaskStartTime(Task task, VirtualMachine vm, const std::vector<double>& queue);

  // inline double TaskReadTime(Task task, VirtualMachine vm);

  inline double ComputeTaskWriteTime(Task task, VirtualMachine vm);

  inline double FileTransferTime(File* file,
                                 Storage vm1,
                                 Storage vm2,
                                 bool check_constraints = false);

  Algorithm* algorithm_;

  /// Allocation of task and files in theirs respective VM or storages
  std::vector<size_t> allocation_;

  std::vector<size_t> ordering_;

  std::vector<int> height_soft_;

  std::vector<double> time_vector_;

  std::vector<double> queue_;

  std::vector<double> start_time_vector_;

  double makespan_;

  double lambda_;

  double transfer_size_;

  std::unordered_map<int, std::vector<std::string>> scheduler_;

  std::unordered_map<size_t, std::vector<size_t>> vm_queue_;

};

#endif  // SRC_MODEL_SOLUTION_H_
