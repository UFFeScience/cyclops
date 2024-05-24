/**
 * \file src/model/solution.h
 * \brief Contains the \c Solution class declaration
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo.raps@gmail.com\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2024
 *
 * This header file contains the \c Solution class
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
#include <boost/algorithm/string.hpp>

#include "src/common/my_random.h"
#include "src/model/dynamic_file.h"
#include "src/model/static_file.h"
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
    explicit Solution(std::shared_ptr<Algorithm> algorithm);

    /// Copy constructor
    Solution(const Solution &other);

    double ComputeObjectiveFunction();

    /// Getter for \c makespan_
    [[nodiscard]] double get_makespan() const { return static_cast<double>(makespan_); }

    /// Getter for \c cost_
    [[nodiscard]] double get_cost() const { return cost_; }

    /// Getter for \c security_exposure_
    [[nodiscard]] double get_security_exposure() const { return security_exposure_; }

    /// Getter for \c objective_value_
    [[nodiscard]] double get_objective_value() const { return objective_value_; }

    /// Adds a Storage to a File
    void SetFileAllocation(size_t position, size_t storage_id) {
        file_allocations_[position] = storage_id;
    }

    /// Calculate de Objective Function of the solution
    double ObjectiveFunction(bool check_storage = true, bool check_sequence = false);

    /// Schedule the \c activation to be executed at \c virtual_machine
    double ScheduleActivation(const std::shared_ptr<Activation> &activation, const std::shared_ptr<VirtualMachine> &vm);

    /// Schedule the \c activation to be executed at \c virtual_machine
    void AllocateTask(const std::shared_ptr<Activation> &, const std::shared_ptr<VirtualMachine> &);

    /// Schedule the \c activation to be executed at \c virtual_machine
    void ClearOrdering();

    /// Schedule the \c activation to be executed at \c virtual_machine
    void AddOrdering(size_t);

    /// Check the files
    inline bool checkFiles();

    ///
    int ComputeTasksHeights(size_t node);

    ///
    bool localSearchN1();

    ///
    bool localSearchN2();

    ///
    bool localSearchN3();

    /// Copy operator
    Solution &operator=(const Solution &) = default;

    /// Concatenation operator
    friend std::ostream &operator<<(std::ostream &os, const Solution &a) {
        return a.Write(os);
    }

protected:
    /// Write this object to the output stream
    std::ostream &Write(std::ostream &os) const;

    /// Computes the time of reading input files for the execution of the \c activation
    size_t ComputeActivationReadTime(const std::shared_ptr<Activation> &,
                                     const std::shared_ptr<VirtualMachine> &,
                                     size_t);

    /// Compute the starting time of the \c task
    size_t ComputeActivationStartTime(size_t activation_id, size_t vm_id);

    /// Compute the file transfer time
    [[nodiscard]] size_t ComputeFileTransferTime(const std::shared_ptr<File> &file,
                                                 const std::shared_ptr<Storage> &storage1,
                                                 const std::shared_ptr<Storage> &storage2,
                                                 bool check_constraints = false) const;

    /// Allocate just one output file selecting storage with minimal time transfer
    size_t AllocateOneOutputFileGreedily(const std::shared_ptr<File>&,
                                         const std::shared_ptr<VirtualMachine> &,
                                         size_t,
                                         size_t,
                                         size_t,
                                         size_t);

    /// Define where the output files of the execution of the \c task will be stored
    size_t AllocateOutputFiles(const std::shared_ptr<Activation> &,
                               const std::shared_ptr<VirtualMachine> &,
                               size_t,
                               size_t,
                               size_t);

    /// Calculate the actual makespan and Allocate the output files
    size_t CalculateMakespanAndAllocateOutputFiles(const std::shared_ptr<Activation> &,
                                                   const std::shared_ptr<VirtualMachine> &);

    /// Compute the file contribution to the security exposure
    double ComputeFileSecurityExposureContribution(const std::shared_ptr<Storage> &storage,
                                                   const std::shared_ptr<File>& file);

    /// A pointer to the Algorithm object that contain the all necessary data
    std::shared_ptr<Algorithm> algorithm_;

    /// Allocation of task in theirs VM
    std::vector<size_t> activation_allocations_;

    /// Allocation of files in theirs storages
    std::vector<size_t> file_allocations_;

    /// Order of the allocated tasks
    std::vector<size_t> ordering_;

    /// The height of each task in terms of dependencies
    std::vector<int> task_height_;

    /// Makespan for each task
    std::vector<size_t> time_vector_;

    /// Final time of each Virtual Machine
    std::vector<size_t> execution_vm_queue_;

    /// Final time of each Virtual Machine
    std::vector<size_t> allocation_vm_queue_;

    /// Makespan of the solution, the total execution time
    size_t makespan_{};

    /// Total cost of the solution
    double cost_{};

    /// Total security exposure of the solution
    double security_exposure_{};

    /// Objective value based on \c makespan_, \c cost_ and \c security_exposure_
    double objective_value_ = std::numeric_limits<double>::max();
};


#endif  // APPROXIMATE_SOLUTIONS_SRC_MODEL_SOLUTION_H_
