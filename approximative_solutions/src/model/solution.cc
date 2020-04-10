#include <numeric>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/bimap.hpp>
#include <boost/algorithm/string.hpp>
#include <utility>
#include <unordered_map>
#include "src/model/static_file.h"
#include "src/model/solution.h"

std::random_device rd_chr;
std::mt19937 engine_chr(rd_chr());

Solution::Solution(Algorithm* algorithm)
    : algorithm_(algorithm),
      allocation_(algorithm->get_tasks_plus_files_size(), std::numeric_limits<size_t>::max()),
      // ordering_(algorithm->get_task_size(), std::numeric_limits<size_t>::max()),
      height_soft_(algorithm->get_task_size(), -1),
      time_vector_(algorithm->get_task_size(), 0.0),
      queue_(algorithm->get_virtual_machine_size(), 0.0),
      start_time_vector_(algorithm->get_task_size(), -1),
      makespan_(std::numeric_limits<double>::max()),
      lambda_(algorithm->get_lambda()),
      transfer_size_(0) {
  // ComputeHeightSoft(algorithm->get_id_source());
  // Encode();
  // ComputeMakespan();
}  // Solution::Solution(Algorithm* algorithm)

// Random encode (new chromosome)
// void Solution::Encode() {
//   std::vector<size_t> seq_list(boost::counting_iterator<int>(0u), boost::counting_iterator<int>(
//       height_soft_.size()));

//   std::sort(begin(seq_list), end(seq_list), [&](const size_t& i, const size_t& j) {
//     return height_soft_[i] < height_soft_[j];
//   });  // Sort a list based on height soft

//   for (auto task : seq_list) {  // Encode ordering Chromosome
//     ordering_.push_back(task);
//   }

//   // Encode allocation chromosome
//   std::uniform_int_distribution<size_t> dis(0, algorithm_->get_vm_size() - 1);
//   for (size_t i = 0; i < algorithm_->get_size(); i++) {
//     allocation_[i] = dis(engine_chr);
//   }
// }  // void Solution::Encode()

// int Solution::ComputeHeightSoft(size_t node) {
//   // std::cout << node << std::endl;
//   int min = std::numeric_limits<int>::max();

//   if (height_soft_[node] != -1) {
//     return height_soft_[node];
//   }

//   if (node != algorithm_->get_id_target()) {
//     // std::vector<size_t>& list = algorithm_->get_successors().find(node)->second;
//     for (auto i : algorithm_->get_successors().find(node)->second) {
//       int value = ComputeHeightSoft(i);
//       min = std::min(value, min);
//     }
//   } else {
//     height_soft_[node] = algorithm_->get_height()[node];
//     return height_soft_[node];
//   }

//   std::uniform_int_distribution<> dis(algorithm_->get_height()[node], min - 1);
//   height_soft_[node] = dis(engine_chr);

//   return height_soft_[node];
//     // return 0;
// }  // int Solution::ComputeHeightSoft(size_t node)

// Start Time
// inline double Solution::TaskStartTime(Task task,
//                                       VirtualMachine vm,
//                                       const std::vector<double>& queue) {
//   // compute wait time
//   double wait_time = 0.0;

//   for (auto previous_task_id : algorithm_->get_predecessors().find(task.get_id())->second) {
//     wait_time = std::max(wait_time, time_vector_[previous_task_id]);
//   }

//   return std::max(wait_time, queue[vm.get_id()]);
// }  // inline double Solution::TaskStartTime(...)

// Read time
// inline double Solution::TaskReadTime(Task task, VirtualMachine vm) {
//   //compute read time
//   double read_time = 0;
//   size_t id_vm_file;

//   // for (auto id_file : task.input) {
//   for (auto file : task.get_input_files()) {
//     // auto file = data->file_map.find(id_file)->second;

//     if (!file->IsStatic()) {  // Dynamic File
//       id_vm_file = allocation_[file->get_id()];
//       // update vm queue
//       auto f_queue = vm_queue_.insert(std::make_pair(id_vm_file, std::vector<size_t>()));
//       f_queue.first->second.push_back(file->get_id());
//     } else {  // Static File
//       id_vm_file = static_cast<StaticFile*>(file)->GetFirstVm();
//     }

//     // auto vm_file = algorithm_->get_vm_map().find(id_vm_file)->second;
//     auto vm_file = algorithm_->get_storage_map().find(id_vm_file)->second;

//     read_time += std::ceil(FileTransferTime(file, vm, vm_file) + (file->get_size() * algorithm_->get_lambda()));
//   }
//   return read_time;
// }  // inline double Solution::TaskReadTime(Task task, VirtualMachine vm) {

// Write time
inline double Solution::ComputeTaskWriteTime(Task task, VirtualMachine vm) {
  // Compute the write time
  double write_time = 0;

  for (auto file : task.get_output_files()) {
    // auto file = data->file_map.find(id_file)->second;
    // auto vm_file = algorithm_->get_vm_map().find(allocation[file->get_id()])->second;
    auto vm_file = algorithm_->get_storage_map().find(allocation_[file->get_id()])->second;

    // Update vm queue
    auto f_queue = vm_queue_.insert(std::make_pair(vm_file.get_id(), std::vector<size_t>()));
    // f_queue.first->second.push_back(id_file);
    f_queue.first->second.push_back(file->get_id());

    write_time += std::ceil(FileTransferTime(file, vm, vm_file)
                            + (file->get_size() * (algorithm_->get_lambda() * 2)));
  }

  return write_time;
}  // inline double Solution::TaskWriteTime(Task task, VirtualMachine vm) {

/**
 * If file reside in the same vm, then the transfer time is 0.0.
 * Otherwise, calculate the transfer time between them using the smallest bandwidth.
 *
 *   \f[
 *      time = \lceil{\min_{storage1.bandwidth(), storage2.bandwidth()} bandwidth}\rceil
 *   \f]
 *
 * Where:
 * \f$ time \f$ is the transfer time between the \c storage1 and \c storage2
 * \f$ storage1.bandwidth() \f$ is the transfer rate of the \c storage1
 * \f$ storage2.bandwidth() \f$ is the transfer rate of the \c storage2
 * \f$ bandwidth \f$ is the minimal transfer rate between the storage1.bandwidth() and storage2.bandwidth()
 *
 * \param[in]  file            File to be transfered
 * \param[in]  storage1        Storage origin/destination
 * \param[in]  storage2        Storage origin/destination
 * \retval     time + penalts  The time to transfer \c file from \c file_vm to \c vm with possible
 *                             applied penalts
 */
inline double Solution::FileTransferTime(File* file,
                                         Storage storage1,
                                         Storage storage2,
                                         bool check_constraints) {
  double time = std::numeric_limits<double>::max();
  double penalts = 0.0;

  DLOG(INFO) << "Compute the transfer time of File[" << file->get_id() << "] to/from VM["
      << storage1.get_id() << "] to Storage[" << storage2.get_id() << "], check_constraints["
      << check_constraints << "]";

  if (check_constraints) {
    for (size_t i = algorithm_->get_task_size(); i < algorithm_->get_tasks_plus_files_size(); ++i) {
      size_t storage_id = allocation_[i];

      // If file 'i' is to be transfered to the same place that file->get_id()
      if (storage_id == storage2.get_id()) {
        int conflict_value = algorithm_->get_conflict_graph().ReturnConflict(i, file->get_id());

        switch (conflict_value)
        {
        case 0:  // Soft constraint; adds penalt
          // DLOG(INFO) << "File[" << file->get_id() << "] has conflict with File[" << i
          //     << "]; add a penalt of " << algorithm_->get_penalt();
          // // Add penalt
          // penalts += algorithm_->get_penalt();
          break;
        case -1:  // No conflicts; no penalt
          break;
        default:  // Hard constraint; just return with std::numeric_limits<double>::max();
          DLOG(INFO) << "time: " << time;
          return time;
          break;
        }
      } // else; allocated file resides in different storage or was not allocated yet
    }  // for (...) {
  }  // if (check_constraints) {

  // If there is no Hard constraints; calculates de transfer time between storages
  // or if check_constraints is false

  // Calculate time
  if (storage1.get_id() != storage2.get_id()) {
    // get the smallest link
    auto link = std::min(storage1.get_bandwidth(), storage2.get_bandwidth());
    time = std::ceil(file->get_size() / link);
  } else {
    time = 0.0;
  }

  DLOG(INFO) << "tranfer_time: " << time + penalts;
  return time + penalts;
}  // inline double Solution::FileTransferTime(File file, Storage vm1, Storage vm2) {

// Compute the fitness of Solution
double Solution::ComputeMakespan(bool check_storage, bool check_sequence) {

  DLOG(INFO) << "Compute Makespan: check_storage[" << check_storage << "], check_sequence["
      << check_sequence << "]";

  if (ordering_.size() != algorithm_->get_task_size()) {
    LOG(FATAL) << "Ordering has a wrong number of elements";
  }

  fill(time_vector_.begin(), time_vector_.end(), -1);
  fill(queue_.begin(), queue_.end(), -1);
  fill(start_time_vector_.begin(), start_time_vector_.end(), -1);

  // std::vector<double> queue(algorithm_->get_vm_size(), 0);
  // queue_.clear;

  if (check_storage && !checkFiles()) {
    std::cerr << "check file error" << std::endl;
    throw;
  }

  scheduler_.clear();
  vm_queue_.clear();

  // compute makespan
  for (auto id_task : ordering_) {  // For each task, do
    // If is not source or target than
    if (id_task != algorithm_->get_id_source() && id_task != algorithm_->get_id_target()) {
      if (check_sequence && !CheckTaskSequence(id_task)) {
        std::cerr << "Encode error - Solution: Error in the precedence relations." << std::endl;
        throw;
      }

      // Load Vm
      auto vm = algorithm_->get_virtual_machine_map().find(allocation_[id_task])->second;
      // auto task = algorithm_->get_task_map().find(id_task)->second;
      auto task = algorithm_->get_task_map_per_id().find(id_task)->second;

      // update vm queue
      auto f_queue = vm_queue_.insert(std::make_pair(vm.get_id(), std::vector<size_t>()));
      f_queue.first->second.push_back(task.get_id());

      // update scheduler
      auto f_scheduler = scheduler_.insert(std::make_pair(vm.get_id(), std::vector<std::string>()));
      f_scheduler.first->second.push_back(task.get_tag());

      // Compute Task Times
      // auto start_time = TaskStartTime(task, vm, queue);
      double start_time = ComputeTaskStartTime(task.get_id(), vm.get_id());
      // auto read_time = TaskReadTime(task, vm);
      double read_time = ComputeTaskReadTime(task, vm);
      // auto run_time = std::ceil(task.get_time() * vm.get_slowdown());  // Seconds
      double run_time = std::ceil(task.get_time() * vm.get_slowdown());  // Seconds
      auto write_time = ComputeTaskWriteTime(task, vm);
      // double write_time = ComputeTaskWriteTime(task, vm);

      double finish_time = std::numeric_limits<double>::max();

      if (start_time != std::numeric_limits<double>::max()
          && read_time != std::numeric_limits<double>::max()
          && write_time != std::numeric_limits<double>::max()) {
        finish_time = start_time + read_time + run_time + write_time;
      } else {
        DLOG(INFO) << "start_time: " << start_time;
        DLOG(INFO) << "read_time: " << read_time;
        DLOG(INFO) << "write_time: " << write_time;
        DLOG(INFO) << "run_time: " << run_time;
        DLOG(INFO) << "finish_time: " << finish_time;
        DLOG(INFO) << "objective_function_: " << makespan_;

        return finish_time;  // return "infinity"
      }

      DLOG(INFO) << "start_time: " << start_time;
      DLOG(INFO) << "read_time: " << read_time;
      DLOG(INFO) << "write_time: " << write_time;
      DLOG(INFO) << "run_time: " << run_time;
      DLOG(INFO) << "finish_time: " << finish_time;

      // auto finish_time = start_time + read_time + run_time + write_time;

      // Update structures
      time_vector_[id_task] = finish_time;
      start_time_vector_[id_task] = start_time;
      queue_[vm.get_id()] = finish_time;
    } else {  // Source and Target tasks
      if (id_task == algorithm_->get_id_source()) {  // Source task
        time_vector_[id_task] = 0;
      } else {  // Target task
        double max_value = 0.0;

        for (auto task : algorithm_->get_predecessors().find(id_task)->second) {
          max_value = std::max(max_value, time_vector_[task]);
        }

        time_vector_[id_task] = max_value;
      }  // } else {  // Target task
    }  // } else {  // Source and Target tasks
  }  // for (auto id_task : ordering_) {  // For each task, do

  makespan_ = time_vector_[algorithm_->get_id_target()];

  DLOG(INFO) << "makespan_: " << makespan_;

  return makespan_;
}  // void Solution::ComputeFitness(bool check_storage, bool check_sequence) {

/* Checks the sequence of tasks is valid */
inline bool Solution::CheckTaskSequence(size_t task) {
  for (auto tk : algorithm_->get_predecessors().find(task)->second) {
    if (time_vector_[tk] == -1) {
      return false;
    }
  }
  return true;
}  // inline bool Solution::CheckTaskSequence(size_t task) {

// Check and organize the file based on the storage capacity
inline bool Solution::checkFiles() {
  bool flag = true;
  int count = 0;

  std::vector<double> aux_storage(algorithm_->get_storage_vet());
  std::vector<size_t> aux(algorithm_->get_virtual_machine_size());
  iota(aux.begin(), aux.end(), 0); // 0,1,2,3,4 ... n

  std::unordered_map<int, std::vector<int>> map_file;

  size_t id_vm;
  // build file map and compute the storage
  // for (auto it : data->file_map) {
  for (auto it : algorithm_->get_file_map_per_id()) {
    if (!it.second->IsStatic()) {
      id_vm = allocation_[it.second->get_id()];
      auto f = map_file.insert(std::make_pair(id_vm, std::vector<int>()));
      f.first->second.push_back(it.second->get_id());
      // auto file = algorithm_->get_file_map().find(it.second.get_id())->second;
      aux_storage[id_vm] -= it.second->get_size();
    }
  }

  do {
    //sort machines based on the storage capacity
    std::sort(aux.begin(), aux.end(), [&](const size_t &a, const size_t &b) {
      return aux_storage[a] < aux_storage[b];
    });

    if (aux_storage[aux[0ul]] < 0.0) {  // If storage is full, start the file heuristic
      std::cout << "Starting file heuristic ..." << std::endl;
      size_t old_vm = aux[0];  // Critical machine
      size_t new_vm = aux[aux.size() - 1];  // Best machine

      auto vet_file = map_file.find(old_vm)->second;

      double min = std::numeric_limits<double>::max();
      // size_t min_file = -1;
      size_t min_file = std::numeric_limits<size_t>::max();

      //search min file (based on the size of file)
      for_each(vet_file.begin(), vet_file.end(), [&](size_t i) {
        std::cout << i << std::endl;
        auto file = algorithm_->get_file_map_per_id().find(i)->second;
        std::cout << file->get_name() << std::endl;
        if (file->get_size() < min) {
          min = file->get_size();
          min_file = file->get_id();
        }
      });

      auto file_min = algorithm_->get_file_map_per_id().find(min_file)->second;

      std::cout << file_min->get_name() << std::endl;
      // MinFile will be move to machine with more empty space
      allocation_[file_min->get_id()] = new_vm;
      // Update aux Storage
      aux_storage[old_vm] += file_min->get_size();
      aux_storage[new_vm] -= file_min->get_size();
      // Update mapFile structure
      map_file[old_vm].erase(remove(map_file[old_vm].begin(), map_file[old_vm].end(), min_file),
                             map_file[old_vm].end());
      map_file[new_vm].push_back(min_file);
    } else flag = false;
    count++;
  } while (flag && count < algorithm_->get_file_size());
  return !flag;
}  // inline bool Solution::checkFiles() {

std::ostream& Solution::write(std::ostream& os) const {
  os << "#!# " << makespan_ << std::endl;
  os << "Tasks: " << std::endl;

  for (auto info : algorithm_->get_virtual_machine_map()) {
    auto vm = info.second;
    os << vm.get_id() << ": ";
    // os << "[" << vm.get_id() << "]" << " <" << vm.get_name() << "> : ";
    auto f = scheduler_.find(vm.get_id());
    if (f != scheduler_.end()) {
      for (auto task_tag: f->second)
        os << task_tag << " ";
    }
    os << std::endl;
  }

  os << std::endl;
  os << "Files: " << std::endl;
  // for (auto info: algorithm_->get_vm_map()) {
  for (auto info : algorithm_->get_storage_map()) {
    auto vm = info.second;
    os << vm.get_id() << ": \n";
    for (auto info2 : algorithm_->get_file_map_per_id()) {
      auto file = info2.second;
      // std::shared_ptr<File> file = info2.second;
      size_t vm_id = file->IsStatic() ? static_cast<StaticFile*>(file)->GetFirstVm()
                                      : allocation_[file->get_id()];
      if (vm_id == vm.get_id())
        os << " " << file->get_name() << "\n";
    }
    os << std::endl;
  }

  os << std::endl;
  os << "Task Sequence: " << std::endl;
  for (auto task_id : ordering_) {
    if (task_id != algorithm_->get_id_source()
        && task_id != algorithm_->get_id_target()) {
      os << algorithm_->get_task_map_per_id().find(task_id)->second.get_name() <<  ", ";
    }
  }
  return os << std::endl;
}  // std::ostream& Solution::write(std::ostream& os) const {

// Compute the fitness of Solution
double Solution::ComputeCost() {
  double cost = 0.0;
  return cost;
}

double Solution::CalculateObjectiveFunction() {
  double time_max = 100.0;
  double cost_max = 100.0;

  double object_function = 0.0;
  double phi = 0.5;
  double makespan = 0.0;
  double cost = 0.0;
  // double makespan = ComputeMakespan();
  // double cost = ComputeCost();

  object_function = (phi * (makespan / time_max)) + ((1 - phi) * (cost / cost_max));

  return object_function;
}

double Solution::AllocateOneOutputFileGreedily(File* file, VirtualMachine vm) {
  double min_time = std::numeric_limits<double>::max();
  size_t min_storage;

  DLOG(INFO) << "Computing time for write the File[" << file->get_id() << "] into VM["
      << vm.get_id() << "]";

  std::unordered_map<size_t, int> available_storages;

  // for all possible storage; compute the transfer time
  for (std::pair<size_t, Storage> storage_pair : algorithm_->get_storage_map()) {
    double time = FileTransferTime(file, vm, storage_pair.second, true);

    if (min_time > time) {
      min_time = time;
      min_storage = storage_pair.first;
    }
  }

  if (min_time == std::numeric_limits<double>::max()) {
    LOG(FATAL) << "There is no storage available";
  }

  DLOG(INFO) << "Allocation of the output File[" << file->get_id() << "] to the Storage["
      << min_storage << "]";

  allocation_[file->get_id()] = min_storage;

  return min_time;
}  // double Solution::AllocateOutputFiles(...) {

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
 * \retval     start_time       The time at the task identified by \c task_id will start executing
 */
inline double Solution::ComputeTaskStartTime(size_t task_id,
                                             size_t vm_id) {
  double start_time = 0.0;

  DLOG(INFO) << "Compute the start time of the Task[" << task_id << "] at VM[" << vm_id << "]";

  for (auto previous_task_id : algorithm_->get_predecessors().find(task_id)->second) {
    start_time = std::max(start_time, time_vector_[previous_task_id]);
  }

  DLOG(INFO) << "start_time: " << start_time;

  return std::max(start_time, queue_[vm_id]);
} // inline double MinMinAlgorithm::ComputeStartTime(...)

inline double Solution::AllocateOutputFiles(Task& task, VirtualMachine& vm) {
  double write_time = 0.0;

  // For each output file allocate the storage that impose the minor write time
  for (auto& file : task.get_output_files()) {
    write_time += AllocateOneOutputFileGreedily(file, vm);
  }

  return write_time;
}

inline double Solution::ComputeTaskReadTime(Task& task, VirtualMachine& vm) {
  double read_time = 0.0;

  DLOG(INFO) << "Compute Read Time of the Task[" << task.get_id() << "] at VM[" << vm.get_id()
      << "]";

  for (File* file : task.get_input_files()) {
    size_t vm_id;

    if (StaticFile* static_file = dynamic_cast<StaticFile*>(file)) {
      vm_id = static_file->GetFirstVm();
    } else {
      vm_id = allocation_[file->get_id()];
    }

    Storage file_vm = algorithm_->get_storage_map().find(vm_id)->second;

    // Ceil of File Transfer Time + File Size * lambda
    double one_file_read_time = FileTransferTime(file, file_vm, vm) + (file->get_size() * lambda_);

    if (one_file_read_time == std::numeric_limits<double>::max()) {
      DLOG(INFO) << "read_time: " << one_file_read_time;
      return one_file_read_time;
    } else {
      read_time += std::ceil(one_file_read_time);
    }
  }  // for (File* file : task.get_input_files()) {

  DLOG(INFO) << "read_time: " << read_time;

  return read_time;
}  // inline double Solution::ComputeTaskReadTime(Task& task, VirtualMachine& vm) {

double Solution::MakespanOfAllocatedTask(Task task, VirtualMachine vm) {
  double start_time = 0.0;
  double read_time = 0.0;
  double write_time = 0.0;
  double run_time = 0.0;
  double makespan = std::numeric_limits<double>::max();

  DLOG(INFO) << "Makespan of the allocated Task[" << task.get_id() << "] at VM["
      << vm.get_id() << "]";
  google::FlushLogFiles(google::INFO);

  if (task.get_id() != algorithm_->get_id_source()
      && task.get_id() != algorithm_->get_id_target()) {
    start_time = ComputeTaskStartTime(task.get_id(), vm.get_id());
    read_time = ComputeTaskReadTime(task, vm);
    write_time = AllocateOutputFiles(task, vm);
  } else if (task.get_id() == algorithm_->get_id_target()) {
    for (auto task_id : algorithm_->get_predecessors().find(task.get_id())->second) {
      start_time = std::max(start_time, time_vector_[task_id]);
    }
  }

  run_time = ceil(task.get_time() * vm.get_slowdown());

  if (start_time != std::numeric_limits<double>::max()
      && read_time != std::numeric_limits<double>::max()
      && write_time != std::numeric_limits<double>::max()) {
    makespan = start_time + read_time + run_time + write_time;
  }

  DLOG(INFO) << "start_time: " << start_time;
  DLOG(INFO) << "read_time: " << read_time;
  DLOG(INFO) << "write_time: " << write_time;
  DLOG(INFO) << "run_time: " << run_time;
  DLOG(INFO) << "makespan: " << makespan;

  return makespan;
}

double Solution::CalculateCost() {
  double cost = 0.0;
  double virtual_machine_cost = 0.0;
  double bucket_fixed_cost = 0.0;
  double bucket_varible_cost = 0.0;

  DLOG(INFO) << "Calculate Cost";

  // Accumulate the Virtual Machine cost
  for (std::pair<size_t, VirtualMachine> vm_pair : algorithm_->get_virtual_machine_map()) {
    VirtualMachine virtual_machine = vm_pair.second;

    virtual_machine_cost += queue_[virtual_machine.get_id()] * virtual_machine.get_cost();
  }

  // Accumulate the Bucket fixed cost
  for (std::pair<size_t, Storage> storage_vm : algorithm_->get_storage_map()) {
    Storage storage = storage_vm.second;

    // If the storage is not a Virtual Machine, i.e. it is a Bucket; then verify that it is been
    // used
    if (storage.get_id() >= algorithm_->get_virtual_machine_size()) {
      for (size_t i = algorithm_->get_task_size();
          i < algorithm_->get_tasks_plus_files_size();
          ++i) {
        // If the Bucket is used; then accumulate de cost and break to the next Storage
        if (allocation_[i] == storage.get_id()) {
          bucket_fixed_cost += storage.get_cost();
          break;
        }
      }
    }
  }

  // Accumulate the Bucket variable cost
  for (std::pair<size_t, Storage> storage_vm : algorithm_->get_storage_map()) {
    Storage storage = storage_vm.second;

    // If the storage is not a Virtual Machine, i.e. it is a Bucket; then calculate the bucket
    // variable cost
    if (storage.get_id() >= algorithm_->get_virtual_machine_size()) {
      for (size_t i = algorithm_->get_task_size();
          i < algorithm_->get_tasks_plus_files_size();
          ++i) {
        // If the Bucket is used; then accumulate de cost and break to the next Storage
        if (allocation_[i] == storage.get_id()) {
          File* file = algorithm_->get_file_map_per_id().find(i)->second;
          bucket_varible_cost += storage.get_cost() * file->get_size();
        }
      }
    }
  }

  DLOG(INFO) << "virtual_machine_cost: " << virtual_machine_cost;
  DLOG(INFO) << "bucket_fixed_cost: " << bucket_fixed_cost;
  DLOG(INFO) << "bucket_varible_cost: " << bucket_varible_cost;

  cost = virtual_machine_cost + bucket_fixed_cost + bucket_varible_cost;

  DLOG(INFO) << "cost: " << cost;

  return cost;
}  // double Solution::CalculateCost() {

double Solution::CalculateSecurityExposure() {
  double security_exposure = 0.0;
  double task_exposure = 0.0;
  double privacy_exposure = 0.0;

  DLOG(INFO) << "Calculate Security Exposure";

  // Accumulate the task exposure
  for (std::pair<size_t, Task> task_pair : algorithm_->get_task_map_per_id()) {
    Task task = task_pair.second;
    for (double requirement_value : task.get_requirements()) {
      task_exposure += requirement_value;
    }
  }

  // Accumulate the privacy_exposure
  for (size_t i = algorithm_->get_task_size();
       i < algorithm_->get_tasks_plus_files_size() - 1;
       ++i) {
    size_t storage1_id = allocation_[i];

    for (size_t j = algorithm_->get_task_size() + 1;
         j < algorithm_->get_tasks_plus_files_size();
         ++j) {
      size_t storage2_id = allocation_[j];

      if (storage1_id != std::numeric_limits<size_t>::max()
          && storage2_id != std::numeric_limits<size_t>::max()
          && storage1_id == storage2_id) {
        int conflict_value = algorithm_->get_conflict_graph().ReturnConflict(i, j);

        switch (conflict_value)
        {
        case 0:  // Soft constraint; adds penalt
          DLOG(INFO) << "File[" << i << "] has conflict with File[" << j << "]";
          privacy_exposure += algorithm_->get_penalt();
          break;
        case -1:  // No conflicts; no penalt
          break;
        default:  // Hard constraint;
          DLOG(FATAL) << "Should not have hard constraints in here!";
          break;
        }
      } // else; allocated file resides in different storage or was not allocated yet
    }  // for (size_t j ...) {
  }  // for (size_t i...) {

  security_exposure = (task_exposure + privacy_exposure)
                    / algorithm_->get_maximum_security_and_privacy_exposure();

  return security_exposure;
}  // double Solution::CalculateSecurityExposure() {

/**
 * Insert the task in the solution and calculate its objective value.
 *
 * Calculates:
 * 1. makespan
 * 2. cost
 * 3. security exposure
 * And then, return the sum of them.
 *
 * \param[in]  task             Task for which we want to find the fitness
 * \param[in]  vm               VM where the task will be executed
 * \retval     objective_value  The objective value of the solution when inserting the \c task
 */
double Solution::AllocateTask(Task task, VirtualMachine vm) {
  double objective_value = std::numeric_limits<double>::max();

  DLOG(INFO) << "Allocate the Task[" << task.get_id() << "] at VM[" << vm.get_id() << "]";
  google::FlushLogFiles(google::INFO);

  allocation_[task.get_id()] = vm.get_id();
  ordering_.push_back(task.get_id());

  // 1. Calculates the makespan
  double makespan = MakespanOfAllocatedTask(task, vm);

  // 2. Calculates the cost
  double cost = CalculateCost();

  // 3. Calculates the security exposure
  double security_exposure = CalculateSecurityExposure();

  DLOG(INFO) << "makespan: " << makespan;
  DLOG(INFO) << "cost: " << cost;
  DLOG(INFO) << "security_exposure: " << security_exposure;

  // Update auxiliary structures (queue_ and time_vector_)
  time_vector_[task.get_id()] = makespan;
  queue_[vm.get_id()] = makespan;

  objective_value = algorithm_->get_alpha_time() * (makespan / algorithm_->get_makespan_max())
                  + algorithm_->get_alpha_budget() * (cost / algorithm_->get_budget_max())
                  + algorithm_->get_alpha_security() * (security_exposure
                      / algorithm_->get_maximum_security_and_privacy_exposure());

  return objective_value;
}  // inline double Solution::AllocateTask(...)
