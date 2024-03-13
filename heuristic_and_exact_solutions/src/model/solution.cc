/**
 * \file src/model/solution.cc
 * \brief Contains the \c Solution class definition
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2021
 *
 * This source file contains the \c Solution class definition
 */

#include <iostream>
#include <deque>
#include <utility>  // Para std::pair
#include <algorithm>  // Para std::find_if
#include "src/model/solution.h"

DECLARE_uint64(number_of_allocation_experiments);

// TODO: Remove those variables.
//auto of_makespan = 0ul;
//auto of_cost = 0.0;
//auto of_security_exposure = 0.0;
//auto of = 0.0;

/**
 * Allocate memory for debugging purpose.
 */
void Solution::MemoryAllocation() {
    x = new int **[n_];
    for (auto i = 0ul; i < n_; ++i) {
        x[i] = new int *[m_];
        for (auto j = 0ul; j < m_; ++j) {
            x[i][j] = new int[t_];
            std::fill_n(x[i][j], t_, -1);
        }
    }

    r = new int ****[n_];
    for (auto i = 0ul; i < n_; ++i) {
        r[i] = new int ***[d_];
        for (auto j = 0ul; j < d_; ++j) {
            r[i][j] = new int **[m_];
            for (auto k = 0ul; k < m_; ++k) {
                r[i][j][k] = new int *[mb_];
                for (auto l = 0ul; l < mb_; ++l) {
                    r[i][j][k][l] = new int[t_];
                    std::fill_n(r[i][j][k][l], t_, -1);
                }
            }
        }
    }

    w = new int ****[n_];
    for (auto i = 0ul; i < n_; ++i) {
        w[i] = new int ***[d_];
        for (auto j = 0ul; j < d_; ++j) {
            w[i][j] = new int **[m_];
            for (auto k = 0ul; k < m_; ++k) {
                w[i][j][k] = new int *[mb_];
                for (auto l = 0ul; l < mb_; ++l) {
                    w[i][j][k][l] = new int[t_];
                    std::fill_n(w[i][j][k][l], t_, -1);
                }
            }
        }
    }

    y = new int **[d_];
    for (auto i = 0ul; i < d_; ++i) {
        y[i] = new int *[mb_];
        for (auto j = 0ul; j < mb_; ++j) {
            y[i][j] = new int[t_ + 1];
            std::fill_n(y[i][j], t_, -1);
        }
    }

    v = new int *[m_];
    for (auto i = 0ul; i < m_; ++i) {
        v[i] = new int[t_];
        std::fill_n(v[i], t_, -1);
    }
}

/**
 * Should be called after Solution::MemoryAllocation().
 */
void Solution::FreeingMemoryAllocated() {
    for (auto i = 0ul; i < n_; ++i) {
        for (size_t j = 0; j < m_; ++j) {
            delete[] x[i][j];
            x[i][j] = nullptr;
        }
        delete[] x[i];
        x[i] = nullptr;
    }
    delete[] x;
    x = nullptr;

    for (auto i = 0ul; i < n_; ++i) {
        for (auto j = 0ul; j < d_; ++j) {
            for (auto k = 0ul; k < m_; ++k) {
                for (auto l = 0ul; l < mb_; ++l) {
                    delete[] r[i][j][k][l];
                    r[i][j][k][l] = nullptr;
                }
                delete[] r[i][j][k];
                r[i][j][k] = nullptr;
            }
            delete[] r[i][j];
            r[i][j] = nullptr;
        }
        delete[] r[i];
        r[i] = nullptr;
    }
    delete[] r;
    r = nullptr;

    for (auto i = 0ul; i < n_; ++i) {
        for (auto j = 0ul; j < d_; ++j) {
            for (auto k = 0ul; k < m_; ++k) {
                for (auto l = 0ul; l < mb_; ++l) {
                    delete[] w[i][j][k][l];
                    w[i][j][k][l] = nullptr;
                }
                delete[] w[i][j][k];
                w[i][j][k] = nullptr;
            }
            delete[] w[i][j];
            w[i][j] = nullptr;
        }
        delete[] w[i];
        w[i] = nullptr;
    }
    delete[] w;
    w = nullptr;

    for (auto i = 0ul; i < d_; ++i) {
        for (auto j = 0ul; j < mb_; ++j) {
            delete[] y[i][j];
            y[i][j] = nullptr;
        }
        delete[] y[i];
        y[i] = nullptr;
    }
    delete[] y;
    y = nullptr;

    for (auto i = 0ul; i < m_; ++i) {
        delete[] v[i];
        v[i] = nullptr;
    }
    delete[] v;
    v = nullptr;
}

/**
 * Parameterised constructor.
 *
 * @param algorithm
 */
Solution::Solution(Algorithm *algorithm)
        : algorithm_(algorithm),
          activation_allocations_(algorithm->GetActivationSize(), std::numeric_limits<size_t>::max()),
          file_allocations_(algorithm->GetFilesSize(), std::numeric_limits<size_t>::max()),
          activation_height_(algorithm->GetActivationSize(), -1),
          activation_finish_time_(algorithm->GetActivationSize(), 0ul),
          vm_finish_time_(algorithm->GetVirtualMachineSize(), 0ul),
          allocation_vm_queue_(algorithm->GetVirtualMachineSize(), 0ul),
          makespan_(0.0),
          cost_(0.0),
          security_exposure_(0.0),
          n_(algorithm_->GetActivationSize() - 2),
          d_(algorithm_->GetFilesSize()),
          m_(algorithm_->GetVirtualMachineSize()),
          mb_(algorithm_->GetStorageSize()),
          t_(static_cast<size_t>(algorithm_->get_makespan_max())) {
    // Initialize the allocation with the static files place information (VM or Bucket)
    for (size_t i = 0ul; i < algorithm_->GetFilesSize(); ++i) {
        auto file = algorithm_->GetFilePerId(i);
        if (auto static_file = std::dynamic_pointer_cast<StaticFile>(file)) {
            SetFileAllocation(file->get_id(), static_file->GetFirstVm());
        }
    }

    // Setting Activations Heights
    ComputeTasksHeights(algorithm->get_id_source());
}

/**
 *
 */
Solution::~Solution() {
    algorithm_ = nullptr;
}

/**
 *
 *
 * @param other
 */
Solution::Solution(const Solution &other)
        : algorithm_(other.algorithm_),
          activation_allocations_(other.activation_allocations_),
          file_allocations_(other.file_allocations_),
          ordering_(other.ordering_),
          activation_height_(other.activation_height_),
          activation_finish_time_(other.activation_finish_time_),
          vm_finish_time_(other.vm_finish_time_),
          allocation_vm_queue_(other.allocation_vm_queue_),
          makespan_(other.makespan_),
          cost_(other.cost_),
          security_exposure_(other.security_exposure_),
          objective_value_(other.objective_value_),
          n_(other.n_),
          d_(other.d_),
          m_(other.m_),
          mb_(other.mb_),
          t_(other.t_) {
    DLOG(INFO) << "Initiating copy constructor of solution ...";
    DLOG(INFO) << "... copy constructor of solution finished";
}

/**
 *
 *
 * @param file_id
 * @param storage_id
 * @param initial_time
 * @param final_time
 */
void Solution::PersistFile(size_t file_id, size_t storage_id, size_t initial_time, size_t final_time) const {
    auto ft = final_time;
    auto msm = static_cast<size_t>(algorithm_->get_makespan_max());

    DLOG(INFO) << "Makespan Max: " << msm << std::endl;
    DLOG(INFO) << "PersistFile(" << file_id << ", " << storage_id << ", " << initial_time << ", " << ft << ");";

    if (initial_time >= t_) {
        DLOG(INFO) << "*** Initial time is greater than makespan max ***" << std::endl;
        return;
    }

    if (final_time > msm) {
        ft = msm;
    }

    for (auto t = initial_time; t < final_time; t++) {
        y[file_id][storage_id][t] = 1;
    }
}

/**
 *
 *
 * @param activation_id
 * @param file_id
 * @param vm_id
 * @param storage_id
 * @param initial_time
 * @param final_time
 */
void Solution::ReadFileForActivation(size_t activation_id, size_t file_id, size_t vm_id, size_t storage_id,
                                     size_t initial_time, size_t final_time) const {
    auto ft = final_time;

    DLOG(INFO) << "Makespan Max: " << t_ << std::endl;
    DLOG(INFO) << "ReadFileForActivation(" << activation_id << ", " << file_id << ", " << vm_id << ", " << storage_id
               << ", " << initial_time << ", " << ft << ");";

    if (initial_time >= t_) {
        DLOG(INFO) << "*** Initial time is greater than makespan max ***" << std::endl;
        return;
    }

    if (final_time > t_) {
        ft = t_;
    }

    auto t = initial_time;
//    for (auto t = initial_time; t < final_time; t++) {
    r[activation_id][file_id][vm_id][storage_id][t] = 1;
//    }
}

/**
 *
 *
 * @param activation_id
 * @param vm_id
 * @param initial_time
 * @param final_time
 */
void Solution::RunActivation(size_t activation_id, size_t vm_id, size_t initial_time, size_t final_time) const {
    auto ft = final_time;
    auto msm = static_cast<size_t>(algorithm_->get_makespan_max());

    DLOG(INFO) << "Makespan Max: " << msm << std::endl;
    DLOG(INFO) << "RunActivation(" << activation_id << ", " << vm_id << ", " << initial_time << ", " << ft << ");";

    if (initial_time >= t_) {
        DLOG(INFO) << "*** Initial time is greater than makespan max ***" << std::endl;
        return;
    }

    if (final_time > msm) {
        ft = msm;
    }

    auto t = initial_time;

    x[activation_id][vm_id][t] = 1;
}

/**
 *
 *
 * @param activation_id
 * @param file_id
 * @param vm_id
 * @param storage_id
 * @param initial_time
 * @param final_time
 */
void Solution::WriteFileForActivation(size_t activation_id, size_t file_id, size_t vm_id, size_t storage_id,
                                      size_t initial_time, size_t final_time) const {

    auto ft = final_time;

    DLOG(INFO) << "Makespan Max: " << t_ << std::endl;
    DLOG(INFO) << "WriteFileForActivation(" << activation_id << ", " << file_id << ", " << vm_id << ", " << storage_id
               << ", " << initial_time << ", " << ft << ");";

    if (initial_time >= t_) {
        DLOG(INFO) << "*** Initial time is greater than makespan max ***" << std::endl;
        return;
    }

    if (final_time > t_) {
        ft = t_;
    }

    auto t = initial_time;

    w[activation_id][file_id][vm_id][storage_id][t] = 1;
}

/**
 *
 *
 * @param vm_id
 * @param initial_time
 * @param final_time
 */
void Solution::AllocateVm(size_t vm_id, size_t initial_time, size_t final_time) const {
    auto ft = final_time;

    DLOG(INFO) << "Makespan Max: " << t_ << std::endl;
    DLOG(INFO) << "AllocateVm(" << vm_id << ", " << initial_time << ", " << ft << ");";

    if (initial_time >= t_) {
        DLOG(INFO) << "*** Initial time is greater than makespan max ***" << std::endl;
        return;
    }

    if (final_time > t_) {
        ft = t_;
    }

    for (auto t = initial_time; t < ft; t++) {
        v[vm_id][t] = 1;
    }
}

double Solution::PopulateExecutionAndAllocationsTimeVectors() {
    for (auto activation_id: ordering_) {
        // Initializations
        auto activation_start_time = 0ul;
        auto activation_read_time = 0ul;
        auto activation_write_time = 0ul;
        size_t activation_run_time;
        size_t finish_time;

        // Load Vm
        auto vm = algorithm_->GetVirtualMachinePerId(activation_allocations_[activation_id]);
        auto vm_id = vm->get_id();
        auto activation = algorithm_->GetActivationPerId(activation_id);

        // TODO: VM finish time and VM allocation time should be within the VM object
        // Compute Activation Start Time
        // What came latter, allocation of the VM or the previous activation finish time
        for (auto previous_task_id: algorithm_->GetPredecessors(activation->get_id())) {
            if (previous_task_id == algorithm_->get_id_source()) {
                activation_start_time = 0ul;
                break;
            }
            activation_start_time = std::max<size_t>(activation_start_time, activation_finish_time_[previous_task_id]);
        }
        activation_start_time = std::max<size_t>(activation_start_time, vm_finish_time_[vm->get_id()]);

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
                storage_id = file_allocations_[file_id];
            }

            if (storage_id == std::numeric_limits<size_t>::max()) {
                LOG(FATAL) << "Wrong storage_id - PopulateExecutionAndAllocationsTimeVectors";
            }

            auto file_storage = algorithm_->GetStoragePerId(storage_id);

            // Ceil of File Transfer Time + File Size * lambda
            auto one_file_read_time = ComputeFileTransferTime(file, file_storage, vm);

            // If reading problem then terminate
            if (one_file_read_time == std::numeric_limits<size_t>::max()) {
                LOG(FATAL) << "Something is wrong with file reading";
            }

            activation_read_time += one_file_read_time;

            // If the storage is some VM (not bucket) different from the execution VM
            // Allocate the necessary VM for the reading
            if (storage_id < algorithm_->GetVirtualMachineSize() and storage_id != vm->get_id()) {
                if (activation_start_time != std::numeric_limits<size_t>::max()
                    && activation_read_time != std::numeric_limits<size_t>::max()) {
                    auto st = activation_start_time + activation_read_time;
                    allocation_vm_queue_[storage_id] = std::max(allocation_vm_queue_[storage_id], st);
                }
            }
        }

        // Compute Run time
        activation_run_time = ceil(activation->get_time() * vm->get_slowdown());

        // Compute Write Time
        auto output_files = activation->get_output_files();
        for (const auto& output_file: output_files) {
            auto output_file_id = output_file->get_id();
            auto storage_id = file_allocations_[output_file_id];
            auto storage = algorithm_->GetStoragePerId(storage_id);
            auto one_file_write_time = ComputeFileTransferTime(output_file, vm, storage);

            // If we could not Write the output_file then terminate!
            if (one_file_write_time == std::numeric_limits<size_t>::max()) {
                LOG(FATAL) << "Something is wrong with output_file writing";
            }

            DLOG(INFO) << "output_file: " << output_file->get_name();
            DLOG(INFO) << "one_file_write_time: " << one_file_write_time;

            activation_write_time += one_file_write_time;

            // If the storage is some VM (not bucket) different from the execution VM
            // Allocate the necessary VM for the writing
            if (storage_id < algorithm_->GetVirtualMachineSize() and storage_id != vm_id) {
                if (activation_start_time != std::numeric_limits<size_t>::max()
                    && activation_read_time != std::numeric_limits<size_t>::max()
                    && activation_run_time != std::numeric_limits<size_t>::max()
                    && activation_write_time != std::numeric_limits<size_t>::max()) {
                    auto write_time = activation_start_time + activation_read_time + activation_run_time
                                      + activation_write_time;

                    // Need to Allocate VM until output_file is writen
                    allocation_vm_queue_[storage_id] = std::max(write_time, allocation_vm_queue_[storage_id]);
                } else {
                    LOG(FATAL) << "Something very very very wrong";
                }
            }
        }

        // Compute Finish Time
        if (activation_start_time == std::numeric_limits<size_t>::max()
            || activation_read_time == std::numeric_limits<size_t>::max()
            || activation_run_time == std::numeric_limits<size_t>::max()
            || activation_write_time == std::numeric_limits<size_t>::max()) {
            finish_time = std::numeric_limits<size_t>::max();
        } else {
            finish_time = activation_start_time + activation_read_time + activation_run_time + activation_write_time;
        }

        // TODO: This should be within the activation object and vm object
        // Update structures
        activation_finish_time_[activation_id] = finish_time;
        if (activation_id != algorithm_->get_id_source() && activation_id != algorithm_->get_id_target()) {
            vm_finish_time_[vm_id] = finish_time;
            allocation_vm_queue_[vm_id] = std::max(finish_time, allocation_vm_queue_[vm_id]);
        }

        DLOG(INFO) << "my_allocation_vm_queue[" << vm_id << "]: " << allocation_vm_queue_[vm_id];
        DLOG(INFO) << "activation_id: " << activation_id;
        DLOG(INFO) << "vm->get_id(): " << vm_id;
        DLOG(INFO) << "activation_start_time: " << activation_start_time;
        DLOG(INFO) << "activation_read_time: " << activation_read_time;
        DLOG(INFO) << "activation_run_time: " << activation_run_time;
        DLOG(INFO) << "activation_write_time: " << activation_write_time;
        DLOG(INFO) << "finish_time: " << finish_time;
    }
}

// Accumulate the Virtual Machine rent cost
double Solution::AccumulateVMCost() {
    double vm_cost = 0.0;
    for (auto i = 0ul; i < algorithm_->GetVirtualMachineSize(); ++i) {
        auto virtual_machine = algorithm_->GetVirtualMachinePerId(i);
        auto max_time = static_cast<double>(allocation_vm_queue_[virtual_machine->get_id()]);
        vm_cost += (max_time * virtual_machine->get_cost());
        DLOG(INFO) << "allocation_vm_queue_[" << virtual_machine->get_id() << "]: "
                   << allocation_vm_queue_[virtual_machine->get_id()];
        DLOG(INFO) << "virtual_machine->get_cost(): " << virtual_machine->get_cost();
    }
    return vm_cost;
}

// Accumulate the Bucket variable cost
double Solution::AccumulateBucketCost() {
    double bucket_cost = 0.0;
    for (auto i = algorithm_->GetVirtualMachineSize(); i < algorithm_->GetStorageSize(); ++i) {
        auto storage = algorithm_->GetStoragePerId(i);

        // If the storage is not a Virtual Machine, i.e. it is a Bucket; then calculate the bucket
        // variable cost
        for (auto j = 0ul; j < algorithm_->GetFilesSize(); ++j) {
            // If the Bucket is used; then accumulate de cost and break to the next Storage
            if (file_allocations_[j] == storage->get_id()) {
                auto file = algorithm_->GetFilePerId(j);
                bucket_cost += (storage->get_cost() * file->get_size_in_GB());
            }
        }
    }
    return bucket_cost;
}

void Solution::ComputeCost() {
//    double virtual_machine_cost = 0.0;
//    double bucket_variable_cost = 0.0;

    virtual_machine_cost_ = 0.0;
    bucket_variable_cost_ = 0.0;

    AccumulateVMCost();
    AccumulateBucketCost();
}

// Accumulate the privacy exposure
double Solution::AccumulatePrivacyExposyre() {
    for (size_t i = 0ul; i < algorithm_->GetFilesSize() - 1; ++i) {
        const size_t storage1_id = file_allocations_[i];

        for (size_t j = i + 1ul; j < algorithm_->GetFilesSize(); ++j) {
            const size_t storage2_id = file_allocations_[j];

            if (storage1_id != std::numeric_limits<size_t>::max()
                && storage2_id != std::numeric_limits<size_t>::max()
                && storage1_id == storage2_id) {
                const int conflict_value = algorithm_->get_conflict_graph().ReturnConflict(i, j);

                if (conflict_value > 0) {
                    DLOG(INFO) << "File[" << i << "] has conflict with File[" << j << "]";
                    privacy_exposure_ += conflict_value;  // Adds the penalties
                }
            }
        }
    }
}

// Accumulate the activation exposure
double Solution::AccumulateActivationExposure() {
    double activation_exposure = 0.0;
    for (auto i = 0ul; i < algorithm_->GetActivationSize(); ++i) {
        auto activation = algorithm_->GetActivationPerId(i);

        for (auto j = 0; j < activation->get_requirements().size(); ++j) {
            auto virtual_machine_id = activation_allocations_[activation->get_id()];

            // If the activation is allocated
            if (virtual_machine_id != std::numeric_limits<size_t>::max()) {
                auto virtual_machine = algorithm_->GetVirtualMachinePerId(virtual_machine_id);

                if (activation->GetRequirementValue(j) > virtual_machine->GetRequirementValue(j)) {
                    activation_exposure += activation->GetRequirementValue(j) - virtual_machine->GetRequirementValue(j);
                }
            }
        }
    }
    return activation_exposure;
}

void Solution::ComputeConfidentialityExposure() {
//    double activation_exposure = 0.0;
//    double privacy_exposure = 0.0;

    activation_exposure_ = 0.0;
    privacy_exposure_ = 0.0;

    activation_exposure_ = AccumulateActivationExposure();
    privacy_exposure_ = AccumulatePrivacyExposyre();
}

double Solution::fetch_makespan() { return activation_finish_time_[ordering_.back()]; }

double Solution::fetch_cost() { return virtual_machine_cost_ + bucket_variable_cost_; }

double Solution::fetch_confidentiality_exposure() const { return activation_exposure_ + privacy_exposure_; }

double Solution::ComputeAndFetchOF() {
    double of;
    of = algorithm_->get_alpha_time() * (static_cast<double>(makespan_) / algorithm_->get_makespan_max())
         + algorithm_->get_alpha_budget() * (cost_ / algorithm_->get_budget_max())
         + algorithm_->get_alpha_security() * (security_exposure_
                                               / algorithm_->get_maximum_security_and_privacy_exposure());
    return of;
}

double Solution::OptimizedComputeObjectiveFunction() {
    DLOG(INFO) << "Compute Optimized Objective Function";

    activation_finish_time_.assign(algorithm_->GetActivationSize(), 0ul);
    vm_finish_time_.assign(algorithm_->GetVirtualMachineSize(), 0ul);
    allocation_vm_queue_.assign(algorithm_->GetVirtualMachineSize(), 0ul);

    PopulateExecutionAndAllocationsTimeVectors();
    ComputeCost();
    ComputeConfidentialityExposure();

    makespan_ = fetch_makespan();
    cost_ = fetch_cost();
    security_exposure_ = fetch_confidentiality_exposure();
    objective_value_ = ComputeAndFetchOF();

    return objective_value_;
}

double Solution::ComputeObjectiveFunction() {
    DLOG(INFO) << "Compute Objective Function";

    // Finish time for each activation
    activation_finish_time_.assign(algorithm_->GetActivationSize(), 0ul);

    // Finish time for each VM
    vm_finish_time_.assign(algorithm_->GetVirtualMachineSize(), 0ul);

    // Allocation time needed for each VM
    allocation_vm_queue_.assign(algorithm_->GetVirtualMachineSize(), 0ul);

    // 1. Calculates the makespan
    for (auto activation_id: ordering_) {
        // Initializations
        auto activation_start_time = 0ul;
        auto activation_read_time = 0ul;
        auto activation_write_time = 0ul;
        size_t activation_run_time;
        size_t finish_time;

        // Load Vm
        auto vm = algorithm_->GetVirtualMachinePerId(activation_allocations_[activation_id]);
        auto vm_id = vm->get_id();
        auto activation = algorithm_->GetActivationPerId(activation_id);

        // Compute Activation Start Time
        // What came latter, allocation of the VM or the previous activation finish time
        for (auto previous_task_id: algorithm_->GetPredecessors(activation->get_id())) {
            if (previous_task_id == algorithm_->get_id_source()) {
                activation_start_time = 0ul;
                break;
            }
            activation_start_time = std::max<size_t>(activation_start_time, activation_finish_time_[previous_task_id]);
        }
        activation_start_time = std::max<size_t>(activation_start_time, vm_finish_time_[vm->get_id()]);

        // Compute Activation Read Time
        for (const auto& file: activation->get_input_files()) {
            size_t storage_id;
            auto file_id = file->get_id();

            if (auto static_file = std::dynamic_pointer_cast<StaticFile>(file)) {
                // If the file is static, get the ID of the storage where the file is stored from its definition
                storage_id = static_file->GetFirstVm();
            } else {
                // If the file is dynamic, get the ID of the storage where the file is stored from its allocation
                storage_id = file_allocations_[file_id];
            }

            if (storage_id == std::numeric_limits<size_t>::max()) {
                LOG(FATAL) << "Wrong storage_id - ComputeObjectiveFunction";
            }

            auto file_storage = algorithm_->GetStoragePerId(storage_id);

            // Ceil of File Transfer Time + File Size * lambda
            auto one_file_read_time = ComputeFileTransferTime(file, file_storage, vm);

            // If reading problem then terminate
            if (one_file_read_time == std::numeric_limits<size_t>::max()) {
                LOG(FATAL) << "Something is wrong with file reading";
            }
            activation_read_time += one_file_read_time;

            // If the storage is some VM (not bucket) different from the execution VM
            // Allocate the necessary VM for the reading
            if (storage_id < algorithm_->GetVirtualMachineSize() and storage_id != vm->get_id()) {
                if (activation_start_time != std::numeric_limits<size_t>::max()
                    && activation_read_time != std::numeric_limits<size_t>::max()) {
                    auto st = activation_start_time + activation_read_time;
                    allocation_vm_queue_[storage_id] = std::max(allocation_vm_queue_[storage_id], st);
                }
            }
        }

        // Compute Run time
        activation_run_time = std::ceil(activation->get_time() * vm->get_slowdown());

        // Compute Write Time
        auto output_files = activation->get_output_files();
        for (const auto& output_file: output_files) {
            auto output_file_id = output_file->get_id();
            auto storage_id = file_allocations_[output_file_id];
            auto storage = algorithm_->GetStoragePerId(storage_id);
            auto one_file_write_time = ComputeFileTransferTime(output_file, vm, storage);

            // If we could not Write the output_file then terminate!
            if (one_file_write_time == std::numeric_limits<size_t>::max()) {
                LOG(FATAL) << "Something is wrong with output_file writing";
            }

            DLOG(INFO) << "output_file: " << output_file->get_name();
            DLOG(INFO) << "one_file_write_time: " << one_file_write_time;

            activation_write_time += one_file_write_time;
            if (storage_id < algorithm_->GetVirtualMachineSize() and storage_id != vm_id) {
                if (activation_start_time != std::numeric_limits<size_t>::max()
                    && activation_read_time != std::numeric_limits<size_t>::max()
                    && activation_run_time != std::numeric_limits<size_t>::max()
                    && activation_write_time != std::numeric_limits<size_t>::max()) {
                    auto write_time = activation_start_time + activation_read_time + activation_run_time
                                      + activation_write_time;

                    // Need to Allocate VM until output_file is writen
                    allocation_vm_queue_[storage_id] = std::max(write_time, allocation_vm_queue_[storage_id]);
                } else {
                    LOG(FATAL) << "Something very very very wrong";
                }
            }
        }

        // Compute Finish Time
        if (activation_start_time == std::numeric_limits<size_t>::max()
            || activation_read_time == std::numeric_limits<size_t>::max()
            || activation_run_time == std::numeric_limits<size_t>::max()
            || activation_write_time == std::numeric_limits<size_t>::max()) {
            finish_time = std::numeric_limits<size_t>::max();
        } else {
            finish_time = activation_start_time + activation_read_time + activation_run_time + activation_write_time;
        }

        // Update structures
        activation_finish_time_[activation_id] = finish_time;
        if (activation_id != algorithm_->get_id_source() && activation_id != algorithm_->get_id_target()) {
            vm_finish_time_[vm_id] = finish_time;
            allocation_vm_queue_[vm_id] = std::max(finish_time, allocation_vm_queue_[vm_id]);
        }

        DLOG(INFO) << "my_allocation_vm_queue[" << vm_id << "]: " << allocation_vm_queue_[vm_id];
        DLOG(INFO) << "activation_id: " << activation_id;
        DLOG(INFO) << "vm->get_id(): " << vm_id;
        DLOG(INFO) << "activation_start_time: " << activation_start_time;
        DLOG(INFO) << "activation_read_time: " << activation_read_time;
        DLOG(INFO) << "activation_run_time: " << activation_run_time;
        DLOG(INFO) << "activation_write_time: " << activation_write_time;
        DLOG(INFO) << "finish_time: " << finish_time;
    }

    makespan_ = activation_finish_time_[ordering_.back()];

    // 2. Calculates the cost
    double virtual_machine_cost = 0.0;
    double bucket_variable_cost = 0.0;

    // Accumulate the Virtual Machine rent cost
    for (size_t i = 0ul; i < algorithm_->GetVirtualMachineSize(); ++i) {
        std::shared_ptr<VirtualMachine> virtual_machine = algorithm_->GetVirtualMachinePerId(i);
        auto max_time = static_cast<double>(allocation_vm_queue_[virtual_machine->get_id()]);
        virtual_machine_cost += (max_time * virtual_machine->get_cost());
        DLOG(INFO) << "allocation_vm_queue_[" << virtual_machine->get_id() << "]: "
                   << allocation_vm_queue_[virtual_machine->get_id()];
        DLOG(INFO) << "virtual_machine->get_cost(): " << virtual_machine->get_cost();
    }

    // Accumulate the Bucket variable cost
    for (size_t i = algorithm_->GetVirtualMachineSize(); i < algorithm_->GetStorageSize(); ++i) {
        std::shared_ptr<Storage> storage = algorithm_->GetStoragePerId(i);

        // If the storage is not a Virtual Machine, i.e. it is a Bucket; then calculate the bucket
        // variable cost
        for (size_t j = 0ul; j < algorithm_->GetFilesSize(); ++j) {
            // If the Bucket is used; then accumulate de cost and break to the next Storage
            if (file_allocations_[j] == storage->get_id()) {
                std::shared_ptr<File> file = algorithm_->GetFilePerId(j);
                bucket_variable_cost += (storage->get_cost() * file->get_size_in_GB());
            }
        }
    }
//    of_cost = virtual_machine_cost + bucket_variable_cost;
    cost_ = virtual_machine_cost + bucket_variable_cost;

    // 3. Calculates the security exposure
    double activation_exposure = 0.0;
    double privacy_exposure = 0.0;

    // Accumulate the activation exposure
    for (auto i = 0ul; i < algorithm_->GetActivationSize(); ++i) {
        auto activation = algorithm_->GetActivationPerId(i);

        for (auto j = 0; j < activation->get_requirements().size(); ++j) {
            auto virtual_machine_id = activation_allocations_[activation->get_id()];

            // If the activation is allocated
            if (virtual_machine_id != std::numeric_limits<size_t>::max()) {
                auto virtual_machine = algorithm_->GetVirtualMachinePerId(virtual_machine_id);

                if (activation->GetRequirementValue(j) > virtual_machine->GetRequirementValue(j)) {
                    activation_exposure += activation->GetRequirementValue(j) - virtual_machine->GetRequirementValue(j);
                }
            }
        }
    }

    // Accumulate the privacy exposure
    for (size_t i = 0ul; i < algorithm_->GetFilesSize() - 1; ++i) {
        const size_t storage1_id = file_allocations_[i];

        for (size_t j = i + 1ul; j < algorithm_->GetFilesSize(); ++j) {
            const size_t storage2_id = file_allocations_[j];

            if (storage1_id != std::numeric_limits<size_t>::max()
                && storage2_id != std::numeric_limits<size_t>::max()
                && storage1_id == storage2_id) {
                const int conflict_value = algorithm_->get_conflict_graph().ReturnConflict(i, j);

                if (conflict_value > 0) {
                    DLOG(INFO) << "File[" << i << "] has conflict with File[" << j << "]";
                    privacy_exposure += conflict_value;  // Adds the penalties
                }
            }
        }
    }

    security_exposure_ = activation_exposure + privacy_exposure;

    objective_value_ = algorithm_->get_alpha_time() * (static_cast<double>(makespan_) / algorithm_->get_makespan_max())
           + algorithm_->get_alpha_budget() * (cost_ / algorithm_->get_budget_max())
           + algorithm_->get_alpha_security() * (security_exposure_
                                                 / algorithm_->get_maximum_security_and_privacy_exposure());

    return objective_value_;
}

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

    if (ordering_.size() != algorithm_->GetActivationSize()) {
        LOG(FATAL) << "Ordering has a wrong number of elements";
    }

    if (check_storage && !checkFiles()) {
        std::cerr << "check file error" << std::endl;
        throw;
    }

    objective_value_ = algorithm_->get_alpha_time() * (static_cast<double>(makespan_) / algorithm_->get_makespan_max())
                       + algorithm_->get_alpha_budget() * (cost_ / algorithm_->get_budget_max())
                       + algorithm_->get_alpha_security() * (security_exposure_
                                                             / algorithm_->get_maximum_security_and_privacy_exposure());

    return objective_value_;
}

/**
 * Time necessary to Write all output files of the \c task executed in the \c virtual_machine
 *
 * \param[in]  task             Activation that will be executed at \c virtual_machine
 * \param[in]  virtual_machine  Virtual machine where the \c task will be executed
 * \retval     write_time       The accumulated time to Write all output files of the \c task
 */
//size_t Solution::ComputeTaskWriteTime(const std::shared_ptr<Activation>& task,
//                                      const std::shared_ptr<VirtualMachine>& virtual_machine) {
//  // Compute Write time
//  size_t write_time = 0ul;
//
////  std::vector<std::shared_ptr<File>> output_files = task->get_output_files();
//
//  for (const std::shared_ptr<File>& file: task->get_output_files()) {
////  for (size_t i = 0ul; i < output_files.size(); ++i) {
////    std::shared_ptr<File> file = output_files[i];
//    std::shared_ptr<Storage> storage_of_the_file = algorithm_->GetStoragePerId(file_allocations_[file->get_id()]);
//
//    // Update vm queue
//    // auto f_queue = storage_queue_.insert(std::make_pair(storage_of_the_file->get_id(),
//    //                                                     std::vector<size_t>()));
//    // TO-DO: remove
//    // virtual_machine_queue_[storage_of_the_file->get_id()].push_back(file->get_id());
//    // f_queue.first->second.push_back(file->get_id());
//
//    // write_time += std::ceil(ComputeFileTransferTime(file, virtual_machine, storage_of_the_file)
//    //                         + (file->get_size() * (algorithm_->get_lambda() * 2)));
//
//    // write_time += std::ceil(ComputeFileTransferTime(file, virtual_machine, storage_of_the_file));
//    write_time += ComputeFileTransferTime(file, virtual_machine, storage_of_the_file);
//  }
//
//  return write_time;
//}  // double Solution::TaskWriteTime(Activation task, VirtualMachine vm) {

/**
 * If file reside in the same vm, then the transfer time is 0.0.
 * Otherwise, calculate the transfer time between them using the smallest bandwidth.
 *
 *   \f[
 *      time = \lceil{\min_{storage1.bandwidth(), storage2.bandwidth()} bandwidth}\rceil
 *   \f]
 *
 * Where:
 * \f$ time \f$ is the transfer time between the \c storage1 and \c storage2
 * \f$ storage1.bandwidth() \f$ is the transfer rate of the \c storage1
 * \f$ storage2.bandwidth() \f$ is the transfer rate of the \c storage2
 * \f$ bandwidth \f$ is the minimal transfer rate between the storage1.bandwidth() and storage2.bandwidth()
 *
 * \param[in]  file               File to transfer from storage1 to storage2
 * \param[in]  storage1           Storage origin/destination
 * \param[in]  storage2           Storage origin/destination
 * \param[in]  check_constraints  Check for hard constraint before computation
 *
 * \retval     time + penalties   The time to transfer \c file from \c file_vm to \c vm with possible
 *                                applied penalties
 */
size_t Solution::ComputeFileTransferTime(const std::shared_ptr<File>& file,
                                         const std::shared_ptr<Storage> &storage1,
                                         const std::shared_ptr<Storage> &storage2,
                                         bool check_constraints) const {
    size_t time = std::numeric_limits<size_t>::max();

//  DLOG(DEBUG) << "Compute the transfer time of File[" << file->get_id() << "] to/from VM["
//      << storage1->get_id() << "] to Storage[" << storage2->get_id() << "], check_constraints["
//      << check_constraints << "]";

    if (check_constraints) {
        for (size_t i = 0ul; i < algorithm_->GetFilesSize(); ++i) {
            size_t storage_id = file_allocations_[i];

            // If file 'i' is to transfer to the same place that file->get_id()
            if (storage_id == storage2->get_id()) {
                int conflict_value = algorithm_->get_conflict_graph().ReturnConflict(i, file->get_id());

                if (conflict_value < 0) {
                    // Hard constraint; just return with std::numeric_limits<double>::max();
                    return time;
                }
            }
        }
    }
//    else {
//        std::cout << "Skipping constraint checking" << std::endl;
//    }

    // If there is no Hard constraints; calculates de transfer time between storages
    // or if check_constraints is false

    // Calculate time
    if (storage1->get_id() != storage2->get_id()) {
        // get the smallest link
        double link = std::min(storage1->get_bandwidth_GBps(), storage2->get_bandwidth_GBps());
        time = std::ceil(file->get_size_in_GB() / link);
    } else {
        time = 1ul;
    }

//  DLOG(DEBUG) << "transfer_time: " << time;
    return time;
}

//double Solution::PopulateExecutionAndAllocationsTimeVectors(bool check_sequence) {
//size_t Solution::PopulateExecutionAndAllocationsTimeVectors() {
//  size_t makespan;
////  size_t makespan;
////  size_t makespan = 0.0;
//
//  // compute makespan
//  // for (auto id_task : ordering_) {  // For each task, do
//  for (unsigned long id_task : ordering_) {}
//    // If is not source or target than
//    if (id_task != algorithm_->get_id_source() && id_task != algorithm_->get_id_target()) {
//      if (check_sequence && !CheckTaskSequence(id_task)) {
//        std::c err << "Encode error - Solution: Error in the precedence relations." << std::endl;
//        throw;
//      }
//
//      // Load Vm
//      std::shared_ptr<VirtualMachine> vm = algorithm_->GetVirtualMachinePerId(activation_allocations_[id_task]);
//      std::shared_ptr<Activation> task = algorithm_->GetActivationPerId(id_task);
//
//      // update vm queue
//      // auto f_queue = storage_queue_.insert(std::make_pair(vm->get_id(), std::vector<size_t>()));
//      // f_queue.first->second.push_back(task->get_id());
//      // storage_queue_[vm->get_id()].push_back(task->get_id());
//
//      // update scheduler
//      // auto f_scheduler = scheduler_.insert(std::make_pair(vm->get_id(), std::vector<std::string>()));
//      // f_scheduler.first->second.push_back(task->get_tag());
//      // scheduler_[vm->get_id()].push_back(task->get_tag());
//
//      // Compute Activation Times
////      double StartTime = ComputeActivationStartTime(task->get_id(), vm->get_id());
////      size_t StartTime = ComputeActivationStartTime(task->get_id(), vm->get_id());
////      aux_start_time = StartTime;
////      double read_time = ComputeTaskReadTimeOther(task, vm);
////      // double run_time = std::ceil(task->get_time() * vm->get_slowdown());  // Seconds
////      double run_time = task->get_time() * vm->get_slowdown();  // Seconds
////      double write_time = ComputeTaskWriteTime(task, vm);
////      size_t StartTime = 0.0;
//      size_t StartTime = 0UL;
//      size_t read_time = 0UL;
//      size_t write_time = 0UL;
//      size_t run_time = std::ceil(task->get_time() * vm->get_slowdown());
//
//      // Could not remember why I need this line bellow
//      aux_runtime = run_time;
//
//      if (task->get_id() != algorithm_->get_id_source()
//          && task->get_id() != algorithm_->get_id_target()) {
//        StartTime = ComputeActivationStartTime(task->get_id(), vm->get_id());
//        aux_start_time = StartTime;
//        read_time = ComputeTaskReadTimeOther(task, vm);
//        aux_read_time = read_time;
//        write_time = ComputeTaskWriteTime(task, vm);
//      } else if (task->get_id() == algorithm_->get_id_target()) {
//        for (auto task_id: algorithm_->GetPredecessors(task->get_id())) {
//          start_time = std::max<size_t>(StartTime, activation_finish_time_[task_id]);
//        }
//      }
//
//      size_t finish_time = std::numeric_limits<size_t>::max();
////      size_t finish_time = std::numeric_limits<size_t>::max();
//
//      if (StartTime != std::numeric_limits<size_t>::max()
//          && read_time != std::numeric_limits<size_t>::max()) {
//        // && write_time != std::numeric_limits<double>::max()) {
////        finish_time = StartTime + std::ceil(read_time) + std::ceil(run_time) + std::ceil(write_time);
//        finish_time = StartTime + read_time + run_time + write_time;
//        // finish_time = StartTime + read_time + run_time;
//      } else {
////        DLOG(INFO) << "start_time: " << StartTime;
////        DLOG(INFO) << "read_time: " << read_time;
//        // DLOG(INFO) << "write_time: " << write_time;
////        DLOG(INFO) << "run_time: " << run_time;
////        DLOG(INFO) << "finish_time: " << finish_time;
////        DLOG(INFO) << "objective_function_: " << makespan_;
//
//        return finish_time;  // return "infinity"
//      }
//
////      DLOG(INFO) << "start_time: " << StartTime;
////      DLOG(INFO) << "read_time: " << read_time;
////      DLOG(INFO) << "write_time: " << write_time;
////      DLOG(INFO) << "run_time: " << run_time;
////      DLOG(INFO) << "finish_time: " << finish_time;
//
//      // auto finish_time = StartTime + read_time + run_time + write_time;
//
//      // Update structures
//      activation_finish_time_[id_task] = finish_time;
//      // start_time_vector_[id_task] = StartTime;
//      vm_finish_time_[vm->get_id()] = finish_time;
////      allocation_vm_queue_[vm->get_id()] = std::max(finish_time, allocation_vm_queue_[vm->get_id()]);
//      allocation_vm_queue_[vm->get_id()] = finish_time;
//    } else {  // Source and Target tasks
//      if (id_task == algorithm_->get_id_source()) {  // Source task
//        activation_finish_time_[id_task] = 0;
//      } else {  // Target task
//        double max_value = 0.0;
////        size_t max_value = 0UL;
//
//        // for (auto task : algorithm_->get_predecessors().find(id_task)->second) {
//        for (auto task: algorithm_->GetPredecessors(id_task)) {
//          max_value = std::max<double>(max_value, activation_finish_time_[task]);
//        }
//
//        activation_finish_time_[id_task] = max_value;
//      }  // } else {  // Target task
//    }  // } else {  // Source and Target tasks
//  }  // for (auto id_task : ordering_) {  // For each task, do
//
//  makespan = activation_finish_time_[algorithm_->get_id_target()];
//
//  return makespan;
//}  // double Solution::PopulateExecutionAndAllocationsTimeVectors() {

/* Checks the sequence of tasks is valid */
//bool Solution::CheckTaskSequence(size_t task) {
//  // for (auto tk : algorithm_->get_predecessors().find(task)->second) {
////  for (auto tk: algorithm_->GetPredecessors(task)) {
////    if (activation_finish_time_[tk] == std::numeric_limits<size_t>::max()) {
////      return false;
////    }
////  }
//  auto v = algorithm_->GetPredecessors(task);
//  bool rv = std::all_of(v.c begin(), v.c end(), [this](size_t tk) {
//    if (activation_finish_time_[tk] == std::numeric_limits<double>::max()) {
//      return false;
//    }
//    return true;
//  });
////  return true;
//  return rv;
//}  // bool Solution::CheckTaskSequence(size_t task) {

// Check and organize the file based on the storage capacity
/**
 *
 * @return
 */
bool Solution::checkFiles() {
    bool flag = true;
    size_t count = 0;

    std::vector<double> aux_storage(algorithm_->get_storage_vet());
    std::vector<size_t> aux(algorithm_->GetVirtualMachineSize());
    iota(aux.begin(), aux.end(), 0);  // 0,1,2,3,4 ... n

    std::unordered_map<size_t, std::vector<size_t>> map_file;

    size_t id_storage;
    // build file map and compute the storage
    // for (auto it : data->file_map) {
    // for (auto it : algorithm_->get_file_map_per_id()) {
    for (size_t it = 0ul; it < algorithm_->GetFilesSize(); ++it) {
        std::shared_ptr<File> file = algorithm_->GetFilePerId(it);

        if (std::shared_ptr<DynamicFile> dynamic_file = std::dynamic_pointer_cast<DynamicFile>(file)) {
            id_storage = file_allocations_[dynamic_file->get_id()];
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
                std::shared_ptr<File> file = algorithm_->GetFilePerId(i);
                std::cout << file->get_name() << std::endl;
                if (file->get_size_in_GB() < min) {
                    min = file->get_size_in_GB();
                    min_file = file->get_id();
                }
            });

            std::shared_ptr<File> file_min = algorithm_->GetFilePerId(min_file);

            std::cout << file_min->get_name() << std::endl;
            // MinFile will be moved to machine with more empty space
            file_allocations_[file_min->get_id()] = new_storage;
            // Update aux Storage
            aux_storage[old_vm] += file_min->get_size();
            aux_storage[new_storage] -= file_min->get_size();
            // Update mapFile structure
            map_file[old_vm].erase(remove(map_file[old_vm].begin(), map_file[old_vm].end(), min_file),
                                   map_file[old_vm].end());
            map_file[new_storage].push_back(min_file);
        } else {
            flag = false;
        }

        count++;
    } while (flag && count < algorithm_->GetFilesSize());
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
    os << "################################################################################################"
       << std::endl;
    os << algorithm_->GetName() << std::endl;
    os << "################################################################################################"
       << std::endl;
    os << "makespan_: " << makespan_ << " seconds" << std::endl;
    os << "makespan_: " << static_cast<double>(makespan_) / 60.0 << " minutes" << std::endl;
    os << "cost_: " << cost_ << std::endl;
    os << "security_exposure_: " << security_exposure_ << std::endl;
    os << "objective_value_: " << objective_value_ << std::endl;
    os << "Maximum security and privacy exposure: "
       << algorithm_->get_maximum_security_and_privacy_exposure()
       << std::endl;

    os << std::endl;
    os << "Ordering: " << std::endl;
    for (size_t i = 0ul; i < ordering_.size(); ++i) {
        os << "ordering_[" << i << "]: \t" << ordering_[i] << "\n";
    }

    os << std::endl;
    os << "Allocations and executions: " << std::endl;
    for (size_t i = 0ul; i < allocation_vm_queue_.size(); ++i) {
        os << "a[" << i << "]: \t" << allocation_vm_queue_[i] << "\t";
    }

    os << std::endl;
    for (size_t i = 0ul; i < vm_finish_time_.size(); ++i) {
        os << "x[" << i << "]: \t" << vm_finish_time_[i] << "\t";
    }
    os << std::endl;

    os << std::endl;
    os << "Activations: " << std::endl;
    for (size_t i = 0ul; i < algorithm_->GetVirtualMachineSize(); ++i) {
        std::shared_ptr<VirtualMachine> vm = algorithm_->GetVirtualMachinePerId(i);

        os << vm->get_id() << ": ";
        for (size_t j = 0ul; j < algorithm_->GetActivationSize(); ++j) {
            std::shared_ptr<Activation> activation = algorithm_->GetActivationPerId(j);
            size_t virtual_machine_id = activation_allocations_[j];

            if (virtual_machine_id == vm->get_id()) {
                os << activation->get_tag() << " ";
            }
        }
        os << std::endl;
    }

    os << std::endl;
    os << "Files: " << std::endl;
    for (size_t i = 0ul; i < algorithm_->GetStorageSize(); ++i) {
        std::shared_ptr<Storage> vm = algorithm_->GetStoragePerId(i);

        os << vm->get_id() << ": \n";
        for (size_t j = 0ul; j < algorithm_->GetFilesSize(); ++j) {
            auto file = algorithm_->GetFilePerId(j);
            size_t storage_id;

            if (std::shared_ptr<StaticFile> static_file = std::dynamic_pointer_cast<StaticFile>(file)) {
                storage_id = static_file->GetFirstVm();
            } else {
                storage_id = file_allocations_[file->get_id()];
            }

            if (storage_id == vm->get_id()) {
                os << " " << file->get_name() << "\n";
            }
        }
        os << std::endl;
    }

    os << std::endl;
    os << "Activation Sequence: " << std::endl;
    for (auto task_id: ordering_) {
        os << algorithm_->GetActivationPerId(task_id)->get_name() << ", ";
    }
    os << std::endl;
    os << "################################################################################################"
       << std::endl;
    os << algorithm_->GetName() << std::endl;
    os << "################################################################################################"
       << std::endl;

    if (x) {
        os << "Printing BEST" << std::endl;

        // -------- x ----------
        for (auto i = 0UL; i < n_; ++i) {
            for (auto j = 0UL; j < m_; ++j) {
                for (auto t = 0UL; t < t_; ++t) {
                    if (x[i][j][t] > 0) {
                        os << "x[" << i << "][" << j << "][" << t << "] = " << x[i][j][t] << std::endl;
                    }
                }
            }
        }

        // -------- r ----------
        for (size_t i = 0UL; i < n_; ++i) {
            auto activation = algorithm_->GetActivationPerId(i + 1ul);
            auto input_files = activation->get_input_files();

            for (auto j = 0UL; j < d_; ++j) {
                for (auto k = 0UL; k < m_; ++k) {
                    for (auto l = 0UL; l < mb_; ++l) {
                        for (auto m = 0UL; m < t_; ++m) {
                            if (r[i][j][k][l][m] > 0) {
                                os << "r[" << i << "][" << j << "][" << k << "][" << l << "][" << m << "] = "
                                   << r[i][j][k][l][m] << std::endl;
                            }
                        }
                    }
                }
            }
        }

        // -------- w ----------
        for (auto i = 0UL; i < n_; ++i) {
            auto activation = algorithm_->GetActivationPerId(i + 1ul);
            auto output_files = activation->get_output_files();

            for (auto j = 0UL; j < d_; ++j) {
                for (auto k = 0UL; k < m_; ++k) {
                    for (auto l = 0UL; l < mb_; ++l) {
                        for (auto m = 0UL; m < t_; ++m) {
                            if (w[i][j][k][l][m] > 0) {
                                os << "w[" << i << "][" << j << "][" << k << "][" << l << "][" << m << "] = "
                                   << w[i][j][k][l][m] << std::endl;
                            }
                        }
                    }
                }
            }
        }

        // -------- y ----------
        for (auto i = 0UL; i < d_; ++i) {
            for (auto j = 0UL; j < mb_; ++j) {
                for (auto k = 0UL; k < t_; ++k) {
                    if (y[i][j][k] > 0) {
                        os << "y[" << i << "][" << j << "][" << k << "] = " << y[i][j][k] << std::endl;
                    }
                }
            }
        }

        // -------- v ----------
        for (auto i = 0UL; i < m_; ++i) {
            for (auto j = 0UL; j < t_; ++j) {
                if (v[i][j] > 0) {
                    os << "v[" << i << "][" << j << "] = " << v[i][j] << std::endl;
                }
            }
        }

        os << "Writing solution timeline information";

        auto *tempo = new int[mb_];

        /* Relate each device with its array of data */
        auto **maq_dado = new int *[mb_];

        // Initializations

        for (auto i = 0ul; i < mb_; i++) {
            tempo[i] = 0;
        }

        for (auto i = 0ul; i < mb_; i++) {
            maq_dado[i] = new int[algorithm_->GetFilesSize()];
            for (auto j = 0ul; j < algorithm_->GetFilesSize(); j++) {
                maq_dado[i][j] = -1;
            }
        }

        // Vector for TEST that saves the actual time the machine spends performing its action (exec., read, Write)
        // Save files per machine

        os << std::endl;
        os << "---------------- Timeline --------------" << std::endl;
        os << "t)\t";
        for (auto j = 0ul; j < algorithm_->GetStorageSize(); j++) {
            os << "(M" << j << ")\t" << "(Y)\t" << "(V)\t";
        }
        os << std::endl;

        os << std::endl;

        // Time
        for (auto check = 0ul, t1 = 0ul; t1 < t_ and check <= 1ul; t1++) {
            os << "t" << t1 << ")\t";

            // Devices
            // Virtual Machines
            for (auto j = 0ul; j < mb_ and check <= 1; j++) {
                std::shared_ptr<Storage> device = algorithm_->GetStoragePerId(j);

                if (auto virtual_machine = std::dynamic_pointer_cast<VirtualMachine>(device)) {
                    // Check if more than one action is started at the same time on the same machine
                    check = 0ul;

                    // During an action (read, execute, Write)
                    tempo[j] = tempo[j] - 1;

                    if (tempo[j] > 0) {
                        check += 1;
                        os << "(" << tempo[j] << ")" << "\t";
                    }

                    // Execution
                    for (auto i = 0ul; i < algorithm_->GetActivationSize() - 2ul and check <= 1ul; i++) {
                        std::shared_ptr<Activation> task = algorithm_->GetActivationPerId(1ul + i);
                        std::vector<std::shared_ptr<File>> input_files = task->get_input_files();

                        auto x_ijt = x[i][j][t1];  // Returns the value of the variable

                        if (x_ijt > 0) {
                            check += 1;
                            os << "x" << i << "\t";

                            if (tempo[j] > 0) {
                                os << "*** RUN ON TOP OF SOMETHING ***";
                                check += 1;
                                break;
                            }

                            tempo[j] = std::ceil(task->get_time() * virtual_machine->get_slowdown());

                            // Checks if the readings were taken by the machine before execution
                            for (auto l = 0ul; l < input_files.size() and check <= 1; l++) {
                                auto file = input_files[l];
                                auto d1 = file->get_id();
                                bool has_read_file = false;

                                for (auto k = 0ul; k < algorithm_->GetStorageSize() and check <= 1ul; k++) {
                                    auto q_max = static_cast<size_t>(algorithm_->get_makespan_max());
                                    for (auto q = 0ul; q < q_max and check <= 1; q++) {
                                        auto r_idjpt = static_cast<float>(r[i][d1][j][k][q]);
                                        // Returns the value of the variable
                                        if (r_idjpt > 0) {
                                            has_read_file = true;
                                        }
                                    }
                                }

                                if (!has_read_file) {
                                    os << "*** PERFORMED THE TASK WITHOUT READING ALL THE ENTRIES *** missing data ="
                                       << d1
                                       << "\n" << file;
                                    check += 1;
                                    break;
                                }
                            }
                            // ----------------------------------------------------------------
                        }

                        if (check > 1) {
                            os << "*** CONFLICT WITH EXECUTION ***";
                            break;
                        }
                    }

                    // Read
                    for (auto i = 0ul; i < algorithm_->GetActivationSize() - 2ul and check <= 1ul; i++) {
                        auto activation = algorithm_->GetActivationPerId(i + 1ul);
                        auto input_files = activation->get_input_files();

                        for (auto l = 0ul; l < input_files.size() and check <= 1; l++) {
                            auto file = input_files[l];
                            auto d1 = file->get_id();

                            for (auto k = 0ul; k < mb_ and check <= 1ul; k++) {
                                std::shared_ptr<Storage> storage = algorithm_->GetStoragePerId(static_cast<size_t>(k));

                                auto r_idjpt = r[i][d1][j][k][t1];

                                // Returns the value of the variable
                                if (r_idjpt > 0) {
                                    auto read_time = ComputeFileTransferTime(file, virtual_machine, storage);

                                    check += 1;
                                    os << "r" << i << "(" << d1 << ")<" << k << "\t";

                                    if (tempo[j] > 0) {
                                        os << "*** READ OVER SOMETHING ***";
                                        check += 1;
                                        break;
                                    }

                                    tempo[j] = static_cast<int>(read_time);

                                    // Check file existence
                                    auto y_djt = static_cast<float>(y[d1][k][t1]);

                                    // Returns the value of the variable
                                    if (y_djt < 1) {
                                        os << "*** READ NON-EXISTENT FILE IN THE MACHINE ***";
                                        check += 1;
                                        break;
                                    }
                                    // ---------------------------
                                }

                                if (check > 1) {
                                    os << "*** CONFLICT WITH READING ***";
                                    break;
                                }
                            }
                        }
                    }

                    // Writing
                    for (auto has_passed = 0ul, i = 0ul; i < n_ and check <= 1; i++) {
                        auto activation = algorithm_->GetActivationPerId(i + 1ul);
                        auto output_files = activation->get_output_files();

                        for (auto l = 0ul; l < output_files.size() and check <= 1; l++) {
                            std::shared_ptr<File> file = output_files[l];
                            auto d1 = file->get_id();

                            for (auto k = 0ul; k < mb_ and check <= 1; k++) {
                                std::shared_ptr<Storage> storage = algorithm_->GetStoragePerId(k);

                                auto w_idjpt = w[i][d1][j][k][t1];

                                // Returns the value of the variable
                                if (w_idjpt > 0) {
                                    auto write_time = ComputeFileTransferTime(file, virtual_machine, storage);

                                    check += 1;

                                    os << "w" << i << "(" << d1 << ")<" << k << "\t";

                                    if (tempo[j] > 0) {
                                        os << "*** WRITE ON SOMETHING ***";
                                        check += 1;
                                        break;
                                    }

                                    tempo[j] = static_cast<int>(write_time);

                                    // Checks if the activation has been executed on the machine before
                                    has_passed = 0;
                                    for (int q = 0; q < algorithm_->get_makespan_max() and check <= 1ul; q++) {
                                        auto x_ijt = static_cast<float>(x[i][j][q]);

                                        if (x_ijt > 0) {
                                            has_passed = 1;
                                        }
                                    }

                                    if (has_passed == 0) {
                                        os << "*** WROTE DATA WITHOUT HAVING PERFORMED THE TASK ON THE MACHINE ***";
                                        check += 1;
                                        break;
                                    }
                                    // -----------------------------------------------
                                }

                                if (check > 1) {
                                    os << "*** CONFLICT WITH WRITING ***";
                                    break;
                                }
                            }
                        }
                    }

                    // Idle machine at this time
                    if (check == 0ul) {
                        os << "-\t";
                    }

                    // Data
                    for (auto d1 = 0ul; d1 < d_ and check <= 1; d1++) {
                        auto y_djt = y[d1][j][t1];
                        // Returns the value of the variable

                        if (y_djt > 0) {
                            maq_dado[j][d1] = 1;
                        }

                        if (maq_dado[j][d1] == 1) {
                            os << d1 << ",";
                        }
                    }

                    os << "\t";

                    // Hiring
                    auto v_jt = v[j][t1];

                    // Returns the value of the variable
                    if (v_jt > 0) {
                        os << "*\t";
                    } else {
                        os << "\t";
                    }
                } else {
                    os << "N\\A\t";

                    // Data
                    for (auto d1 = 0ul; d1 < d_ and check <= 1; d1++) {
                        auto y_djt = y[d1][j][t1];

                        if (y_djt > 0) {
                            maq_dado[j][d1] = 1;
                        }

                        if (maq_dado[j][d1] == 1) {
                            os << d1 << ",";
                        }
                    }

                    os << "\t";

                    // Hiring
                    os << "N\\A\t";
                }
            }
            os << std::endl;
        }
        os << "---------------------------------------------" << std::endl;

        // Releasing resources

        delete[] tempo;
        for (size_t i = 0; i < algorithm_->GetStorageSize(); i++) {
            delete[] maq_dado[i];
        }
        delete[] maq_dado;
    }

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
size_t Solution::AllocateOneOutputFileGreedily(const std::shared_ptr<File>& file,
                                               const std::shared_ptr<VirtualMachine> &vm,
                                               const size_t start_time,
                                               const size_t read_time,
                                               const size_t run_time,
                                               const size_t partial_write_time) {
    DLOG(INFO) << "Computing time for Write the File[" << file->get_id() << "] into VM[" << vm->get_id() << "]";

    double partial_objective_value;
    double best_objective_value = std::numeric_limits<double>::max();
    size_t best_write_one_file_time = std::numeric_limits<size_t>::max();
//  double best_cost = std::numeric_limits<double>::max();
    double best_security_exposure = std::numeric_limits<double>::max();
    size_t best_storage_id = std::numeric_limits<size_t>::max();

    auto available_storages = std::vector<std::shared_ptr<Storage>>(algorithm_->GetStorageSize());

    for (size_t i = 0ul; i < available_storages.size(); ++i) {
        available_storages[i] = algorithm_->GetStoragePerId(i);
    }

    // Shuffle the output files for better randomness between the solutions
    std::shuffle(available_storages.begin(), available_storages.end(), generator());

    // for all possible storage; compute the transfer time
    for (size_t i = 0ul; i < available_storages.size(); ++i) {
        std::shared_ptr<Storage> storage = available_storages[i];
        // 1. Calculates the File Transfer Time
        size_t write_one_file_time;
        if (boost::algorithm::ends_with(algorithm_->GetName(), "_hard")) {
            write_one_file_time = ComputeFileTransferTime(file, vm, storage, true);
        } else {
            DLOG(INFO) << "Skipping constraint checking";
            write_one_file_time = ComputeFileTransferTime(file, vm, storage, false);
        }

        if (write_one_file_time == std::numeric_limits<size_t>::max()) {
            continue;  // Hard-constraint
        }

        // 2. Calculates the File Contribution to the Cost
        double cost;
        double virtual_machine_cost = 0.0;
        double allocation_cost = 0.0;
        double bucket_variable_cost = 0.0;

        // Writing cost
        virtual_machine_cost += static_cast<double>(write_one_file_time) * vm->get_cost();

        // Allocation cost
        if (auto inner_vm = std::dynamic_pointer_cast<VirtualMachine>(storage)) {
            size_t total_time = start_time + read_time + run_time + partial_write_time + write_one_file_time;
            if (total_time > allocation_vm_queue_[inner_vm->get_id()]) {
                size_t diff = total_time - allocation_vm_queue_[inner_vm->get_id()];
                allocation_cost = static_cast<double>(diff) * inner_vm->get_cost();
            }
        }

        // Storage cost
        if (storage->get_id() >= algorithm_->GetVirtualMachineSize()) {
            bucket_variable_cost += storage->get_cost() * file->get_size_in_GB();
        }

        cost = virtual_machine_cost + allocation_cost + bucket_variable_cost;

        // 3. Calculates the File Security Exposure Contribution
        double security_exposure = ComputeFileSecurityExposureContribution(storage, file);

        DLOG(INFO) << "write_one_file_time: " << write_one_file_time;
        DLOG(INFO) << "cost: " << cost;
        DLOG(INFO) << "security_exposure: " << security_exposure;

        partial_objective_value =
                algorithm_->get_alpha_time() *
                (static_cast<double>(write_one_file_time) / algorithm_->get_makespan_max())
                + algorithm_->get_alpha_budget() * (cost / algorithm_->get_budget_max())
                + algorithm_->get_alpha_security()
                  * (security_exposure / algorithm_->get_maximum_security_and_privacy_exposure());

        if (best_objective_value > partial_objective_value) {
            best_objective_value = partial_objective_value;
            best_storage_id = storage->get_id();
            best_write_one_file_time = write_one_file_time;
//      best_cost = cost;
            best_security_exposure = security_exposure;

            if (i >= FLAGS_number_of_allocation_experiments - 1) {
                break;
            }
        }
    }

    if (best_objective_value == std::numeric_limits<double>::max()) {
        LOG(FATAL) << "There is no storage available";
    }

    DLOG(INFO) << "Allocation of the output File[" << file->get_id() << "] to the Storage[" << best_storage_id << "]";

    // Allocate file
    file_allocations_[file->get_id()] = best_storage_id;

    // Store the file contribution to the makespan, cost and security expose
    security_exposure_ += best_security_exposure;

    if (best_storage_id < algorithm_->GetVirtualMachineSize() && best_storage_id != vm->get_id()) {
        DLOG(INFO) << "allocation_vm_queue_: " << start_time;
        DLOG(INFO) << "allocation_vm_queue_: " << read_time;
        DLOG(INFO) << "allocation_vm_queue_: " << run_time;
        DLOG(INFO) << "allocation_vm_queue_: " << partial_write_time;
        DLOG(INFO) << "allocation_vm_queue_: " << best_write_one_file_time;

        allocation_vm_queue_[best_storage_id] = std::max(start_time + read_time + run_time + partial_write_time
                                                         + best_write_one_file_time,
                                                         allocation_vm_queue_[best_storage_id]);
        DLOG(INFO) << "allocation_vm_queue_[" << best_storage_id << "]: " << allocation_vm_queue_[best_storage_id];
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
size_t Solution::ComputeActivationStartTime(size_t activation_id, size_t vm_id) {
    DLOG(INFO) << "Compute the start time of the Activation[" << activation_id << "] at VM[" << vm_id << "]";

    size_t start_time = 0UL;

    for (auto previous_task_id: algorithm_->GetPredecessors(activation_id)) {
        start_time = std::max<size_t>(start_time, activation_finish_time_[previous_task_id]);
    }

    DLOG(INFO) << "StartTime: " << start_time;

    return std::max<size_t>(start_time, vm_finish_time_[vm_id]);
}

size_t Solution::AllocateOutputFiles(const std::shared_ptr<Activation> &task,
                                     const std::shared_ptr<VirtualMachine> &vm,
                                     const size_t start_time,
                                     const size_t read_time,
                                     const size_t run_time) {
    auto write_time = 0ul;

    auto output_files = task->get_output_files();

    // TODO: see if this behaviour is better
    // Shuffle the output files for better randomness between the solutions
    if (output_files.size() > 1) {
        std::shuffle(output_files.begin(), output_files.end(), generator());
    }

    // For each output file Allocate the storage that impose the minor Write time
    for (auto &file: output_files) {
        if (file_allocations_[file->get_id()] == std::numeric_limits<size_t>::max()) {
            write_time += AllocateOneOutputFileGreedily(file, vm, start_time, read_time, run_time, write_time);
        } else {
            std::shared_ptr<Storage> storage = algorithm_->GetStoragePerId(file_allocations_[file->get_id()]);
            write_time += ComputeFileTransferTime(file, vm, storage);
        }
    }

    return write_time;
}

size_t Solution::ComputeActivationReadTime(const std::shared_ptr<Activation> &activation,
                                           const std::shared_ptr<VirtualMachine> &vm,
                                           const size_t start_time) {
    DLOG(INFO) << "Compute Read Time of the Activation[" << activation->get_id() << "] at VM[" << vm->get_id()
               << "]";

    auto read_time = 0ul;

    for (const auto &file: activation->get_input_files()) {
        size_t storage_id;

        if (auto static_file = std::dynamic_pointer_cast<StaticFile>(file)) {
            storage_id = static_file->GetFirstVm();
        } else {
            storage_id = file_allocations_[file->get_id()];
        }

        if (storage_id == std::numeric_limits<size_t>::max()) {
            LOG(FATAL) << "Wrong storage_id - ComputeActivationReadTime";
        }

        std::shared_ptr<Storage> file_vm = algorithm_->GetStoragePerId(storage_id);

        size_t one_file_read_time = ComputeFileTransferTime(file, file_vm, vm);

        if (one_file_read_time == std::numeric_limits<size_t>::max()) {
            DLOG(INFO) << "read_time: " << one_file_read_time;

            return std::numeric_limits<size_t>::max();
        } else {
            // read_time += std::ceil(one_file_read_time);
            read_time += one_file_read_time;
            if (storage_id < algorithm_->GetVirtualMachineSize() and storage_id != vm->get_id()) {
                allocation_vm_queue_[storage_id] = std::max(allocation_vm_queue_[storage_id], (start_time + read_time));
                DLOG(INFO) << "storage_id";
                DLOG(INFO) << "allocation_vm_queue_[" << storage_id << "]: " << allocation_vm_queue_[storage_id];
            }
        }
    }

    DLOG(INFO) << "read_time: " << read_time;

    return read_time;
}

//size_t Solution::ComputeTaskReadTimeOther(const std::shared_ptr<Activation>& task,
//                                          const std::shared_ptr<VirtualMachine>& vm) {
//  double read_time = 0.0;
//
////  DLOG(INFO) << "Compute Read Time of the Activation[" << task->get_id() << "] at VM[" << vm->get_id()
////             << "]";
//
//  for (const auto& file: task->get_input_files()) {
//    size_t storage_id;
//
////    if (std::shared_ptr<StaticFile> static_file = std::dynamic_pointer_cast<StaticFile>(file)) {
////      storage_id = static_file->GetFirstVm();
////    } else {
////      storage_id = file_allocations_[file->get_id()];
////    }
//    storage_id = file->GetStorageId();
//
//    std::shared_ptr<Storage> file_vm = algorithm_->GetStoragePerId(storage_id);
//
//    // Ceil of File Transfer Time + File Size * lambda
//    double one_file_read_time = ComputeFileTransferTime(file, file_vm, vm);
//
//    if (one_file_read_time == std::numeric_limits<double>::max()) {
//      DLOG(INFO) << "read_time: " << one_file_read_time;
//      return one_file_read_time;
//    } else {
//      // read_time += std::ceil(one_file_read_time);
//      read_time += one_file_read_time;
//      if (storage_id < algorithm_->GetVirtualMachineSize() and storage_id != vm->get_id()) {
//        double diff = (aux_start_time + read_time) - vm_finish_time_[storage_id];
//
//        if (diff > 0.0) {
//          std::shared_ptr<VirtualMachine> virtual_machine = algorithm_->GetVirtualMachinePerId(storage_id);
//
////          cost_ += diff * virtual_machine->get_cost();
//          allocation_vm_queue_[storage_id] = aux_start_time + read_time;
////          vm_finish_time_[storage_id] = aux_start_time + read_time;
//          // std::c out << "diff: " << diff << " virtual_machine->get_cost(): " << virtual_machine->get_cost()
//          //           << " storage_id: " << storage_id << " vm: " << vm->get_id() << std::endl;
//        }
//      }
//      // allocation_vm_queue_[storage_id] = std::max(aux_start_time
//      //     + one_file_read_time, allocation_vm_queue_[storage_id]);
//    }
//  }  // for (std::shared_ptr<File> file : task.get_input_files()) {
//
////  DLOG(INFO) << "read_time: " << read_time;
//
//  return read_time;
//}  // double Solution::ComputeActivationReadTime(Activation& task, VirtualMachine& vm) {

/**
 * This method is important for calculate de makespan and to Allocate the output files into
 * Storages(VMs and Buckets)
 *
 * \param[in]  activation             Activation with which the output files will be allocated
 * \param[in]  virtual_machine  VM where the activation will be executed
 * \retval     makespan         The objective value of the solution when inserting the \c activation
 */
size_t Solution::CalculateMakespanAndAllocateOutputFiles(const std::shared_ptr<Activation> &activation,
                                                         const std::shared_ptr<VirtualMachine> &virtual_machine) {
    DLOG(INFO) << "Makespan of the allocated Activation[" << activation->get_id() << "] at VM["
               << virtual_machine->get_id() << "]";

    size_t start_time;
    size_t read_time;
    size_t run_time;
    size_t write_time;
    size_t finish_time = std::numeric_limits<size_t>::max();

    start_time = ComputeActivationStartTime(activation->get_id(), virtual_machine->get_id());
    read_time = ComputeActivationReadTime(activation, virtual_machine, start_time);
    run_time = std::ceil(activation->get_time() * virtual_machine->get_slowdown());
    write_time = AllocateOutputFiles(activation, virtual_machine, start_time, read_time, run_time);

    if (start_time != std::numeric_limits<size_t>::max()
        && read_time != std::numeric_limits<size_t>::max()
        && write_time != std::numeric_limits<size_t>::max()) {
        finish_time = start_time + read_time + run_time + write_time;
    } else {
        DLOG(FATAL) << "Something is wrong!";
    }

//    DLOG(INFO) << "start_time: " << StartTime;
//    DLOG(INFO) << "read_time: " << read_time;
//    DLOG(INFO) << "run_time: " << run_time;
//    DLOG(INFO) << "write_time: " << write_time;
//    DLOG(INFO) << "finish_time: " << finish_time;

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
    // double task_exposure = 0.0;
    double privacy_exposure = 0.0;

//  DLOG(INFO) << "Calculate Security Exposure";

    // Accumulate the privacy_exposure
    for (size_t i = 0ul; i < algorithm_->GetFilesSize(); ++i) {
        size_t storage1_id = file_allocations_[i];

        if (storage1_id != std::numeric_limits<size_t>::max()
            && storage->get_id() != std::numeric_limits<size_t>::max()
            && storage1_id == storage->get_id()) {
            // int conflict_value = algorithm_->get_conflict_graph().ReturnConflict(i, file->get_id());
            int conflict_value = algorithm_->get_conflict_graph().ReturnConflict(file->get_id(), i);

            if (conflict_value > 0) {
                DLOG(INFO) << "File[" << i << "] has conflict with File[" << file->get_id() << "]";
                privacy_exposure += conflict_value;  // Adds the penalties
            }
        }  // else; allocated file resides in different storage or was not allocated yet
    }  // for (size_t i...) {

    // DLOG(INFO) << "task_exposure: " << task_exposure;
//  DLOG(INFO) << "privacy_exposure: " << privacy_exposure;

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
    DLOG(INFO) << "Begin schedule the Activation[" << activation->get_id() << "] at VM[" << vm->get_id() << "]";

    // Allocate Activation
    activation_allocations_[activation->get_id()] = vm->get_id();
    ordering_.push_back(activation->get_id());

    // 1. Calculates the finish_time
    size_t finish_time = CalculateMakespanAndAllocateOutputFiles(activation, vm);

    // Update auxiliary structures (queue_ and activation_finish_time_)
    // This update is important for the cost calculation
    activation_finish_time_[activation->get_id()] = finish_time;
    vm_finish_time_[vm->get_id()] = finish_time;
    allocation_vm_queue_[vm->get_id()] = std::max<size_t>(finish_time, allocation_vm_queue_[vm->get_id()]);
    DLOG(INFO) << "vm->get_id()";
    DLOG(INFO) << "allocation_vm_queue_[" << vm->get_id() << "]: " << allocation_vm_queue_[vm->get_id()];

    makespan_ = activation_finish_time_[ordering_.back()];

    // 2. Calculates the cost contribution of the activation execution at the virtual machine
    DLOG(INFO) << "Calculates the cost contribution of the Virtual Machine Cost of the scheduled activation";
    double virtual_machine_cost = 0.0;
    double bucket_variable_cost = 0.0;

    // Accumulate the Virtual Machine cost
    for (size_t i = 0ul; i < algorithm_->GetVirtualMachineSize(); ++i) {
        std::shared_ptr<VirtualMachine> virtual_machine = algorithm_->GetVirtualMachinePerId(i);
        virtual_machine_cost += static_cast<double>(allocation_vm_queue_[virtual_machine->get_id()])
                                * virtual_machine->get_cost();
        DLOG(INFO) << "allocation_vm_queue_[virtual_machine->get_id()]: "
                << allocation_vm_queue_[virtual_machine->get_id()];
        DLOG(INFO) << "virtual_machine->get_cost(): " << virtual_machine->get_cost();
    }

    // Accumulate the Bucket variable cost
    for (size_t i = algorithm_->GetVirtualMachineSize(); i < algorithm_->GetStorageSize(); ++i) {
        std::shared_ptr<Storage> storage = algorithm_->GetStoragePerId(i);

        // If the storage is not a Virtual Machine, i.e. it is a Bucket; then calculate the bucket
        // variable cost
        for (size_t j = 0ul; j < algorithm_->GetFilesSize(); ++j) {
            // If the Bucket is used; then accumulate de cost and break to the next Storage
            if (file_allocations_[j] == storage->get_id()) {
                std::shared_ptr<File> file = algorithm_->GetFilePerId(j);
                bucket_variable_cost += (storage->get_cost() * file->get_size_in_GB());
                DLOG(INFO) << "storage->get_cost(): " << storage->get_cost();
                DLOG(INFO) << "file->get_size_in_GB(): " << file->get_size_in_GB();
            }
        }
    }
    cost_ = virtual_machine_cost + bucket_variable_cost;

    // 3. Calculates the security exposure
    DLOG(INFO) << "Accumulate the activation Exposure of the scheduled activation";

    // Accumulate the activation exposure
    for (size_t j = 0; j < activation->get_requirements().size(); ++j) {
        size_t virtual_machine_id = activation_allocations_[activation->get_id()];

        // If the activation is allocated
        if (virtual_machine_id != std::numeric_limits<size_t>::max()) {
            std::shared_ptr<VirtualMachine> virtual_machine = algorithm_->GetVirtualMachinePerId(virtual_machine_id);

            if (activation->GetRequirementValue(j) > virtual_machine->GetRequirementValue(j)) {
                security_exposure_ += activation->GetRequirementValue(j) - virtual_machine->GetRequirementValue(j);
            }
        }
    }

    // If solution is unfeasible return size_t max
    if (makespan_ == std::numeric_limits<size_t>::max()
        || cost_ == std::numeric_limits<double>::max()
        || security_exposure_ == std::numeric_limits<double>::max()) {
        objective_value_ = std::numeric_limits<double>::max();
    } else { // o.w. return O.F.
        objective_value_ =
                algorithm_->get_alpha_time() * (static_cast<double>(makespan_) / algorithm_->get_makespan_max())
                + algorithm_->get_alpha_budget() * (cost_ / algorithm_->get_budget_max())
                + algorithm_->get_alpha_security() * (security_exposure_
                                                      / algorithm_->get_maximum_security_and_privacy_exposure());
    }

    DLOG(INFO) << "Makespan " << makespan_ << ", cost " << cost_ << ", security " << security_exposure_ << ", o.f. "
               << objective_value_;

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
    if (node != algorithm_->get_id_target()) {
        for (auto i: algorithm_->GetSuccessors(node)) {
            int value = ComputeTasksHeights(i);
            min = std::min(value, min);
        }
    } else {
        activation_height_[node] = algorithm_->get_height()[node];
        return activation_height_[node];
    }
    std::uniform_int_distribution<> dis(algorithm_->get_height()[node], min - 1);
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
    size_t best_known_makespan = makespan_;
    double best_known_cost = cost_;
    double best_known_security_exposure_ = security_exposure_;
    for (size_t i = 1; i < algorithm_->GetActivationSize() - 2; i++) {
        for (size_t j = i + 1; j < algorithm_->GetActivationSize() - 1; j++) {
            if (activation_allocations_[i] != activation_allocations_[j]) {
                auto i_vm = activation_allocations_[i];
                auto j_vm = activation_allocations_[j];
                std::deque<std::pair<size_t, size_t>> files_changed;
                // Do the swap
                iter_swap(activation_allocations_.begin() + static_cast<long int>(i),
                          activation_allocations_.begin() + static_cast<long int>(j));
                auto i_activation = algorithm_->GetActivationPerId(i);
                auto i_input_files = i_activation->get_input_files();
                for (const auto &input_file : i_input_files) {
                    if (auto dynamic_file = std::dynamic_pointer_cast<DynamicFile>(input_file)) {
                        auto file_id = dynamic_file->get_id();
                        auto file_allocation = file_allocations_[file_id];
                        if (file_allocation == i_vm) {
                            auto it = std::find_if(files_changed.begin(), files_changed.end(),
                                                   [file_id](const std::pair<size_t, size_t>& my_pair) {
                                                       return my_pair.first == file_id;
                                                   });
                            if (it == files_changed.end()) {
                                file_allocations_[file_id] = j_vm;
                                files_changed.emplace_back(file_id, file_allocation);
                            }
                        }
                    }
                }
                auto i_output_files = i_activation->get_output_files();
                for (const auto &output_file : i_output_files) {
                    if (auto dynamic_file = std::dynamic_pointer_cast<DynamicFile>(output_file)) {
                        auto file_id = dynamic_file->get_id();
                        auto file_allocation = file_allocations_[file_id];
                        if (file_allocation == i_vm) {
                            auto it = std::find_if(files_changed.begin(), files_changed.end(),
                                                   [file_id](const std::pair<size_t, size_t>& my_pair) {
                                                       return my_pair.first == file_id;
                                                   });
                            if (it == files_changed.end()) {
                                file_allocations_[file_id] = j_vm;
                                files_changed.emplace_back(file_id, file_allocation);
                            }
                        }
                    }
                }
                auto j_activation = algorithm_->GetActivationPerId(j);
                auto j_input_files = j_activation->get_input_files();
                for (const auto &input_file : j_input_files) {
                    if (auto dynamic_file = std::dynamic_pointer_cast<DynamicFile>(input_file)) {
                        auto file_id = dynamic_file->get_id();
                        auto file_allocation = file_allocations_[file_id];
                        if (file_allocation == j_vm) {
                            auto it = std::find_if(files_changed.begin(), files_changed.end(),
                                                   [file_id](const std::pair<size_t, size_t>& my_pair) {
                                                       return my_pair.first == file_id;
                                                   });
                            if (it == files_changed.end()) {
                                file_allocations_[file_id] = i_vm;
                                files_changed.emplace_back(file_id, file_allocation);
                            }
                        }
                    }
                }
                auto j_output_files = j_activation->get_output_files();
                for (const auto &output_file : j_output_files) {
                    if (auto dynamic_file = std::dynamic_pointer_cast<DynamicFile>(output_file)) {
                        auto file_id = dynamic_file->get_id();
                        auto file_allocation = file_allocations_[file_id];
                        if (file_allocation == j_vm) {
                            auto it = std::find_if(files_changed.begin(), files_changed.end(),
                                                   [file_id](const std::pair<size_t, size_t>& my_pair) {
                                                       return my_pair.first == file_id;
                                                   });
                            if (it == files_changed.end()) {
                                file_allocations_[file_id] = i_vm;
                                files_changed.emplace_back(file_id, file_allocation);
                            }
                        }
                    }
                }
                ComputeObjectiveFunction();
                DLOG(INFO) << "... localSearchN1 : " << objective_value_ << " < " << best_known_of
                           << std::endl;
                if (objective_value_ < best_known_of) {
                    DLOG(INFO) << "... localSearchN1 : " << objective_value_ << " < " << best_known_of
                               << std::endl;
                    DLOG(INFO) << "... ending localSearchN1 local search";
                    return true;
                }

                // Return elements
                iter_swap(activation_allocations_.begin() + static_cast<long int>(i),
                          activation_allocations_.begin() + static_cast<long int>(j));
                while (!files_changed.empty()) {
                    auto my_pair = files_changed.front();
                    file_allocations_[my_pair.first] = my_pair.second;
                    files_changed.pop_front();
                }
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
    size_t best_known_makespan = makespan_;
    double best_known_cost = cost_;
    double best_known_security_exposure_ = security_exposure_;
    // for each task, do
    for (size_t i = 0; i < algorithm_->GetActivationSize(); i++) {
        auto task_i = ordering_[i];
        for (size_t j = i + 1; j < algorithm_->GetActivationSize(); j++) {
            auto task_j = ordering_[j];
            if (activation_height_[task_i] == activation_height_[task_j]) {
                // Do the swap
                iter_swap(ordering_.begin() + static_cast<long int>(i), ordering_.begin() + static_cast<long int>(j));
                ComputeObjectiveFunction();
                DLOG(INFO) << "new objective value " << objective_value_ << " i " << i << " j " << j << std::endl;
                if (objective_value_ < best_known_of) {
                    DLOG(INFO) << "... localSearchN2 : " << objective_value_ << " < " << best_known_of
                               << std::endl;
                    DLOG(INFO) << "... ending localSearchN2 local search";
                    return true;
                }
                // Return elements
                iter_swap(ordering_.begin() + static_cast<long int>(i), ordering_.begin() + static_cast<long int>(j));
//                ComputeObjectiveFunction();
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
    size_t best_known_makespan = makespan_;
    double best_known_cost = cost_;
    double best_known_security_exposure_ = security_exposure_;
    for (size_t i = 0; i < algorithm_->GetActivationSize(); ++i) {
        std::deque<std::pair<size_t, size_t>> files_changed;
        auto old_vm = activation_allocations_[i];
        for (size_t j = 0; j < algorithm_->GetVirtualMachineSize(); j++) {
            if (old_vm != j) {
                activation_allocations_[i] = j;
                auto i_activation = algorithm_->GetActivationPerId(i);
                auto i_input_files = i_activation->get_input_files();
                for (const auto &input_file : i_input_files) {
                    if (auto dynamic_file = std::dynamic_pointer_cast<DynamicFile>(input_file)) {
                        auto file_id = dynamic_file->get_id();
                        auto file_allocation = file_allocations_[file_id];
                        if (file_allocation == old_vm) {
                            auto it = std::find_if(files_changed.begin(), files_changed.end(),
                                                   [file_id](const std::pair<size_t, size_t>& my_pair) {
                                                       return my_pair.first == file_id;
                                                   });
                            if (it == files_changed.end()) {
                                file_allocations_[file_id] = old_vm;
                                files_changed.emplace_back(file_id, file_allocation);
                            }
                        }
                    }
                }
                auto i_output_files = i_activation->get_output_files();
                for (const auto &output_file : i_output_files) {
                    if (auto dynamic_file = std::dynamic_pointer_cast<DynamicFile>(output_file)) {
                        auto file_id = dynamic_file->get_id();
                        auto file_allocation = file_allocations_[file_id];
                        if (file_allocation == old_vm) {
                            auto it = std::find_if(files_changed.begin(), files_changed.end(),
                                                   [file_id](const std::pair<size_t, size_t>& my_pair) {
                                                       return my_pair.first == file_id;
                                                   });
                            if (it == files_changed.end()) {
                                file_allocations_[file_id] = old_vm;
                                files_changed.emplace_back(file_id, file_allocation);
                            }
                        }
                    }
                }
                ComputeObjectiveFunction();
//                makespan_ = of_makespan;
//                cost_ = of_cost;
//                security_exposure_ = of_security_exposure;
//                objective_value_ = of;
                DLOG(INFO) << "new objective value " << objective_value_ << " i " << i << " j " << j << std::endl;
                if (objective_value_ < best_known_of) {
                    DLOG(INFO) << "... localSearchN3 : " << objective_value_ << " < " << best_known_of
                               << std::endl;
                    DLOG(INFO) << "... ending localSearchN3 local search";
                    return true;
                }
            }
        }
        activation_allocations_[i] = old_vm;
        while (!files_changed.empty()) {
            auto my_pair = files_changed.front();
            file_allocations_[my_pair.first] = my_pair.second;
            files_changed.pop_front();
        }
        // Rollback
        objective_value_ = best_known_of;
        makespan_ = best_known_makespan;
        cost_ = best_known_cost;
        security_exposure_ = best_known_security_exposure_;
    }
    DLOG(INFO) << "... ending localSearchN3 local search";
    return false;
}

void Solution::SwapActivationsVms(size_t i, size_t j) {
    std::deque<std::pair<size_t, size_t>> files_changed;
    // Do the swap
    iter_swap(activation_allocations_.begin() + static_cast<long int>(i),
              activation_allocations_.begin() + static_cast<long int>(j));
    auto i_activation = algorithm_->GetActivationPerId(i);
    auto i_input_files = i_activation->get_input_files();
    for (const auto &input_file : i_input_files) {
        if (auto dynamic_file = std::dynamic_pointer_cast<DynamicFile>(input_file)) {
            auto file_id = dynamic_file->get_id();
            auto file_allocation = file_allocations_[file_id];
            if (file_allocation == i_vm) {
                auto it = std::find_if(files_changed.begin(), files_changed.end(),
                                       [file_id](const std::pair<size_t, size_t>& my_pair) {
                                           return my_pair.first == file_id;
                                       });
                if (it == files_changed.end()) {
                    file_allocations_[file_id] = j_vm;
                    files_changed.emplace_back(file_id, file_allocation);
                }
            }
        }
    }
    auto i_output_files = i_activation->get_output_files();
    for (const auto &output_file : i_output_files) {
        if (auto dynamic_file = std::dynamic_pointer_cast<DynamicFile>(output_file)) {
            auto file_id = dynamic_file->get_id();
            auto file_allocation = file_allocations_[file_id];
            if (file_allocation == i_vm) {
                auto it = std::find_if(files_changed.begin(), files_changed.end(),
                                       [file_id](const std::pair<size_t, size_t>& my_pair) {
                                           return my_pair.first == file_id;
                                       });
                if (it == files_changed.end()) {
                    file_allocations_[file_id] = j_vm;
                    files_changed.emplace_back(file_id, file_allocation);
                }
            }
        }
    }
    auto j_activation = algorithm_->GetActivationPerId(j);
    auto j_input_files = j_activation->get_input_files();
    for (const auto &input_file : j_input_files) {
        if (auto dynamic_file = std::dynamic_pointer_cast<DynamicFile>(input_file)) {
            auto file_id = dynamic_file->get_id();
            auto file_allocation = file_allocations_[file_id];
            if (file_allocation == j_vm) {
                auto it = std::find_if(files_changed.begin(), files_changed.end(),
                                       [file_id](const std::pair<size_t, size_t>& my_pair) {
                                           return my_pair.first == file_id;
                                       });
                if (it == files_changed.end()) {
                    file_allocations_[file_id] = i_vm;
                    files_changed.emplace_back(file_id, file_allocation);
                }
            }
        }
    }
    auto j_output_files = j_activation->get_output_files();
    for (const auto &output_file : j_output_files) {
        if (auto dynamic_file = std::dynamic_pointer_cast<DynamicFile>(output_file)) {
            auto file_id = dynamic_file->get_id();
            auto file_allocation = file_allocations_[file_id];
            if (file_allocation == j_vm) {
                auto it = std::find_if(files_changed.begin(), files_changed.end(),
                                       [file_id](const std::pair<size_t, size_t>& my_pair) {
                                           return my_pair.first == file_id;
                                       });
                if (it == files_changed.end()) {
                    file_allocations_[file_id] = i_vm;
                    files_changed.emplace_back(file_id, file_allocation);
                }
            }
        }
    }
}

void Solution::SwapActivationsOrders(size_t i, size_t j) {

}

void Solution::ChangeActivationVm(size_t i) {

}
