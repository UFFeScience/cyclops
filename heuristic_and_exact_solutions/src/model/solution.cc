/**
 * \file src/model/solution.cc
 * \brief Contains the \c Solution class definition
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo.raps@gmail.com\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2024
 *
 * This source file contains the \c Solution class definition
 */

#include <iostream>
#include <deque>
#include <utility>  // Para std::pair
#include <algorithm>  // Para std::find_if
#include <iomanip>
#include <boost/iterator/counting_iterator.hpp>
#include "src/model/solution.h"

DECLARE_uint64(number_of_allocation_experiments);

constexpr size_t NUM_CHROMOSOMES = 50;
constexpr size_t NUM_GENERATIONS = 100;
constexpr double MUTATION_PROBABILITY = 0.10;
constexpr double ALPHA = 0.3;

// Constructors

/**
 * Parameterised constructor.
 *
 * @param configuration
 */
Solution::Solution(const std::shared_ptr<Configuration> &configuration)
        : configuration_(configuration),
          activation_allocations_(configuration->get_activations_size(), std::numeric_limits<size_t>::max()),
          file_manager_(configuration->get_files_size(),
                        configuration->get_storages_size(),
                        configuration->get_conflict_graph(),
                        configuration->get_check_for_hard_constraints()),
          activation_height_(configuration->get_activations_size(), -1),
          makespan_(std::numeric_limits<double>::max()),
          virtual_machine_cost_(std::numeric_limits<double>::max()),
          bucket_variable_cost_(std::numeric_limits<double>::max()),
          cost_(std::numeric_limits<double>::max()),
          activation_exposure_(std::numeric_limits<double>::max()),
          file_privacy_exposure_(std::numeric_limits<double>::max()),
          security_exposure_(configuration_->get_maximum_security_and_privacy_exposure()) {
    // Initialize the allocation with the static files place information (VM or Bucket)
    for (size_t i = 0ul; i < configuration_->get_files_size(); ++i) {
        auto file = configuration_->get_file_per_id(i);
        if (auto static_file = std::dynamic_pointer_cast<StaticFile>(file)) {
            SetFileAllocation(file->get_id(), static_file->GetFirstVm());
        }
    }

    // Setting Activations Heights
    ComputeTasksHeights(configuration->get_source_id());

    // Initialize the Activation Execution Data
    for (auto i = 0ul; i < configuration->get_activations_size(); ++i) {
        ActivationExecutionData aed{configuration->get_activations_size(), configuration->get_virtual_machines_size()};
        activation_execution_data_.push_back(aed);
    }
}

// Public Methods

//Mutation on allocation chromosome
void Solution::Mutate(double probability) {
    std::uniform_int_distribution<> idis(0, configuration_->get_virtual_machines_size() - 1);

    for (size_t i{}; i < configuration_->get_activations_size(); i++) {
        if (((float) random() / (float) RAND_MAX) <= probability) {
            activation_allocations_[i] = my_rand<size_t>(0, configuration_->get_virtual_machines_size() - 1);
        }
    }

    for (size_t i{}; i < configuration_->get_files_size(); i++) {
        if (((float) random() / (float) RAND_MAX) <= probability) {
//            file_manager_.set_file_allocation(i, my_rand<size_t>(0, configuration_->get_virtual_machines_size() - 1));
            auto new_vm_id = my_rand<size_t>(0, configuration_->get_virtual_machines_size() - 1);
            if (new_vm_id != file_manager_.get_file_allocation(i)) file_manager_.ChangeFileAllocation(i, new_vm_id);
        }
    }

    OptimizedComputeObjectiveFunction();
}

// Random encode (new chromosome)
void Solution::Encode() {
    std::vector<size_t> seq_list(boost::counting_iterator<size_t>(0ull), boost::counting_iterator<size_t>(
            activation_height_.size())
    );

    // TODO: Height should be size_t also
    // TODO: passing by reference is too much, I think
    sort(begin(seq_list), end(seq_list), [&](const size_t& i, const size_t& j) {
        return activation_height_[i] < activation_height_[j];
    }); //sort a list based on height soft

    // Encode ordering Chromosome
    for (auto activation_id : seq_list) {
        ordering_.push_back(activation_id);
    }

    // Encode allocation chromosome
    for (size_t i{}; i < configuration_->get_activations_size(); i++) {
        activation_allocations_[i] = my_rand<size_t>(0, configuration_->get_virtual_machines_size() - 1);
    }

    for (size_t i{}; i < configuration_->get_files_size(); i++) {
        auto file = configuration_->get_file_per_id(i);
        if (!std::dynamic_pointer_cast<StaticFile>(file)) {
            file_manager_.set_file_allocation(i, my_rand<size_t>(0, configuration_->get_virtual_machines_size() - 1));
        }
    }
}

/*crossover*/
Solution Solution::Crossover(Solution partner) {
    Solution chr(partner);

    size_t activation_size = configuration_->get_activations_size();

    size_t point_ordering = my_rand<size_t>(0, activation_size - 1);
    size_t point_allocation = my_rand<size_t>(0, activation_size - 1);
    size_t point_file = my_rand<size_t>(0, configuration_->get_files_size() - 1);

    // Allocation crossover (Single point crossover)
    for (size_t i{}; i < point_allocation; i++) {
        chr.activation_allocations_[i] = activation_allocations_[i];
    }

    // Allocation crossover (Single point crossover)
    for (size_t i{}; i < point_file; i++) {
        if (chr.file_manager_.get_file_allocation(i) != file_manager_.get_file_allocation(i)) {
            chr.file_manager_.ChangeFileAllocation(i, file_manager_.get_file_allocation(i));
        }
    }

    // Ordering crossover
    std::vector<bool> aux(activation_size, false);
    chr.ordering_.clear();

    // Ordering crossover first part self -> chr
    for (size_t i{}; i < point_ordering; i++) {
        chr.ordering_.push_back(ordering_[i]);
        aux[ordering_[i]] = true;
    }

    // Ordering crossover second part partner -> chr
    for (size_t i{}; i < activation_size; i++) {
        if (!aux[partner.ordering_[i]]) {
            chr.ordering_.push_back(partner.ordering_[i]);
        }
    }

    return chr;
}

double Solution::GetActivationStartTime(size_t act_id, size_t vm_id) {
    double start_time{};

    // What came later, occupation of the VM or the previous activation finish time
    for (auto predecessor_id : configuration_->get_predecessors_ids(act_id)) {
        if (predecessor_id == configuration_->get_source_id()) {
            break;
        }
        start_time = std::max(start_time, activation_execution_data_[predecessor_id].get_activation_finish_time());
    }

    auto ordering_size = ordering_.size();

    if (ordering_size > 0) {
        return std::max(start_time, activation_execution_data_[ordering_size - 1].get_vm_finish_time(vm_id));
    } else {
        return start_time;
    }
}

double Solution::GetActivationReadTime(const size_t act_id, const size_t vm_id) {
    double read_time{};

    auto activation = configuration_->get_activation_per_id(act_id);
    for (const auto& file: activation->get_input_files()) {
        size_t storage_id;
        auto file_id = file->get_id();

        // TODO: This should be inside the file object, something like GetStorage()
        // TODO: Maybe, it is better initialize all static files with corresponding VM and drop the casting
//        if (auto static_file = std::dynamic_pointer_cast<StaticFile>(file)) {
            // If the file is static, get the ID of the storage where the file is stored from its definition
//            storage_id = static_file->GetFirstVm();
//        } else {
            // If the file is dynamic, get the ID of the storage where the file is stored from its allocation
            storage_id = file_manager_.get_file_allocation(file_id);
//        }

        if (storage_id == std::numeric_limits<size_t>::max()) {
            LOG(FATAL) << "Wrong storage_id - PopulateExecutionAndAllocationsTimeVectors";
        }

//        auto file_storage = configuration_->get_storage_per_id(storage_id);

        // Ceil of File Transfer Time + File Size * lambda
        auto one_file_read_time = file->GetFileTransfer(storage_id, vm_id);

        // If reading problem then terminate
        if (one_file_read_time == std::numeric_limits<double>::max()) {
            LOG(FATAL) << "Something is wrong with file reading";
        }

        read_time += one_file_read_time;

        // TODO: Doing this another time

//        // If the storage is some VM (not bucket) different from the execution VM
//        // Allocate the necessary VM for the reading
//        if (storage_id < configuration_->get_virtual_machines_size() and storage_id != vm->get_id()) {
//            if (start_time != std::numeric_limits<double>::max()
//                && read_time != std::numeric_limits<double>::max()) {
//                double finish_read_time = start_time + read_time;
//                activation_execution_data_[act_id].set_vm_allocation_time(
//                        storage_id,
//                        std::max(activation_execution_data_[act_id].get_vm_allocation_time(storage_id),
//                                 finish_read_time));
//            }
//        }
    }

    return read_time;
}

double Solution::GetActivationRunTime(const size_t act_id, const size_t vm_id) {
    auto activation = configuration_->get_activation_per_id(act_id);
    auto virtual_machine = configuration_->get_virtual_machine_per_id(vm_id);
    return activation->get_time() * virtual_machine->get_slowdown();
}

double Solution::GetActivationWriteTime(const size_t act_id, const size_t vm_id) {
    DLOG(INFO) << act_id << " " << vm_id;
    // All files are being stored on the VM executing the activation
    return 0.0;

//    double write_time{};
//    auto activation = configuration_->get_activation_per_id(act_id);
//    auto virtual_machine = configuration_->get_virtual_machine_per_id(vm_id);
//
//    auto output_files = activation->get_output_files();
//    for (const auto& output_file: output_files) {
//        size_t output_file_id = output_file->get_id();
//        size_t storage_id = file_manager_.get_file_allocation(output_file_id);
//        std::shared_ptr<Storage> storage = configuration_->get_storage_per_id(storage_id);
//        double one_file_write_time = output_file->GetFileTransfer(virtual_machine->get_id(), storage->get_id());
//
//        // If we could not Write the output_file then terminate!
//        if (one_file_write_time == std::numeric_limits<double>::max()) {
//            LOG(FATAL) << "Something is wrong with output_file writing";
//        }
//
//        write_time += one_file_write_time;
//
////        // If the storage is some VM (not bucket) different from the execution VM
////        // Allocate the necessary VM for the writing
////        if (storage_id < configuration_->get_virtual_machines_size() and storage_id != vm_id) {
////            if (activation_start_time != std::numeric_limits<double>::max()
////                && activation_read_time != std::numeric_limits<double>::max()
////                && activation_run_time != std::numeric_limits<double>::max()
////                && activation_write_time != std::numeric_limits<double>::max()) {
////                double finish_write_time = activation_start_time
////                                           + activation_read_time
////                                           + activation_run_time
////                                           + activation_write_time;
////
////                // Need to Allocate VM until output_file is writen
////                activation_execution_data_[activation_id].set_vm_allocation_time(
////                        storage_id,
////                        std::max(
////                                activation_execution_data_[activation_id].get_vm_allocation_time(storage_id),
////                                finish_write_time));
////            } else {
////                LOG(FATAL) << "Something very very very wrong";
////            }
////        }
//    }
//
//    return write_time;
}

size_t Solution::GetDistance(const Solution& solution) {
    size_t distance{};
    size_t activation_size = configuration_->get_activations_size();

    // compute the distance based on position
    for (size_t i{}; i < activation_size; i++) {
        if (activation_allocations_[i] != solution.activation_allocations_[i])
            distance += 1;
    }

    for (size_t i{}; i < configuration_->get_files_size(); i++) {
        if (file_manager_.get_file_allocation(i) != solution.file_manager_.get_file_allocation(i))
            distance += 1;
    }

    // TODO: Better understand this
    // compute the distance based on swaps required
    std::vector<size_t> aux_ordering(ordering_);
    for (size_t i{}; i < activation_size; i++) {
        if (aux_ordering[i] != solution.ordering_[i]) {
            distance += 1;
            for (size_t j = i + 1; j < activation_size; j++) {
                if (aux_ordering[j] == solution.ordering_[i]) {
                    iter_swap(aux_ordering.begin() + static_cast<long>(i), aux_ordering.begin() + static_cast<long>(j));
                }
            }
        }
    }

    return distance;
}

//double SetActivationOutputFilesLocations(const size_t act_id, const size_t vm_id) {
//
//}

void Solution::PopulateExecutionAndAllocationsTimeVectors(size_t start_of_ordering) {
    for (auto index = start_of_ordering; index < ordering_.size(); index++) {
        auto activation_id = ordering_[index];
        // Initializations
        double activation_start_time{};
        double activation_read_time{};
        double activation_write_time{};
        double activation_run_time;
        double finish_time;

        // Fetching some previous Activation Execution Data
        for (auto vm_id = 0ul; vm_id < configuration_->get_virtual_machines_size(); vm_id++) {
            auto previous_activation_id = ordering_[index - 1];
            activation_execution_data_[activation_id].set_vm_finish_time(vm_id,
                    activation_execution_data_[previous_activation_id].get_vm_finish_time(vm_id));
            activation_execution_data_[activation_id].set_vm_allocation_time(vm_id,
                    activation_execution_data_[previous_activation_id].get_vm_allocation_time(vm_id));
        }

        // Load Vm
        auto vm = configuration_->get_virtual_machine_per_id(activation_allocations_[activation_id]);
        auto vm_id = vm->get_id();
        auto activation = configuration_->get_activation_per_id(activation_id);

        // TODO: VM finish time and VM allocation time could be within the VM object
        // Compute Activation Start Time
        // What came later, occupation of the VM or the previous activation finish time
        for (auto previous_activation_id : configuration_->get_predecessors_ids(activation->get_id())) {
            if (previous_activation_id == configuration_->get_source_id()) {
//                activation_start_time = 0ul;
                break;
            }
            activation_start_time = std::max(
                    activation_start_time,
                    activation_execution_data_[previous_activation_id].get_activation_finish_time());
        }

        activation_start_time = std::max(
                activation_start_time,
                activation_execution_data_[activation_id].get_vm_finish_time(vm_id));

        // Compute Activation Read Time
        for (const auto& file: activation->get_input_files()) {
            size_t storage_id;
            auto file_id = file->get_id();

            // TODO: This should be inside the file object, something like GetStorage()
            if (auto static_file = std::dynamic_pointer_cast<StaticFile>(file)) {
                // If the file is static, get the ID of the storage where the file is stored from its definition
                storage_id = static_file->GetFirstVm();
            } else {
                // If the file is dynamic, get the ID of the storage where the file is stored from its allocation
                storage_id = file_manager_.get_file_allocation(file_id);
            }

            if (storage_id == std::numeric_limits<size_t>::max()) {
                LOG(FATAL) << "Wrong storage_id - PopulateExecutionAndAllocationsTimeVectors";
            }

            auto file_storage = configuration_->get_storage_per_id(storage_id);

            // Ceil of File Transfer Time + File Size * lambda
            auto one_file_read_time = file->GetFileTransfer(file_storage->get_id(), vm->get_id());

            // If reading problem then terminate
            if (one_file_read_time == std::numeric_limits<double>::max()) {
                LOG(FATAL) << "Something is wrong with file reading";
            }

            activation_read_time += one_file_read_time;

            // If the storage is some VM (not bucket) different from the execution VM
            // Allocate the necessary VM for the reading
            if (storage_id < configuration_->get_virtual_machines_size() and storage_id != vm->get_id()) {
                if (activation_start_time != std::numeric_limits<double>::max()
                        && activation_read_time != std::numeric_limits<double>::max()) {
                    double finish_read_time = activation_start_time + activation_read_time;
                    activation_execution_data_[activation_id].set_vm_allocation_time(
                            storage_id,
                            std::max(
                                    activation_execution_data_[activation_id].get_vm_allocation_time(storage_id),
                                    finish_read_time));
                }
            }
        }

        // Compute Run time
//        activation_run_time = ceil(activation->get_time() * vm->get_slowdown());
        activation_run_time = activation->get_time() * vm->get_slowdown();

        // Compute Write Time
        auto output_files = activation->get_output_files();
        for (const auto& output_file: output_files) {
            size_t output_file_id = output_file->get_id();
            size_t storage_id = file_manager_.get_file_allocation(output_file_id);
            std::shared_ptr<Storage> storage = configuration_->get_storage_per_id(storage_id);
            double one_file_write_time = output_file->GetFileTransfer(vm->get_id(), storage->get_id());

            // If we could not Write the output_file then terminate!
            if (one_file_write_time == std::numeric_limits<double>::max()) {
                LOG(FATAL) << "Something is wrong with output_file writing";
            }

#ifndef NDEBUG
            DLOG(INFO) << "output_file: " << output_file->get_name();
            DLOG(INFO) << "one_file_write_time: " << one_file_write_time;
#endif
            activation_write_time += one_file_write_time;

            // If the storage is some VM (not bucket) different from the execution VM
            // Allocate the necessary VM for the writing
            if (storage_id < configuration_->get_virtual_machines_size() and storage_id != vm_id) {
                if (activation_start_time != std::numeric_limits<double>::max()
                        && activation_read_time != std::numeric_limits<double>::max()
                        && activation_run_time != std::numeric_limits<double>::max()
                        && activation_write_time != std::numeric_limits<double>::max()) {
                    double finish_write_time = activation_start_time
                                             + activation_read_time
                                             + activation_run_time
                                             + activation_write_time;

                    // Need to Allocate VM until output_file is writen
                    activation_execution_data_[activation_id].set_vm_allocation_time(
                            storage_id,
                            std::max(
                                    activation_execution_data_[activation_id].get_vm_allocation_time(storage_id),
                                    finish_write_time));
                } else {
                    LOG(FATAL) << "Something very very very wrong";
                }
            }
        }

        // Compute Finish Time
        if (activation_start_time == std::numeric_limits<double>::max()
            || activation_read_time == std::numeric_limits<double>::max()
            || activation_run_time == std::numeric_limits<double>::max()
            || activation_write_time == std::numeric_limits<double>::max()) {
            finish_time = std::numeric_limits<double>::max();
        } else {
            finish_time = activation_start_time + activation_read_time + activation_run_time + activation_write_time;
        }

        // TODO: This should be within the activation object and vm object
        // Update structures
        activation_execution_data_[activation_id].set_activation_finish_time(finish_time);
        activation_execution_data_[activation_id].set_vm_finish_time(vm_id, finish_time);
        activation_execution_data_[activation_id].set_vm_allocation_time(
                vm_id,
                std::max(
                        activation_execution_data_[activation_id].get_vm_allocation_time(vm_id),
                        finish_time));

#ifndef NDEBUG
        DLOG(INFO) << "my_allocation_vm_queue[" << vm_id << "]: "
                << activation_execution_data_[activation_id].get_vm_allocation_time(vm_id);
        DLOG(INFO) << "activation_id: " << activation_id;
        DLOG(INFO) << "vm->get_id(): " << vm_id;
        DLOG(INFO) << "activation_start_time: " << activation_start_time;
        DLOG(INFO) << "activation_read_time: " << activation_read_time;
        DLOG(INFO) << "activation_run_time: " << activation_run_time;
        DLOG(INFO) << "activation_write_time: " << activation_write_time;
        DLOG(INFO) << "finish_time: " << finish_time;
#endif
    }
}

// Accumulate the Virtual Machine rent cost
double Solution::AccumulateVMCost() {
    double vm_cost = 0.0;
    for (auto i = 0ul; i < configuration_->get_virtual_machines_size(); ++i) {
        auto virtual_machine = configuration_->get_virtual_machine_per_id(i);
        auto alloc_time = static_cast<double>(activation_execution_data_[ordering_.back()].get_vm_allocation_time(i));
        vm_cost += ((alloc_time) * virtual_machine->get_cost());  // Billed per hour
#ifndef NDEBUG
        DLOG(INFO) << "allocation_vm_queue_[" << i << "]: " << activation_execution_data_[ordering_.back()]
                .get_vm_allocation_time(i);
        DLOG(INFO) << "virtual_machine->get_cost(): " << virtual_machine->get_cost();
#endif
    }
    return vm_cost;
}

// Accumulate the Bucket variable cost
double Solution::AccumulateBucketCost() {
    double bucket_cost = 0.0;
    for (auto i = configuration_->get_virtual_machines_size(); i < configuration_->get_storages_size(); ++i) {
        auto storage = configuration_->get_storage_per_id(i);

        // If the storage is not a Virtual Machine, i.e. it is a Bucket; then calculate the bucket
        // variable cost
        for (auto j = 0ul; j < configuration_->get_files_size(); ++j) {
            // If the Bucket is used; then accumulate de cost and break to the next Storage
            if (file_manager_.get_file_allocation(j) == i) {
                auto file = configuration_->get_file_per_id(j);
                bucket_cost += (storage->get_cost() * file->get_size_in_GB());
            }
        }
    }
    return bucket_cost;
}

void Solution::ComputeCost() {
    virtual_machine_cost_ = AccumulateVMCost();
    bucket_variable_cost_ = AccumulateBucketCost();
}

// Accumulate the activation exposure
double Solution::AccumulateActivationExposure() {

    double activation_exposure = 0.0;
    for (auto i = 0ul; i < configuration_->get_activations_size(); ++i) {

        auto activation = configuration_->get_activation_per_id(i);
        for (auto j = 0ul; j < activation->get_requirements().size(); ++j) {

            auto virtual_machine_id = activation_allocations_[i];
            if (virtual_machine_id != std::numeric_limits<size_t>::max()) {

                auto virtual_machine = configuration_->get_virtual_machine_per_id(virtual_machine_id);
                if (activation->GetRequirementValue(j) > virtual_machine->GetRequirementValue(j)) {

                    activation_exposure += activation->GetRequirementValue(j) - virtual_machine->GetRequirementValue(j);
                }
            }
        }
    }
    return activation_exposure;
}

// Accumulate the privacy exposure
double Solution::AccumulatePrivacyExposure() {
    auto privacy_exposure = static_cast<double>(file_manager_.get_file_privacy_exposure());
    DLOG(INFO) << "AccumulatePrivacyExposure privacy_exposure " << privacy_exposure;
    return privacy_exposure;
}

void Solution::ComputeConfidentialityExposure() {
    activation_exposure_ = 0.0;
    file_privacy_exposure_ = 0.0;

    activation_exposure_ = AccumulateActivationExposure();
    file_privacy_exposure_ = AccumulatePrivacyExposure();
}

//double Solution::fetch_makespan() { return activation_finish_time_[ordering_.back()]; }

double Solution::fetch_makespan() const {

    return activation_execution_data_[ordering_.back()].get_activation_finish_time();
}

double Solution::fetch_cost() const {

    return virtual_machine_cost_ + bucket_variable_cost_;
}

double Solution::fetch_confidentiality_exposure() const {

    DLOG(INFO) << "fetch_confidentiality_exposure " << activation_exposure_ << " " << file_privacy_exposure_;
    return activation_exposure_ + file_privacy_exposure_;
}

double Solution::ComputeAndFetchOF() {
    double of;

    of = configuration_->get_alpha_time() * makespan_ / configuration_->get_makespan_max()
         + configuration_->get_alpha_budget() * (cost_ / configuration_->get_budget_max())
         + configuration_->get_alpha_security() * (security_exposure_
                                                   / configuration_->get_maximum_security_and_privacy_exposure());

    return of;
}

double Solution::OptimizedComputeObjectiveFunction(size_t start_of_ordering) {
    DLOG(INFO) << "Compute Optimized Objective Function";

    PopulateExecutionAndAllocationsTimeVectors(start_of_ordering);
    ComputeCost();
    ComputeConfidentialityExposure();

    makespan_ = fetch_makespan();
    cost_ = fetch_cost();
    security_exposure_ = fetch_confidentiality_exposure();
    objective_value_ = ComputeAndFetchOF();

    return objective_value_;
}

//double Solution::HeaOptimizedComputeObjectiveFunction(size_t start_of_ordering) {
//    DLOG(INFO) << "Compute Optimized Objective Function";
//
//    PopulateExecutionAndAllocationsTimeVectors(start_of_ordering);
////    ComputeCost();
////    ComputeConfidentialityExposure();
//
//    makespan_ = fetch_makespan();
////    cost_ = fetch_cost();
////    security_exposure_ = fetch_confidentiality_exposure();
////    objective_value_ = HeaComputeAndFetchOF();
//
//    objective_value_ = makespan_;
//
//    return makespan_;
//}

/**
 * Compute the objective value. The objective value is composed by three weighted normalized values:
 *
 * 1. makespan
 * 2. cost
 * 3. security exposure
 *
 *   \f[
 *      \mathrm{min\ \ \ \ \ \ \ }  &
 *      % makespam
 *      \alpha_t \cdot (\frac{z^T}{t_{max}}) + & \label{fo1}\\
 *      % financial cost
 *       &\alpha_b \cdot (  \sum_{j \in M} \frac{c_{j}^M z^T_{j}}{c_{max}}+ \sum_{j \in B_f} \sum_{l \in L_j} \frac{c^B_{jl} b_{jl}}{c_{max}} +
 *      \sum_{j \in B_v} \sum_{l \in L_j} \frac{c^B_{jl} q_{jl}}{c_{max}} )+ & \label{fo2}\\
 *      % security violation penalty
 *      &\alpha_s \cdot (
 *      \sum_{i \in N} \sum_{r \in R} \frac{e^{ri}}{s_{max}} +
 *      \sum_{(d_1,d_2) \in E_s} \frac{p_{d_1d_2} w_{d_1d_2}}{s_{max}}) \label{fo3} &
 *   \f]
 *
 * \param[in]  check_storage    Check the storage for capacity
 * \param[in]  check_sequence   Check the correctness of the sequence
 *
 * \retval     objective_value  The sum of the normalized values of makespan, cost and security exposure
 */
double Solution::ObjectiveFunction(bool check_storage, bool check_sequence) {
    DLOG(INFO) << "Compute Makespan: check_storage[" << check_storage << "], check_sequence[" << check_sequence << "]";

    if (ordering_.size() != configuration_->get_activations_size()) {
        LOG(FATAL) << "Ordering has a wrong number of elements";
    }

    if (check_storage && !checkFiles()) {
        std::cerr << "check file error" << std::endl;
        throw;
    }

    objective_value_ = configuration_->get_alpha_time() * (static_cast<double>(makespan_) / configuration_->get_makespan_max())
                       + configuration_->get_alpha_budget() * (cost_ / configuration_->get_budget_max())
                       + configuration_->get_alpha_security() * (security_exposure_
                                                                 / configuration_->get_maximum_security_and_privacy_exposure());

    return objective_value_;
}

// Check and organize the file based on the storage capacity
/**
 *
 * @return
 */
bool Solution::checkFiles() {
    bool flag = true;
    size_t count = 0;

    std::vector<double> aux_storage(configuration_->get_storage_vet());
    std::vector<size_t> aux(configuration_->get_virtual_machines_size());
    iota(aux.begin(), aux.end(), 0);  // 0,1,2,3,4 ... n

    std::unordered_map<size_t, std::vector<size_t>> map_file;

    size_t id_storage;
    // build file map and compute the storage
    // for (auto it : data->file_map) {
    // for (auto it : configuration_->get_file_map_per_id()) {
    for (size_t it = 0ul; it < configuration_->get_files_size(); ++it) {
        std::shared_ptr<File> file = configuration_->get_file_per_id(it);

        if (std::shared_ptr<DynamicFile> dynamic_file = std::dynamic_pointer_cast<DynamicFile>(file)) {
            id_storage = file_manager_.get_file_allocation(dynamic_file->get_id());
            auto f = map_file.insert(std::make_pair(id_storage, std::vector<size_t>()));
            f.first->second.push_back(dynamic_file->get_id());
            aux_storage[id_storage] -= dynamic_file->get_size_in_GB();
        }
    }

    do {
        // Sort machines based on the storage capacity
        std::sort(aux.begin(), aux.end(), [&](const size_t &a, const size_t &b) {
            return aux_storage[a] < aux_storage[b];
        });

        if (aux_storage[aux[0ul]] < 0.0) {  // If storage is full, start the file heuristic
            std::cout << "Starting file heuristic ..." << std::endl;
            size_t old_vm = aux[0];  // Critical machine
            size_t new_storage = aux[aux.size() - 1];  // Best machine

            auto vet_file = map_file.find(old_vm)->second;

            double min = std::numeric_limits<double>::max();
            // size_t min_file = -1;
            size_t min_file = std::numeric_limits<size_t>::max();

            // Search min file (based on the size of file)
            for_each(vet_file.begin(), vet_file.end(), [&](size_t i) {
                std::cout << i << std::endl;
                std::shared_ptr<File> file = configuration_->get_file_per_id(i);
                std::cout << file->get_name() << std::endl;
                if (file->get_size_in_GB() < min) {
                    min = file->get_size_in_GB();
                    min_file = file->get_id();
                }
            });

            std::shared_ptr<File> file_min = configuration_->get_file_per_id(min_file);

            std::cout << file_min->get_name() << std::endl;
            // MinFile will be moved to machine with more empty space
            file_manager_.set_file_allocation(file_min->get_id(), new_storage);
            // Update aux Storage
            aux_storage[old_vm] += file_min->get_size_in_GB();
            aux_storage[new_storage] -= file_min->get_size_in_GB();
            // Update mapFile structure
            map_file[old_vm].erase(remove(map_file[old_vm].begin(), map_file[old_vm].end(), min_file),
                                   map_file[old_vm].end());
            map_file[new_storage].push_back(min_file);
        } else {
            flag = false;
        }

        count++;
    } while (flag && count < configuration_->get_files_size());
    return !flag;
}  // bool Solution::checkFiles() {

/**
 *
 * @param os
 * @return
 */
std::ostream &Solution::Write(std::ostream &os) const {
    DLOG(INFO) << "Writing solution information";

    os << std::endl;
//    os << "################################################################################################"
//       << std::endl;
//    os << configuration_->get_name() << std::endl;
    os << "################################################################################################"
       << std::endl;
    os << "makespan_: " << makespan_ << " seconds" << std::endl;
    os << "makespan_: " << static_cast<double>(makespan_) / 60.0 << " minutes" << std::endl;
    os << "cost_: " << cost_ << std::endl;
    os << "security_exposure_: " << security_exposure_ << std::endl;
    os << "objective_value_: " << objective_value_ << std::endl;
    os << "Maximum security and privacy exposure: "
       << configuration_->get_maximum_security_and_privacy_exposure()
       << std::endl;

    os << std::endl;
    os << "Ordering: " << std::endl;
    for (size_t i = 0ul; i < ordering_.size(); ++i) {
        os << "ordering_[" << i << "]: \t" << ordering_[i] << "\n";
    }

    os << std::endl;
    os << "Allocations and executions: " << std::endl;
    for (size_t i = 0ul; i < configuration_->get_virtual_machines_size(); ++i) {
        os << "a[" << i << "]: \t" << activation_execution_data_[ordering_.back()].get_vm_allocation_time(i) << "\t";
    }

    os << std::endl;
    for (size_t i = 0ul; i < configuration_->get_virtual_machines_size(); ++i) {
        os << "x[" << i << "]: \t" << activation_execution_data_[ordering_.back()].get_vm_finish_time(i) << "\t";
    }
    os << std::endl << std::endl;

    auto at = configuration_->get_alpha_time();
    auto ac = configuration_->get_alpha_budget();
    auto as = configuration_->get_alpha_security();
    auto arcl = configuration_->get_alpha_restrict_candidate_list();

    os << std::fixed << std::setprecision(6);
    os << "Alphas: " << std::endl;
    os << R"(| $\alpha_{time}$ | $\alpha_{cost}$ | $\alpha_{security}$ | $\alpha_{LRC}$ |)" << std::endl;
    os << "|:----------------:|:----------------:|:--------------------:|:----------------------------------:|" << std::endl;
    os << "| " << at << " | " << ac << " | " << as << " | " << arcl << " |" << std::endl;

    os << std::endl;
    os << "Activations: " << std::endl;
    for (size_t i = 0ul; i < configuration_->get_virtual_machines_size(); ++i) {
        std::shared_ptr<VirtualMachine> vm = configuration_->get_virtual_machine_per_id(i);

        os << "**MV" << vm->get_id() + 1 << "**: \\" << std::endl;
        for (size_t j = 0ul; j < configuration_->get_activations_size(); ++j) {
            std::shared_ptr<Activation> activation = configuration_->get_activation_per_id(j);
            size_t virtual_machine_id = activation_allocations_[j];

            if (virtual_machine_id == vm->get_id()) {
                os << activation->get_tag() << " --> ";
            }
        }
        os << "\\" << std::endl;
    }

    os << std::endl;
    os << "Files: " << std::endl;
    for (size_t i = 0ul; i < configuration_->get_storages_size(); ++i) {
        auto storage = configuration_->get_storage_per_id(i);

        if (const auto s = std::dynamic_pointer_cast<VirtualMachine>(storage)) {
            os << "|**MV" << s->get_id() + 1 << "**| |" << std::endl;
        } else {
            os << "|***Bucket" << storage->get_id() - configuration_->get_virtual_machines_size() + 1 << "***| |" << std::endl;
        }

        for (size_t j = 0ul; j < configuration_->get_files_size(); ++j) {
            auto file = configuration_->get_file_per_id(j);
            size_t storage_id;

            if (std::shared_ptr<StaticFile> static_file = std::dynamic_pointer_cast<StaticFile>(file)) {
                storage_id = static_file->GetFirstVm();
            } else {
                storage_id = file_manager_.get_file_allocation(file->get_id());
            }

            if (storage_id == storage->get_id()) {
                os << "| | " << file->get_name() << " |\n";
            }
        }
        os << std::endl;
    }

    os << std::endl;
    os << "Activation Sequence: " << std::endl;
    for (auto task_id: ordering_) {
        os << configuration_->get_activation_per_id(task_id)->get_tag() << " --> ";
    }
    os << std::endl;
//    os << "################################################################################################"
//       << std::endl;
//    os << configuration_->get_name() << std::endl;
    os << "################################################################################################"
       << std::endl;

    DLOG(INFO) << "Getting out from Solution::Write method";

    return os;
}

/**
 *
 *
 * @param file
 * @param vm
 * @param start_time
 * @param read_time
 * @param run_time
 * @param partial_write_time
 * @return
 */
double Solution::AllocateOneOutputFileGreedily(const std::shared_ptr<Activation> &activation,
                                               const std::shared_ptr<File>& file,
                                               const std::shared_ptr<VirtualMachine> &vm,
                                               const double start_time,
                                               const double read_time,
                                               const double run_time,
                                               const double partial_write_time) {
#ifndef NDEBUG
    DLOG(INFO) << "Computing time for Write the File[" << file->get_id() << "] into VM[" << vm->get_id() << "]";
#endif
    auto activation_id = activation->get_id();
    double partial_objective_value;
    double best_objective_value = std::numeric_limits<double>::max();
    double best_write_one_file_time = std::numeric_limits<double>::max();
    double best_security_exposure = std::numeric_limits<double>::max();
    size_t best_storage_id = std::numeric_limits<size_t>::max();

    auto available_storages = std::vector<std::shared_ptr<Storage>>(configuration_->get_storages_size());
    for (size_t i = 0ul; i < available_storages.size(); ++i) {
        available_storages[i] = configuration_->get_storage_per_id(i);
    }

    // Shuffle the output files for better randomness between the solutions
    std::shuffle(available_storages.begin(), available_storages.end(), generator());

    // for all possible storage; compute the transfer time
    for (size_t i = 0ul; i < available_storages.size(); ++i) {
        auto storage = available_storages[i];
        
        // 1. Calculates the File Transfer Time
//        size_t write_one_file_time;
        if (file_manager_.FileHasHardConstraintsAgainstVmFiles(file->get_id(), storage->get_id())) {
//            write_one_file_time = std::numeric_limits<size_t>::max();
            continue;
        }  // Hard-constraint

        double write_one_file_time = file->GetFileTransfer(vm->get_id(), storage->get_id());

        // 2. Calculates the File Contribution to the Cost
        double cost;
        double virtual_machine_cost = 0.0;
        double allocation_cost = 0.0;
        double bucket_variable_cost = 0.0;

        // Writing cost
        virtual_machine_cost += write_one_file_time * vm->get_cost();

        // Allocation cost
        if (auto inner_vm = std::dynamic_pointer_cast<VirtualMachine>(storage)) {
            double total_time = start_time + read_time + run_time + partial_write_time + write_one_file_time;
            if (total_time > activation_execution_data_[activation_id].get_vm_allocation_time(inner_vm->get_id())) {
                double diff = total_time
                        - activation_execution_data_[activation_id].get_vm_allocation_time(inner_vm->get_id());
                allocation_cost = static_cast<double>(diff) * inner_vm->get_cost();
            }
        }

        // Storage cost
        if (storage->get_id() >= configuration_->get_virtual_machines_size()) {
            bucket_variable_cost += storage->get_cost() * file->get_size_in_GB();
        }

        cost = virtual_machine_cost + allocation_cost + bucket_variable_cost;

        // 3. Calculates the File Security Exposure Contribution
        double security_exposure = ComputeFileSecurityExposureContribution(storage, file);

        DLOG(INFO) << "write_one_file_time: " << write_one_file_time;
        DLOG(INFO) << "cost: " << cost;
        DLOG(INFO) << "security_exposure: " << security_exposure;

        partial_objective_value =
                configuration_->get_alpha_time() *
                (static_cast<double>(write_one_file_time) / configuration_->get_makespan_max())
                + configuration_->get_alpha_budget() * (cost / configuration_->get_budget_max())
                + configuration_->get_alpha_security()
                  * (security_exposure / configuration_->get_maximum_security_and_privacy_exposure());

        if (best_objective_value > partial_objective_value) {
            best_objective_value = partial_objective_value;
            best_storage_id = storage->get_id();
            best_write_one_file_time = write_one_file_time;
            best_security_exposure = security_exposure;

            if (i >= FLAGS_number_of_allocation_experiments - 1) {
                break;
            }
        }
    }

    if (best_objective_value == std::numeric_limits<double>::max()) {
        LOG(FATAL) << "There is no storage available";
    }

#ifndef NDEBUG
    DLOG(INFO) << "Allocation of the output File[" << file->get_id() << "] to the Storage[" << best_storage_id << "]";
#endif

    // Allocate file
    file_manager_.set_file_allocation(file->get_id(), best_storage_id);

    // Store the file contribution to the makespan, cost and security expose
    security_exposure_ += best_security_exposure;

    if (best_storage_id < configuration_->get_virtual_machines_size() && best_storage_id != vm->get_id()) {
        DLOG(INFO) << "allocation_vm_queue_: " << start_time;
        DLOG(INFO) << "allocation_vm_queue_: " << read_time;
        DLOG(INFO) << "allocation_vm_queue_: " << run_time;
        DLOG(INFO) << "allocation_vm_queue_: " << partial_write_time;
        DLOG(INFO) << "allocation_vm_queue_: " << best_write_one_file_time;

        auto vm_allocation_time = activation_execution_data_[activation_id]
                .get_vm_allocation_time(best_storage_id);
        auto computed_time = start_time + read_time + run_time + partial_write_time + best_write_one_file_time;
        activation_execution_data_[activation_id].set_vm_allocation_time(best_storage_id,
                std::max(computed_time, vm_allocation_time));
#ifndef NDEBUG
        DLOG(INFO) << "allocation_vm_queue_[" << best_storage_id << "]: "
                << activation_execution_data_[activation_id].get_vm_allocation_time(best_storage_id);
#endif
    }

    return best_write_one_file_time;
}

/**
 * Calculates the initial task time and returns the maximum between:
 * 1. The longest execution time among all previous tasks.
 * 2. The execution time of all tasks scheduled on the VM.
 *
 * For each previous task
 *   \f[
 *      time = \max_{time, fitness_{previous_task_id}} time
 *   \f]
 *
 * Where:
 * \f$ time \f$ is the maximum finish time of all previous task of the task identified by \c activation_id
 * \f$ fitness_{previous_task_id}} \f$ is the finish time of the previous task
 *
 * \param[in]  activation_id Activation id for which we want to find the start time
 * \param[in]  vm_id         VM where the task will be executed
 * \retval     StartTime     The time at the task identified by \c activation_id will start executing
 */
double Solution::ComputeActivationStartTime(size_t activation_id, size_t vm_id) {
    DLOG(INFO) << "Compute the start time of the Activation[" << activation_id << "] at VM[" << vm_id << "]";
    double start_time{};

    for (auto previous_task_id: configuration_->get_predecessors_ids(activation_id)) {
        auto activation_finish_time = activation_execution_data_[previous_task_id].get_activation_finish_time();
        start_time = std::max(start_time, activation_finish_time);
    }

    DLOG(INFO) << "StartTime: " << start_time;

    return std::max(start_time, activation_execution_data_[activation_id].get_vm_finish_time(vm_id));
}

double Solution::AllocateOutputFiles(const std::shared_ptr<Activation> &activation,
                                     const std::shared_ptr<VirtualMachine> &vm,
                                     const double start_time,
                                     const double read_time,
                                     const double run_time) {

    double write_time{};
    auto output_files = activation->get_output_files();

    // TODO: see if this behaviour is better
    // Shuffle the output files for better randomness between the solutions
    if (output_files.size() > 1) {
        std::shuffle(output_files.begin(), output_files.end(), generator());
    }

    // For each output file Allocate the storage that impose the minor Write time
    for (auto &file: output_files) {
        if (file_manager_.get_file_allocation(file->get_id()) == std::numeric_limits<size_t>::max()) {
            write_time += AllocateOneOutputFileGreedily(activation, file, vm, start_time, read_time, run_time,
                                                        write_time);
        } else {
            std::shared_ptr<Storage> storage = configuration_->get_storage_per_id(file_manager_.get_file_allocation(
                    file->get_id()));
            write_time = file->GetFileTransfer(vm->get_id(), storage->get_id());
        }
    }

    return write_time;
}

double Solution::ComputeActivationReadTime(const std::shared_ptr<Activation> &activation,
                                           const std::shared_ptr<VirtualMachine> &vm,
                                           const double start_time) {
#ifndef NDEBUG
    DLOG(INFO) << "Compute Read Time of the Activation[" << activation->get_id() << "] at VM[" << vm->get_id() << "]";
#endif
    double read_time{};
    auto activation_id = activation->get_id();

    for (const auto &file: activation->get_input_files()) {
        size_t storage_id;

        if (auto static_file = std::dynamic_pointer_cast<StaticFile>(file)) {
            storage_id = static_file->GetFirstVm();
        } else {
            storage_id = file_manager_.get_file_allocation(file->get_id());
        }

        if (storage_id == std::numeric_limits<size_t>::max()) {
            LOG(FATAL) << "Wrong storage_id - ComputeActivationReadTime";
        }

        std::shared_ptr<Storage> file_vm = configuration_->get_storage_per_id(storage_id);

        auto one_file_read_time = file->GetFileTransfer(file_vm->get_id(), vm->get_id());

        if (one_file_read_time == std::numeric_limits<double>::max()) {
            DLOG(INFO) << "read_time: " << one_file_read_time;

            return std::numeric_limits<double>::max();
        } else {
            // read_time += std::ceil(one_file_read_time);
            read_time += one_file_read_time;

            if (storage_id < configuration_->get_virtual_machines_size() and storage_id != vm->get_id()) {
                auto allocation_time = std::max((start_time + read_time),
                        activation_execution_data_[activation_id].get_vm_allocation_time(storage_id));
                activation_execution_data_[activation_id].set_vm_allocation_time(storage_id, allocation_time);
#ifndef NDEBUG
                DLOG(INFO) << "storage_id";
                DLOG(INFO) << "allocation_vm_queue_[" << storage_id << "]: "
                        << activation_execution_data_[activation_id].get_vm_allocation_time(storage_id);
#endif
            }
        }
    }

    DLOG(INFO) << "read_time: " << read_time;

    return read_time;
}

/**
 * This method is important for calculate de makespan and to Allocate the output files into
 * Storages(VMs and Buckets)
 *
 * \param[in]  activation             Activation with which the output files will be allocated
 * \param[in]  virtual_machine  VM where the activation will be executed
 * \retval     makespan         The objective value of the solution when inserting the \c activation
 */
double Solution::CalculateMakespanAndAllocateOutputFiles(const std::shared_ptr<Activation> &activation,
                                                         const std::shared_ptr<VirtualMachine> &virtual_machine) {
#ifndef NDEBUG
    DLOG(INFO) << "Makespan of the allocated Activation[" << activation->get_id() << "] at VM["
               << virtual_machine->get_id() << "]";
#endif
    double finish_time = std::numeric_limits<double>::max();
    double start_time, read_time, run_time, write_time;

    start_time = ComputeActivationStartTime(activation->get_id(), virtual_machine->get_id());
    read_time = ComputeActivationReadTime(activation, virtual_machine, start_time);
//    run_time = std::ceil(activation->get_time() * virtual_machine->get_slowdown());
    run_time = activation->get_time() * virtual_machine->get_slowdown();
    write_time = AllocateOutputFiles(activation, virtual_machine, start_time, read_time, run_time);

    if (start_time != std::numeric_limits<double>::max()
        && read_time != std::numeric_limits<double>::max()
        && write_time != std::numeric_limits<double>::max()) {
        finish_time = start_time + read_time + run_time + write_time;
    } else {
        LOG(FATAL) << "Something is wrong!";
    }

    DLOG(INFO) << "Start time: " << start_time << ", read_time: " << read_time << ", run_time: " << run_time
               << "write_time: " << write_time << "finish_time: " << finish_time;

    return finish_time;
}

/**
 *
 *
 * @param storage
 * @param file
 * @return
 */
double Solution::ComputeFileSecurityExposureContribution(const std::shared_ptr<Storage> &storage,
                                                         const std::shared_ptr<File>& file) {
    double security_exposure;
    double privacy_exposure = 0.0;

    // Accumulate the privacy_exposure
    for (size_t i = 0ul; i < configuration_->get_files_size(); ++i) {
        size_t storage1_id = file_manager_.get_file_allocation(i);
        
        if (storage1_id != std::numeric_limits<size_t>::max()
            && storage->get_id() != std::numeric_limits<size_t>::max()
            && storage1_id == storage->get_id()) {
            // int conflict_value = configuration_->get_conflict_graph().ReturnConflict(i, file->get_id());
            int conflict_value = configuration_->get_conflict_graph()->ReturnConflict(file->get_id(), i);

            if (conflict_value > 0) {
#ifndef NDEBUG
                DLOG(INFO) << "File[" << i << "] has conflict with File[" << file->get_id() << "]";
#endif
                privacy_exposure += conflict_value;  // Adds the penalties
            }
        }
    }

    security_exposure = privacy_exposure;

    DLOG(INFO) << "security_exposure: " << security_exposure;

    return security_exposure;
}

/**
 *
 *
 * @param activation
 * @param vm
 */
void Solution::AllocateTask(const std::shared_ptr<Activation> &activation,
                            const std::shared_ptr<VirtualMachine> &vm) {
    // Allocate Activation
    activation_allocations_[activation->get_id()] = vm->get_id();
}

/**
 *
 */
void Solution::ClearOrdering() {
    ordering_.clear();
}

/**
 *
 * @param id
 */
void Solution::AddOrdering(const size_t id) {
    ordering_.push_back(id);
}

/**
 * Insert the activation in the solution and calculate its objective value.
 *
 * Calculates:
 * 1. makespan
 * 2. cost
 * 3. security exposure
 * And then, return the sum of them.
 *
 * \param[in]  activation             Activation for which we want to find the fitness
 * \param[in]  vm               VM where the activation will be executed
 * \retval     objective_value  The objective value of the solution when inserting the \c activation
 */
double Solution::ScheduleActivation(const std::shared_ptr<Activation> &activation,
                                    const std::shared_ptr<VirtualMachine> &vm) {
#ifndef NDEBUG
    DLOG(INFO) << "Begin schedule the Activation[" << activation->get_id() << "] at VM[" << vm->get_id() << "]";
#endif
    auto activation_id = activation->get_id();
    // Allocate Activation
    activation_allocations_[activation->get_id()] = vm->get_id();
    ordering_.push_back(activation->get_id());

    // Fetching some previous Activation Execution Data
    if (ordering_.size() > 1ul) {
        for (auto vm_id = 0ul; vm_id < configuration_->get_virtual_machines_size(); vm_id++) {
            auto previous_activation_id = ordering_[ordering_.size() - 2];
            activation_execution_data_[activation_id].set_vm_finish_time(vm_id,
                    activation_execution_data_[previous_activation_id].get_vm_finish_time(vm_id));
            activation_execution_data_[activation_id].set_vm_allocation_time(vm_id,
                    activation_execution_data_[previous_activation_id].get_vm_allocation_time(vm_id));
        }
    }

    // 1. Calculates the finish_time
    double finish_time = CalculateMakespanAndAllocateOutputFiles(activation, vm);

    // Update auxiliary structures (queue_ and activation_finish_time_)
    // This update is important for the cost calculation
//    activation_finish_time_[activation->get_id()] = finish_time;
//    vm_finish_time_[vm->get_id()] = finish_time;
//    vm_allocation_time_[vm->get_id()] = std::max<size_t>(finish_time, vm_allocation_time_[vm->get_id()]);
//    DLOG(INFO) << "vm->get_id()";
//    DLOG(INFO) << "allocation_vm_queue_[" << vm->get_id() << "]: " << vm_allocation_time_[vm->get_id()];
//    makespan_ = activation_finish_time_[ordering_.back()];

    activation_execution_data_[activation_id].set_activation_finish_time(finish_time);
    activation_execution_data_[activation_id].set_vm_finish_time(vm->get_id(), finish_time);
    auto allocation_time = activation_execution_data_[activation_id].get_vm_allocation_time(vm->get_id());
    activation_execution_data_[activation_id].set_vm_allocation_time(vm->get_id(),
            std::max(finish_time, allocation_time));
#ifndef NDEBUG
    DLOG(INFO) << "vm->get_id()";
    DLOG(INFO) << "allocation_vm_queue_[" << vm->get_id() << "]: "
            << activation_execution_data_[activation_id].get_vm_allocation_time(vm->get_id());
#endif
    makespan_ = activation_execution_data_[activation_id].get_activation_finish_time();

    // 2. Calculates the cost contribution of the activation execution at the virtual machine
    DLOG(INFO) << "Calculates the cost contribution of the Virtual Machine Cost of the scheduled activation";
    double virtual_machine_cost = 0.0;
    double bucket_variable_cost = 0.0;

    // Accumulate the Virtual Machine cost
//    for (size_t i = 0ul; i < configuration_->get_virtual_machines_size(); ++i) {
//        std::shared_ptr<VirtualMachine> virtual_machine = configuration_->get_virtual_machine_per_id(i);
//        virtual_machine_cost += static_cast<double>(vm_allocation_time_[virtual_machine->get_id()])
//                                * virtual_machine->get_cost();
//        DLOG(INFO) << "allocation_vm_queue_[virtual_machine->get_id()]: "
//                   << vm_allocation_time_[virtual_machine->get_id()];
//        DLOG(INFO) << "virtual_machine->get_cost(): " << virtual_machine->get_cost();
//    }
    for (size_t i = 0ul; i < configuration_->get_virtual_machines_size(); ++i) {
        std::shared_ptr<VirtualMachine> virtual_machine = configuration_->get_virtual_machine_per_id(i);
        auto vm_allocation_time = activation_execution_data_[activation_id].get_vm_allocation_time(
                virtual_machine->get_id());
        auto alloc_time = static_cast<double>(vm_allocation_time);
//        virtual_machine_cost += static_cast<double>(vm_allocation_time) * virtual_machine->get_cost();
//        virtual_machine_cost += (alloc_time / 3600) * virtual_machine->get_cost();
        virtual_machine_cost += alloc_time * virtual_machine->get_cost();
#ifndef NDEBUG
        DLOG(INFO) << "allocation_vm_queue_[virtual_machine->get_id()]: "
                   << activation_execution_data_[activation_id].get_vm_allocation_time(virtual_machine->get_id());
        DLOG(INFO) << "virtual_machine->get_cost(): " << virtual_machine->get_cost();
#endif
    }

    // Accumulate the Bucket variable cost
    for (size_t i = configuration_->get_virtual_machines_size(); i < configuration_->get_storages_size(); ++i) {
        std::shared_ptr<Storage> storage = configuration_->get_storage_per_id(i);

        // If the storage is not a Virtual Machine, i.e. it is a Bucket; then calculate the bucket
        // variable cost
        for (size_t j = 0ul; j < configuration_->get_files_size(); ++j) {
            // If the Bucket is used; then accumulate de cost and break to the next Storage
            if (file_manager_.get_file_allocation(j) == storage->get_id()) {
                std::shared_ptr<File> file = configuration_->get_file_per_id(j);
                bucket_variable_cost += (storage->get_cost() * file->get_size_in_GB());
#ifndef NDEBUG
                DLOG(INFO) << "storage->get_cost(): " << storage->get_cost();
                DLOG(INFO) << "file->get_size_in_GB(): " << file->get_size_in_GB();
#endif
            }
        }
    }
    cost_ = virtual_machine_cost + bucket_variable_cost;

    // 3. Calculates the security exposure
    DLOG(INFO) << "Accumulate the activation Exposure of the scheduled activation";

    // Accumulate the activation exposure
//    for (size_t j = 0; j < activation->get_requirements().size(); ++j) {
//        size_t virtual_machine_id = activation_allocations_[activation->get_id()];
//
//        // If the activation is allocated
//        if (virtual_machine_id != std::numeric_limits<size_t>::max()) {
//            std::shared_ptr<VirtualMachine> virtual_machine = configuration_->get_virtual_machine_per_id(virtual_machine_id);
//
//            if (activation->GetRequirementValue(j) > virtual_machine->GetRequirementValue(j)) {
//                security_exposure_ += activation->GetRequirementValue(j) - virtual_machine->GetRequirementValue(j);
//            }
//        }
//    }

    ComputeConfidentialityExposure();

    security_exposure_ = fetch_confidentiality_exposure();

    // If solution is unfeasible return size_t max
    if (makespan_ == std::numeric_limits<double>::max()
        || cost_ == std::numeric_limits<double>::max()
        || security_exposure_ == std::numeric_limits<double>::max()) {
        objective_value_ = std::numeric_limits<double>::max();
    } else { // o.w. return O.F.
        objective_value_ =
                configuration_->get_alpha_time() * (static_cast<double>(makespan_) / configuration_->get_makespan_max())
                + configuration_->get_alpha_budget() * (cost_ / configuration_->get_budget_max())
                + configuration_->get_alpha_security() * (security_exposure_
                                                          / configuration_->get_maximum_security_and_privacy_exposure());
    }

    DLOG(INFO) << "Makespan " << makespan_
               << ", cost " << cost_
               << ", security " << security_exposure_
               << ", o.f. " << objective_value_;

    return objective_value_;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

/**
 *
 *
 * @param node
 *
 * @return
 */
int Solution::ComputeTasksHeights(size_t node) {
    int min = std::numeric_limits<int>::max();
    if (activation_height_[node] != -1)
        return activation_height_[node];
    if (node != configuration_->get_target_id()) {
        for (auto i: configuration_->get_successors_ids(node)) {
            int value = ComputeTasksHeights(i);
            min = std::min(value, min);
        }
    } else {
        activation_height_[node] = configuration_->get_height(node);
        return activation_height_[node];
    }
    std::uniform_int_distribution<> dis(configuration_->get_height(node), min - 1);
    activation_height_[node] = dis(generator());
    return activation_height_[node];
}

#pragma clang diagnostic pop

/**
 * N1 - Swap-vm
 * For each pair of Activations (i, j), if them both are not assigned to the same VM, swap VMs and recompute O.F.
 * If better O.F. keep it, o.w. undo the swapping and restore O.F. values.
 * @return
 */
bool Solution::localSearchN1() {
    DLOG(INFO) << "Executing localSearchN1 local search ...";
    double best_known_of = objective_value_;
    double best_known_makespan = makespan_;
    double best_known_cost = cost_;
    double best_known_security_exposure_ = security_exposure_;
    for (auto i = 1ul; i < configuration_->get_activations_size() - 2ul; i++) {
        for (auto j = i + 2ul; j < configuration_->get_activations_size() - 1ul; j++) {
            if (activation_allocations_[i] != activation_allocations_[j]) {
                bool was_file_changed;
                auto i_vm = activation_allocations_[i];
                auto j_vm = activation_allocations_[j];
                std::deque<std::pair<size_t, size_t>> files_changed;
                // Do the swap
                iter_swap(activation_allocations_.begin() + static_cast<long int>(i),
                          activation_allocations_.begin() + static_cast<long int>(j));
                auto i_activation = configuration_->get_activation_per_id(i);
                auto i_input_files = i_activation->get_input_files();
                for (const auto &input_file : i_input_files) {
                    if (auto dynamic_file = std::dynamic_pointer_cast<DynamicFile>(input_file)) {
                        auto file_id = dynamic_file->get_id();
//                        auto file_allocation = file_allocations_[file_id];
                        auto file_allocation = file_manager_.get_file_allocation(file_id);
                        if (file_allocation == i_vm) {
                            auto it = std::find_if(files_changed.begin(), files_changed.end(),
                                                   [file_id](const std::pair<size_t, size_t>& my_pair) {
                                                       return my_pair.first == file_id;
                                                   });
                            if (it == files_changed.end()) {
//                                file_allocations_[file_id] = j_vm;
//                                file_manager_.set_file_allocation(file_id, j_vm);
                                if (file_manager_.get_file_allocation(file_id) != j_vm) {
                                    was_file_changed = file_manager_.ChangeFileAllocation(file_id, j_vm);
                                } else {
                                    was_file_changed = false;
                                }

                                if (was_file_changed) {
                                    files_changed.emplace_back(file_id, file_allocation);
                                }
                            }
                        }
                    }
                }
                auto i_output_files = i_activation->get_output_files();
                for (const auto &output_file : i_output_files) {
                    if (auto dynamic_file = std::dynamic_pointer_cast<DynamicFile>(output_file)) {
                        auto file_id = dynamic_file->get_id();
//                        auto file_allocation = file_allocations_[file_id];
                        auto file_allocation = file_manager_.get_file_allocation(file_id);
                        if (file_allocation == i_vm) {
                            auto it = std::find_if(files_changed.begin(), files_changed.end(),
                                                   [file_id](const std::pair<size_t, size_t>& my_pair) {
                                                       return my_pair.first == file_id;
                                                   });
                            if (it == files_changed.end()) {
//                                file_allocations_[file_id] = j_vm;
//                                file_manager_.set_file_allocation(file_id, j_vm);
                                if (file_manager_.get_file_allocation(file_id) != j_vm) {
                                    was_file_changed = file_manager_.ChangeFileAllocation(file_id, j_vm);
                                } else {
                                    was_file_changed = false;
                                }

                                if (was_file_changed) {
                                    files_changed.emplace_back(file_id, file_allocation);
                                }
                            }
                        }
                    }
                }
                auto j_activation = configuration_->get_activation_per_id(j);
                auto j_input_files = j_activation->get_input_files();
                for (const auto &input_file : j_input_files) {
                    if (auto dynamic_file = std::dynamic_pointer_cast<DynamicFile>(input_file)) {
                        auto file_id = dynamic_file->get_id();
//                        auto file_allocation = file_allocations_[file_id];
                        auto file_allocation = file_manager_.get_file_allocation(file_id);
                        if (file_allocation == j_vm) {
                            auto it = std::find_if(files_changed.begin(), files_changed.end(),
                                                   [file_id](const std::pair<size_t, size_t>& my_pair) {
                                                       return my_pair.first == file_id;
                                                   });
                            if (it == files_changed.end()) {
//                                file_allocations_[file_id] = i_vm;
//                                file_manager_.set_file_allocation(file_id, i_vm);
                                if (file_manager_.get_file_allocation(file_id) != i_vm) {
                                    was_file_changed = file_manager_.ChangeFileAllocation(file_id, i_vm);
                                } else {
                                    was_file_changed = false;
                                }

                                if (was_file_changed) {
                                    files_changed.emplace_back(file_id, file_allocation);
                                }
                            }
                        }
                    }
                }
                auto j_output_files = j_activation->get_output_files();
                for (const auto &output_file : j_output_files) {
                    if (auto dynamic_file = std::dynamic_pointer_cast<DynamicFile>(output_file)) {
                        auto file_id = dynamic_file->get_id();
//                        auto file_allocation = file_allocations_[file_id];
                        auto file_allocation = file_manager_.get_file_allocation(file_id);
                        if (file_allocation == j_vm) {
                            auto it = std::find_if(files_changed.begin(), files_changed.end(),
                                                   [file_id](const std::pair<size_t, size_t>& my_pair) {
                                                       return my_pair.first == file_id;
                                                   });
                            if (it == files_changed.end()) {
//                                file_allocations_[file_id] = i_vm;
//                                file_manager_.set_file_allocation(file_id, i_vm);
                                if (file_manager_.get_file_allocation(file_id) != i_vm) {
                                    was_file_changed = file_manager_.ChangeFileAllocation(file_id, i_vm);
                                } else {
                                    was_file_changed = false;
                                }

                                if (was_file_changed) {
                                    files_changed.emplace_back(file_id, file_allocation);
                                }
                            }
                        }
                    }
                }
//                ComputeObjectiveFunction();
                //
//                auto it = std::find_if(ordering_.begin(), ordering_.end(), [&](size_t elem) {
//                    return (elem == ordering_[i]) || (elem == ordering_[j]);
//                });
//                auto index = std::distance(ordering_.begin(), it);
//                OptimizedComputeObjectiveFunction(index);
                OptimizedComputeObjectiveFunction();
                DLOG(INFO) << "... localSearchN1 : " << objective_value_ << " < " << best_known_of
                           << std::endl;
                if (objective_value_ < best_known_of) {
                    DLOG(INFO) << "... localSearchN1 : " << objective_value_ << " < " << best_known_of
                               << std::endl;
                    DLOG(INFO) << "... ending localSearchN1 local search";
//                    std::cout << "LS One O.F. " << objective_value_ << std::endl;
                    return true;
                }

                // Return elements
                iter_swap(activation_allocations_.begin() + static_cast<long int>(i),
                          activation_allocations_.begin() + static_cast<long int>(j));
                while (!files_changed.empty()) {
                    auto my_pair = files_changed.front();
//                    file_allocations_[my_pair.first] = my_pair.second;
//                    file_manager_.set_file_allocation(my_pair.first, my_pair.second);
                    was_file_changed = file_manager_.ChangeFileAllocation(my_pair.first, my_pair.second);
                    // TODO: It seams that the fatal log below is never reachable. ChangeFileAllocation already treats
                    // same origin fault.
                    if (!was_file_changed) {
                        LOG(FATAL) << "Makes no sense, file should be able to move back";
                    }
                    files_changed.pop_front();
                }
                //
//                OptimizedComputeObjectiveFunction(index);
//                OptimizedComputeObjectiveFunction();
                objective_value_ = best_known_of;
                makespan_ = best_known_makespan;
                cost_ = best_known_cost;
                security_exposure_ = best_known_security_exposure_;
            }
        }
    }
    DLOG(INFO) << "... ending localSearchN1 local search";
    return false;
}

/**
 * N2 - Swap position
 *
 * @return
 */
bool Solution::localSearchN2() {
    DLOG(INFO) << "Executing localSearchN2 local search ...";
    double best_known_of = objective_value_;
    double best_known_makespan = makespan_;
    double best_known_cost = cost_;
    double best_known_security_exposure_ = security_exposure_;
    // for each task, do
    for (auto i = 1ul; i < configuration_->get_activations_size() - 2ul; i++) {
        auto task_i = ordering_[i];
        for (auto j = i + 2ul; j < configuration_->get_activations_size() - 1ul; j++) {
            auto task_j = ordering_[j];
            if (activation_height_[task_i] == activation_height_[task_j]) {
                // Do the swap
                iter_swap(ordering_.begin() + static_cast<long int>(i), ordering_.begin() + static_cast<long int>(j));
//                ComputeObjectiveFunction();
//
//                auto it = std::find_if(ordering_.begin(), ordering_.end(), [&](size_t elem) {
//                    return (elem == ordering_[i]) || (elem == ordering_[j]);
//                });
//                auto index = std::distance(ordering_.begin(), it);
//                OptimizedComputeObjectiveFunction(index);
                OptimizedComputeObjectiveFunction();
                DLOG(INFO) << "new objective value " << objective_value_ << " i " << i << " j " << j << std::endl;
                if (objective_value_ < best_known_of) {
                    DLOG(INFO) << "... localSearchN2 : " << objective_value_ << " < " << best_known_of
                               << std::endl;
                    DLOG(INFO) << "... ending localSearchN2 local search";
//                    std::cout << "LS Two O.F. " << objective_value_ << std::endl;
                    return true;
                }
                // Return elements
                iter_swap(ordering_.begin() + static_cast<long int>(i), ordering_.begin() + static_cast<long int>(j));
                //
//                OptimizedComputeObjectiveFunction(index);
//                ComputeObjectiveFunction();
//                OptimizedComputeObjectiveFunction();
                objective_value_ = best_known_of;
                makespan_ = best_known_makespan;
                cost_ = best_known_cost;
                security_exposure_ = best_known_security_exposure_;
            } else {
                break;
            }
        }
    }
    DLOG(INFO) << "... ending localSearchN2 local search";
    return false;
}

/**
 * N3 = Move-1 Element
 *
 * @return
 */
bool Solution::localSearchN3() {

    DLOG(INFO) << "Executing localSearchN3 local search ...";
    double best_known_of = objective_value_;
    double best_known_makespan = makespan_;
    double best_known_cost = cost_;
    double best_known_security_exposure_ = security_exposure_;
    for (auto i = 1ul; i < configuration_->get_activations_size() - 1ul; ++i) {
        bool was_file_changed;
        std::deque<std::pair<size_t, size_t>> files_changed;
        auto old_vm_id = activation_allocations_[i];
        for (auto new_vm_id = 0ul; new_vm_id < configuration_->get_virtual_machines_size(); new_vm_id++) {
            if (old_vm_id != new_vm_id) {
                activation_allocations_[i] = new_vm_id;
                auto i_activation = configuration_->get_activation_per_id(i);
                auto i_input_files = i_activation->get_input_files();
                for (const auto &input_file : i_input_files) {
                    if (auto dynamic_file = std::dynamic_pointer_cast<DynamicFile>(input_file)) {
                        auto file_id = dynamic_file->get_id();
//                        auto file_allocation = file_allocations_[file_id];
                        auto file_allocation = file_manager_.get_file_allocation(file_id);
                        if (file_allocation == old_vm_id) {
                            auto it = std::find_if(files_changed.begin(), files_changed.end(),
                                                   [file_id](const std::pair<size_t, size_t>& my_pair) {
                                                       return my_pair.first == file_id;
                                                   });
                            if (it == files_changed.end()) {
//                                file_allocations_[file_id] = old_vm_id;
//                                file_manager_.set_file_allocation(file_id, old_vm_id);
                                if (file_manager_.get_file_allocation(file_id) != new_vm_id) {
                                    was_file_changed = file_manager_.ChangeFileAllocation(file_id, new_vm_id);
                                } else {
                                    was_file_changed = false;
                                }

                                if (was_file_changed) {
                                    files_changed.emplace_back(file_id, file_allocation);
                                }
                            }
                        }
                    }
                }
                auto i_output_files = i_activation->get_output_files();
                for (const auto &output_file : i_output_files) {
                    if (auto dynamic_file = std::dynamic_pointer_cast<DynamicFile>(output_file)) {
                        auto file_id = dynamic_file->get_id();
//                        auto file_allocation = file_allocations_[file_id];
                        auto file_allocation = file_manager_.get_file_allocation(file_id);
                        if (file_allocation == old_vm_id) {
                            auto it = std::find_if(files_changed.begin(), files_changed.end(),
                                                   [file_id](const std::pair<size_t, size_t>& my_pair) {
                                                       return my_pair.first == file_id;
                                                   });
                            if (it == files_changed.end()) {
//                                file_allocations_[file_id] = old_vm_id;
//                                file_manager_.set_file_allocation(file_id, old_vm_id);
                                if (file_manager_.get_file_allocation(file_id) != new_vm_id) {
                                    was_file_changed = file_manager_.ChangeFileAllocation(file_id, new_vm_id);
                                } else {
                                    was_file_changed = false;
                                }

                                if (was_file_changed) {
                                    files_changed.emplace_back(file_id, file_allocation);
                                }
                            }
                        }
                    }
                }
//                ComputeObjectiveFunction();
                //
//                auto it = std::find_if(ordering_.begin(), ordering_.end(), [&](size_t elem) {
//                    return elem == ordering_[i];
//                });
//                auto index = std::distance(ordering_.begin(), it);
//                OptimizedComputeObjectiveFunction(index);
                OptimizedComputeObjectiveFunction();
//                makespan_ = of_makespan;
//                cost_ = of_cost;
//                security_exposure_ = of_security_exposure;
//                objective_value_ = of;
                DLOG(INFO) << "new objective value " << objective_value_ << " i " << i << " new_vm_id " << new_vm_id << std::endl;
                if (objective_value_ < best_known_of) {
                    DLOG(INFO) << "... localSearchN3 : " << objective_value_ << " < " << best_known_of
                               << std::endl;
                    DLOG(INFO) << "... ending localSearchN3 local search";
//                    std::cout << "LS Three O.F. " << objective_value_ << std::endl;
                    return true;
                }
                // Change back
                activation_allocations_[i] = old_vm_id;
                while (!files_changed.empty()) {
                    auto my_pair = files_changed.front();
//            file_allocations_[my_pair.first] = my_pair.second;
//            file_manager_.set_file_allocation(my_pair.first, my_pair.second);
                    was_file_changed = file_manager_.ChangeFileAllocation(my_pair.first, my_pair.second);
                    // TODO: maybe the code below isn't necessary
                    if (!was_file_changed) {
                        LOG(FATAL) << "Makes no sense, file should be able to move back";
                    }
                    files_changed.pop_front();
                }
                //
//                OptimizedComputeObjectiveFunction(index);
//                OptimizedComputeObjectiveFunction();
                objective_value_ = best_known_of;
                makespan_ = best_known_makespan;
                cost_ = best_known_cost;
                security_exposure_ = best_known_security_exposure_;
            }
        }
    }
    DLOG(INFO) << "... ending localSearchN3 local search";
    return false;
}

// =========== HEA Local search functions  ========= //

// N1 - Swap-vm
bool Solution::HeaLocalSearchN1() {
    double best_known_of = objective_value_;
    double best_known_makespan = makespan_;
    double best_known_cost = cost_;
    double best_known_security_exposure_ = security_exposure_;
    size_t data_size = configuration_->get_activations_size() + configuration_->get_files_size();

    for (size_t i{}; i < data_size; i++) {
        for (size_t j = i + 1; j < data_size; j++) {
            size_t vm_a = (i >= configuration_->get_activations_size())
                    ? file_manager_.get_file_allocation(i - configuration_->get_activations_size())
                    : activation_allocations_[i];
            size_t vm_b = (j >= configuration_->get_activations_size())
                    ? file_manager_.get_file_allocation(j - configuration_->get_activations_size())
                    : activation_allocations_[j];
            if (vm_a != vm_b) {
                //do the swap
                if (i >= configuration_->get_activations_size()) {
                    if (file_manager_.get_file_allocation(i - configuration_->get_activations_size()) != vm_b) {
                        file_manager_.ChangeFileAllocation(i - configuration_->get_activations_size(), vm_b);
                    }
                } else {
                    activation_allocations_[i] = vm_b;
                }
                if (j >= configuration_->get_activations_size()) {
                    if (file_manager_.get_file_allocation(j - configuration_->get_activations_size()) != vm_a) {
                        file_manager_.ChangeFileAllocation(j - configuration_->get_activations_size(), vm_a);
                    }
                } else {
                    activation_allocations_[j] = vm_a;
                }
                OptimizedComputeObjectiveFunction();
                if (get_objective_value() < best_known_of) {
                    return true;
                }
                //return elements
                if (i >= configuration_->get_activations_size()) {
                    if (file_manager_.get_file_allocation(i - configuration_->get_activations_size()) != vm_a) {
                        file_manager_.ChangeFileAllocation(i - configuration_->get_activations_size(), vm_a);
                    }
                } else {
                    activation_allocations_[i] = vm_a;
                }
                if (j >= configuration_->get_activations_size()) {
                    if (file_manager_.get_file_allocation(j - configuration_->get_activations_size()) != vm_b) {
                        file_manager_.ChangeFileAllocation(j - configuration_->get_activations_size(), vm_b);
                    }
                } else {
                    activation_allocations_[j] = vm_b;
                }
                objective_value_ = best_known_of;
                makespan_ = best_known_makespan;
                cost_ = best_known_cost;
                security_exposure_ = best_known_security_exposure_;
            }
        }
    }
    return false;
}

// N2 - Swap position
bool Solution::HeaLocalSearchN2() {
    double best_known_of = objective_value_;
    double best_known_makespan = makespan_;
    double best_known_cost = cost_;
    double best_known_security_exposure_ = security_exposure_;

    // For each task, do
    for (size_t i{}; i < configuration_->get_activations_size(); i++) {
        auto task_i = ordering_[i];
        for (size_t j = i + 1; j < configuration_->get_activations_size(); j++) {
            auto task_j = ordering_[j];
            if (activation_height_[task_i] == activation_height_[task_j]) {
                // Do the swap
                iter_swap(ordering_.begin() + static_cast<long>(i), ordering_.begin() + static_cast<long>(j));
                OptimizedComputeObjectiveFunction();
                if (get_objective_value() < best_known_of) {
                    return true;
                }
                // Return elements
                objective_value_ = best_known_of;
                makespan_ = best_known_makespan;
                cost_ = best_known_cost;
                security_exposure_ = best_known_security_exposure_;
            } else
                break;
        }
    }
    return false;
}

// N3 = Move-1 Element
bool Solution::HeaLocalSearchN3() {
    double best_known_of = objective_value_;
    double best_known_makespan = makespan_;
    double best_known_cost = cost_;
    double best_known_security_exposure_ = security_exposure_;
    size_t data_size = configuration_->get_activations_size() + configuration_->get_files_size();

    for (size_t act_or_file_id{}; act_or_file_id < data_size; ++act_or_file_id) {
        size_t old_vm = (act_or_file_id >= configuration_->get_activations_size())
                      ? file_manager_.get_file_allocation(act_or_file_id - configuration_->get_activations_size())
                      : activation_allocations_[act_or_file_id];
        for (size_t vm_id{}; vm_id < configuration_->get_virtual_machines_size(); vm_id++) {
            if (old_vm != vm_id) {
                if (act_or_file_id >= configuration_->get_activations_size()) {
                    if (file_manager_.get_file_allocation(act_or_file_id - configuration_->get_activations_size())
                            != vm_id) {
                        file_manager_.ChangeFileAllocation(act_or_file_id - configuration_->get_activations_size(),
                                                           vm_id);
                    }
                } else {
                    activation_allocations_[act_or_file_id] = vm_id;
                }
                OptimizedComputeObjectiveFunction();
                if (get_objective_value() < best_known_of) {
                    return true;
                }
            }
        }
        // Return elements
        objective_value_ = best_known_of;
        makespan_ = best_known_makespan;
        cost_ = best_known_cost;
        security_exposure_ = best_known_security_exposure_;
    }
    return false;
}
