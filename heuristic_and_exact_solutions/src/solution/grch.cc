/**
 * \file src/solution/greedy_randomized_constructive_heuristic.cc
 * \brief Contains the \c Algorithm class methods for the Greed Randomized Constructive Heuristic
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo.raps@gmail.com\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2024
 *
 * This source file contains the methods for the \c Grch class that run the Greed Randomized Constructive Heuristic
 * methods
 */

#include "src/solution/grch.h"

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
Solution Grch::ScheduleAvailTasks(std::vector<std::shared_ptr<Activation>> avail_activations, Solution &solution) {
    DLOG(INFO) << "Scheduling the availed activations to the solution ...";
    Solution best_solution = solution;

    // As long as there are allocations to be allocated, do
    while (!avail_activations.empty()) {
        std::list<std::pair<std::shared_ptr<Activation>, Solution>> avail_solutions;

        // 1. Computing time
        for (const auto &activation: avail_activations) {
            // The solution with the best O.F. after choosing a specific VM
            Solution best_temporary_solution(shared_from_this());

            // Begin with a BIG O.F.
            auto best_of = std::numeric_limits<double>::max();
            auto best_vm = virtual_machines_[0ul];

            // Compute the O.F. in each Vm
            for (auto &vm: virtual_machines_) {
                auto new_solution = best_solution;
                auto of = new_solution.ScheduleActivation(activation, vm);

                // Select the best VM
                if ((of < best_of)
                    || (of == best_of
                        && vm->get_cost() < best_vm->get_cost())
                    || (of == best_of
                        && vm->get_cost() == best_vm->get_cost()
                        && vm->get_slowdown() < best_vm->get_slowdown())) {
                    best_of = of;
//                    best_vm = virtual_machines_[vm->get_id()];
                    best_vm = vm;
                    best_temporary_solution = new_solution;
                }
            }

            // Put the best current solution in the list
            avail_solutions.emplace_back(activation, best_temporary_solution);
        }

        // Sorting elements
        avail_solutions.sort([&](const std::pair<std::shared_ptr<Activation>, Solution> &a,
                                      const std::pair<std::shared_ptr<Activation>, Solution> &b) {
            return a.second.get_objective_value() < b.second.get_objective_value();
        });
        auto sol_size = static_cast<double>(avail_solutions.size());
        auto upper_limit = std::min<size_t>(std::ceil<size_t>(alpha_restrict_candidate_list_ * sol_size), sol_size);

        auto position = 0ul;
        if (!avail_solutions.empty()) {
            position = my_rand<size_t>(0ul, upper_limit);
        }

        auto selected_candidate = std::next(avail_solutions.begin(), static_cast<long>(position));

        best_solution = selected_candidate->second;

        DLOG(INFO) << "Selected Activation from Restrict Candidate List[" << selected_candidate->first->get_id() << "]";
        DLOG(INFO) << "Removing Activation[" << selected_candidate->first->get_id() << "]";

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
void Grch::Run() {
    DLOG(INFO) << "Executing GRCH (Greedy Randomized Constructive Heuristic) ...";
    double time_s;
    Solution best_solution(shared_from_this());
    auto number_of_iterations = 0ul;
    auto best_solution_iteration = 0ul;
    double best_solution_time;
    for (auto i = 0ul; i < FLAGS_number_of_iteration; ++i) {
        std::vector<std::shared_ptr<Activation>> activation_list;
        std::vector<std::shared_ptr<Activation>> avail_activations;

        Solution solution(shared_from_this());

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

        // The activation_list is sorted by the height(t). While activation_list is not empty do
        DLOG(INFO) << "Doing scheduling";
        while (!activation_list.empty()) {
            auto task = activation_list.front();  // Get the first task

            /*
             *  Gets the next available activations of the same height and shuffles them.
             */

            avail_activations.clear();
            while (!activation_list.empty()
                   && height_[task->get_id()] == height_[activation_list.front()->get_id()]) {
                // build list of ready tasks, that is the tasks which the predecessor was finish
                DLOG(INFO) << "Putting " << activation_list.front()->get_id() << " in avail_activations";
                avail_activations.push_back(activation_list.front());
                activation_list.erase(activation_list.begin());
            }

            DLOG(INFO) << "Shuffling activation list";
            std::shuffle(avail_activations.begin(), avail_activations.end(), generator());

            // Schedule the ready tasks (same height)
            solution = ScheduleAvailTasks(avail_activations, solution);
        }

        DLOG(INFO) << "Scheduling done";
        DLOG(INFO) << solution;
        time_s = ((double) clock() - (double) t_start) / CLOCKS_PER_SEC;  // Processing time
        number_of_iterations++;
        if (solution.get_objective_value() < best_solution.get_objective_value()) {
            best_solution = solution;
            best_solution_iteration = number_of_iterations;
            best_solution_time = time_s;
        }
    }
    
    std::cout << std::fixed
              << best_solution.get_objective_value()
              << " " << best_solution.get_makespan()
              << " " << best_solution.get_cost()
              << " " << best_solution.get_security_exposure() / get_maximum_security_and_privacy_exposure()
              << " " << time_s
              << " " << number_of_iterations
              << " " << best_solution_iteration
              << " " << best_solution_time
              << std::endl;

    DLOG(INFO) << "... ending GRCH (Greedy Randomized Constructive Heuristic)";
}
