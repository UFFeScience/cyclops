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
#include <memory>  // Para std::make_shared

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
    // Store the best solutions
    int *** x = nullptr;  // Run the activation
    int ***** r = nullptr;  // Read files for the activation
    int ***** w = nullptr;  // Write files for the activation
    int *** y = nullptr;  // Files associated with storages
    int ** v = nullptr;  // Virtual machine allocation

    /// Constructor declaration
    explicit Solution(Algorithm *algorithm);

    /// Copy constructor
    Solution(const Solution &other);

    /// Destructor
    ~Solution();

    void MemoryAllocation();

    void FreeingMemoryAllocated();

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
        file_manager_.set_file_allocation(position, storage_id);;
    }

    /// Calculate de Objective Function of the solution
    double ObjectiveFunction(bool check_storage = true, bool check_sequence = false);

    /// Schedule the \c activation to be executed at \c virtual_machine
    double ScheduleActivation(const std::shared_ptr<Activation> &activation, const std::shared_ptr<VirtualMachine> &vm);

    /// Verify that que sequence of the task; terminate if the sequence is broken
//  inline bool CheckTaskSequence(size_t);

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

    bool localSearchN1();

    bool localSearchN2();

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
//  double ComputeActivationStartTime(size_t activation_id, size_t vm_id);
    size_t ComputeActivationStartTime(size_t task, size_t vm);

    /// Compute the file transfer time
//    [[nodiscard]] size_t ComputeFileTransferTime(const std::shared_ptr<File> file,
//                                                 const std::shared_ptr<Storage> storage1,
//                                                 const std::shared_ptr<Storage> storage2) const;

    /// Allocate just one output file selecting storage with minimal time transfer
    size_t AllocateOneOutputFileGreedily(const std::shared_ptr<Activation> &,
                                         const std::shared_ptr<File>&,
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
    Algorithm *algorithm_{};

    /// Allocation of task in theirs VM
    std::vector<size_t> activation_allocations_;

    ///
    FileManager file_manager_;

    /// Allocation of files in theirs storages
//    std::vector<size_t> file_allocations_;

    /// Order of the allocated tasks
    std::vector<size_t> ordering_;

    /// The height of each task in terms of dependencies
    std::vector<int> activation_height_;

    /// Auxiliary data, helps recalculate important information in case of modifications, and swaps
//    std::vector<std::shared_ptr<ActivationExecutionData>> activation_execution_data_;
    std::vector<ActivationExecutionData> activation_execution_data_;

//    /// Makespan for each task
//    std::vector<size_t> activation_finish_time_;
//
//    /// Final time of each Virtual Machine
//    std::vector<size_t> vm_finish_time_;
//
//    /// Total allocation time needed for each VM
//    std::vector<size_t> vm_allocation_time_;

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

    // For printing the solution timeline
    size_t n_, d_, m_, mb_, t_;

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
};


#endif  // APPROXIMATE_SOLUTIONS_SRC_MODEL_SOLUTION_H_
