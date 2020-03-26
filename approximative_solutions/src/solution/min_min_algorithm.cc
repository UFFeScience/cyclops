/**
 * \file src/exec_manager/execution.cc
 * \brief Contains the \c Algorithm class methods.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \date 2020
 *
 * This source file contains the methods from the \c Algorithm class that run the mode the
 * approximate solution.
 */

#include <list>
#include <vector>
#include <limits>
#include <cmath>
#include <memory>
#include <glog/logging.h>

#include "src/solution/min_min_algorithm.h"
#include "src/model/static_file.h"

/**
 * If file reside in the same vm, then return 0.0.
 * Otherwise, calculate the transfer time between them using the smallest bandwidth.
 *
 *   \f[
 *      time = \lceil{\min_{vm.bandwidth(), file\_vm.bandwidth()} bandwidth}\rceil
 *   \f]
 *
 * Where:
 * \f$ time \f$ is the transfer time between the \c vm and \c file_vm
 * \f$ vm.bandwidth() \f$ is the transfer rate of the \c vm
 * \f$ file\_vm.bandwidth() \f$ is the transfer rate of the \c file_vm
 * \f$ bandwidth \f$ is the minimal transfer rate between the vm.bandwidth() and file\_vm.bandwidth()
 *
 * \param[in]  file      File to be transfered
 * \param[in]  vm        VM where the file will be transferred
 * \param[in]  file_vm   VM where the file is
 * \retval     time      The time to transfer \c file from \c file_vm to \c vm
 */
inline double FileTransferTime(File* file, VirtualMachine vm, VirtualMachine file_vm) {
  double time = 0.0;

  if (vm.get_id() != file_vm.get_id()) {
    auto link = std::min(vm.get_bandwidth(), file_vm.get_bandwidth());
    time = std::ceil(file->get_size() / link);
  }

  return time;
}  // inline double FileTransferTime(File* file, VirtualMachine vm, VirtualMachine file_vm) {

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
 * \f$ time \f$ is the maximum finish time of all previous task of the task identified by \c task_id
 * \f$ fitness_{previous_task_id}} is the finish time of the previous task
 *
 * \param[in]  task_id          Task id for which we want to find the start time
 * \param[in]  vm               VM where the task will be executed
 * \param[in]  fittness_vector  A vector containing fitness of all tasks
 * \param[in]  queue            A vector containing fitness time on the VMs
 * \retval     time             The time at the task identified by \c task_id will start executing
 */
inline double MinMinAlgorithm::ComputeStartTime(size_t task_id,
                                                size_t vm_id,
                                                std::vector<double> fitness_vector,
                                                std::vector<double> queue) {
  double time = 0.0;

  for (auto previous_task_id : prec_.find(task_id)->second) {
    time = std::max(time, fitness_vector[previous_task_id]);
  }

  return std::max(time, queue[vm_id]);
}

/**
 * Calculates:
 * 1. start_time
 * 2. read_time
 * 3. write_time
 * And then, return the sum of them.
 *
 * \param[in]  task                    Task for which we want to find the fitness
 * \param[in]  vm                      VM where the task will be executed
 * \param[in]  fittness_vector         A vector containing fitness of all tasks
 * \param[in]  queue                   A vector containing fitness time on the VMs
 * \param[in]  allocation              A vector containing the places (VMs) of all tasks and files
 * \param[in]  lambda                  A multiplier to adjust the reading and writing times of the input and output files
 * \retval     time                    The time at the task identified by \c task_id will start executing
 */
inline double MinMinAlgorithm::ComputeFitness(Task task,
                                              VirtualMachine vm,
                                              std::vector<double>& fitness_vector,
                                              std::vector<double>& queue,
                                              std::vector<size_t>& allocation,
                                              double lambda) {
  double start_time = 0;
  double read_time = 0;
  double write_time = 0;

  if (task.get_id() != id_source_ && task.get_id() != id_target_) {

    // Compute Start Time
    start_time = ComputeStartTime(task.get_id(), vm.get_id(), fitness_vector, queue);

    // Read time;
    for (File* file : task.get_input_files()) {
      size_t vm_id = file->IsStatic() ? static_cast<StaticFile*>(file)->GetFirstVm()
                                      : allocation[file->get_id()];
      VirtualMachine file_vm = vm_map_.find(vm_id)->second;

      // Ceil of File Transfer Time + File Size * lambda
      read_time += std::ceil(FileTransferTime(file, vm, file_vm) + (file->get_size() * lambda));
    }

    // Write time
    for (auto& file : task.get_output_files()) {
      write_time += ceil(file->get_size() * (2 * lambda));
    }
  } else if (task.get_id() == id_target_) {
    for (auto tk : prec_.find(task.get_id())->second) {
      start_time = std::max(start_time, fitness_vector[tk]);
    }
  }

  auto run_time = ceil(task.get_time() * vm.get_slowdown());

  // cout << "task: " << data->idToString(task) << " vm: " << vm << endl;
  //cout << "start time: " << start_time << " read time: " << read_time << " Run time: " << run_time << " write_time: " << write_time << endl;

  return start_time + read_time + run_time + write_time;
}

/**
 * Do the scheduling
 *
 * \param[in]  avail_tasks            Avail tasks to be processed
 * \param[in]  fitness_vector         A vector containing fitness of all tasks
 * \param[in]  queue                  A vector containing fitness time on the VMs
 * \param[in]  allocation             A vector containing the places (VMs) of all tasks and files
 * \param[in]  task_ordering          The linear order of the tasks
 * \param[in]  lambda                 A multiplier to adjust the reading and writing times of the input and output files
 */
void MinMinAlgorithm::schedule(std::list<Task> avail_tasks,
                               std::vector<double>& fitness_vector,
                               std::vector<double>& queue,
                               std::vector<size_t>& allocation,
                               std::list<size_t>& task_ordering,
                               double lambda) {
  while (!avail_tasks.empty()) {
    double iteration_minimal_time = std::numeric_limits<double>::max();
    size_t iteration_minimal_vm_id;
    Task& iteration_minimal_task = task_map_per_id_.find(0)->second;

    // 1. Compute time phase
    for (auto task : avail_tasks) {
      // Compute the finish time off all tasks in each Vm
      double min_time = std::numeric_limits<double>::max();
      size_t min_vm_id = 0;

      for (std::pair<size_t, VirtualMachine> pair : vm_map_) {
        VirtualMachine& vm = pair.second;
        double time = ComputeFitness(task, vm, fitness_vector, queue, allocation, lambda);
        VirtualMachine& min_vm = vm_map_.find(min_vm_id)->second;

        // It is necessary to determine another information, when it is draw
        // when it is equal, select the cheapest one.
        if (time < min_time) { // Get minimum time and minimum vm
          min_time = time;
          min_vm_id = vm.get_id();
        } else if (time == min_time && vm.get_cost() < min_vm.get_cost()) {
          min_time = time;
          min_vm_id = vm.get_id();
        } else if (time == min_time
            && vm.get_cost() == min_vm.get_cost()
            && vm.get_slowdown() < min_vm.get_slowdown()) {
          min_time = time;
          min_vm_id = vm.get_id();
        }
      }

      if (iteration_minimal_time > min_time) {
        iteration_minimal_task = task;
        iteration_minimal_time = min_time;
        iteration_minimal_vm_id = min_vm_id;
      }
    }

    // Schedule the Global Minimal Task

    // Update auxiliary structures (queue and ft_vector)
    fitness_vector[iteration_minimal_task.get_id()] = iteration_minimal_time;
    queue[iteration_minimal_vm_id] = iteration_minimal_time;

    // The two structure that make up the solution
    task_ordering.push_back(iteration_minimal_task.get_id());
    allocation[iteration_minimal_task.get_id()] = iteration_minimal_vm_id;

    // Update file_place
    for (auto file : iteration_minimal_task.get_output_files()) {
      allocation[file->get_id()] = iteration_minimal_vm_id;
    }

    avail_tasks.remove(iteration_minimal_task);  // Remove task scheduled
  }  // while (!avail_tasks.empty()) {
}  // void MinMinAlgorithm::schedule(...)

/**
 * This method executes the algorithm
 */
void MinMinAlgorithm::Run() {
  DLOG(INFO) << "Executing MinMin algorithm...";
  // google::FlushLogFiles(google::INFO);

  std::list<Task> task_list;

  std::list<Task> avail_tasks;

  std::vector<double> ft_vector(size_, 0.0);
  std::vector<double> queue(vm_size_, 0.0);

  std::vector<size_t> allocation(size_, 0ul);
  std::list<size_t> task_ordering(0ul);

  Solution solution(this);

  // Start task list
  DLOG(INFO) << "Initialize task list";
  for (auto task : task_map_per_id_) {
    task_list.push_back(task.second);
  }

  // Order by height
  DLOG(INFO) << "Order by height";
  task_list.sort([&](const Task& a, const Task& b) {
    return height_[a.get_id()] < height_[b.get_id()];
  });

  // The task_list is sorted by the height(t). While task_list is not empty do
  DLOG(INFO) << "Doing scheduling";
  google::FlushLogFiles(google::INFO);
  while (!task_list.empty()) {
    auto task = task_list.front();  // Get the first task
    DLOG(INFO) << "First task: " << task.get_id();
    avail_tasks.clear();
    while (!task_list.empty()
        && height_[task.get_id()] == height_[task_list.front().get_id()]) {
      // build list of ready tasks, that is the tasks which the predecessor was finish
      avail_tasks.push_back(task_list.front());
      DLOG(INFO) << "Putting " << task_list.front().get_id() << " in avail_tasks";
      task_list.pop_front();
    }
    // Schedule the ready tasks
    schedule(avail_tasks, ft_vector, queue, allocation, task_ordering, lambda_);
  }

  DLOG(INFO) << "Scheduling done";
  // google::FlushLogFiles(google::INFO);

  for (size_t i = 0; i < size_; i++) {
    solution.allocation[i] = allocation[i];
  }

  solution.ordering.clear();
  solution.ordering.insert(solution.ordering.end(),
                           task_ordering.begin(),
                           task_ordering.end());

  solution.ComputeFitness(true, true);

  LOG(INFO) << "MinMIn fitness: " << solution.fitness << " seconds";
  LOG(INFO) << "MinMin fitness: " << solution.fitness / 60.0 << " minutes";
  LOG(INFO) << solution;
  solution.print();
  DLOG(INFO) << "... ending MinMin algorithm";
}  // end of MinMin::run() method

MinMinAlgorithm::~MinMinAlgorithm() {

}
