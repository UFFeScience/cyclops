/**
 * \file src/solution/grch.cc
 * \brief Contains the \c Configuration class methods for the Greed Randomized Constructive Heuristic
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo.raps@gmail.com\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 *
 * This source file contains the methods for the \c Grch class that start the Greed Randomized Constructive Heuristic
 * methods
 */

#include <iomanip>
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
void Grch::ScheduleAvailTasks(std::vector<std::shared_ptr<Activation>> avail_activations, Solution& solution) {
    DLOG(INFO) << "Scheduling the availed activations to the solution ...";
//    Solution best_solution{std::move(solution)};
//    Solution best_solution{solution};

    // As long as there are allocations to be allocated, do
    while (!avail_activations.empty()) {
        std::list<std::pair<std::shared_ptr<Activation>, Solution>> avail_solutions;

        // 1. Computing time
        for (const auto& activation : avail_activations) {
            // The solution with the best O.F. after choosing a specific VM
            Solution best_temporary_solution(get_configuration());

            // Begin with a BIG O.F.
            auto best_of = std::numeric_limits<double>::max();
            auto virtual_machines = GetVirtualMachines();
            // TODO: I think that the initialization bellow is not needed
            auto best_vm = virtual_machines[0ul];

            // TODO: Return the virtual_machines iterator instead of the object
            // Compute the O.F. in each Vm
            for (auto& vm : virtual_machines) {
//                auto new_solution = best_solution;
                auto new_solution = solution;
                auto of = new_solution.ScheduleActivation(activation, vm);

                // Select the best VM
                if ((of < best_of)
                    || (of == best_of
                        && vm->get_cost() < best_vm->get_cost())
                    || (of == best_of
                        && vm->get_cost() == best_vm->get_cost()
                        && vm->get_slowdown() < best_vm->get_slowdown())) {
                    best_of = of;
                    best_vm = vm;
                    best_temporary_solution = new_solution;
                }
            }

            // Put the best current solution in the list
            avail_solutions.emplace_back(activation, best_temporary_solution);
        }

        if (!avail_solutions.empty()) {
            // Sorting elements
            avail_solutions.sort([&](const std::pair<std::shared_ptr<Activation>, Solution>& a,
                                     const std::pair<std::shared_ptr<Activation>, Solution>& b) {
                return a.second.get_objective_value() < b.second.get_objective_value();
            });
            auto sol_size = static_cast<double>(avail_solutions.size());
            auto upper_limit = std::min<size_t>(
                    std::ceil(sol_size * GetAlphaRestrictCandidateList()),
                    avail_solutions.size()) - 1ul;
            auto position = my_rand<size_t>(0ul, upper_limit);
            if (position >= avail_solutions.size()) LOG(FATAL)
                    << "Position can not be greater then or equal to avail_solutions size";
            auto selected_candidate = std::next(avail_solutions.begin(), static_cast<long>(position));
            solution = selected_candidate->second;
#ifndef NDEBUG
            DLOG(INFO) << "Selected Activation from Restrict Candidate List[" << selected_candidate->first->get_id()
                       << "]";
            DLOG(INFO) << "Removing Activation[" << selected_candidate->first->get_id() << "]";
#endif
            // Remove task scheduled
            auto my_pos = std::find(avail_activations.begin(), avail_activations.end(),
                                    selected_candidate->first);
            avail_activations.erase(my_pos);
        } else {
            LOG(FATAL) << "Something is strange";
        }
    }

    DLOG(INFO) << "... availed activations scheduled";
//    return best_solution;
//    solution = best_solution;
}

/**
 *
 */
void Grch::Run() {
    std::cout << "Begin RUN: " << std::endl;
    DLOG(INFO) << "Executing GRCH (Greedy Randomized Constructive Heuristic) ...";
    double time_s;
    Solution best_solution(get_configuration());
//    auto max_iter_without_improve = 10ul;
//    auto iter_without_improve = 1ul;
//    auto number_of_iterations = 0ul;
    auto max_iter = 100ul;
    auto best_solution_iteration = 0ul;
    double best_solution_time;
//    for (auto i = 0ul; i < std::numeric_limits<size_t>::max(); ++i) {
    for (auto i = 0ul; i < max_iter; ++i) {
        std::vector<std::shared_ptr<Activation>> activation_list;
        std::vector<std::shared_ptr<Activation>> avail_activations;
        Solution solution(get_configuration());

        // Start task list
        DLOG(INFO) << "Initialize activation list";
        for (const auto& activation : GetActivations()) {
#ifndef NDEBUG
            DLOG(INFO) << "Inserting activation " << activation->get_id();
#endif
            activation_list.push_back(activation);
        }

        // Order by height
        DLOG(INFO) << "Order by height";
        auto heights = GetHeights();
        std::sort(activation_list.begin(), activation_list.end(),
                  [&](const std::shared_ptr<Activation> &a, const std::shared_ptr<Activation> &b) {
            return heights[a->get_id()] < heights[b->get_id()];
        });

        // The activation_list is sorted by the height(t). While activation_list is not empty do
        DLOG(INFO) << "Doing scheduling";
        while (!activation_list.empty()) {
            auto activation = activation_list.front();  // Get the first activation

            /*
             *  Gets the next available activations of the same height and shuffles them.
             */
            avail_activations.clear();
            while (!activation_list.empty()
                   && heights[activation->get_id()] == heights[activation_list.front()->get_id()]) {
                // build list of ready tasks, that is the tasks which the predecessor was finish
#ifndef NDEBUG
                DLOG(INFO) << "Putting " << activation_list.front()->get_id() << " in avail_activations";
#endif
                avail_activations.push_back(activation_list.front());
                activation_list.erase(activation_list.begin());
            }

            DLOG(INFO) << "Shuffling activation list";
            std::shuffle(avail_activations.begin(), avail_activations.end(), generator());

            // Schedule the ready tasks (same height)
            ScheduleAvailTasks(avail_activations, solution);
        }

        DLOG(INFO) << "Scheduling done";
        DLOG(INFO) << solution;
        time_s = ((double) clock() - (double) t_start) / CLOCKS_PER_SEC;  // Processing time
//        number_of_iterations++;
        if (solution.get_objective_value() < best_solution.get_objective_value()) {
//            iter_without_improve = 1ul;
            best_solution = solution;
//            best_solution_iteration = number_of_iterations;
            best_solution_iteration = i;
            best_solution_time = time_s;
        }
//        else {
//            iter_without_improve += 1ul;
//        }
        
//        if ((iter_without_improve > max_iter_without_improve) || (time_s > best_solution.get_makespan() * 0.1)) {
//            break;
//        }
    }

    best_solution.OptimizedComputeObjectiveFunction();
    
    LOG(INFO) << best_solution;

    std::cout << std::fixed << std::setprecision(6)
              << best_solution.get_objective_value()
              << " " << best_solution.get_makespan()
              << " " << best_solution.get_cost()
              << " " << best_solution.get_security_exposure() / GetMaximumConfidentialityExposure()
              << " " << time_s
              << " " << max_iter
              << " " << best_solution_iteration
              << " " << best_solution_time
              << std::endl;

    DLOG(INFO) << "... ending GRCH (Greedy Randomized Constructive Heuristic)";
}
