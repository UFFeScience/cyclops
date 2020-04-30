/**
 * \file src/solution/greedy_ramdomized_constructive_heuristic.cc
 * \brief Contains the \c Algorithm class methods.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2020
 *
 * This source file contains the methods from the \c GreedyRandomizedConstructiveHeuristic class
 * that run the mode the approximate solution.
 */

#include "src/solution/greedy_randomized_constructive_heuristic.h"

#include <gflags/gflags.h>
#include <glog/logging.h>

#include <list>
#include <vector>
#include <limits>
#include <cmath>
#include <memory>
#include <ctime>
#include <utility>

#include "src/model/static_file.h"
#include "src/model/storage.h"

DECLARE_uint64(number_of_iteration);

// std::random_device rd_chr;
// std::mt19937 engine_chr(rd_chr());

/**
 * Do the scheduling
 *
 * Use allocates the availed task into \c allocation_ and store the execution ordering into
 * \c ordering_.
 *
 * \param[in]  avail_tasks     Avail tasks to be processed
 * \param[in]  solution        The solution to be built
 */
void GreedyRandomizedConstructiveHeuristic::ScheduleAvailTasks(std::list<Task*> avail_tasks,
                                                               Solution& solution) {
  while (!avail_tasks.empty()) {
    double total_minimal_objective_value = std::numeric_limits<double>::max();
    double total_maximum_objective_value = 0.0;

    std::list<std::pair<Task*, Solution>> avail_solutions;

    // 1. Compute time phase
    for (auto task : avail_tasks) {
      Solution best_solution = solution;

      // Compute the finish time off all tasks in each Vm
      double task_minimal_objective_value = std::numeric_limits<double>::max();
      size_t min_vm_id = 0;

      for (VirtualMachine* vm : virtual_machines_) {
        Solution new_solution = solution;

        double objective_value = new_solution.ScheduleTask(task, vm);

        VirtualMachine* min_vm = virtual_machines_[min_vm_id];

        if (objective_value < task_minimal_objective_value) {
          task_minimal_objective_value = objective_value;
          min_vm_id = vm->get_id();
          best_solution = new_solution;
        } else if (objective_value == task_minimal_objective_value
            && vm->get_cost() < min_vm->get_cost()) {
          task_minimal_objective_value = objective_value;
          min_vm_id = vm->get_id();
          best_solution = new_solution;
        } else if (objective_value == task_minimal_objective_value
            && vm->get_cost() == min_vm->get_cost()
            && vm->get_slowdown() < min_vm->get_slowdown()) {
          task_minimal_objective_value = objective_value;
          min_vm_id = vm->get_id();
          best_solution = new_solution;
        }
      }  // for (std::pair<size_t, VirtualMachine> pair : vm_map_) {

      if (task_minimal_objective_value > total_maximum_objective_value) {
        total_maximum_objective_value = task_minimal_objective_value;
      }

      if (task_minimal_objective_value < total_minimal_objective_value) {
        total_minimal_objective_value = task_minimal_objective_value;
      }

      avail_solutions.push_back(std::make_pair(task, best_solution));
    }  // for (auto task : avail_tasks) {

    std::list<std::pair<Task*, Solution>> retricted_candidate_list;

    for (std::pair<Task*, Solution> candidate_pair : avail_solutions) {
      if (candidate_pair.second.get_objective_value()
          <= total_minimal_objective_value + (alpha_restrict_candidate_list_
                                              * (total_maximum_objective_value
                                                 - total_minimal_objective_value))) {
        retricted_candidate_list.push_back(candidate_pair);
      }
    }

    retricted_candidate_list.sort([&](const std::pair<Task*, Solution>& a,
                                      const std::pair<Task*, Solution>& b) {
      return a.second.get_objective_value() < b.second.get_objective_value();
    });

    size_t position = static_cast<size_t>(rand()) % retricted_candidate_list.size();

    std::list<std::pair<Task*, Solution>>::iterator selected_candidate =
        std::next(retricted_candidate_list.begin(), static_cast<unsigned int>(position));

    solution = selected_candidate->second;

    DLOG(INFO) << "Selected Task from Restrict Candidate List[" << selected_candidate->first << "]";
    DLOG(INFO) << "Removing Task[" << selected_candidate->first << "]";

    avail_tasks.remove(selected_candidate->first);  // Remove task scheduled
  }  // while (!avail_tasks.empty()) {
}  // void GreedyRandomizedConstructiveHeuristic::schedule(...)

void GreedyRandomizedConstructiveHeuristic::Run() {
  DLOG(INFO) << "Executing Greedy Randomized Constructive Heuristic ...";
  // google::FlushLogFiles(google::INFO);

  // std::srand(unsigned(std::time(0)));

  Solution best_solution(this);

  for (size_t i = 0; i < FLAGS_number_of_iteration; ++i) {
    std::list<Task*> task_list;
    std::list<Task*> avail_tasks;

    Solution solution(this);

    // Initialize the allocation with the static files place information (VM or Bucket)
    for (File* file : files_) {
      if (StaticFile* static_file = dynamic_cast<StaticFile*>(file)) {
        solution.SetFileAllocation(file->get_id(), static_file->GetFirstVm());
      }
    }

    // Start task list
    DLOG(INFO) << "Initialize task list";
    // for (auto task : task_map_per_id_) {
    for (Task* task : tasks_) {
      task_list.push_back(task);
    }

    // Order by height
    DLOG(INFO) << "Order by height";
    task_list.sort([&](const Task* a, const Task* b) {
      return height_[a->get_id()] < height_[b->get_id()];
    });

    // The task_list is sorted by the height(t). While task_list is not empty do
    DLOG(INFO) << "Doing scheduling";
    google::FlushLogFiles(google::INFO);
    while (!task_list.empty()) {
      auto task = task_list.front();  // Get the first task

      avail_tasks.clear();
      while (!task_list.empty()
          && height_[task->get_id()] == height_[task_list.front()->get_id()]) {
        // build list of ready tasks, that is the tasks which the predecessor was finish
        DLOG(INFO) << "Putting " << task_list.front()->get_id() << " in avail_tasks";
        avail_tasks.push_back(task_list.front());
        task_list.pop_front();
      }

      // Schedule the ready tasks
      ScheduleAvailTasks(avail_tasks, solution);
    }

    DLOG(INFO) << "Scheduling done";
    // google::FlushLogFiles(google::INFO);

    // solution.ObjectiveFunction(false, false);

    if (best_solution.get_objective_value() > solution.get_objective_value()) {
      best_solution = solution;
    }

    LOG(INFO) << solution;
  }  // for (size_t i = 0; i < FLAGS_number_of_iteration; ++i) {

  // std::cout << best_solution << std::endl;

  // best_solution.ObjectiveFunction(false, false);

  DLOG(INFO) << best_solution;
  // std::cerr << best_solution;

  // best_solution.ObjectiveFunction(false, false);
  // std::cout << best_solution;

  std::cout << best_solution.get_makespan()
      << " " << best_solution.get_cost()
      << " " << best_solution.get_security_exposure()
      << " " << best_solution.get_objective_value() << std::endl;

  DLOG(INFO) << "... ending Greedy Randomized Constructive Heuristic";
}  // end of GreedyRandomizedConstructiveHeuristic::run() method
