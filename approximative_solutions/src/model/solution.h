/**
 * \file src/model/solution.h
 * \brief Contains the \c Solution class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2020
 *
 * This header file contains the \c Solution class.
 */

#ifndef APPROXIMATIVE_SOLUTIONS_SRC_MODEL_SOLUTION_H_
#define APPROXIMATIVE_SOLUTIONS_SRC_MODEL_SOLUTION_H_

#include <algorithm>
#include <iostream>
#include <list>
#include <limits>
#include <random>
#include <string>
#include <vector>

#include "src/solution/algorithm.h"

/// Foward declaration of the class Algorithm, needed because of the circular reference
class Algorithm;

/**
 * \class Solution solution.h "src/model/solution.h"
 * \brief Represents the solution for the execution of a Scientific Workflow
 */
class Solution {
 public:
  /// Constructor declaration
  explicit Solution(Algorithm* algorithm);

  /// Copy constructor
  Solution(const Solution &other) = default;
  // Solution(const Solution &other)
  //     : algorithm_(other.algorithm_),
  //       allocation_(other.allocation_),
  //       ordering_(other.ordering_),
  //       time_vector_(other.time_vector_),
  //       queue_(other.queue_),
  //       start_time_vector_(other.start_time_vector_),
  //       makespan_(other.makespan_),
  //       lambda_(other.lambda_),
  //       scheduler_(other.scheduler_),
  //       storage_queue_(other.storage_queue_) { }

  /// Destructor
  ~Solution() { algorithm_ = nullptr; }

  /// Getter for \c allocation_
  std::vector<size_t> get_allocation() const { return allocation_; }

  /// Getter for \c ordering_
  std::vector<size_t> get_ordering() const { return ordering_; }

  /// Getter for makespan of the solution
  double get_makespan() const { return makespan_; }

  /// Getter for cost of the solution
  double get_cost() const { return cost_; }

  /// Getter for \c objective_value_
  double get_objective_value() const { return objective_value_; }

  /// Adds a Virtual Machine/Storage to a Task/File
  void SetAllocation(size_t position, size_t storage_id) {
    allocation_[position] = storage_id;
  }

  /// Calculate de Objective Function of the solution
  double ObjectiveFunction(bool check_storage = true, bool check_sequence = false);

  /// Schedule the \c task to be executed at \c virtual_machine
  double ScheduleTask(Task task, VirtualMachine virtual_machine);

  /// Verify that que sequence of the task; terminate if the sequence is broken
  inline bool CheckTaskSequence(size_t);

  /// Chek the files
  inline bool checkFiles();

  /// Copy operator
  Solution& operator=(const Solution&) = default;

  /// Concatenation operator
  friend std::ostream& operator<<(std::ostream& os, const Solution& a) {
    return a.write(os);
  }

 protected:
  /// Write this object to the output stream
  std::ostream& write(std::ostream& os) const;

  /// Add a task id to the end of the ordering vector
  void AddOrdering(size_t task_id) {
    ordering_.push_back(task_id);
  }

  /// Computes the time of reading input files for the execution of the \c task
  double ComputeTaskReadTime(Task& task, VirtualMachine& vm);

  /// Compute the starting time of the \c task
  double ComputeTaskStartTime(size_t task, size_t vm);

  // Compute the makespan of the solution
  double ComputeMakespan(bool check_sequence);

  /// Compute the cost of the solution
  double ComputeCost();

  /// Caculate the security exposure of the solution
  double ComputeSecurityExposure();

  /// Compute the time for write all output files of the \c task executed at \c virtual_machine
  inline double ComputeTaskWriteTime(Task task, VirtualMachine virtual_machine);

  /// Compute the file transfer time
  inline double ComputeFileTransferTime(File* file,
                                        Storage vm1,
                                        Storage vm2,
                                        bool check_constraints = false);

  /// Allocate just one output file selecting storage with minimal time transfer
  double AllocateOneOutputFileGreedily(File* file, VirtualMachine vm);

  /// Define where the output files of the execution of the \c task will be stored
  double AllocateOutputFiles(Task& task, VirtualMachine& vm);

  /// Calculate the actual makespan and allocate the output files
  double CalculateMakespanAndAllocateOutputFiles(Task task, VirtualMachine vm);

  /// Compute the file contribution to the cost
  double ComputeFileCostContribution(File* file,
                                     VirtualMachine virtual_machine,
                                     Storage storage,
                                     double time);

  /// Compute the file contribution to the security exposure
  double ComputeFileSecurityExposureContribution(Storage storage, File* file);

  /// A pointer to the Algorithm object that contain the all necessary data
  Algorithm* algorithm_;

  /// Allocation of task and files in theirs respective VM or storages
  std::vector<size_t> allocation_;

  /// Order of the allocated tasks
  std::vector<size_t> ordering_;

  /// Makespan for each task
  std::vector<double> time_vector_;

  // Final time of each Virtual Machine
  std::vector<double> queue_;

  /// Start time of each task
  std::vector<double> start_time_vector_;

  /// Makespan of the solution
  double makespan_;

  /// Total cost of the solution
  double cost_;

  /// Total security exposure of the solution
  double security_exposure_;

  /// Objective value based on \c makespan_, \c cost_ and \c security_exposure_
  double objective_value_ = std::numeric_limits<double>::max();

  ///
  double lambda_;

  /// The list of the task to be executed
  std::unordered_map<int, std::vector<std::string>> scheduler_;

  /// Map containing the list of files by storage
  std::unordered_map<size_t, std::vector<size_t>> storage_queue_;
};

#endif  // APPROXIMATIVE_SOLUTIONS_SRC_MODEL_SOLUTION_H_
