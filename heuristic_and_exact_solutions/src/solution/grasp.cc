/**
 * \file src/solution/grasp.cc
 * \brief Contains the \c Algorithm class methods.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2021
 *
 * This source file contains the methods from the \c Grasp class
 * that run the mode the approximate solution.
 */

#include <src/common/my_random.h>
#include "src/solution/grasp.h"

DECLARE_uint64(number_of_iteration);

void Grasp::localSearch(Solution &solution) {
    DLOG(INFO) << "Executing localSearch ...";
    bool proceed = true;
    while (proceed) {
        double time_s = ((double) clock() - (double) t_start) / CLOCKS_PER_SEC;    // Processing time;
        proceed = solution.localSearchN1();
        double time_f = ((double) clock() - (double) t_start) / CLOCKS_PER_SEC;    // Processing time;
        lsFile1.writeLine(time_f - time_s);
        if (!proceed) {
            time_s = ((double) clock() - (double) t_start) / CLOCKS_PER_SEC;    // Processing time;
            proceed = solution.localSearchN2();
            time_f = ((double) clock() - (double) t_start) / CLOCKS_PER_SEC;    // Processing time;
            lsFile2.writeLine(time_f - time_s);
        }
        if (!proceed) {
            time_s = ((double) clock() - (double) t_start) / CLOCKS_PER_SEC;    // Processing time;
            proceed = solution.localSearchN3();
            time_f = ((double) clock() - (double) t_start) / CLOCKS_PER_SEC;    // Processing time;
            lsFile3.writeLine(time_f - time_s);
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
    auto rng = std::default_random_engine {};

    double time_s;

    Solution best_solution(this);

    size_t max_iter = 5ul;
    size_t iter = 0ul;
    double delta = 0.005;
    double baseline = best_solution.get_objective_value();

    for (size_t o = 0; o < std::numeric_limits<size_t>::max(); ++o) {
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
//        activation_list.sort([&](const std::shared_ptr<Activation> &a, const std::shared_ptr<Activation> &b) {
//            return height_[a->get_id()] < height_[b->get_id()];
//        });

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
//                activation_list.pop_front();
                activation_list.erase(activation_list.begin());
            }

            DLOG(INFO) << "Shuffling activation list";
            std::shuffle(avail_activations.begin(), avail_activations.end(), rng);

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
        if (baseline - new_of >= delta) {
            iter = 0ul;
            baseline = new_of;
        } else {
            iter += 1ul;
        }

        if (iter >= max_iter) {
            break;
        }

        time_s = ((double) clock() - (double) t_start) / CLOCKS_PER_SEC;    // Processing time
        iterationFile.writeLine(o, time_s);
    }

    DLOG(INFO) << best_solution;
    std::cout << best_solution;

    time_s = ((double) clock() - (double) t_start) / CLOCKS_PER_SEC;    // Processing time

    tttFile.writeLine(best_solution.get_objective_value(),
                      static_cast<size_t>(best_solution.get_makespan()),
                      best_solution.get_cost(),
                      best_solution.get_security_exposure(),
                      time_s);

    std::cerr << best_solution.get_makespan() << " " << best_solution.get_cost() << " "
              << best_solution.get_security_exposure() / get_maximum_security_and_privacy_exposure() << " "
              << best_solution.get_objective_value() << std::endl << time_s << std::endl;

    DLOG(INFO) << "... ending GRASP";
}
