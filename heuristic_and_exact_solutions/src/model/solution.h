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
    explicit Solution(Algorithm *algorithm);

    /// Copy constructor
    Solution(const Solution &other) = default;

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

    /// Return the id of the Virtual Machine allocated to the \c Task identified by \c task_id
    size_t GetTaskAllocation(size_t task_id) const {
        return task_allocations_[task_id];
    }

    /// Return the id of the Virtual Machine allocated to the \c Task identified by \c task_id
    size_t GetFileAllocation(size_t file_id) const {
        return file_allocations_[file_id];
    }

    /// Adds a Virtual Machine to a Task
    void SetTaskAllocation(size_t position, size_t vm_id) {
        task_allocations_[position] = vm_id;
    }

    /// Adds a Storage to a File
    void SetFileAllocation(size_t position, size_t storage_id) {
        file_allocations_[position] = storage_id;
    }

    /// Calculate de Objective Function of the solution
    double ObjectiveFunction(bool check_storage = true, bool check_sequence = false);

    /// Schedule the \c task to be executed at \c virtual_machine
    double ScheduleTask(Task *task, VirtualMachine *virtual_machine);

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
    Solution &operator=(const Solution &) = default;

    /// Concatenation operator
    friend std::ostream &operator<<(std::ostream &os, const Solution &a) {
        return a.write(os);
    }

protected:
    /// Write this object to the output stream
    std::ostream &write(std::ostream &os) const;

    /// Add a task id to the end of the ordering vector
//  void AddOrdering(size_t task_id) {
//    ordering_.push_back(task_id);
//  }

    /// Computes the time of reading input files for the execution of the \c task
    double ComputeTaskReadTime(Task *task, VirtualMachine *vm);

    /// Compute the starting time of the \c task
    double ComputeTaskStartTime(size_t task, size_t vm);

    // Compute the makespan of the solution
    double ComputeMakespan(bool check_sequence);

    /// Compute the cost of the solution
    double ComputeCost();

    /// Caculate the security exposure of the solution
    double ComputeSecurityExposure();

    /// Compute the time for write all output files of the \c task executed at \c virtual_machine
    inline double ComputeTaskWriteTime(Task *task, VirtualMachine *virtual_machine);

    /// Compute the file transfer time
    inline double ComputeFileTransferTime(File *file,
                                          Storage *vm1,
                                          Storage *vm2,
                                          bool check_constraints = false);

    /// Allocate just one output file selecting storage with minimal time transfer
    double AllocateOneOutputFileGreedily(File *file, VirtualMachine *vm);

    /// Define where the output files of the execution of the \c task will be stored
    double AllocateOutputFiles(Task *task, VirtualMachine *vm);

    /// Calculate the actual makespan and allocate the output files
    double CalculateMakespanAndAllocateOutputFiles(Task *task, VirtualMachine *vm);

    /// Compute the file contribution to the cost
    double ComputeFileCostContribution(File *file, Storage *storage, VirtualMachine *virtual_machine,
                                       double time);

    /// Compute the file contribution to the security exposure
    double ComputeFileSecurityExposureContribution(Storage *storage, File *file);

    /// A pointer to the Algorithm object that contain the all necessary data
    Algorithm *algorithm_;

    /// Allocation of task in theirs VM
    std::vector <size_t> task_allocations_;

    /// Allocation of files in theirs storages
    std::vector <size_t> file_allocations_;

    /// Order of the allocated tasks
    std::vector <size_t> ordering_;

    /// The height of each task in terms of dependencies
    std::vector<int> task_height_;

    /// Makespan for each task
    std::vector<double> time_vector_;

    /// Final time of each Virtual Machine
    std::vector<double> execution_vm_queue_;

    /// Final time of each Virtual Machine
    std::vector<double> allocation_vm_queue_;

    /// Makespan of the solution, the total execution time
    double makespan_;

    /// Total cost of the solution
    double cost_;

    /// Total security exposure of the solution
    double security_exposure_;

    /// Objective value based on \c makespan_, \c cost_ and \c security_exposure_
    double objective_value_ = std::numeric_limits<double>::max();
};

#endif  // APPROXIMATIVE_SOLUTIONS_SRC_MODEL_SOLUTION_H_
