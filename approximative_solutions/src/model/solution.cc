#include <numeric>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/bimap.hpp>
#include <boost/algorithm/string.hpp>
#include <utility>
#include "src/model/static_file.h"
#include "src/model/solution.h"

std::random_device rd_chr;
std::mt19937 engine_chr(rd_chr());

Solution::Solution(Algorithm* algorithm) :
    algorithm_(algorithm),
    allocation(algorithm->get_size(), std::numeric_limits<size_t>::max()),
    height_soft_(algorithm->get_task_size(), -1),
    time_vector(algorithm->get_task_size(), -1),
    start_time_vector(algorithm->get_task_size(), -1),
    ordering(0),
    fitness(0),
    lambda_(algorithm->get_lambda()),
    transfer_size(0) {
  ComputeHeightSoft(algorithm->get_id_source());
  Encode();
  ComputeFitness();
}  // Solution::Solution(Algorithm* algorithm)

// Random encode (new chromosome)
void Solution::Encode() {
  std::vector<size_t> seq_list(boost::counting_iterator<int>(0u), boost::counting_iterator<int>(
      height_soft_.size()));

  std::sort(begin(seq_list), end(seq_list), [&](const size_t& i, const size_t& j) {
    return height_soft_[i] < height_soft_[j];
  });  // Sort a list based on height soft

  for (auto task : seq_list) {  // Encode ordering Chromosome
    ordering.push_back(task);
  }

  // Encode allocation chromosome
  std::uniform_int_distribution<size_t> dis(0, algorithm_->get_vm_size() - 1);
  for (size_t i = 0; i < algorithm_->get_size(); i++) {
    allocation[i] = dis(engine_chr);
  }
}  // void Solution::Encode()

int Solution::ComputeHeightSoft(size_t node) {
  // std::cout << node << std::endl;
  int min = std::numeric_limits<int>::max();

  if (height_soft_[node] != -1) {
    return height_soft_[node];
  }

  if (node != algorithm_->get_id_target()) {
    // std::vector<size_t>& list = algorithm_->get_successors().find(node)->second;
    for (auto i : algorithm_->get_successors().find(node)->second) {
      int value = ComputeHeightSoft(i);
      min = std::min(value, min);
    }
  } else {
    height_soft_[node] = algorithm_->get_height()[node];
    return height_soft_[node];
  }

  std::uniform_int_distribution<> dis(algorithm_->get_height()[node], min - 1);
  height_soft_[node] = dis(engine_chr);

  return height_soft_[node];
    // return 0;
}  // int Solution::ComputeHeightSoft(size_t node)

// Start Time
inline double Solution::TaskStartTime(Task task,
                                      VirtualMachine vm,
                                      const std::vector<double>& queue) {
  // compute wait time
  double wait_time = 0.0;

  for (auto previous_task_id : algorithm_->get_predecessors().find(task.get_id())->second) {
    wait_time = std::max(wait_time, time_vector[previous_task_id]);
  }

  return std::max(wait_time, queue[vm.get_id()]);
}  // inline double Solution::TaskStartTime(...)

// Read time
inline double Solution::TaskReadTime(Task task, VirtualMachine vm) {
  //compute read time
  double read_time = 0;
  size_t id_vm_file;

  // for (auto id_file : task.input) {
  for (auto file : task.get_input_files()) {
    // auto file = data->file_map.find(id_file)->second;

    if (!file->IsStatic()) {  // Dynamic File
      id_vm_file = allocation[file->get_id()];
      // update vm queue
      auto f_queue = vm_queue.insert(std::make_pair(id_vm_file, std::vector<size_t>()));
      f_queue.first->second.push_back(file->get_id());
    } else {  // Static File
      id_vm_file = static_cast<StaticFile*>(file)->GetFirstVm();
    }

    // auto vm_file = algorithm_->get_vm_map().find(id_vm_file)->second;
    auto vm_file = algorithm_->get_storage_map().find(id_vm_file)->second;

    read_time += std::ceil(FileTransferTime(*file, vm, vm_file) + (file->get_size() * algorithm_->get_lambda()));
  }
  return read_time;
}  // inline double Solution::TaskReadTime(Task task, VirtualMachine vm) {

// Write time
inline double Solution::TaskWriteTime(Task task, VirtualMachine vm) {
  // Compute the write time
  double write_time = 0;

  for (auto file :task.get_output_files()) {
    // auto file = data->file_map.find(id_file)->second;
    // auto vm_file = algorithm_->get_vm_map().find(allocation[file->get_id()])->second;
    auto vm_file = algorithm_->get_storage_map().find(allocation[file->get_id()])->second;

    // Update vm queue
    auto f_queue = vm_queue.insert(std::make_pair(vm_file.get_id(), std::vector<size_t>()));
    // f_queue.first->second.push_back(id_file);
    f_queue.first->second.push_back(file->get_id());

    write_time += std::ceil(FileTransferTime(*file, vm, vm_file) + (file->get_size() * (algorithm_->get_lambda() * 2)));
  }
  return write_time;
}  // inline double Solution::TaskWriteTime(Task task, VirtualMachine vm) {

// Transfer Time
inline double Solution::FileTransferTime(File file, Storage storage1, Storage storage2) {
  double time = 0.0;

  if (storage1.get_id() != storage2.get_id()) {
    // get the smallest link
    auto link = std::min(storage1.get_bandwidth(), storage2.get_bandwidth());
    time = std::ceil(file.get_size() / link);
  }

  return time;  // Otherwise
}  // inline double Solution::FileTransferTime(File file, Storage vm1, Storage vm2) {

// Compute the fitness of Solution
void Solution::ComputeFitness(bool check_storage, bool check_sequence) {
  fill(time_vector.begin(), time_vector.end(), -1);
  fill(start_time_vector.begin(), start_time_vector.end(), -1);

  std::vector<double> queue(algorithm_->get_vm_size(), 0);

  if (check_storage && !checkFiles()) {
      std::cerr << "check file error" << std::endl;
      throw;
  }

  scheduler.clear();
  vm_queue.clear();
  // compute makespan
  for (auto id_task : ordering) {  // For each task, do
    // If is not root or sink than
    if (id_task != algorithm_->get_id_source() && id_task != algorithm_->get_id_target()) {
      if (check_sequence && !CheckTaskSequence(id_task)) {
        std::cerr << "Encode error - Solution: Error in the precedence relations." << std::endl;
        throw;
      }

      // Load Vm
      auto vm = algorithm_->get_vm_map().find(allocation[id_task])->second;
      // auto task = algorithm_->get_task_map().find(id_task)->second;
      auto task = algorithm_->get_task_map_per_id().find(id_task)->second;

      // update vm queue
      auto f_queue = vm_queue.insert(std::make_pair(vm.get_id(), std::vector<size_t>()));
      f_queue.first->second.push_back(task.get_id());

      // update scheduler
      auto f_scheduler = scheduler.insert(std::make_pair(vm.get_id(), std::vector<std::string>()));
      f_scheduler.first->second.push_back(task.get_tag());

      // Compute Task Times
      auto start_time = TaskStartTime(task, vm, queue);
      auto read_time = TaskReadTime(task, vm);
      auto run_time = std::ceil(task.get_time() * vm.get_slowdown());  // Seconds
      auto write_time = TaskWriteTime(task, vm);
      auto finish_time = start_time + read_time + run_time + write_time;

      // Update structures
      time_vector[id_task] = finish_time;
      start_time_vector[id_task] = start_time;
      queue[vm.get_id()] = finish_time;
    } else {  // Source and Target tasks
      if (id_task == algorithm_->get_id_source()) {
        time_vector[id_task] = 0;
      } else {  // Target task
        double max_value = 0.0;
        for (auto tk : algorithm_->get_predecessors().find(id_task)->second)
            max_value = std::max(max_value, time_vector[tk]);
        time_vector[id_task] = max_value;
      }
    }
  }
  fitness = time_vector[algorithm_->get_id_target()];
}  // void Solution::ComputeFitness(bool check_storage, bool check_sequence) {

/* Checks the sequence of tasks is valid */
inline bool Solution::CheckTaskSequence(size_t task) {
  for (auto tk : algorithm_->get_predecessors().find(task)->second) {
    if (time_vector[tk] == -1) {
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
  std::vector<size_t> aux(algorithm_->get_vm_size());
  iota(aux.begin(), aux.end(), 0); // 0,1,2,3,4 ... n

  std::unordered_map<int, std::vector<int>> map_file;

  size_t id_vm;
  // build file map and compute the storage
  // for (auto it : data->file_map) {
  for (auto it : algorithm_->get_file_map_per_id()) {
    if (!it.second->IsStatic()) {
      id_vm = allocation[it.second->get_id()];
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
      allocation[file_min->get_id()] = new_vm;
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

void Solution::print() {
  std::cout << "#!# " << fitness << std::endl;
  std::cout << "Tasks: " << std::endl;

  for (auto info : algorithm_->get_vm_map()) {
    auto vm = info.second;
    std::cout << vm.get_id() << ": ";
    // std::cout << "[" << vm.get_id() << "]" << " <" << vm.get_name() << "> : ";
    auto f = scheduler.find(vm.get_id());
    if (f != scheduler.end()) {
      for (auto task_tag: f->second)
        std::cout << task_tag << " ";
    }
    std::cout << std::endl;
  }

  std::cout << std::endl;
  std::cout << "Files: " << std::endl;
  // for (auto info: algorithm_->get_vm_map()) {
  for (auto info : algorithm_->get_storage_map()) {
    auto vm = info.second;
    std::cout << vm.get_id() << ": " << std::endl;
    for (auto info2 : algorithm_->get_file_map_per_id()) {
      auto file = info2.second;
      // std::shared_ptr<File> file = info2.second;
      size_t vm_id = file->IsStatic() ? static_cast<StaticFile*>(file)->GetFirstVm()
                                      : allocation[file->get_id()];
      if (vm_id == vm.get_id())
        std::cout << " " << file->get_name() << "\n";
    }
    std::cout << std::endl;
  }

  std::cout << std::endl;
  std::cout << "Task Sequence: " << std::endl;
  for (auto task_id : ordering) {
    if (task_id != algorithm_->get_id_source()
        && task_id != algorithm_->get_id_target()) {
      std::cout << algorithm_->get_task_map_per_id().find(task_id)->second.get_name() <<  ", ";
    }
  }
  std::cout << std::endl;
}  // void Solution::print()

std::ostream& Solution::write(std::ostream& os) const {
  os << "#!# " << fitness << std::endl;
  os << "Tasks: " << std::endl;

  for (auto info : algorithm_->get_vm_map()) {
    auto vm = info.second;
    os << vm.get_id() << ": ";
    // os << "[" << vm.get_id() << "]" << " <" << vm.get_name() << "> : ";
    auto f = scheduler.find(vm.get_id());
    if (f != scheduler.end()) {
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
                                      : allocation[file->get_id()];
      if (vm_id == vm.get_id())
        os << " " << file->get_name() << "\n";
    }
    os << std::endl;
  }

  os << std::endl;
  os << "Task Sequence: " << std::endl;
  for (auto task_id : ordering) {
    if (task_id != algorithm_->get_id_source()
        && task_id != algorithm_->get_id_target()) {
      os << algorithm_->get_task_map_per_id().find(task_id)->second.get_name() <<  ", ";
    }
  }
  return os << std::endl;
}  // std::ostream& Solution::write(std::ostream& os) const {
