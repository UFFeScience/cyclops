/**
 * \file src/solution/min_min.cc
 * \brief Contains the \c Configuration class methods.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo.raps@gmail.com\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 *
 * This source file contains the methods from the \c MinMin class
 * that start the mode the approximate solution.
 */

#include "src/solution/min_min.h"
#include <vector>

DECLARE_uint64(number_of_iteration);

double MinMin::CalculateFinishTime(
        Solution& solution,
        const size_t act_id,
        const size_t vm_id,
        const std::vector<ActivationAssignment>& activations_assignments,
        const std::vector<double>& virtual_machines_available_time
) {
    double start_time = 0.0, read_time, run_time, write_time;

//    start_time = solution.GetActivationStartTime(act_id, vm_id);

    // What came later, occupation of the VM or the previous activation finish time
    for (auto predecessor_id : GetPredecessors(act_id)) {
        if (predecessor_id == GetSourceId()) {
            break;
        }
        if (activations_assignments[predecessor_id].completion_time == std::numeric_limits<double>::max()) {
            continue;
        }
        start_time = std::max(start_time, activations_assignments[predecessor_id].completion_time);
    }
//    auto ordering_size = ordering_.size();

    start_time = std::max(start_time, virtual_machines_available_time[vm_id]);
    read_time = solution.GetActivationReadTime(act_id, vm_id);
    run_time = solution.GetActivationRunTime(act_id, vm_id);
//     TODO: Change the method to not receive activation and vm object, using IDs instead
//    auto activation = get_activation_per_id(act_id);
//    auto virtual_machine = get_virtual_machine_per_id(vm_id);
//    write_time = solution.AllocateOutputFiles(activation, virtual_machine, start_time, read_time, run_time);
    write_time = solution.GetActivationWriteTime(act_id, vm_id);

//    DLOG(INFO) << "act_id: " << act_id << " vm_id: " << vm_id << " st: " << start_time << " rt: " << read_time
//            <<  " xt: " << run_time << " wt: " << write_time;

    return start_time + read_time + run_time + write_time;
}

// Function to get the index of the task with the global minimum completion time
ActivationAssignment MinMin::FindMinMin(
        Solution& best_solution,
        const std::list<size_t>& available_activations,
        const std::vector<ActivationAssignment>& activations_assignments,
        const std::vector<double>& virtual_machines_available_time
) {
    ActivationAssignment best = {std::numeric_limits<size_t>::max(),
                                 std::numeric_limits<size_t>::max(),
                                 std::numeric_limits<double>::max()};

    for (auto activation_id : available_activations) {
//        if (activation_id == GetSourceId()) {
//            best = {activation_id, 0, 0.0};
//            break;
//        }
        DLOG(INFO) << "Analysing Activation " << activation_id;

        double minECT = std::numeric_limits<double>::max();
        size_t best_vm = std::numeric_limits<size_t>::max();

        for (size_t vm_id{}; vm_id < GetVirtualMachinesSize(); ++vm_id) {
            double ect = CalculateFinishTime(best_solution,
                                             activation_id,
                                             vm_id,
                                             activations_assignments,
                                             virtual_machines_available_time);
            if (ect < minECT) {
                minECT = ect;
                best_vm = vm_id;
            }
        }

        DLOG(INFO) << "Activation minimum estimated completion time " << minECT;

        if (minECT < best.completion_time) {
            best = {activation_id, best_vm, minECT};
        }
    }

    if (best.completion_time == std::numeric_limits<double>::max()) {
        LOG(FATAL) << "Not gonna happen";
    }

    return best;
}

void MinMin::MinMinScheduling(Solution& best_solution) {
    std::list<size_t> activations;
    std::list<size_t> available_activations;
    std::vector<ActivationAssignment> activations_assignments{
            GetActivationsSize(),
            { std::numeric_limits<size_t>::max(),
              std::numeric_limits<size_t>::max(),
              std::numeric_limits<double>::max() }
    };
    std::vector<double> virtual_machines_available_time(GetVirtualMachinesSize(), 0.0);

    for (const auto& activation : GetActivations()) {
        activations.push_back(activation->get_id());
    }

    activations.sort([&](const size_t a, const size_t b) {
        return GetHeight(a) < GetHeight(b);
    });

    while (!activations.empty()) {
        available_activations.clear();
        size_t act_id = activations.front(); // Get the first activation

        while (!activations.empty() && GetHeight(act_id) == GetHeight(activations.front())) {
            // Build list of ready activations, that is the activations which the predecessor was finish
            available_activations.push_back(activations.front());
            activations.pop_front();
        }

        while (!available_activations.empty()) {
            // Schedule available activations
            ActivationAssignment best = FindMinMin(best_solution,
                                                   available_activations,
                                                   activations_assignments,
                                                   virtual_machines_available_time);

            if (best.activation_id != std::numeric_limits<size_t>::max()) {
                DLOG(INFO) << "best.activation_id: " << best.activation_id << " best.vm_id: " << best.vm_id
                        << " best.completion_time: " << best.completion_time;
                best_solution.AllocateTask(GetActivationPerId(best.activation_id), GetVirtualMachinePerId(best.vm_id));
                for (const auto& file : GetOutputFiles(best.activation_id)) {
                    best_solution.SetFileAllocation(file->get_id(), best.vm_id);
                }
                best_solution.AddOrdering(best.activation_id);
                activations_assignments[best.activation_id] = best;
                virtual_machines_available_time[best.vm_id] = best.completion_time;
//                best_solution.OptimizedComputeObjectiveFunction();
            }

            available_activations.remove(best.activation_id);
        }
    }

//    best_solution.OptimizedComputeObjectiveFunction();
}

void MinMin::Run() {
    DLOG(INFO) << "Executing MinMin ...";
    DLOG(INFO) << "Initialising ranks variables";

    // Defining variables

    Solution best_solution{get_configuration()};

    MinMinScheduling(best_solution);

    DLOG(INFO) << "Compute Objective Function";
    best_solution.OptimizedComputeObjectiveFunction();

    double time_s = ((double) clock() - (double) t_start) / CLOCKS_PER_SEC;  // Processing time

    std::cout << std::fixed << std::setprecision(6)
              << best_solution.get_objective_value()
              << " " << best_solution.get_makespan()
              << " " << best_solution.get_cost()
              << " " << best_solution.get_security_exposure() / GetMaximumConfidentialityExposure()
              << " " << time_s
              << " 1"
              << " 1"
              << " " << time_s
              << std::endl;

    LOG(INFO) << best_solution;

    DLOG(INFO) << "... ending MinMin";
    // TODO: Printing the scheduling
    // TODO: See the warning messages
}
