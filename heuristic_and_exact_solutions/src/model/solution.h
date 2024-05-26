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
#include <memory>

#include "src/common/my_random.h"
#include "src/model/dynamic_file.h"
#include "src/model/static_file.h"
#include "src/solution/algorithm.h"
#include "src/model/activation.h"
#include "src/model/activation_execution_data.h"
#include "src/model/file_manager.h"

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

    ///
    double OptimizedComputeObjectiveFunction(size_t start_of_ordering = 1ul);

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
        file_manager_.set_file_allocation(position, storage_id);
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
    void PopulateExecutionAndAllocationsTimeVectors(size_t start_of_ordering = 1ul);

    ///
    void ComputeCost();

    ///
    void ComputeConfidentialityExposure();

    ///
    double ComputeAndFetchOF();

    ///
    [[nodiscard]] size_t fetch_makespan() const;

    ///
    [[nodiscard]] double fetch_cost() const;

    ///
    [[nodiscard]] double fetch_confidentiality_exposure() const;

    ///
    double AccumulateVMCost();

    ///
    double AccumulateBucketCost();

    ///
    double AccumulateActivationExposure();

    ///
    double AccumulatePrivacyExposure();

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

    /// Allocate just one output file selecting storage with minimal time transfer
    size_t AllocateOneOutputFileGreedily(const std::shared_ptr<Activation> &activation,
                                         const std::shared_ptr<File> &file,
                                         const std::shared_ptr<VirtualMachine> &vm,
                                         size_t start_time,
                                         size_t read_time,
                                         size_t run_time,
                                         size_t partial_write_time);

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

    ///
    FileManager file_manager_;

    /// Order of the allocated tasks
    std::vector<size_t> ordering_;

    /// The height of each task in terms of dependencies
    std::vector<int> activation_height_;

    /// Auxiliary data, helps recalculate important information in case of modifications, and swaps
    std::vector<ActivationExecutionData> activation_execution_data_;

    /// Makespan of the solution, the total execution time
    size_t makespan_{};

    /// The cost for allocate Virtual Machines for the execution
    double virtual_machine_cost_{};

    /// The cost for utilize the utilize the storage from Buckets for the execution
    double bucket_variable_cost_{};

    /// Total cost of the solution
    double cost_{};

    /// The accumulation of the activation exposure
    double activation_exposure_{};

    /// The accumulation of the the privacy exposure
    double file_privacy_exposure_{};

    /// Total security exposure of the solution
    double security_exposure_{};

    /// Objective value based on \c makespan_, \c cost_ and \c security_exposure_
    double objective_value_ = std::numeric_limits<double>::max();
};


#endif  // APPROXIMATE_SOLUTIONS_SRC_MODEL_SOLUTION_H_
