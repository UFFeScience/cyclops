/**
 * \file src/solution/heft.cc
 * \brief Contains the \c Algorithm class methods.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2022
 *
 * This source file contains the methods from the \c Heft class
 * that run the mode the approximate solution.
 */

#include "src/solution/heft.h"

DECLARE_uint64(number_of_iteration);

/* Average computation cost */
/**
 * Compute the computation cost of a given activation on each virtual machine and return the average of that
 * computation.
 *
 * @param activation_id
 * @return
 */
double Heft::ComputeAverageComputationCost(size_t activation_id) {
    // TODO: Insert debug message
    auto average_computation_cost = 0.0;

    for (const auto &vm : virtual_machines_) {
        average_computation_cost += ComputationCost(activation_id, vm->get_id());
    }

    return average_computation_cost / static_cast<double>(GetVirtualMachineSize());
}

std::vector<size_t> Heft::intersection(std::vector<size_t> v1, std::vector<size_t> v2) {
    std::vector<size_t> v3;

    sort(v1.begin(), v1.end());
    sort(v2.begin(), v2.end());
    set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), back_inserter(v3));

    return v3;
}

/**
 * Computes the average cost (in time) of communication between VMs considering the transfer of input files from
 * activation.
 *
 * @param activation_id
 * @param vm_id
 * @return
 */
double Heft::CommunicationCostStatic(size_t activation_id, size_t vm_id) {
    // TODO: Insert debug message
    // TODO: use the transfer file computation method
    auto cost = 0.0;

    auto activation = activations_[activation_id];
    auto target_vm = virtual_machines_[vm_id];

    for (const auto &file : activation->get_input_files()) {
//        if (auto *static_file = dynamic_cast<StaticFile *>(file)) {
        if (auto static_file = std::dynamic_pointer_cast<StaticFile>(file)) {
            auto origin_vm = virtual_machines_[static_file->GetFirstVm()];
            auto bandwidth = std::min(origin_vm->get_bandwidth_in_GBps(), target_vm->get_bandwidth_in_GBps());

            // Calculate time
            if (origin_vm->get_id() != target_vm->get_id()) {
                cost += ceil(file->get_size_in_GB() / bandwidth);
            } else {
                cost = 1.0;
            }
        }
    }

    DLOG(INFO) << "CommunicationCostStatic(" << activation_id << ", " << vm_id << ") " << std::fixed
            << std::setprecision(6) << cost << ".";

    return cost;
}

/* Compute communication cost of dynamic files */
/**
 *
 *
 * @param activation_id_i
 * @param activation_id_j
 * @param vm_id_i
 * @param vm_id_j
 * @return
 */
double Heft::CommunicationCostOfDynamicFiles(size_t activation_id_i,
                                             size_t activation_id_j,
                                             size_t vm_id_i,
                                             size_t vm_id_j) {
    // TODO: Insert debug message
    double cost = 0.0;

    // If VMs are the same, just return zero
    if (vm_id_i == vm_id_j) {
        return 1.0;
    }

    auto activation_i = activations_[activation_id_i];
    auto activation_j = activations_[activation_id_j];

    auto vm_i = virtual_machines_[vm_id_i];
    auto vm_j = virtual_machines_[vm_id_j];

    // Get the lowest bandwidth
    auto bandwidth = std::min(vm_i->get_bandwidth_in_GBps(), vm_j->get_bandwidth_in_GBps());

    std::vector<size_t> activation_i_output_files_id;
    for (const auto& o: activation_i->get_output_files()) {
        activation_i_output_files_id.push_back(o->get_id());
    }

    std::vector<size_t> activation_j_input_files_id;
    for (const auto& i: activation_j->get_input_files()) {
        activation_i_output_files_id.push_back(i->get_id());
    }

    // Get the files that are written by activation i and read by activation j
    auto common_files = intersection(activation_i_output_files_id, activation_j_input_files_id);

    // Now, sum all the cost (in time) to transfer all those files
    for (auto file_id: common_files) {
        auto file = files_[file_id];
        // TODO: Use the appropriated method
        cost += ceil(file->get_size_in_GB() / bandwidth);
    }

    return cost;
}

/* Average communication cost */
/**
 * Calculate the average of the communication cost, that mean the sum of the costs to transfer all static files needed
 * by activation j plus all the costs to transfer all the dynamic files produced by activation i that is consumed by
 * activation j.
 *
 * PS:
 * . Do not take into consideration communication between VMs and Buckets.
 *
 * @param activation_id_i
 * @param activation_id_j
 * @param lambda
 *
 * @return
 */
double Heft::ComputeAverageCommunicationCost(size_t activation_id_i, size_t activation_id_j) {
    // TODO: Insert debug message
    double communication_cost = 0.0;
    double static_files_communication_cost = 0.0;
    double dynamic_files_communication_cost = 0.0;

    // Just a protection, if exists only one virtual machine then, just, return zero.
    if (GetVirtualMachineSize() == 1) {
        return communication_cost;
    }

    // Calculate the number of possible virtual machine pairs.
    auto number_of_vm = GetVirtualMachineSize();
    auto number_of_vm_pairs = GetVirtualMachineSize() * (GetVirtualMachineSize() - 1);

    // Compute the static file communication cost for each vm.
    for (const auto &vm: virtual_machines_) {
        static_files_communication_cost = CommunicationCostStatic(activation_id_j, vm->get_id());
    }

    // Compute the communication between activation i and activation j for each pair of VMs.
    for (const auto &vm_i : virtual_machines_) {
        for (const auto &vm_j: virtual_machines_) {
            if (vm_i->get_id() != vm_j->get_id()) {
                dynamic_files_communication_cost += CommunicationCostOfDynamicFiles(activation_id_i,
                                                                                    activation_id_j,
                                                                                    vm_i->get_id(),
                                                                                    vm_j->get_id());
            }
        }
    }

    // Calculate the average cost
    communication_cost = static_files_communication_cost / static_cast<double>(number_of_vm)
                       + dynamic_files_communication_cost / static_cast<double>(number_of_vm_pairs);

    return communication_cost;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
/* Rank of Task*/
/**
 * Calculate the rank of every activation from the bottom (exit activation) to the top, hence upward.
 *
 * @param activation_id
 * @param communication_plus_max_successor_rank_cache
 * @return
 */
double Heft::RankUpward(size_t activation_id, std::vector<double> &communication_plus_max_successor_rank_cache) {
    // TODO: paste the equation on the documentation
    auto successors = GetSuccessors(activation_id);

    // Prepare the lambda function to calculate the second term of the equation
    auto rank = [&](size_t inner_activation_id) {
        auto communication_cost = ComputeAverageCommunicationCost(activation_id, inner_activation_id);
        auto successor_rank = RankUpward(inner_activation_id, communication_plus_max_successor_rank_cache);

        return communication_cost + successor_rank;
    };

    if (!successors.empty()) {
        auto max_value = 0.0;

        for (auto successor_activation_id: successors) {
            double communication_plus_successor_rank;

            if (communication_plus_max_successor_rank_cache[successor_activation_id] == -1) {
                // Get the rank of the successor
                communication_plus_successor_rank = rank(successor_activation_id);
                // Store the computed value
                communication_plus_max_successor_rank_cache[successor_activation_id] = communication_plus_successor_rank;
            } else {  // Use the cached value for the communication plus the max successor rank.
                communication_plus_successor_rank = communication_plus_max_successor_rank_cache[successor_activation_id];
            }

            max_value = std::max(max_value, communication_plus_successor_rank);
        }
        // Check if activation_id is source (this ensures that the source activation has the greatest rank)
//        activation_id == get_id_source() ? max_value *= 2 : max_value;
        // TODO: See if this is really necessary
        return ComputeAverageComputationCost(activation_id) + max_value;
    } else {  // Computation of the exit activation
        return ComputeAverageComputationCost(activation_id);
    }
}
#pragma clang diagnostic pop

/**
 *
 *
 * @param activation_id
 * @param vm_id
 * @return
 */
double Heft::ComputationCost(size_t activation_id, size_t vm_id) {
    // TODO: migrate this method to Algorithm class
    auto task = activations_[activation_id];
    auto vm = virtual_machines_[vm_id];
    return ceil(task->get_time() * vm->get_slowdown());
}

/**
 * Seek for the earliest possible start time.
 *
 * @param vm_orders is the list of events scheduled in a virtual machine
 * @param desired_start_time is the desired time to start a execution of a activation
 * @param duration
 * @return
 */
double FindFirstGap(std::vector<Event> vm_orders, double desired_start_time, double duration) {
    /*
     * Find the first gap in an agent's list of jobs.
     * The gap must be after `desired_start_time` and of length at least duration.
     */

    // (previous comment): No activation: can fit it in whenever the job is ready to run.
    // Virtual machine has no activations scheduled, so, can start whenever its desire.
    if (vm_orders.empty()) {
        return desired_start_time;
    }

    /*
     * Try to fit it in between each pair of Events, but first prepend dummy_plus_vm_orders dummy Event which ends at time 0 to check for
     * gaps before any real Event starts.
     */
    std::vector<Event> aux(1);
    auto dummy_plus_vm_orders = boost::join(aux, vm_orders);
//    auto dummy_plus_vm_orders = std::merge(aux, vm_orders);
//    std::vector<Event> dummy_plus_vm_orders;
//    std::merge(aux.begin(), aux.end(), vm_orders.begin(), vm_orders.end(), std::inserter(dummy_plus_vm_orders, dummy_plus_vm_orders.begin()));

    for (auto i = 0ul; i < dummy_plus_vm_orders.size() - 1; i++) {
        auto earliest_start = std::max<double>(desired_start_time, dummy_plus_vm_orders[static_cast<long>(i)].end);

        if (dummy_plus_vm_orders[static_cast<long>(i + 1ul)].start - earliest_start > duration) {
            return earliest_start;
        }
    }

    // No gaps found: put it at the end, or whenever the activation is ready
    return std::max(vm_orders.back().end, desired_start_time);
}

/* Earliest time that task can be executed on vm */
/**
 *
 *
 * @param activation_id
 * @param vm_id is the VM id that we wants to execute the activation defined by activation_id
 * @param activation_on contains the VM ids that initiate each activation
 * @param orders
 * @param end_time
 * @return
 */
double Heft::StartTime(size_t activation_id,  // IN
                       size_t vm_id,  // IN
                       std::vector<size_t> activation_on,  //
                       std::map<size_t, std::vector<Event>> orders,  //
                       std::vector<double> end_time) {  // IN
    auto activation_computation_cost = ComputationCost(activation_id, vm_id);
    auto communication_read_cost = 0.0;
    auto max_value = 0.0;
    auto predecessors = GetPredecessors(activation_id);
    auto queue_value = 0.0;
    double start_time;

    // Communication cost
    communication_read_cost += CommunicationCostStatic(activation_id, vm_id);
    if (activation_id != get_id_source() and !predecessors.empty()) {
        // For each predecessor of the activation
        for (auto predecessor_id : predecessors) {
            max_value = std::max(end_time[predecessor_id], max_value);
            // TODO: lacking of the static files
            communication_read_cost += CommunicationCostOfDynamicFiles(predecessor_id, activation_id, activation_on[predecessor_id],
                                                                       vm_id);
        }
    }

    auto pair_of_key_and_a_vector_of_events = orders.find(vm_id);
    auto vector_of_events = pair_of_key_and_a_vector_of_events->second;

    if (pair_of_key_and_a_vector_of_events != orders.end()) {
        if (!vector_of_events.empty()) {
            auto &event = vector_of_events.back();

            queue_value = event.end;
        }
    }

    max_value = std::max(max_value, queue_value);

    start_time = communication_read_cost + max_value;

    return FindFirstGap(vector_of_events, start_time, activation_computation_cost);
}

/*
 * Allocate task to the vm with the earliest finish time
 */
/**
 *
 *
 * @param activation_id
 * @param activation_allocation
 * @param vm_keys
 * @param orders
 * @param end_time
 */
void Heft::Allocate(size_t activation_id,
                    std::vector<size_t> &activation_allocation,
                    std::vector<size_t> vm_keys,
                    std::map<size_t, std::vector<Event>> &orders, // IN, OUT
                    std::vector<double> &end_time) {
    Event event;

    // Initialise the start time lambda function
    auto st = [&](size_t id_vm) {
        return StartTime(activation_id, id_vm, activation_allocation, orders, end_time);
    };

    // Initialise the finish time lambda function
    auto ft = [&](size_t id_vm) {
        return st(id_vm) + ComputationCost(activation_id, id_vm);
    };

    // Sort vms based on task finish time
    // For each processor P_k in the processor-set (P_k E P) do:
    // TODO: Do better, iterate through VM list, just once
    std::sort(vm_keys.begin(), vm_keys.end(), [&](const size_t &vm_a, const size_t &vm_b) {
        // Compute the Earliest Execution Finish Time of activation n_i.
        auto finish_time_a = ft(vm_a);
        auto finish_time_b = ft(vm_b);
        auto result = finish_time_a < finish_time_b;

        DLOG(INFO) << std::fixed << std::setprecision(6) << "activation_id " << activation_id << ", vm_a " << vm_a
                   << ", finish_time_a " << finish_time_a << ", vm_b " << vm_b << ", finish_time_b " << finish_time_b
                   << ", result " << result << ".";

        return result;
    });

    auto vm_id = vm_keys.front();  // Get the selected VM id.
    auto start = st(vm_id);
    auto end = ft(vm_id);

    DLOG(INFO) << "Selected VM id: " << vm_id;

    // Store the ending time
    end_time[activation_id] = end;

    // Initialising the event object
    event.id = activation_id;
    event.start = start;
    event.end = end;

    auto pair_of_vector_of_events = orders.find(vm_id);

    // Assign activation n_i to VM P_j that minimizes the Earliest Execution Finish Time of activation n_i
    pair_of_vector_of_events->second.push_back(event);

    // Sort the orders of events
    sort(pair_of_vector_of_events->second.begin(), pair_of_vector_of_events->second.end(),
         [&](const Event &event_a, const Event &event_b) {
             return event_a.start < event_b.start;
         });

    activation_allocation[activation_id] = vm_id;
}

// Get the next task based on the start time and remove the task
// if there is no task, return -1
/**
 *
 *
 * @param orders
 * @return
 */
static size_t GetNextTask(std::map<size_t, std::vector<Event>> &orders) {
    auto min_start_time = std::numeric_limits<double>::max();
    auto activation_id = std::numeric_limits<size_t>::max();
    auto vm_id = std::numeric_limits<size_t>::max();
    for (auto info: orders) {
        if (!info.second.empty()) {
            if (info.second.begin()->start < min_start_time) {
                min_start_time = info.second.begin()->start;
                activation_id = info.second.begin()->id;
                vm_id = info.first;
            }
        }
    }

    if (activation_id != std::numeric_limits<size_t>::max()) {
        orders.find(vm_id)->second.erase(orders.find(vm_id)->second.begin());
    }

    return activation_id;
}

/**
 * Refs.:
 * 1. https://github.com/VanillaBase1lb/HEFT/blob/main/heft.cpp
 */
void Heft::Run() {
    DLOG(INFO) << "Executing HEFT ...";

    DLOG(INFO) << "Initialising ranks variables";

    // Defining variables

    Solution best_solution(shared_from_this());

    std::map<size_t, std::vector<Event>> activation_order;

    std::vector<Activation> scheduling_list;  // Will be filled with activation IDs.

    std::vector<double> end_time(GetActivationSize(), 0.0);
    std::vector<double> ranku(GetActivationSize(), 0.0);
    std::vector<double> communication_plus_max_successor_rank_cache(GetActivationSize(), -1.0);

    std::vector<size_t> activation_on(GetActivationSize(), -1ul);

    // Initialising variables

    DLOG(INFO) << "Creating a list of VM IDs";
    std::vector<size_t> vm_ids;
    for (const auto &item : virtual_machines_) {
        VirtualMachine vm = *item;
        vm_ids.push_back(vm.get_id());
    }

    DLOG(INFO) << "Build activation_order struct (event_map)";
    for (auto vm_id: vm_ids) {
        activation_order.insert(make_pair(vm_id, std::vector<Event>()));
    }

    DLOG(INFO) << "Building the scheduling_list";
    for (const auto& item: activations_) {
        Activation act = *item;
        scheduling_list.push_back(act);
    }

    // Begin the algorithm

    DLOG(INFO) << "No. of scheduling_list: " << GetActivationSize();
    DLOG(INFO) << "No. of processors: " << GetVirtualMachineSize();

    DLOG(INFO) << "Compute the ranku for all activations by traversing the graph UPWARD, starting from the exit "
                  "activation (reached through recursive function).";
    // TODO: ranku vs rank (comment)
    std::for_each(scheduling_list.begin(), scheduling_list.end(), [&](const Activation &activation) {
        auto activation_id = activation.get_id();
        ranku[activation_id] = RankUpward(activation_id, communication_plus_max_successor_rank_cache);
    });

    DLOG(INFO) << "Sorting the activations in a scheduling list by a non-increasing order of ranku values.";
    // TODO: ranku vs rank (comment)
    std::sort(scheduling_list.begin(), scheduling_list.end(),
              [&](const Activation &activation_i, const Activation &activation_j) {
                  return ranku[activation_i.get_id()] > ranku[activation_j.get_id()];
              });

    DLOG(INFO) << "The upward rank values:";
    for (const auto &activation : scheduling_list) {
        auto index = activation.get_id();
        DLOG(INFO) << "Activation " << index << ": " <<  std::fixed << std::setprecision(6) << ranku[index];
    }

    // ### I stopped here!!! ###

    /*
     * TODO: a lot of variables created and populated isolated, maybe initialize everything in the beginning or create
     * more Methods
     */

    DLOG(INFO) << "Allocating scheduling_list";
//    for (auto activation = scheduling_list.begin(); activation != scheduling_list.end(); activation++) {
    // While there are unscheduled activations in the list do:
    for (const auto &activation : scheduling_list) {
        // Select the first activation, n_i, from the list for scheduling.
        Allocate(activation.get_id(), activation_on, vm_ids, activation_order, end_time);
    }

    // build allocation
    DLOG(INFO) << "Fulfilling the solution object";
    // TODO: Allocate directly into the solution object instead of translating one solution into another
    for (const auto &pair_key_event : activation_order) {
        auto vm_id = pair_key_event.first;
        auto events = pair_key_event.second;

        for (auto event : events) {
            auto activation = activations_[event.id];

            DLOG(INFO) << "Allocating [" << activation->get_name() << "], [" << activation->get_id() << "], VM[" << vm_id << "]";
            auto vm = virtual_machines_[vm_id];
            best_solution.AllocateTask(activation, vm);

            // Update output files;
            DLOG(INFO) << "Allocating files";
            for (const auto& out: activation->get_output_files()) {
                best_solution.SetFileAllocation(out->get_id(), vm_id);
            }
        }
    }

    // Build ordering
    DLOG(INFO) << "Clear ordering";
    best_solution.ClearOrdering();

    // Add root
    best_solution.AddOrdering(this->get_id_source());

    DLOG(INFO) << "Add ordering";
    auto activation_id = GetNextTask(activation_order);

    do {
        if (activation_id != get_id_source() && activation_id != get_id_target()) {
            best_solution.AddOrdering(activation_id);
        }
        activation_id = GetNextTask(activation_order);
    } while (activation_id != std::numeric_limits<size_t>::max());

    // add sink
    best_solution.AddOrdering(get_id_target());

    DLOG(INFO) << "Compute Objective Function";
    best_solution.ComputeObjectiveFunction();

    double time_s = ((double) clock() - (double) t_start) / CLOCKS_PER_SEC;  // Processing time

    std::cout << std::fixed
              << best_solution.get_objective_value()
              << " " << best_solution.get_makespan()
              << " " << best_solution.get_cost()
              << " " << best_solution.get_security_exposure() / get_maximum_security_and_privacy_exposure()
              << " " << time_s
              << " 1"
              << " 1"
              << " " << time_s
              << std::endl;

    DLOG(INFO) << "... ending HEFT";
    // TODO: Printing the scheduling
    // TODO: See the warning messages
}
