/**
 * \file src/solution/greedy_randomized_constructive_heuristic.cc
 * \brief Contains the \c Algorithm class methods.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2020
 *
 * This source file contains the methods from the \c Heft class
 * that run the mode the approximate solution.
 */

#include "src/solution/grch_hard.h"

#include "src/model/static_file.h"

DECLARE_uint64(number_of_iteration);

/**
 * Do the scheduling
 *
 * Use allocates the availed task into \c allocation_ and store the execution ordering into
 * \c ordering_.
 *
 * \param[in]  avail_activations     Avail tasks to be processed
 * \param[in]  solution        The solution to be built
 */
Solution GrchHard::ScheduleAvailTasks(std::vector<std::shared_ptr<Activation>> avail_activations, Solution &solution) {
    DLOG(INFO) << "Scheduling the availed activations to the solution ...";
    Solution best_solution = solution;
    while (!avail_activations.empty()) {
        double total_minimal_objective_value = std::numeric_limits<double>::max();
        double total_maximum_objective_value = 0.0;

        std::list<std::pair<std::shared_ptr<Activation>, Solution>> avail_solutions;

        // 1. Compute time phase
        for (const auto &activation: avail_activations) {
            // The solution with the best O.F. after choosing a specific VM
            Solution best_selected_vm_solution(this);

            // Compute the finish time off all tasks in each Vm
            // Begin with a BIG O.F.
            double best_actual_objective_value = std::numeric_limits<double>::max();
            std::shared_ptr<VirtualMachine> best_vm = virtual_machines_[0ul];

            // For each VM
            // Select the best VM
            for (const auto &vm: virtual_machines_) {
                Solution new_solution = best_solution;

                double objective_value = new_solution.ScheduleActivation(activation, vm);

                if ((objective_value < best_actual_objective_value)
                    || (objective_value == best_actual_objective_value
                        && vm->get_cost() < best_vm->get_cost())
                    || (objective_value == best_actual_objective_value
                        && vm->get_cost() == best_vm->get_cost()
                        && vm->get_slowdown() < best_vm->get_slowdown())) {
                    best_actual_objective_value = objective_value;
                    best_vm = virtual_machines_[vm->get_id()];
                    best_selected_vm_solution = new_solution;
                }
            }

            if (best_actual_objective_value > total_maximum_objective_value) {
                total_maximum_objective_value = best_actual_objective_value;
            }

            if (best_actual_objective_value < total_minimal_objective_value) {
                total_minimal_objective_value = best_actual_objective_value;
            }

            // Put the best current solution in the list
            avail_solutions.emplace_back(activation, best_selected_vm_solution);
        }

        std::list<std::pair<std::shared_ptr<Activation>, Solution>> restricted_candidate_list;

        for (const auto &candidate_pair: avail_solutions) {
            if (candidate_pair.second.get_objective_value()
                <= total_minimal_objective_value + (alpha_restrict_candidate_list_
                                                    * (total_maximum_objective_value
                                                       - total_minimal_objective_value))) {
                restricted_candidate_list.push_back(candidate_pair);
            }
        }

        restricted_candidate_list.sort([&](const std::pair<std::shared_ptr<Activation>, Solution> &a,
                                           const std::pair<std::shared_ptr<Activation>, Solution> &b) {
            return a.second.get_objective_value() < b.second.get_objective_value();
        });

        auto position = 0ul;
        if (!restricted_candidate_list.empty()) {
            position = my_rand<size_t>(0ul, restricted_candidate_list.size() - 1ul);
        }

        auto selected_candidate = std::next(restricted_candidate_list.begin(),
                                            static_cast<long>(position));

        best_solution = selected_candidate->second;

        DLOG(INFO) << "Selected Activation from Restrict Candidate List[" << selected_candidate->first->get_id() << "]";
        DLOG(INFO) << "Removing Activation[" << selected_candidate->first->get_id() << "]";

//        avail_activations.remove(selected_candidate->first);  // Remove task scheduled
        // Remove task scheduled
        auto my_pos = std::find(avail_activations.begin(), avail_activations.end(), selected_candidate->first);
        avail_activations.erase(my_pos);
    }

    DLOG(INFO) << "... availed activations scheduled";

    return best_solution;
}

/**
 *
 */
void GrchHard::Run() {
    DLOG(INFO) << "Executing GRCH (Greedy Randomized Constructive Heuristic) ...";
    auto rng = std::default_random_engine {};

    Solution best_solution(this);

    for (size_t i = 0; i < FLAGS_number_of_iteration; ++i) {
        std::vector<std::shared_ptr<Activation>> activation_list;
        std::vector<std::shared_ptr<Activation>> avail_activations;

        Solution solution(this);

        // Start task list
        DLOG(INFO) << "Initialize activation list";
        for (const auto &activation: activations_) {
            DLOG(INFO) << "Inserting activation " << activation->get_id();
            activation_list.push_back(activation);
        }

        // Order by height
        DLOG(INFO) << "Order by height";
        std::sort(activation_list.begin(), activation_list.end(),
                  [&](const std::shared_ptr<Activation> &a, const std::shared_ptr<Activation> &b) {
            return height_[a->get_id()] < height_[b->get_id()];
        });
//        activation_list.sort([&](const std::shared_ptr<Activation> &a, const std::shared_ptr<Activation> &b) {
//            return height_[a->get_id()] < height_[b->get_id()];
//        });

#ifndef NDEBUG
        {  // Just for debugging purpose
            size_t index = 0ul;
            for (const auto &activation: activation_list) {
                DLOG(INFO) << ++index << ": activation " << activation->get_id();
            }
        }
#endif

        // The activation_list is sorted by the height(t). While activation_list is not empty do
        DLOG(INFO) << "Doing scheduling";
        while (!activation_list.empty()) {
            auto task = activation_list.front();  // Get the first task

            avail_activations.clear();
            while (!activation_list.empty()
                   && height_[task->get_id()] == height_[activation_list.front()->get_id()]) {
                // build list of ready tasks, that is the tasks which the predecessor was finish
                DLOG(INFO) << "Putting " << activation_list.front()->get_id() << " in avail_activations";
                avail_activations.push_back(activation_list.front());
                activation_list.erase(activation_list.begin());
            }

            DLOG(INFO) << "Shuffling activation list";
            std::shuffle(avail_activations.begin(), avail_activations.end(), rng);

#ifndef NDEBUG
            {  // Just for debugging purpose
                size_t index = 0ul;
                for (const auto &activation: avail_activations) {
                    DLOG(INFO) << ++index << ": activation " << activation->get_id();
                }
            }
#endif

            // Schedule the ready tasks (same height)
            solution = ScheduleAvailTasks(avail_activations, solution);
        }

        DLOG(INFO) << "Scheduling done";

        if (best_solution.get_objective_value() > solution.get_objective_value()) {
            best_solution = solution;
        }

        DLOG(INFO) << solution;
    }

#ifndef NDEBUG
    best_solution.MemoryAllocation();
    best_solution.ComputeObjectiveFunction();
    DLOG(INFO) << best_solution;
//    std::cout << best_solution;
    best_solution.FreeingMemoryAllocated();
#endif

    auto time_s = ((double) clock() - (double) t_start) / CLOCKS_PER_SEC;  // Processing time

    std::cerr << best_solution.get_makespan()
              << " " << best_solution.get_cost()
              << " " << best_solution.get_security_exposure() / get_maximum_security_and_privacy_exposure()
              << " " << best_solution.get_objective_value() << std::endl
              << time_s << std::endl;

    DLOG(INFO) << "... ending GRCH (Greedy Randomized Constructive Heuristic)";
}
