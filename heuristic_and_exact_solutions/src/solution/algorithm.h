#ifndef APPROXIMATE_SOLUTIONS_SRC_SOLUTION_ALGORITHM_H_
#define APPROXIMATE_SOLUTIONS_SRC_SOLUTION_ALGORITHM_H_

#include <memory>
#include "src/solution/configuration.h"

class Algorithm : public std::enable_shared_from_this<Algorithm> {
public:
    // Constructors

    ///
    explicit Algorithm(std::shared_ptr<Configuration> configuration);

    // Public methods

    ///
    [[nodiscard]] std::shared_ptr<Activation> GetActivationPerId(size_t act_id) {
        return configuration_->get_activation_per_id(act_id);
    }

    ///
    [[nodiscard]] std::vector<std::shared_ptr<Activation>> GetActivations() {
        return configuration_->get_activations();
    }

    ///
    [[nodiscard]] size_t GetActivationsSize() {
        return configuration_->get_activations_size();
    }

    // TODO: Change the name budget --> cost
    ///
    [[nodiscard]] double GetAlphaCost() {
        return configuration_->get_alpha_budget();
    };

    ///
    [[nodiscard]] double GetAlphaSecurity() {
        return configuration_->get_alpha_security();
    };

    ///
    [[nodiscard]] double GetAlphaTime() {
        return configuration_->get_alpha_time();
    };

    ///
    [[nodiscard]] double GetAlphaRestrictCandidateList() {
        return configuration_->get_alpha_restrict_candidate_list();
    };

    ///
    [[nodiscard]] size_t GetBucketsSize() {
        return configuration_->get_buckets_size();
    }

    ///
    [[nodiscard]] int GetConflictValue(size_t file, size_t other_file) {
        return configuration_->get_conflict_value(file, other_file);
    }

    ///
    [[nodiscard]] std::shared_ptr<File> GetFilePerId(size_t file_id) {
        return configuration_->get_file_per_id(file_id);
    }

    ///
    [[nodiscard]] size_t GetFilesSize() {
        return configuration_->get_files_size();
    }

    ///
    [[nodiscard]] int GetHeight(const size_t activation_id) {
        return configuration_->get_height(activation_id);
    }

    ///
    [[nodiscard]] std::vector<int> GetHeights() {
        return configuration_->get_heights();
    }

    ///
    [[nodiscard]] double GetMakespanDeadline() {
        return configuration_->get_makespan_max();
    }

    ///
    [[nodiscard]] double GetMaximumBudget() {
        return configuration_->get_budget_max();
    }

    ///
    [[nodiscard]] double GetMaximumConfidentialityExposure() {
        return configuration_->get_maximum_security_and_privacy_exposure();
    }

    ///
    [[nodiscard]] std::vector<std::shared_ptr<File>> GetOutputFiles(size_t activation_id) {
        return configuration_->get_activation_per_id(activation_id)->get_output_files();
    }

    ///
    [[nodiscard]] std::vector<size_t> GetPredecessors(size_t act_id) {
        return configuration_->get_predecessors_ids(act_id);
    }

    ///
    [[nodiscard]] size_t GetRequirementsSize() {
        return configuration_->get_requirements_size();
    }

    ///
    [[nodiscard]] std::shared_ptr<Storage> GetStoragePerId(size_t storage_id) {
        return configuration_->get_storage_per_id(storage_id);
    }

    ///
    [[nodiscard]] size_t GetStoragesSize() {
        return configuration_->get_storages_size();
    }

    ///
    [[nodiscard]] size_t GetSourceId() {
        return configuration_->get_source_id();
    }

    ///
    [[nodiscard]] std::vector<size_t> GetSuccessors(size_t act_id) {
        return configuration_->get_successors_ids(act_id);
    }

    ///
    [[nodiscard]] std::shared_ptr<VirtualMachine> GetVirtualMachinePerId(size_t vm_id) {
        return configuration_->get_virtual_machine_per_id(vm_id);
    };

    ///
    [[nodiscard]] std::vector<std::shared_ptr<VirtualMachine>> GetVirtualMachines() {
        return configuration_->get_virtual_machines();
    };

    ///
    [[nodiscard]] size_t GetVirtualMachinesSize() {
        return configuration_->get_virtual_machines_size();
    };

    ///
    [[nodiscard]] size_t GetTargetId() {
        return configuration_->get_target_id();
    }

    /**
     * \brief Returns an object derived from Configuration according to \c name parameter.
     */
    static std::shared_ptr<Algorithm> ReturnAlgorithm(
            const std::string& algorithm,
            std::shared_ptr<Configuration> configuration
    );

    /**
     * \brief Executes the algorithm.
     */
    virtual void Run() = 0;

    // Getters and setters

    ///
    [[nodiscard]] std::shared_ptr<Configuration> get_configuration() {
        return configuration_;
    }

    ///
    virtual std::string get_name() const = 0;

protected:
    // TODO: Maybe, not the best place
    ///
    clock_t t_start = clock();

private:
    std::shared_ptr<Configuration> configuration_;
};

#endif // APPROXIMATE_SOLUTIONS_SRC_SOLUTION_ALGORITHM_H_
