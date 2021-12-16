/**
 * \file src/solution/min_min_algorithm.cc
 * \brief Contains the \c Algorithm class methods.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2021
 *
 * This source file contains the methods from the \c Algorithm class that run the mode the
 * approximate solution.
 */

#include "src/solution/min_min_algorithm.h"

#include "src/model/static_file.h"

/**
 * Do the scheduling
 *
 * Use allocates the availed task into \c allocation_ and store the execution ordering into
 * \c ordering_.
 *
 * \param[in]  avail_tasks     Avail tasks to be processed
 * \param[in]  solution        The solution to be built
 */
void MinMinAlgorithm::ScheduleAvailTasks(std::list<std::shared_ptr<Activation>> avail_tasks, Solution& solution) {
  while (!avail_tasks.empty()) {
    double iteration_minimal_objective_value = std::numeric_limits<double>::max();
    size_t iteration_minimal_vm_id;
    size_t iteration_minimal_task_id;
    Solution best_solution = solution;

    // 1. Compute time phase
    for (const auto& task : avail_tasks) {
      // Compute the finish time off all tasks in each Vm
      double min_objective_function = std::numeric_limits<double>::max();
      size_t min_vm_id = 0;

      for (const auto& vm : virtual_machines_) {
        Solution new_solution = solution;

        double objective_value = new_solution.ScheduleTask(task, vm);

        std::shared_ptr<VirtualMachine> min_vm = virtual_machines_[min_vm_id];

        // It is necessary to determine another information, when it is draw
        // when it is equal, select the cheapest one.
        if ((objective_value < min_objective_function)
            || (objective_value == min_objective_function
                && vm->get_cost() < min_vm->get_cost())
            || (objective_value == min_objective_function
                && vm->get_cost() == min_vm->get_cost()
                && vm->get_slowdown() < min_vm->get_slowdown())) {  // Get minimum OF and minimum VM
          min_objective_function = objective_value;
          min_vm_id = vm->get_id();
        }

        if (iteration_minimal_objective_value > min_objective_function) {
          iteration_minimal_task_id = task->get_id();
          iteration_minimal_objective_value = min_objective_function;
          iteration_minimal_vm_id = min_vm_id;
          best_solution = new_solution;
        }
      }  // for (std::pair<size_t, VirtualMachine> pair : vm_map_) {
    }  // for (auto task : avail_tasks) {

    DLOG(INFO) << "Best Solution: Activation[" << iteration_minimal_task_id
        << "] and VM[" << iteration_minimal_vm_id << "]";

    solution = best_solution;

    DLOG(INFO) << "Removing Activation[" << iteration_minimal_task_id << "]";
    std::shared_ptr<Activation> my_task = tasks_[iteration_minimal_task_id];
    avail_tasks.remove(my_task);  // Remove task scheduled
  }  // while (!avail_tasks.empty()) {
}  // void MinMinAlgorithm::schedule(...)

/**
 * This method executes the algorithm
 */
void MinMinAlgorithm::Run() {
  DLOG(INFO) << "Executing MinMin algorithm...";
  // google::FlushLogFiles(google::INFO);

  std::list<std::shared_ptr<Activation>> task_list;
  std::list<std::shared_ptr<Activation>> avail_tasks;

  Solution solution(this);

  // Initialize the allocation with the static files place information (VM or Bucket)
  for (const std::shared_ptr<File>& file : files_) {
//    if (std::shared_ptr<StaticFile> static_file = std::dynamic_pointer_cast<StaticFile>(file)) {
////      solution.SetFileAllocation(file->get_id(), static_file->GetFirstVm());
//      solution.SetFileAllocation(file->get_id(), static_file->GetFirstVm());
//    }
    solution.SetFileAllocation(file->get_id(), file->GetStorageId());
  }

  // Start task list
  DLOG(INFO) << "Initialize task list";
  for (const auto& task : tasks_) {
    task_list.push_back(task);
  }

  // Order by height
  DLOG(INFO) << "Order by height";
  task_list.sort([&](const std::shared_ptr<Activation>& a, const std::shared_ptr<Activation>& b) {
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

  solution.ObjectiveFunction(false, false);

  LOG(INFO) << solution;
  std::cout << solution;
  DLOG(INFO) << "... ending MinMin algorithm";
}  // end of MinMin::run() method
