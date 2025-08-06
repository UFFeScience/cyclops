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
#include "src/solution/configuration.h"
#include "src/model/activation.h"
#include "src/model/activation_execution_data.h"
#include "src/model/file_manager.h"

/// Forward declaration of the class Configuration, needed because of the circular reference
class Configuration;

/// Forward declaration of the class Configuration, needed because of the circular reference
class Activation;

/// Forward declaration of the class Configuration, needed because of the circular reference
class VirtualMachine;

/**
 * \class Solution solution.h "src/model/solution.h"
 * \brief Represents the solution for the execution of a Scientific Workflow
 */
class Solution {
public:
    // Constructors

    explicit Solution(const std::shared_ptr<Configuration>& configuration);

    /// Copy constructor
    Solution(const Solution&) = default;

    /// Move constructor
//    Solution(Solution&&) = default;

    // Public Methods

    ///
    double GetActivationStartTime(size_t act_id, size_t vm_id);

    ///
    double GetActivationReadTime(size_t act_id, size_t vm_id);

    ///
    double GetActivationRunTime(size_t act_id, size_t vm_id);

    /// Define where the output files of the execution of the \c task will be stored
    double AllocateOutputFiles(
            const std::shared_ptr<Activation>& activation,
            const std::shared_ptr<VirtualMachine>& vm,
            double start_time,
            double read_time,
            double run_time
    );

    ///
    double GetActivationWriteTime(size_t act_id, size_t vm_id);

    ///
    size_t GetDistance(const Solution& solution);

    ///
    void Mutate(double probability);

    ///
    void Encode();

    ///
    Solution Crossover(Solution partner);

    ///
//    double SetActivationOutputFilesLocations(size_t act_id, size_t vm_id);

    ///
    double OptimizedComputeObjectiveFunction(size_t start_of_ordering = 1ul);

    ///
//    double HeaOptimizedComputeObjectiveFunction(size_t start_of_ordering = 1ul);

    /// Getter for \c makespan_
    [[nodiscard]] double get_makespan() const { return static_cast<double>(makespan_); }

    /// Getter for \c cost_
    [[nodiscard]] double get_cost() const { return cost_; }

    /// Getter for \c security_exposure_
    [[nodiscard]] double get_security_exposure() const {

        return security_exposure_;
    }

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
    double HeaComputeAndFetchOF();

    ///
    [[nodiscard]] double fetch_makespan() const;

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

    ///
    bool HeaLocalSearchN1();

    ///
    bool HeaLocalSearchN2();

    ///
    bool HeaLocalSearchN3();

    // Operators

    /// Copy assignment
    Solution& operator=(const Solution&) = default;

    /// Move assignment
    Solution& operator=(Solution&&) = default;

    /// Concatenation operator
    friend std::ostream &operator<<(std::ostream &os, const Solution &a) {
        return a.Write(os);
    }

    // Getters and Setters
    std::vector<size_t> get_ordering() {
        return ordering_;
    }

    // TODO: Remover esses acessos, criar forma mais eficiente de alterar e manter os estado

    size_t get_activation_allocation(size_t act_id) {
        return activation_allocations_[act_id];
    }

    void set_activation_allocation(size_t act_id, size_t vm_id) {
        activation_allocations_[act_id] = vm_id;
    }

    size_t get_file_allocation(size_t file_id) {
        return file_manager_.get_file_allocation(file_id);
    }

    void set_file_allocation(size_t file_id, size_t vm_id) {
        file_manager_.set_file_allocation(file_id, vm_id);
    }

    void change_file_allocation(size_t file_id, size_t vm_id) {
        file_manager_.ChangeFileAllocation(file_id, vm_id);
    }

protected:
    /// Write this object to the output stream
    std::ostream &Write(std::ostream &os) const;

    /// Computes the time of reading input files for the execution of the \c activation
    double ComputeActivationReadTime(
            const std::shared_ptr<Activation> &activation,
            const std::shared_ptr<VirtualMachine> &vm,
            double start_time
    );

    /// Compute the starting time of the \c task
    double ComputeActivationStartTime(
            size_t activation_id,
            size_t vm_id
    );

    /// Allocate just one output file selecting storage with minimal time transfer
    double AllocateOneOutputFileGreedily(
            const std::shared_ptr<Activation> &activation,
            const std::shared_ptr<File> &file,
            const std::shared_ptr<VirtualMachine> &vm,
            double start_time,
            double read_time,
            double run_time,
            double partial_write_time
    );

    /// Calculate the actual makespan and Allocate the output files
    double CalculateMakespanAndAllocateOutputFiles(
            const std::shared_ptr<Activation> &activation,
            const std::shared_ptr<VirtualMachine> &virtual_machine
    );

    /// Compute the file contribution to the security exposure
    double ComputeFileSecurityExposureContribution(
            const std::shared_ptr<Storage> &storage,
            const std::shared_ptr<File>& file
    );

    /// A pointer to the Configuration object that contain the all necessary data
    std::shared_ptr<Configuration> configuration_;

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
    double makespan_{};

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
