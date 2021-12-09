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

#ifndef APPROXIMATE_SOLUTIONS_SRC_MODEL_SOLUTION_H_
#define APPROXIMATE_SOLUTIONS_SRC_MODEL_SOLUTION_H_

#include <algorithm>
#include <iostream>
#include <list>
#include <limits>
#include <random>
#include <string>
#include <vector>

#include "src/solution/algorithm.h"
#include "src/model/activation.h"

/// Forward declaration of the class Algorithm, needed because of the circular reference
class Algorithm;

/// Forward declaration of the class Algorithm, needed because of the circular reference
class Activation;

/// Forward declaration of the class Algorithm, needed because of the circular reference
class VirtualMachine;

/**
 * \class Solution solution.h "src/model/solution.h"
 * \brief Represents the solution for the execution of a Scientific Workflow
 */
class Solution {
 public:
  /// Constructor declaration
  explicit Solution(Algorithm* algorithm);

  /// Copy constructor
  Solution(const Solution& other);
  Solution(Algorithm* algorithm,
           const std::vector<size_t>& activation_allocations,
           const std::vector<size_t>& file_allocations,
           const std::vector<size_t>& ordering,
           const std::vector<int>& task_height,
           const std::vector<double>& time_vector,
           const std::vector<double>& execution_vm_queue,
           const std::vector<double>& allocation_vm_queue,
           double makespan,
           double cost,
           double security_exposure,
           double objective_value,
           const std::vector<std::shared_ptr<VirtualMachine>>& virtual_machines,
           const std::vector<std::shared_ptr<Activation>>& activations);

  /// Destructor
  ~Solution() { algorithm_ = nullptr; }

  /// Getter for \c makespan_
  double get_makespan() const { return makespan_; }

  /// Getter for \c cost_
  double get_cost() const { return cost_; }

  /// Getter for \c security_exposure_
  double get_security_exposure() const { return security_exposure_; }

  /// Getter for \c objective_value_
  double get_objective_value() const { return objective_value_; }

  /// Return the id of the Virtual Machine allocated to the \c std::shared_ptr<Activation> identified by \c task_id
//  size_t GetTaskAllocation(size_t task_id) const {
//    return activation_allocations_[task_id];
//  }

  /// Return the id of the Virtual Machine allocated to the \c std::shared_ptr<Activation> identified by \c task_id
//  size_t GetFileAllocation(size_t file_id) const {
//    return file_allocations_[file_id];
//  }

  /// Adds a Virtual Machine to a std::shared_ptr<Activation>
//  void SetTaskAllocation(size_t position, size_t vm_id) {
//    activation_allocations_[position] = vm_id;
//  }

  /// Adds a Storage to a File
  void SetFileAllocation(size_t position, size_t storage_id) {
    file_allocations_[position] = storage_id;
  }

  /// Calculate de Objective Function of the solution
  double ObjectiveFunction(bool check_storage = true, bool check_sequence = false);

  /// Schedule the \c activation to be executed at \c virtual_machine
  double ScheduleTask(const std::shared_ptr<Activation>& activation, const std::shared_ptr<VirtualMachine>& virtual_machine);

  /// Verify that que sequence of the task; terminate if the sequence is broken
  inline bool CheckTaskSequence(size_t);

  /// Chek the files
  inline bool checkFiles();

  ///
  int ComputeTasksHeights(size_t);

  bool localSearchN1();

  bool localSearchN2();

  bool localSearchN3();

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
//  void AddOrdering(size_t task_id) {
//    ordering_.push_back(task_id);
//  }

  /// Computes the time of reading input files for the execution of the \c task
  double ComputeTaskReadTime(const std::shared_ptr<Activation>& task, const std::shared_ptr<VirtualMachine>& vm);

  /// Computes the time of reading input files for the execution of the \c task
  double ComputeTaskReadTimeOther(const std::shared_ptr<Activation>&, const std::shared_ptr<VirtualMachine>&);

  /// Compute the starting time of the \c task
  double ComputeTaskStartTime(size_t task, size_t vm);
//  size_t ComputeTaskStartTime(size_t task, size_t vm);

  // Compute the makespan of the solution
  double ComputeMakespan(bool check_sequence);
//  size_t ComputeMakespan(bool check_sequence);

  /// Compute the cost of the solution
  double ComputeCost();

  /// Caculate the security exposure of the solution
  double ComputeSecurityExposure();

  // Compute the makespan of the solution
//  double GetMakespan() { return makespan_; }

  /// Compute the cost of the solution
//  double GetCost() { return cost_; }

  /// Caculate the security exposure of the solution
//  double GetSecurityExposure() { return security_exposure_; }

  /// Compute the time for write all output files of the \c task executed at \c virtual_machine
  inline double ComputeTaskWriteTime(const std::shared_ptr<Activation>& task, const std::shared_ptr<VirtualMachine>& virtual_machine);

  /// Compute the file transfer time
  inline double ComputeFileTransferTime(const std::shared_ptr<File>& file,
                                        const std::shared_ptr<Storage>& vm1,
                                        const std::shared_ptr<Storage>& vm2,
                                        bool check_constraints = false);

  /// Allocate just one output file selecting storage with minimal time transfer
  double AllocateOneOutputFileGreedily(const std::shared_ptr<File>& file, const std::shared_ptr<VirtualMachine>& vm);

  /// Define where the output files of the execution of the \c task will be stored
  double AllocateOutputFiles(const std::shared_ptr<Activation>& task, const std::shared_ptr<VirtualMachine>& vm);

  /// Calculate the actual makespan and allocate the output files
//  double CalculateMakespanAndAllocateOutputFiles(std::shared_ptr<Activation> *task, VirtualMachine *vm);
//  size_t CalculateMakespanAndAllocateOutputFiles(const std::shared_ptr<Activation>& task, const std::shared_ptr<VirtualMachine>& vm);
  double CalculateMakespanAndAllocateOutputFiles(const std::shared_ptr<Activation>& task,
                                                 const std::shared_ptr<VirtualMachine>& vm);

  /// Compute the file contribution to the cost
  double ComputeFileCostContribution(const std::shared_ptr<File>& file, const std::shared_ptr<Storage>& storage, const std::shared_ptr<VirtualMachine>& virtual_machine,
                                     double time);

  /// Compute the file contribution to the security exposure
  double ComputeFileSecurityExposureContribution(const std::shared_ptr<Storage>& storage, const std::shared_ptr<File>& file);

  /// A pointer to the Algorithm object that contain the all necessary data
  Algorithm* algorithm_{};

  /// Allocation of task in theirs VM
  std::vector<size_t> activation_allocations_;

  /// Allocation of files in theirs storages
  std::vector<size_t> file_allocations_;

  /// Order of the allocated tasks
  std::vector<size_t> ordering_;

  /// The height of each task in terms of dependencies
  std::vector<int> task_height_;

  /// Makespan for each task
  std::vector<double> time_vector_;
//  std::vector<size_t> time_vector_;

  /// Final time of each Virtual Machine
  std::vector<double> execution_vm_queue_;
//  std::vector<size_t> execution_vm_queue_;

  /// Final time of each Virtual Machine
  std::vector<double> allocation_vm_queue_;
//  std::vector<size_t> allocation_vm_queue_;

  /// Makespan of the solution, the total execution time
  double makespan_{};
//  size_t makespan_;

  /// Total cost of the solution
  double cost_{};

  /// Total security exposure of the solution
  double security_exposure_{};

  /// Objective value based on \c makespan_, \c cost_ and \c security_exposure_
  double objective_value_ = std::numeric_limits<double>::max();

  /// The virtual machines associated with the solution
  std::vector<std::shared_ptr<VirtualMachine>> virtual_machines_;

  ///
  std::vector<std::shared_ptr<Activation>> activations_;

};

#endif  // APPROXIMATE_SOLUTIONS_SRC_MODEL_SOLUTION_H_
