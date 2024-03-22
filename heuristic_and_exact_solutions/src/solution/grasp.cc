/**
 * \file src/solution/grasp.cc
 * \brief Contains the \c Algorithm class methods
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo.raps@gmail.com\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2024
 *
 * This source file contains the methods from the \c Grasp class
 * that run the mode the approximate solution
 */

#include "src/solution/grasp.h"

DECLARE_uint64(number_of_iteration);

void Grasp::localSearch(Solution &solution) {
    DLOG(INFO) << "Executing localSearch ...";
    bool proceed = true;
    while (proceed) {
        double time_s = ((double) clock() - (double) t_start) / CLOCKS_PER_SEC;  // Processing time;
        proceed = solution.localSearchN3();
        double time_f = ((double) clock() - (double) t_start) / CLOCKS_PER_SEC;  // Processing time;
        auto elapsed_time = time_f - time_s;
        lsn_time_1 += elapsed_time;
        if (!proceed) {
            time_s = ((double) clock() - (double) t_start) / CLOCKS_PER_SEC;  // Processing time;
            proceed = solution.localSearchN1();
            time_f = ((double) clock() - (double) t_start) / CLOCKS_PER_SEC;  // Processing time;
            elapsed_time = time_f - time_s;
            lsn_time_2 += elapsed_time;
        } else {
            lsn_noi_1++;
            continue;
        }
        if (!proceed) {
            time_s = ((double) clock() - (double) t_start) / CLOCKS_PER_SEC;    // Processing time;
            proceed = solution.localSearchN2();
            time_f = ((double) clock() - (double) t_start) / CLOCKS_PER_SEC;    // Processing time;
            elapsed_time = time_f - time_s;
            lsn_time_3 += elapsed_time;
        } else {
            lsn_noi_2++;
        }
        if (proceed) {
            lsn_noi_3++;
        }
    }
    DLOG(INFO) << "... ending localSearch";
}

// https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
//template<typename ... Args>
//std::string string_format( const std::string& format, Args ... args )
//{
//  size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
//  if( size <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
//  std::unique_ptr<char[]> buf( new char[ size ] );
//  snprintf( buf.get(), size, format.c_str(), args ... );
//  return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
//}
/**
 * Print out.
 *
 *      <makespan> <cost> <security_exposure> <O.F.>
 *      <time_in_seconds>
 *
 * Where,
 * <makespan> -
 * <cost> -
 * <security_exposure> -
 * <O.F.> -
 * <time_in_seconds> -
 */
void Grasp::Run() {
    DLOG(INFO) << "Executing GRASP Heuristic ...";
    double time_s;
    Solution best_solution(this);
    auto max_iter_without_improve = 10ul;
    auto iter_without_improve = 1ul;
    auto number_of_iterations = 0ul;
    auto best_solution_iteration = 0ul;
    double best_solution_time;
    auto baseline = best_solution.get_objective_value();
    for (auto o = 0ul; o < std::numeric_limits<size_t>::max(); ++o) {
        // 1. Construction phase (GreedyRandomizedAlgorithm)
        std::vector<std::shared_ptr<Activation>> activation_list;
        std::vector<std::shared_ptr<Activation>> avail_activations;
        Solution solution(this);
        // Start activation list
        DLOG(INFO) << "Initialize activations list";
        for (const auto &activations: activations_) {
            activation_list.push_back(activations);
        }
        // Order by height
        DLOG(INFO) << "Order by height";
        std::sort(activation_list.begin(), activation_list.end(),
                  [&](const std::shared_ptr<Activation> &a, const std::shared_ptr<Activation> &b) {
                      return height_[a->get_id()] < height_[b->get_id()];
                  });
        // The activation_list is sorted by the height(t). While activation_list is not empty do
        DLOG(INFO) << "Doing scheduling";
        while (!activation_list.empty()) {
            auto task = activation_list.front();  // Get the first activation
            avail_activations.clear();
            while (!activation_list.empty()
                   && height_[task->get_id()] == height_[activation_list.front()->get_id()]) {
                // Build list of ready tasks, that is the tasks which the predecessor was finish
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
        // 2. S ← LocalSearch(S);
        localSearch(solution);
        // Store the best solution
        if (best_solution.get_objective_value() > solution.get_objective_value()) {
            best_solution = solution;
            time_s = ((double) clock() - (double) t_start) / CLOCKS_PER_SEC;    // Processing time
            tttFile.writeLine(best_solution.get_objective_value(),
                              static_cast<size_t>(best_solution.get_makespan()),
                              best_solution.get_cost(),
                              best_solution.get_security_exposure(),
                              time_s);
        }
        double new_of = best_solution.get_objective_value();
        time_s = ((double) clock() - (double) t_start) / CLOCKS_PER_SEC;    // Processing time
        number_of_iterations++;
        if (new_of < baseline) {
            iter_without_improve = 1ul;
            baseline = new_of;
            best_solution_iteration = number_of_iterations;
            best_solution_time = time_s;
        } else {
            iter_without_improve += 1ul;
        }
        if (iter_without_improve > max_iter_without_improve || (time_s > best_solution.get_makespan() * 0.1)) {
            break;
        }
    }
#ifndef NDEBUG
    best_solution.MemoryAllocation();
    best_solution.ComputeObjectiveFunction();
    DLOG(INFO) << best_solution;
    best_solution.FreeingMemoryAllocated();
#endif
    std::cout << std::fixed
            << best_solution.get_objective_value()
            << " " << best_solution.get_makespan()
            << " " << best_solution.get_cost()
            << " " << best_solution.get_security_exposure() / get_maximum_security_and_privacy_exposure()
            << " " << time_s
            << " " << number_of_iterations
            << " " << best_solution_iteration
            << " " << best_solution_time
            << " " << lsn_time_1
            << " " << lsn_noi_1
            << " " << lsn_time_2
            << " " << lsn_noi_2
            << " " << lsn_time_3
            << " " << lsn_noi_3
            << std::endl;
    DLOG(INFO) << "... ending GRASP";
}
