/**
 * \file src/solution/configuration.h
 * \brief Contains the \c Configuration class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2024
 *
 * This header file contains the \c Configuration class that handles different execution modes.
 */

#ifndef APPROXIMATE_SOLUTIONS_SRC_SOLUTION_CONFIGURATION_H_
#define APPROXIMATE_SOLUTIONS_SRC_SOLUTION_CONFIGURATION_H_

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

#include "src/model/file.h"
#include "src/model/requirement.h"
#include "src/model/activation.h"
#include "src/model/virtual_machine.h"
#include "src/model/bucket.h"
#include "src/model/solution.h"
#include "src/model/conflict_graph.h"

class Solution;

/**
 * \class Configuration configuration.h "src/solution/configuration.h"
 * \brief Executes the appropriate routines.
 *
 * This class is responsible for calling the appropriate methods for read the input files and
 * execute a specific implementation.
 */

class Configuration {
public:
    // Constructors

    ///
    Configuration(
            std::string tasks_and_files,
            std::string cluster,
            std::string conflict_graph,
            double alpha_time,
            double alpha_budget,
            double alpha_security,
            double alpha_restrict_candidate_list
    );

    Configuration(const Configuration& other) = default;

    // Public methods

    /// Read input files.
    void ReadInputFiles(const std::string &tasks_and_files_file,
                        const std::string &cluster_file,
                        const std::string &conflict_graph_file);

    /// Getter for \c id_source_
    [[nodiscard]] size_t get_source_id() const { return id_source_; }

    /// Getter for \c id_target_
    [[nodiscard]] size_t get_target_id() const { return id_target_; }

    /// Getter for \c conflict_graph_
    std::shared_ptr<ConflictGraph> get_conflict_graph() { return conflict_graph_; }

    /// Getter for \c storage_vet_
    [[nodiscard]] std::vector<double>& get_storage_vet() { return storage_vet_; }

    /// Getter for \c height_
    [[nodiscard]] int get_height(const size_t activation_id) {
        return height_[activation_id];
    }

    /// Getter for \c bucket_size_
    [[nodiscard]] size_t get_buckets_size() const { return bucket_size_; }

    /// Return the size of the \c activations_
    [[nodiscard]] size_t get_activations_size() const { return activations_.size(); }

    /// Return the size of the \c files_
    [[nodiscard]] size_t get_files_size() const { return files_.size(); }

    /// Return the size of the \c virtual_machines
    [[nodiscard]] size_t get_virtual_machines_size() const { return virtual_machines_.size(); }

    /// Return the size of the \c storages_
    [[nodiscard]] size_t get_storages_size() const { return storages_.size(); }

    /// Return the size of the \c requirements_
    [[nodiscard]] size_t get_requirements_size() const { return requirements_.size(); }

    /// Return a pointer to the \c File identified by \c id
    [[nodiscard]] std::shared_ptr<File> get_file_per_id(size_t id) { return files_[id]; }

    /// Return a pointer to the \c Activation identified by \c id
    [[nodiscard]] std::shared_ptr<Activation> get_activation_per_id(size_t id) { return activations_[id]; }

    /// Return a pointer to the \c Storage identified by \c id
    [[nodiscard]] std::shared_ptr<Storage> get_storage_per_id(size_t id) { return storages_[id]; }

    /// Return a pointer to the \c VirtualMachine identified by \c id
    [[nodiscard]] std::shared_ptr<VirtualMachine> get_virtual_machine_per_id(size_t id) { return virtual_machines_[id]; }

    /// Return a reference to the successors of the \c Activation identified by \c activation_id
    [[nodiscard]] std::vector<size_t>& get_successors_ids(size_t activation_id) { return successors_[activation_id]; }

    /// Return a reference to the predecessors of the \c Activation identified by \c activation_id
    [[nodiscard]] std::vector<size_t>& get_predecessors_ids(size_t activation_id) { return predecessors_[activation_id]; }

    /// Getter for makespan_max_
    [[nodiscard]] double get_makespan_max() const { return makespan_max_; }

    /// Getter for \c budget_max_
    [[nodiscard]] double get_budget_max() const { return budget_max_; }

    /// Getter for \c alpha_time_
    [[nodiscard]] double get_alpha_time() const { return alpha_time_; }

    /// Getter for \c alpha_budget_
    [[nodiscard]] double get_alpha_budget() const { return alpha_budget_; }

    /// Getter for \c alpha_security_
    [[nodiscard]] double get_alpha_security() const { return alpha_security_; }

    /// Getter for \c alpha_security_
    [[nodiscard]] double get_alpha_restrict_candidate_list() const { return alpha_restrict_candidate_list_; }

    /// Getter for \c maximum_security_and_privacy_exposure_
    [[nodiscard]] double get_maximum_security_and_privacy_exposure() const {
        return maximum_security_and_privacy_exposure_;
    }

    ///
    [[nodiscard]] std::vector<std::shared_ptr<Activation>> get_activations() {
        return activations_;
    }

    ///
    [[nodiscard]] int get_conflict_value(size_t file, size_t other_file) {
        return conflict_graph_->ReturnConflict(file, other_file);
    }

    ///
    [[nodiscard]] std::vector<int> get_heights() {
        return height_;
    }

    ///
    [[nodiscard]] std::vector<std::shared_ptr<VirtualMachine>> get_virtual_machines() {
        return virtual_machines_;
    }

    ///
    void SetAlphas(double alpha_time,
                   double alpha_budget,
                   double alpha_security,
                   double alpha_restrict_candidate_list) {
        alpha_time_ = alpha_time;
        alpha_budget_ = alpha_budget;
        alpha_security_ = alpha_security;
        alpha_restrict_candidate_list_ = alpha_restrict_candidate_list;
    }

    ///
    void CalculateMaximumSecurityAndPrivacyExposure();

    // Getters and setters

    ///
    [[nodiscard]] bool get_check_for_hard_constraints() {
        return check_for_hard_constraints_;
    }

    ///
    void set_check_for_hard_constraints(bool check_for_hard_constraints) {
        check_for_hard_constraints_ = check_for_hard_constraints;
    }

protected:
    ///
    void ReadTasksAndFiles(const std::string &tasks_and_files_file,
                           std::unordered_map<std::string, std::shared_ptr<File>> &file_map_per_name);

    ///
    void ReadCluster(const std::string &);

    ///
    void ReadConflictGraph(const std::string &conflict_graph,
                           std::unordered_map<std::string, std::shared_ptr<File>> &file_map_per_name);

    ///
    void ComputeHeight(size_t, int);

	///
    void ComputeFileTransferMatrix();

    ///
    size_t static_file_size_{};

    ///
    size_t dynamic_file_size_{};

    ///
    double makespan_max_{};

    ///
    double budget_max_{};

    ///
    size_t id_source_{};

    ///
    size_t id_target_{};

    ///
    std::vector<Requirement> requirements_;

    ///
    std::vector<double> storage_vet_;  // storage of vm

    ///
    std::vector<std::shared_ptr<File>> files_;

    ///
    std::vector<std::shared_ptr<Activation>> activations_;

    ///
    std::vector<std::shared_ptr<Storage>> storages_;

    ///
    std::vector<std::shared_ptr<VirtualMachine>> virtual_machines_;

    ///
    std::vector<std::vector<size_t>> successors_;

    ///
    std::vector<std::vector<size_t>> predecessors_;

    /// Number of the buckets
    size_t bucket_size_{};

    ///
    bool check_for_hard_constraints_{true};

    ///
    std::vector<int> height_;

    ///
    std::shared_ptr<ConflictGraph> conflict_graph_;
//    ConflictGraph conflict_graph_{};

    /// The weight of the time
    double alpha_time_{};

    /// The weight of the budget
    double alpha_budget_{};

    /// The weight of the security
    double alpha_security_{};

    ///
    double alpha_restrict_candidate_list_{};

    ///
    double maximum_security_and_privacy_exposure_{};
};

#endif  // APPROXIMATE_SOLUTIONS_SRC_SOLUTION_CONFIGURATION_H_
