/**
 * \file src/model/solution.cc
 * \brief Contains the \c Solution class definition
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2021
 *
 * This source file contains the \c Solution class definition
 */

#include "src/model/solution.h"

#include <boost/algorithm/string.hpp>
#include <src/common/my_random.h>
#include "src/model/dynamic_file.h"
#include "src/model/static_file.h"

DECLARE_uint64(number_of_allocation_experiments);

double aux_start_time = 0.0;
//size_t aux_start_time = 0UL;
double aux_read_time = 0.0;
double aux_runtime = 0.0;

/// Parameterised constructor
Solution::Solution(Algorithm* algorithm)
    : algorithm_(algorithm),
      activation_allocations_(algorithm->GetTaskSize(), std::numeric_limits<size_t>::max()),
      file_allocations_(algorithm->GetFileSize(), std::numeric_limits<size_t>::max()),
      task_height_(algorithm->GetTaskSize(), -1),
      time_vector_(algorithm->GetTaskSize(), 0.0),
//      time_vector_(algorithm->GetTaskSize(), std::numeric_limits<size_t>::max()),
      execution_vm_queue_(algorithm->GetVirtualMachineSize(), 0.0),
      allocation_vm_queue_(algorithm->GetVirtualMachineSize(), 0.0),
      makespan_(0.0),
      cost_(0.0),
      security_exposure_(0.0) {
  // Initialising Virtual Machines
  virtual_machines_.reserve(algorithm->GetVirtualMachineSize());
  for (size_t i = 0; i < algorithm->GetVirtualMachineSize(); i++) {
    auto p = algorithm->GetVirtualMachinePerId(i);
    virtual_machines_.emplace_back(std::make_shared<VirtualMachine>(*(p.get())));
  }

  // Initialising Activations
  activations_.reserve(algorithm->GetTaskSize());
  for (size_t i = 0; i < algorithm->GetTaskSize(); i++) {
    auto p = algorithm->GetActivationPerId(i);
    activations_.emplace_back(std::make_shared<Activation>(*(p.get())));
  }
  // Setting Predecessors and Successors of the Activations
  for (auto& act : activations_) {
    // Setting Predecessors
    auto predecessors = algorithm_->GetPredecessors(act->get_id());
    std::vector<std::weak_ptr<Activation>> predecessors_of_activations;
    std::for_each(predecessors.begin(), predecessors.end(), [&predecessors_of_activations, this](size_t activation_id) {
      predecessors_of_activations.emplace_back(activations_[activation_id]);
    });
    act->SetPredecessors(predecessors_of_activations);
    // Setting Successors
    auto successors = algorithm_->GetSuccessors(act->get_id());
    std::vector<std::weak_ptr<Activation>> successors_of_activations;
    std::for_each(successors.begin(), successors.end(), [&successors_of_activations, this](size_t activation_id) {
      successors_of_activations.emplace_back(activations_[activation_id]);
    });
    act->SetSuccessors_(successors_of_activations);
  }

  // Initialising Files
  files_.reserve(algorithm->GetFileSize());
  for (size_t i = 0; i < algorithm->GetFileSize(); i++) {
    auto p = algorithm->GetFilePerId(i);

    if (std::shared_ptr<StaticFile> static_file = std::dynamic_pointer_cast<StaticFile>(p)) {
      files_.emplace_back(std::make_shared<StaticFile>(*(static_file.get())));
    } else if (std::shared_ptr<DynamicFile> dynamic_file = std::dynamic_pointer_cast<DynamicFile>(p)) {
      files_.emplace_back(std::make_shared<DynamicFile>(*(dynamic_file.get())));
    }
  }

  // Initialising Storages
  storages_.reserve(algorithm->GetStorageSize());
  for (size_t i = 0; i < algorithm->GetStorageSize(); i++) {
    auto p = algorithm->GetStoragePerId(i);

    if (auto vm = std::dynamic_pointer_cast<VirtualMachine>(p)) {
      storages_.emplace_back(std::make_shared<VirtualMachine>(*(vm.get())));
    } else if (auto bucket = std::dynamic_pointer_cast<Bucket>(p)) {
      storages_.emplace_back(std::make_shared<Bucket>(*(bucket.get())));
    }
  }
  // Initialising file storage for static files
  for (auto file : files_) {
    if (auto static_file = std::dynamic_pointer_cast<StaticFile>(file)) {
      auto storage = storages_[static_file->GetFirstVm()];
      file->AddStorage(storage);
    }
  }

  // Setting Activations Heights
  ComputeTasksHeights(algorithm->get_id_source());
}  // Solution::Solution(Algorithm* algorithm)

/// Copy constructor
//Solution::Solution(const Solution& other) {
//  algorithm_ = other.algorithm_;
//  activation_allocations_ = other.activation_allocations_;
//
//}
Solution::Solution(const Solution& other)
    : algorithm_(other.algorithm_),
      activation_allocations_(other.activation_allocations_),
      file_allocations_(other.file_allocations_),
      ordering_(other.ordering_),
      task_height_(other.task_height_),
      time_vector_(other.time_vector_),
      execution_vm_queue_(other.execution_vm_queue_),
      allocation_vm_queue_(other.allocation_vm_queue_),
      makespan_(other.makespan_),
      cost_(other.cost_),
      security_exposure_(other.security_exposure_),
      objective_value_(other.objective_value_),
      virtual_machines_(other.virtual_machines_),
      activations_(other.activations_),
      storages_(other.storages_) {
  DLOG(INFO) << "Initiating copy constructor of solution ...";
  // Copying Virtual Machines
  virtual_machines_.clear();
  for (size_t i = 0; i < other.virtual_machines_.size(); i++) {
    auto p = virtual_machines_[i];
    virtual_machines_.emplace_back(std::make_shared<VirtualMachine>(*(p.get())));
  }
  // Copying Virtual Machines
  activations_.clear();
  for (size_t i = 0; i < other.activations_.size(); i++) {
    auto p = other.activations_[i];
    activations_.emplace_back(std::make_shared<Activation>(*(p.get())));
  }

  // Copying Files
  files_.clear();
  for (size_t i = 0; i < other.files_.size(); i++) {
    auto p = other.files_[i];

    if (auto static_file = std::dynamic_pointer_cast<StaticFile>(p)) {
      auto f = std::make_shared<StaticFile>(*(static_file.get()));
      files_.emplace_back(f);
      auto storage_id = static_file->GetStorageId();
      if (storage_id != std::numeric_limits<size_t>::max()) {
        DLOG(INFO) << "Adding Storage [" << storage_id << "] to Static file [" << f->get_id() << "]";
        google::FlushLogFiles(google::INFO);
        files_[i]->AddStorage(storages_[storage_id]);
      } else {
        LOG(FATAL) << "Should not be Static files without storage";
      }
    } else if (auto dynamic_file = std::dynamic_pointer_cast<DynamicFile>(p)) {
      auto f = std::make_shared<DynamicFile>(*(dynamic_file.get()));
      files_.emplace_back(f);
      auto storage_id = dynamic_file->GetStorageId();
      if (storage_id != std::numeric_limits<size_t>::max()) {
        DLOG(INFO) << "Adding Storage [" << storage_id << "] to Dynamic file [" << f->get_id() << "]";
        google::FlushLogFiles(google::INFO);
        files_[i]->AddStorage(storages_[storage_id]);
      }
    } else {
      LOG(FATAL) << "Should be only Static and Dynamic files";
    }
  }

  DLOG(INFO) << "... copy constructor of solution finished";
};

/**
 * Compute the objective value. The objective value is composed by three weighted normalized values:
 *
 * 1. makespan
 * 2. cost
 * 3. security exposure
 *
 *   \f[
 *      \mathrm{min\ \ \ \ \ \ \ }  &
 *      % makespam
 *      \alpha_t \cdot (\frac{z^T}{t_{max}}) + & \label{fo1}\\
 *      % financial cost
 *       &\alpha_b \cdot (  \sum_{j \in M} \frac{c_{j}^M z^T_{j}}{c_{max}}+ \sum_{j \in B_f} \sum_{l \in L_j} \frac{c^B_{jl} b_{jl}}{c_{max}} +
 *      \sum_{j \in B_v} \sum_{l \in L_j} \frac{c^B_{jl} q_{jl}}{c_{max}} )+ & \label{fo2}\\
 *      % security violation penalty
 *      &\alpha_s \cdot (
 *      \sum_{i \in N} \sum_{r \in R} \frac{e^{ri}}{s_{max}} +
 *      \sum_{(d_1,d_2) \in E_s} \frac{p_{d_1d_2} w_{d_1d_2}}{s_{max}}) \label{fo3} &
 *   \f]
 *
 * \param[in]  check_storage    Check the storage for capacity
 * \param[in]  check_sequence   Check the correctness of the sequence
 * \retval     objective_value  The sum of the normalized values of makespan, cost and security exposure
 */
double Solution::ObjectiveFunction(bool check_storage, bool check_sequence) {
  DLOG(INFO) << "Compute Makespan: check_storage[" << check_storage << "], check_sequence["
             << check_sequence << "]";

  if (ordering_.size() != algorithm_->GetTaskSize()) {
    LOG(FATAL) << "Ordering has a wrong number of elements";
  }

//  fill(time_vector_.begin(), time_vector_.end(), 0.0);
//  fill(execution_vm_queue_.begin(), execution_vm_queue_.end(), 0.0);

//  fill(time_vector_.begin(), time_vector_.end(), 0UL);
//  fill(allocation_vm_queue_.begin(), allocation_vm_queue_.end(), 0UL);
//  fill(execution_vm_queue_.begin(), execution_vm_queue_.end(), 0UL);
  // fill(start_time_vector_.begin(), start_time_vector_.end(), -1);

  // for (size_t i = 0ul; i < scheduler_.size(); ++i) {
  //   scheduler_[i].clear();
  // }

  // for (size_t i = 0ul; i < storage_queue_.size(); ++i) {
  //   storage_queue_[i].clear();
  // }

  if (check_storage && !checkFiles()) {
    std::cerr << "check file error" << std::endl;
    throw;
  }

  // scheduler_.clear();
  // storage_queue_.clear();

  // 1. Calculates the makespan
//  makespan_ = ComputeMakespan(check_sequence);
//  makespan_ = GetMakespan();

  // 2. Calculates the cost
//  cost_ = ComputeCost();
//  cost_ = GetCost();

  // 3. Calculates the security exposure
//  security_exposure_ = ComputeSecurityExposure();
//  security_exposure_ = GetSecurityExposure();

//  DLOG(INFO) << "makespan: " << makespan_;
//  DLOG(INFO) << "cost: " << cost_;
//  DLOG(INFO) << "security_exposure: " << security_exposure_;

  objective_value_ = algorithm_->get_alpha_time() * (static_cast<double>(makespan_) / algorithm_->get_makespan_max())
      + algorithm_->get_alpha_budget() * (cost_ / algorithm_->get_budget_max())
      + algorithm_->get_alpha_security() * (security_exposure_
          / algorithm_->get_maximum_security_and_privacy_exposure());

//  DLOG(INFO) << "objective_value_: " << objective_value_;

  return objective_value_;
}  // void Solution::ComputeFitness(bool check_storage, bool check_sequence) {

/**
 * Time necessary to write all output files of the \c task executed in the \c virtual_machine
 *
 * \param[in]  task             Activation that will be executed at \c virtual_machine
 * \param[in]  virtual_machine  Virtual machine where the \c task will be executed
 * \retval     write_time       The accumulated time to write all output files of the \c task
 */
double Solution::ComputeTaskWriteTime(const std::shared_ptr<Activation>& task,
                                      const std::shared_ptr<VirtualMachine>& virtual_machine) {
  // Compute write time
  double write_time = 0;

  // std::vector<std::shared_ptr<File>> output_files = task->get_output_files();

  for (const std::shared_ptr<File>& file: task->get_output_files()) {
    // for (size_t i = 0ul; i < output_files.size(); ++i) {
    // std::shared_ptr<File> file = output_files[i];
    std::shared_ptr<Storage> storage_of_the_file = algorithm_->GetStoragePerId(file_allocations_[file->get_id()]);

    // Update vm queue
    // auto f_queue = storage_queue_.insert(std::make_pair(storage_of_the_file->get_id(),
    //                                                     std::vector<size_t>()));
    // TO-DO: remove
    // virtual_machine_queue_[storage_of_the_file->get_id()].push_back(file->get_id());
    // f_queue.first->second.push_back(file->get_id());

    // write_time += std::ceil(ComputeFileTransferTime(file, virtual_machine, storage_of_the_file)
    //                         + (file->get_size() * (algorithm_->get_lambda() * 2)));

    // write_time += std::ceil(ComputeFileTransferTime(file, virtual_machine, storage_of_the_file));
    write_time += ComputeFileTransferTime(file, virtual_machine, storage_of_the_file);
  }

  return write_time;
}  // double Solution::TaskWriteTime(Activation task, VirtualMachine vm) {

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
 * \param[in]  file               File to transfer from storage1 to storage2
 * \param[in]  storage1           Storage origin/destination
 * \param[in]  storage2           Storage origin/destination
 * \param[in]  check_constraints  Check for hard constraint before computation
 * \retval     time + penalties   The time to transfer \c file from \c file_vm to \c vm with possible
 *                                applied penalties
 */
double Solution::ComputeFileTransferTime(const std::shared_ptr<File>& file,
                                         const std::shared_ptr<Storage>& storage1,
                                         const std::shared_ptr<Storage>& storage2,
                                         bool check_constraints) {
  double time = std::numeric_limits<double>::max();

//  DLOG(DEBUG) << "Compute the transfer time of File[" << file->get_id() << "] to/from VM["
//      << storage1->get_id() << "] to Storage[" << storage2->get_id() << "], check_constraints["
//      << check_constraints << "]";

  if (check_constraints) {
    for (size_t i = 0ul; i < algorithm_->GetFileSize(); ++i) {
      size_t storage_id = file_allocations_[i];

      // If file 'i' is to transfer to the same place that file->get_id()
      if (storage_id == storage2->get_id()) {
        int conflict_value = algorithm_->get_conflict_graph().ReturnConflict(i, file->get_id());

        if (conflict_value < 0) {
          // Hard constraint; just return with std::numeric_limits<double>::max();
          return time;
        }
      }  // else; allocated file resides in different storage or was not allocated yet
    }  // for (...) {
  }  // if (check_constraints) {

  // If there is no Hard constraints; calculates de transfer time between storages
  // or if check_constraints is false

  // Calculate time
  if (storage1->get_id() != storage2->get_id()) {
    // get the smallest link
    double link = std::min(storage1->get_bandwidth_GBps(), storage2->get_bandwidth_GBps());
    time = std::ceil(file->get_size_in_GB() / link);
    // time = file->get_size() / link;
  } else {
    time = 1.0;
  }

//  DLOG(DEBUG) << "tranfer_time: " << time;
  return time;
}  // double Solution::FileTransferTime(File file, Storage vm1, Storage vm2) {

//double Solution::ComputeMakespan(bool check_sequence) {
double Solution::ComputeMakespan(bool check_sequence) {
  double makespan;
//  size_t makespan;
//  size_t makespan = 0.0;

  // compute makespan
  // for (auto id_task : ordering_) {  // For each task, do
  for (unsigned long id_task : ordering_) {
    // If is not source or target than
    if (id_task != algorithm_->get_id_source() && id_task != algorithm_->get_id_target()) {
      if (check_sequence && !CheckTaskSequence(id_task)) {
        std::cerr << "Encode error - Solution: Error in the precedence relations." << std::endl;
        throw;
      }

      // Load Vm
      std::shared_ptr<VirtualMachine> vm = algorithm_->GetVirtualMachinePerId(activation_allocations_[id_task]);
      std::shared_ptr<Activation> task = algorithm_->GetActivationPerId(id_task);

      // update vm queue
      // auto f_queue = storage_queue_.insert(std::make_pair(vm->get_id(), std::vector<size_t>()));
      // f_queue.first->second.push_back(task->get_id());
      // storage_queue_[vm->get_id()].push_back(task->get_id());

      // update scheduler
      // auto f_scheduler = scheduler_.insert(std::make_pair(vm->get_id(), std::vector<std::string>()));
      // f_scheduler.first->second.push_back(task->get_tag());
      // scheduler_[vm->get_id()].push_back(task->get_tag());

      // Compute Activation Times
//      double start_time = ComputeActivationStartTime(task->get_id(), vm->get_id());
//      size_t start_time = ComputeActivationStartTime(task->get_id(), vm->get_id());
//      aux_start_time = start_time;
//      double read_time = ComputeTaskReadTimeOther(task, vm);
//      // double run_time = std::ceil(task->get_time() * vm->get_slowdown());  // Seconds
//      double run_time = task->get_time() * vm->get_slowdown();  // Seconds
//      double write_time = ComputeTaskWriteTime(task, vm);
//      size_t start_time = 0.0;
      double start_time = 0.0;
      double read_time = 0.0;
      double write_time = 0.0;
      double run_time = task->get_time() * vm->get_slowdown();

      // Could not remember why I need this line bellow
      aux_runtime = run_time;

      if (task->get_id() != algorithm_->get_id_source()
          && task->get_id() != algorithm_->get_id_target()) {
        start_time = ComputeActivationStartTime(task->get_id(), vm->get_id());
        aux_start_time = start_time;
        read_time = ComputeTaskReadTimeOther(task, vm);
        aux_read_time = read_time;
        write_time = ComputeTaskWriteTime(task, vm);
      } else if (task->get_id() == algorithm_->get_id_target()) {
        for (auto task_id: algorithm_->GetPredecessors(task->get_id())) {
          start_time = std::max<double>(start_time, time_vector_[task_id]);
        }
      }

      double finish_time = std::numeric_limits<double>::max();
//      size_t finish_time = std::numeric_limits<size_t>::max();

      if (start_time != std::numeric_limits<double>::max()
          && read_time != std::numeric_limits<double>::max()) {
        // && write_time != std::numeric_limits<double>::max()) {
//        finish_time = start_time + std::ceil(read_time) + std::ceil(run_time) + std::ceil(write_time);
        finish_time = start_time + read_time + run_time + write_time;
        // finish_time = start_time + read_time + run_time;
      } else {
//        DLOG(INFO) << "start_time: " << start_time;
//        DLOG(INFO) << "read_time: " << read_time;
        // DLOG(INFO) << "write_time: " << write_time;
//        DLOG(INFO) << "run_time: " << run_time;
//        DLOG(INFO) << "finish_time: " << finish_time;
//        DLOG(INFO) << "objective_function_: " << makespan_;

        return finish_time;  // return "infinity"
      }

//      DLOG(INFO) << "start_time: " << start_time;
//      DLOG(INFO) << "read_time: " << read_time;
//      DLOG(INFO) << "write_time: " << write_time;
//      DLOG(INFO) << "run_time: " << run_time;
//      DLOG(INFO) << "finish_time: " << finish_time;

      // auto finish_time = start_time + read_time + run_time + write_time;

      // Update structures
      time_vector_[id_task] = finish_time;
      // start_time_vector_[id_task] = start_time;
      execution_vm_queue_[vm->get_id()] = finish_time;
//      allocation_vm_queue_[vm->get_id()] = std::max(finish_time, allocation_vm_queue_[vm->get_id()]);
      allocation_vm_queue_[vm->get_id()] = finish_time;
    } else {  // Source and Target tasks
      if (id_task == algorithm_->get_id_source()) {  // Source task
        time_vector_[id_task] = 0;
      } else {  // Target task
        double max_value = 0.0;
//        size_t max_value = 0UL;

        // for (auto task : algorithm_->get_predecessors().find(id_task)->second) {
        for (auto task: algorithm_->GetPredecessors(id_task)) {
          max_value = std::max<double>(max_value, time_vector_[task]);
        }

        time_vector_[id_task] = max_value;
      }  // } else {  // Target task
    }  // } else {  // Source and Target tasks
  }  // for (auto id_task : ordering_) {  // For each task, do

  makespan = time_vector_[algorithm_->get_id_target()];

  return makespan;
}  // double Solution::ComputeMakespan() {

/* Checks the sequence of tasks is valid */
bool Solution::CheckTaskSequence(size_t task) {
  // for (auto tk : algorithm_->get_predecessors().find(task)->second) {
//  for (auto tk: algorithm_->GetPredecessors(task)) {
//    if (time_vector_[tk] == std::numeric_limits<size_t>::max()) {
//      return false;
//    }
//  }
  auto v = algorithm_->GetPredecessors(task);
  bool rv = std::all_of(v.cbegin(), v.cend(), [this](size_t tk) {
    if (time_vector_[tk] == std::numeric_limits<double>::max()) {
      return false;
    }
    return true;
  });
//  return true;
  return rv;
}  // bool Solution::CheckTaskSequence(size_t task) {

// Check and organize the file based on the storage capacity
bool Solution::checkFiles() {
  bool flag = true;
  size_t count = 0;

  std::vector<double> aux_storage(algorithm_->get_storage_vet());
  std::vector<size_t> aux(algorithm_->GetVirtualMachineSize());
  iota(aux.begin(), aux.end(), 0);  // 0,1,2,3,4 ... n

  std::unordered_map<size_t, std::vector<size_t>> map_file;

  size_t id_storage;
  // build file map and compute the storage
  // for (auto it : data->file_map) {
  // for (auto it : algorithm_->get_file_map_per_id()) {
  for (size_t it = 0ul; it < algorithm_->GetFileSize(); ++it) {
    std::shared_ptr<File> file = algorithm_->GetFilePerId(it);

    if (std::shared_ptr<DynamicFile> dynamic_file = std::dynamic_pointer_cast<DynamicFile>(file)) {
      id_storage = file_allocations_[dynamic_file->get_id()];
      auto f = map_file.insert(std::make_pair(id_storage, std::vector<size_t>()));
      f.first->second.push_back(dynamic_file->get_id());
      aux_storage[id_storage] -= dynamic_file->get_size_in_GB();
    }
  }

  do {
    // Sort machines based on the storage capacity
    std::sort(aux.begin(), aux.end(), [&](const size_t& a, const size_t& b) {
      return aux_storage[a] < aux_storage[b];
    });

    if (aux_storage[aux[0ul]] < 0.0) {  // If storage is full, start the file heuristic
      std::cout << "Starting file heuristic ..." << std::endl;
      size_t old_vm = aux[0];  // Critical machine
      size_t new_storage = aux[aux.size() - 1];  // Best machine

      auto vet_file = map_file.find(old_vm)->second;

      double min = std::numeric_limits<double>::max();
      // size_t min_file = -1;
      size_t min_file = std::numeric_limits<size_t>::max();

      // Search min file (based on the size of file)
      for_each(vet_file.begin(), vet_file.end(), [&](size_t i) {
        std::cout << i << std::endl;
        std::shared_ptr<File> file = algorithm_->GetFilePerId(i);
        std::cout << file->get_name() << std::endl;
        if (file->get_size_in_GB() < min) {
          min = file->get_size_in_GB();
          min_file = file->get_id();
        }
      });

      std::shared_ptr<File> file_min = algorithm_->GetFilePerId(min_file);

      std::cout << file_min->get_name() << std::endl;
      // MinFile will be moved to machine with more empty space
      file_allocations_[file_min->get_id()] = new_storage;
      file_min->AddStorage(storages_[new_storage]);
      // Update aux Storage
      aux_storage[old_vm] += file_min->get_size();
      aux_storage[new_storage] -= file_min->get_size();
      // Update mapFile structure
      map_file[old_vm].erase(remove(map_file[old_vm].begin(), map_file[old_vm].end(), min_file),
                             map_file[old_vm].end());
      map_file[new_storage].push_back(min_file);
    } else {
      flag = false;
    }

    count++;
  } while (flag && count < algorithm_->GetFileSize());
  return !flag;
}  // bool Solution::checkFiles() {

std::ostream& Solution::write(std::ostream& os) const {
  DLOG(INFO) << "Writing solution information";
  google::FlushLogFiles(google::INFO);
  os << std::endl;
  os << "################################################################################################" << std::endl;
  os << algorithm_->GetName() << std::endl;
  os << "################################################################################################" << std::endl;
  os << "makespan_: " << makespan_ << " seconds" << std::endl;
  os << "makespan_: " << makespan_ / 60.0 << " minutes" << std::endl;
  os << "cost_: " << cost_ << std::endl;
  os << "security_exposure_: " << security_exposure_ << std::endl;
  os << "objective_value_: " << objective_value_ << std::endl;
  os << "Maximum security and privacy exposure: "
     << algorithm_->get_maximum_security_and_privacy_exposure()
     << std::endl;

  for (size_t i = 0ul; i < allocation_vm_queue_.size(); ++i) {
    os << "a[" << i << "]: \t" << allocation_vm_queue_[i] << "\t";
  }
  os << std::endl;

  for (size_t i = 0ul; i < execution_vm_queue_.size(); ++i) {
    os << "x[" << i << "]: \t" << execution_vm_queue_[i] << "\t";
  }
  os << std::endl;

  os << std::endl;
  os << "Activations: " << std::endl;
  for (size_t i = 0ul; i < algorithm_->GetVirtualMachineSize(); ++i) {
    std::shared_ptr<VirtualMachine> vm = algorithm_->GetVirtualMachinePerId(i);

    os << vm->get_id() << ": ";
    for (size_t j = 1ul; j < algorithm_->GetTaskSize() - 1; ++j) {
      std::shared_ptr<Activation> task = algorithm_->GetActivationPerId(j);
      size_t virtual_machine_id = activation_allocations_[j];

      if (virtual_machine_id == vm->get_id()) {
        os << task->get_tag() << " ";
      }
    }
    os << std::endl;
  }

  os << std::endl;
  os << "Activations (2): " << std::endl;
  DLOG(INFO) << "Activations (2)";
  google::FlushLogFiles(google::INFO);
  for (auto& vm : virtual_machines_) {
    os << vm->get_id() << ": ";
    auto activation = vm->GetFirstActivation().lock();
    DLOG(INFO) << "Activations (2)";
    google::FlushLogFiles(google::INFO);
    while (activation) {
      DLOG(INFO) << "VM[" << vm->get_id() << "] activation: " << *activation;
      google::FlushLogFiles(google::INFO);
      os << activation->get_tag() << " ";
      activation = activation->GetNextVmActivation().lock();
    }
    os << std::endl;
  }

//  for (size_t i = 0ul; i < algorithm_->GetVirtualMachineSize(); ++i) {
//    std::shared_ptr<VirtualMachine> vm = algorithm_->GetVirtualMachinePerId(i);
//    os << vm->get_id() << ": ";
//    auto activation = vm->GetFirstActivation();
//    while (activation) {
//      os << activation->get_tag() << " ";
//      activation = activation->GetNextVmActivation();
//      std::cout << "Deu ruim!!!!" << std::endl;
//    }
//
//    for (size_t j = 1ul; j < algorithm_->GetTaskSize() - 1; ++j) {
//      std::shared_ptr<Activation> task = algorithm_->GetActivationPerId(j);
//      size_t virtual_machine_id = activation_allocations_[j];
//
//      if (virtual_machine_id == vm->get_id()) {
//        os << task->get_tag() << " ";
//      }
//    }
//    os << std::endl;
//  }

  os << std::endl;
  os << "Files: " << std::endl;
  for (size_t i = 0ul; i < algorithm_->GetStorageSize(); ++i) {
    std::shared_ptr<Storage> vm = algorithm_->GetStoragePerId(i);

    os << vm->get_id() << ": \n";
    for (size_t j = 0ul; j < algorithm_->GetFileSize(); ++j) {
      auto file = algorithm_->GetFilePerId(j);
      size_t storage_id;

//      if (std::shared_ptr<StaticFile> static_file = std::dynamic_pointer_cast<StaticFile>(file)) {
//        storage_id = static_file->GetFirstVm();
//      } else {
//        storage_id = file_allocations_[file->get_id()];
//      }
      storage_id = file->GetStorageId();

      if (storage_id == vm->get_id()) {
        os << " " << file->get_name() << "\n";
      }
    }
    os << std::endl;
  }

  os << std::endl;
  os << "Activation Sequence: " << std::endl;
  for (auto task_id: ordering_) {
//    if (task_id != algorithm_->get_id_source()
//        && task_id != algorithm_->get_id_target()) {
      os << algorithm_->GetActivationPerId(task_id)->get_name() << ", ";
//    }
  }
  os << std::endl;
  os << "################################################################################################" << std::endl;
  os << algorithm_->GetName() << std::endl;
  os << "################################################################################################" << std::endl;
  return os;
}  // std::ostream& Solution::write(std::ostream& os) const {

double Solution::AllocateOneOutputFileGreedily(const std::shared_ptr<File>& file,
                                               const std::shared_ptr<VirtualMachine>& virtual_machine) {
  double objective_value;
  double allocated_objective_value = std::numeric_limits<double>::max();
  double allocation_time = std::numeric_limits<double>::max();
  double allocation_cost = std::numeric_limits<double>::max();
  double allocation_security_exposure = std::numeric_limits<double>::max();
  size_t allocated_storage = std::numeric_limits<size_t>::max();

  DLOG(INFO) << "Computing time for write the File[" << file->get_id() << "] into VM["
             << virtual_machine->get_id() << "]";

  // std::unordered_map<size_t, int> available_storages;

  std::vector<size_t> available_storages = std::vector<size_t>(algorithm_->GetStorageSize());

  for (size_t i = 0ul; i < available_storages.size(); ++i) {
    available_storages[i] = i;
  }

  // Shuffle the output files for better randomness between the solutions
  std::shuffle(available_storages.begin(), available_storages.end(), generator());

  // for all possible storage; compute the transfer time
  // size_t attempts = 0;
  for (size_t i = 0ul; i < available_storages.size(); ++i) {
    // std::shared_ptr<Storage> storage = algorithm_->GetStoragePerId(i);
    std::shared_ptr<Storage> storage = algorithm_->GetStoragePerId(available_storages[i]);
    // 1. Calculates the File Transfer Time
    double time = ComputeFileTransferTime(file, virtual_machine, storage, true);

    // if (objective_value < std::numeric_limits<double>::max()) {
    if (time == std::numeric_limits<double>::max()) {
      // ++attempts;
      continue;  // Hard-constraint
    }

    // 2. Calculates the File Contribution to the Cost
    // double cost = ComputeFileCostContribution(file, virtual_machine, storage_pair.second, time);
    double cost = ComputeFileCostContribution(file, storage, virtual_machine,
                                              aux_start_time + aux_read_time + aux_runtime + time);
    // double cost = 0.0;

    // 3. Calculates the File Security Exposure Contribution
    // double security_exposure = ComputeFileSecurityExposureContribution(storage_pair.second,
    //                                                                    file);
    double security_exposure = ComputeFileSecurityExposureContribution(storage, file);
    // double security_exposure = Solution::ComputeSecurityExposure();
    // double security_exposure = 0.0;

    DLOG(INFO) << "time: " << time;
    DLOG(INFO) << "cost: " << cost;
    DLOG(INFO) << "security_exposure: " << security_exposure;

    // Normalize
    // objective_value = time;
    // else {
    //   min_objective_value = objective_value;
    //   min_storage = i;
    //   // break;
    // }

    objective_value = algorithm_->get_alpha_time() * (time / algorithm_->get_makespan_max())
        + algorithm_->get_alpha_budget() * (cost / algorithm_->get_budget_max())
        + algorithm_->get_alpha_security() * (security_exposure
            /
                algorithm_->get_maximum_security_and_privacy_exposure());

    if (allocated_objective_value > objective_value) {
      allocated_objective_value = objective_value;
      // min_storage = i;
      // allocated_storage = available_storages[i];
      allocated_storage = storage->get_id();
      allocation_time = time;
      allocation_cost = cost;
      allocation_security_exposure = security_exposure;

      if (i >= FLAGS_number_of_allocation_experiments - 1) {
        break;
      }
      // break;
    }

    // if (attempts >= FLAGS_number_of_allocation_experiments) {
    // break;
    // } else {
    // ++attempts;
    // }
  }  // for (size_t i = 0ul; i < available_storages.size(); ++i) {

  if (allocated_objective_value == std::numeric_limits<double>::max()) {
    LOG(FATAL) << "There is no storage available";
  }

  DLOG(INFO) << "Allocation of the output File[" << file->get_id() << "] to the Storage["
             << allocated_storage << "]";

  // Allocate file
  file_allocations_[file->get_id()] = allocated_storage;
  file->AddStorage(storages_[allocated_storage]);

  // Store the file contribution to the makespan, cost and security expose
  // makespan_ += allocation_time;
  cost_ += allocation_cost;
  security_exposure_ += allocation_security_exposure;
  // security_exposure_ = allocation_security_exposure;

  if (allocated_storage < algorithm_->GetVirtualMachineSize()) {
    allocation_vm_queue_[allocated_storage] = std::max<double>(aux_start_time + aux_read_time
                                                                   + aux_runtime + allocation_time,
                                                               allocation_vm_queue_[allocated_storage]);
  }

  // return min_objective_value;
  return allocation_time;
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
 * \f$ time \f$ is the maximum finish time of all previous task of the task identified by \c activation_id
 * \f$ fitness_{previous_task_id}} \f$ is the finish time of the previous task
 *
 * \param[in]  activation_id          Activation id for which we want to find the start time
 * \param[in]  vm_id            VM where the task will be executed
 * \retval     start_time       The time at the task identified by \c activation_id will start executing
 */
double Solution::ComputeActivationStartTime(size_t activation_id, size_t vm_id) {
//size_t Solution::ComputeActivationStartTime(size_t activation_id, size_t vm_id) {
//  size_t start_time = 0UL;
  double start_time = 0.0;

//  DLOG(INFO) << "Compute the start time of the Activation[" << activation_id << "] at VM[" << vm_id << "]";

  // for (auto previous_task_id : algorithm_->get_predecessors().find(activation_id)->second) {
  for (auto previous_task_id: algorithm_->GetPredecessors(activation_id)) {
    start_time = std::max<double>(start_time, time_vector_[previous_task_id]);
  }

//  DLOG(INFO) << "start_time: " << start_time;

//  return std::max(start_time, execution_vm_queue_[vm_id]);
  return std::max<double>(std::ceil(start_time), execution_vm_queue_[vm_id]);
}  // double MinMinAlgorithm::ComputeStartTime(...)

double Solution::AllocateOutputFiles(const std::shared_ptr<Activation>& task,
                                     const std::shared_ptr<VirtualMachine>& vm) {
  double write_time = 0.0;

  std::vector<std::shared_ptr<File>> my_files = task->get_output_files();

  // Shuffle the output files for better randomness between the solutions
  if (my_files.size() > 1) {
    std::shuffle(my_files.begin(), my_files.end(), generator());
  }

  // For each output file allocate the storage that impose the minor write time
  for (auto& file: my_files) {
    // write_time += std::ceil(AllocateOneOutputFileGreedily(file, vm));
    if (file_allocations_[file->get_id()] == std::numeric_limits<size_t>::max()) {
      write_time += AllocateOneOutputFileGreedily(file, vm);
    } else {
      std::shared_ptr<Storage> storage = algorithm_->GetStoragePerId(file_allocations_[file->get_id()]);

      write_time += ComputeFileTransferTime(file, vm, storage);
    }
  }

  return write_time;
}

double Solution::ComputeTaskReadTime(const std::shared_ptr<Activation>& activation,
                                     const std::shared_ptr<VirtualMachine>& vm) {
  double read_time = 0.0;

//  DLOG(INFO) << "Compute Read Time of the Activation[" << activation->get_id() << "] at VM[" << vm->get_id()
//             << "]";

  for (const auto& file: activation->get_input_files()) {
    size_t storage_id;

    if (std::shared_ptr<StaticFile> static_file = std::dynamic_pointer_cast<StaticFile>(file)) {
      storage_id = static_file->GetFirstVm();
//      storage_id = static_file->GetStorageId();
      DLOG(INFO) << "Static file [" << static_file->get_id() <<"]";
      google::FlushLogFiles(google::INFO);
    } else {
      storage_id = file_allocations_[file->get_id()];
      DLOG(INFO) << "Dynamic file [" << file->get_id() << "]";
      google::FlushLogFiles(google::INFO);
    }

//    storage_id = file->GetStorageId();

    if (storage_id == std::numeric_limits<size_t>::max()) {
      LOG(FATAL) << "Wrong storage_id";
    }

//    storage_id = file->GetStorageId();

    std::shared_ptr<Storage> file_vm = algorithm_->GetStoragePerId(storage_id);
//    std::shared_ptr<Storage> file_vm = storage->GetStoragePerId(storage_id);

    // Ceil of File Transfer Time + File Size * lambda
    double one_file_read_time = ComputeFileTransferTime(file, file_vm, vm);

    if (one_file_read_time == std::numeric_limits<double>::max()) {
      DLOG(INFO) << "read_time: " << one_file_read_time;
      google::FlushLogFiles(google::INFO);
      return one_file_read_time;
    } else {
      // read_time += std::ceil(one_file_read_time);
      read_time += one_file_read_time;
      if (storage_id < algorithm_->GetVirtualMachineSize() and storage_id != vm->get_id()) {
        double diff = (aux_start_time + read_time) - allocation_vm_queue_[storage_id];

        if (diff > 0.0) {
          std::shared_ptr<VirtualMachine> virtual_machine = algorithm_->GetVirtualMachinePerId(storage_id);

          cost_ += diff * virtual_machine->get_cost();
          allocation_vm_queue_[storage_id] = aux_start_time + read_time;
          // std::cout << "diff: " << diff << " virtual_machine->get_cost(): " << virtual_machine->get_cost()
          //           << " storage_id: " << storage_id << " vm: " << vm->get_id() << std::endl;
        }
      }
      // allocation_vm_queue_[storage_id] = std::max(aux_start_time
      //     + one_file_read_time, allocation_vm_queue_[storage_id]);
    }
  }  // for (std::shared_ptr<File> file : activation.get_input_files()) {

//  DLOG(INFO) << "read_time: " << read_time;

  return read_time;
}  // double Solution::ComputeTaskReadTime(Activation& activation, VirtualMachine& vm) {

double Solution::ComputeTaskReadTimeOther(const std::shared_ptr<Activation>& task,
                                          const std::shared_ptr<VirtualMachine>& vm) {
  double read_time = 0.0;

//  DLOG(INFO) << "Compute Read Time of the Activation[" << task->get_id() << "] at VM[" << vm->get_id()
//             << "]";

  for (const auto& file: task->get_input_files()) {
    size_t storage_id;

//    if (std::shared_ptr<StaticFile> static_file = std::dynamic_pointer_cast<StaticFile>(file)) {
//      storage_id = static_file->GetFirstVm();
//    } else {
//      storage_id = file_allocations_[file->get_id()];
//    }
    storage_id = file->GetStorageId();

    std::shared_ptr<Storage> file_vm = algorithm_->GetStoragePerId(storage_id);

    // Ceil of File Transfer Time + File Size * lambda
    double one_file_read_time = ComputeFileTransferTime(file, file_vm, vm);

    if (one_file_read_time == std::numeric_limits<double>::max()) {
      DLOG(INFO) << "read_time: " << one_file_read_time;
      return one_file_read_time;
    } else {
      // read_time += std::ceil(one_file_read_time);
      read_time += one_file_read_time;
      if (storage_id < algorithm_->GetVirtualMachineSize() and storage_id != vm->get_id()) {
        double diff = (aux_start_time + read_time) - execution_vm_queue_[storage_id];

        if (diff > 0.0) {
          std::shared_ptr<VirtualMachine> virtual_machine = algorithm_->GetVirtualMachinePerId(storage_id);

//          cost_ += diff * virtual_machine->get_cost();
          allocation_vm_queue_[storage_id] = aux_start_time + read_time;
//          execution_vm_queue_[storage_id] = aux_start_time + read_time;
          // std::cout << "diff: " << diff << " virtual_machine->get_cost(): " << virtual_machine->get_cost()
          //           << " storage_id: " << storage_id << " vm: " << vm->get_id() << std::endl;
        }
      }
      // allocation_vm_queue_[storage_id] = std::max(aux_start_time
      //     + one_file_read_time, allocation_vm_queue_[storage_id]);
    }
  }  // for (std::shared_ptr<File> file : task.get_input_files()) {

//  DLOG(INFO) << "read_time: " << read_time;

  return read_time;
}  // double Solution::ComputeTaskReadTime(Activation& task, VirtualMachine& vm) {

/**
 * This method is important for calculate de makespan and to allocate the output files into
 * Storages(VMs and Buckets)
 *
 * \param[in]  activation             Activation with which the output files will be allocated
 * \param[in]  virtual_machine  VM where the activation will be executed
 * \retval     makespan         The objective value of the solution when inserting the \c activation
 */
double Solution::CalculateMakespanAndAllocateOutputFiles(const std::shared_ptr<Activation>& activation,
                                                         const std::shared_ptr<VirtualMachine>& virtual_machine) {
  double start_time = 0.0;
//  size_t start_time = 0.0;
  double read_time = 0.0;
  double write_time = 0.0;
  double run_time;
//  size_t makespan = std::numeric_limits<size_t>::max();
  double makespan = std::numeric_limits<double>::max();

  DLOG(INFO) << "Makespan of the allocated Activation[" << activation->get_id() << "] at VM["
      << virtual_machine->get_id() << "]";
  google::FlushLogFiles(google::INFO);

  run_time = ceil(activation->get_time() * virtual_machine->get_slowdown());
  aux_runtime = run_time;

  if (activation->get_id() != algorithm_->get_id_source()
      && activation->get_id() != algorithm_->get_id_target()) {
    start_time = ComputeActivationStartTime(activation->get_id(), virtual_machine->get_id());
//    aux_start_time = start_time;
    aux_start_time = start_time;
    read_time = ComputeTaskReadTime(activation, virtual_machine);
    aux_read_time = read_time;
    write_time = AllocateOutputFiles(activation, virtual_machine);
  } else if (activation->get_id() == algorithm_->get_id_target()) {
    for (auto task_id: algorithm_->GetPredecessors(activation->get_id())) {
      start_time = std::max<double>(start_time, time_vector_[task_id]);
    }
  }
// run_time = activation->get_time() * virtual_machine->get_slowdown();

  if (start_time != std::numeric_limits<double>::max()
      && read_time != std::numeric_limits<double>::max()
      && write_time != std::numeric_limits<double>::max()) {
    makespan = start_time + read_time + run_time + write_time;
//    makespan = start_time + std::ceil(read_time) + std::ceil(run_time) + std::ceil(write_time);
    // makespan = start_time + read_time + run_time;
  } else {
    DLOG(FATAL) << "Something is wrong!";
    google::FlushLogFiles(google::INFO);
  }

  DLOG(INFO) << "start_time: " << start_time;
  DLOG(INFO) << "read_time: " << read_time;
  DLOG(INFO) << "write_time: " << write_time;
  DLOG(INFO) << "run_time: " << run_time;
  DLOG(INFO) << "makespan: " << makespan;
  google::FlushLogFiles(google::INFO);

  // Store the start time of the activation
  // start_time_vector_[activation->get_id()] = start_time;

  return makespan;
}  // double Solution::CalculateMakespanAndAllocateOutputFiles(...)

double Solution::ComputeCost() {
  double cost;
  double virtual_machine_cost = 0.0;
//  double bucket_fixed_cost = 0.0;
  double bucket_variable_cost = 0.0;

//  DLOG(INFO) << "Calculate Cost";

  // Accumulate the Virtual Machine cost
  for (size_t i = 0ul; i < algorithm_->GetVirtualMachineSize(); ++i) {
    std::shared_ptr<VirtualMachine> virtual_machine = algorithm_->GetVirtualMachinePerId(i);
    auto max_time = static_cast<double>(allocation_vm_queue_[virtual_machine->get_id()]);
//    double max_time = static_cast<double>(std::max<size_t>(execution_vm_queue_[virtual_machine->get_id()],
//                                                           allocation_vm_queue_[virtual_machine->get_id()]));
    virtual_machine_cost += max_time * virtual_machine->get_cost();
  }

  // Accumulate the Bucket fixed cost
//  for (std::pair<size_t, Storage> storage_vm: algorithm_->get_storage_vet()) {
//    Storage storage = storage_vm.second;
//  for (size_t i = 0ul; i < algorithm_->GetStorageSize(); ++i) {
//    Storage *storage = algorithm_->GetStoragePerId(i);
//    // If the storage is not a Virtual Machine, i.e. it is a Bucket; then verify that it is been
//    // used
//    if (storage->get_id() >= algorithm_->GetVirtualMachineSize()) {
//      for (size_t i = algorithm_->GetTaskSize();
//           i < algorithm_->GetTaskSize() + algorithm_->GetFileSize();
//           ++i) {
//        // If the Bucket is used; then accumulate de cost and break to the next Storage
//        if (activation_allocations_[i] == storage->get_id()) {
//          bucket_fixed_cost += storage->get_cost();
//          break;
//        }
//      }
//    }
//  }

  // Accumulate the Bucket variable cost
  // for (std::pair<size_t, std::shared_ptr<Storage>> storage_vm : algorithm_->get_storage_map()) {
  for (size_t i = algorithm_->GetVirtualMachineSize(); i < algorithm_->GetStorageSize(); ++i) {
    std::shared_ptr<Storage> storage = algorithm_->GetStoragePerId(i);

    // If the storage is not a Virtual Machine, i.e. it is a Bucket; then calculate the bucket
    // variable cost
//    if (storage->get_id() >= algorithm_->GetVirtualMachineSize()) {
    for (size_t j = 0ul; j < algorithm_->GetFileSize(); ++j) {
      // If the Bucket is used; then accumulate de cost and break to the next Storage
      if (file_allocations_[j] == storage->get_id()) {
        std::shared_ptr<File> file = algorithm_->GetFilePerId(j);

        bucket_variable_cost += (storage->get_cost() * file->get_size_in_GB());
      }
    }
//    }
  }

//  DLOG(INFO) << "virtual_machine_cost: " << virtual_machine_cost;
  // DLOG(INFO) << "bucket_fixed_cost: " << bucket_fixed_cost;
//  DLOG(INFO) << "bucket_variable_cost: " << bucket_variable_cost;

  // cost = virtual_machine_cost + bucket_fixed_cost + bucket_variable_cost;
  cost = virtual_machine_cost + bucket_variable_cost;

//  DLOG(INFO) << "cost: " << cost;

  return cost;
}  // double Solution::CalculateCost() {

double Solution::ComputeFileCostContribution(const std::shared_ptr<File>& file,
                                             const std::shared_ptr<Storage>& storage,
                                             const std::shared_ptr<VirtualMachine>& virtual_machine,
                                             double time) {
  double cost = 0.0;
  // double virtual_machine_cost = 0.0;
  // double bucket_fixed_cost = 0.0;
  // double bucket_variable_cost = 0.0;

//  DLOG(INFO) << "Calculate File Cost";

  if (std::shared_ptr<VirtualMachine> vm = std::dynamic_pointer_cast<VirtualMachine>(storage)) {
    if (vm->get_id() != virtual_machine->get_id()) {
      double diff = time - allocation_vm_queue_[vm->get_id()];

      if (diff > 0.0) {
        cost += diff * vm->get_cost();
      }

      // DLOG(INFO) << "virtual_machine_cost: " << virtual_machine_cost;
    }
  } else {
    // Bucket cost
    if (storage->get_id() >= algorithm_->GetVirtualMachineSize()) {
      // bucket_variable_cost += storage->get_cost() * file->get_size();
      cost += storage->get_cost() * file->get_size_in_GB();
      // std::cout << file->get_size_in_GB() << " " << storage->get_cost() << " " << cost << std::endl;
    }

    // DLOG(INFO) << "bucket_variable_cost: " << bucket_variable_cost;
  }

  // cost = virtual_machine_cost + bucket_variable_cost;

//  DLOG(INFO) << "cost: " << cost;

  return cost;
}  // double Solution::CalculateCost() {

double Solution::ComputeFileSecurityExposureContribution(const std::shared_ptr<Storage>& storage,
                                                         const std::shared_ptr<File>& file) {
  double security_exposure;
  // double task_exposure = 0.0;
  double privacy_exposure = 0.0;

//  DLOG(INFO) << "Calculate Security Exposure";

  // Accumulate the privacy_exposure
  for (size_t i = 0ul; i < algorithm_->GetFileSize(); ++i) {
    size_t storage1_id = file_allocations_[i];

    if (storage1_id != std::numeric_limits<size_t>::max()
        && storage->get_id() != std::numeric_limits<size_t>::max()
        && storage1_id == storage->get_id()) {
      // int conflict_value = algorithm_->get_conflict_graph().ReturnConflict(i, file->get_id());
      int conflict_value = algorithm_->get_conflict_graph().ReturnConflict(file->get_id(), i);

      if (conflict_value > 0) {
        DLOG(INFO) << "File[" << i << "] has conflict with File[" << file->get_id() << "]";
        privacy_exposure += conflict_value;  // Adds the penalts
      }
    }  // else; allocated file resides in different storage or was not allocated yet
  }  // for (size_t i...) {

  // DLOG(INFO) << "task_exposure: " << task_exposure;
//  DLOG(INFO) << "privacy_exposure: " << privacy_exposure;

  security_exposure = privacy_exposure;

  DLOG(INFO) << "security_exposure: " << security_exposure;

  return security_exposure;
}  // double Solution::CalculateSecurityExposure() {

double Solution::ComputeSecurityExposure() {
  double security_exposure;
  double task_exposure = 0.0;
  double privacy_exposure = 0.0;

//  DLOG(INFO) << "Calculate Security Exposure";

  // Accumulate the task exposure
  for (size_t i = 0ul; i < algorithm_->GetTaskSize(); ++i) {
    std::shared_ptr<Activation> task = algorithm_->GetActivationPerId(i);

    // for (double requirement_value : task.get_requirements()) {
    for (size_t j = 0; j < task->get_requirements().size(); ++j) {
      size_t virtual_machine_id = activation_allocations_[task->get_id()];

      // If the task is allocated
      if (virtual_machine_id != std::numeric_limits<size_t>::max()) {
        std::shared_ptr<VirtualMachine> virtual_machine = algorithm_->GetVirtualMachinePerId(virtual_machine_id);

        if (task->GetRequirementValue(j) > virtual_machine->GetRequirementValue(j)) {
          task_exposure += task->GetRequirementValue(j) - virtual_machine->GetRequirementValue(j);
        }
      }
    }
  }

  // Accumulate the privacy_exposure
  for (size_t i = 0ul; i < algorithm_->GetFileSize() - 1; ++i) {
    const size_t storage1_id = file_allocations_[i];

    for (size_t j = i + 1ul; j < algorithm_->GetFileSize(); ++j) {
      const size_t storage2_id = file_allocations_[j];

      if (storage1_id != std::numeric_limits<size_t>::max()
          && storage2_id != std::numeric_limits<size_t>::max()
          && storage1_id == storage2_id) {
        const int conflict_value = algorithm_->get_conflict_graph().ReturnConflict(i, j);

        if (conflict_value > 0) {
          DLOG(INFO) << "File[" << i << "] has conflict with File[" << j << "]";
          privacy_exposure += conflict_value;  // Adds the penalts
        }
      }  // else; allocated file resides in different storage or was not allocated yet
    }  // for (size_t j ...) {
  }  // for (size_t i...) {


//  DLOG(INFO) << "task_exposure: " << task_exposure;
//  DLOG(INFO) << "privacy_exposure: " << privacy_exposure;

  security_exposure = task_exposure + privacy_exposure;

//  DLOG(INFO) << "security_exposure: " << security_exposure;

  return security_exposure;
}  // double Solution::CalculateSecurityExposure() {

/**
 * Insert the activation in the solution and calculate its objective value.
 *
 * Calculates:
 * 1. makespan
 * 2. cost
 * 3. security exposure
 * And then, return the sum of them.
 *
 * \param[in]  activation             Activation for which we want to find the fitness
 * \param[in]  vm               VM where the activation will be executed
 * \retval     objective_value  The objective value of the solution when inserting the \c activation
 */
double Solution::ScheduleTask(const std::shared_ptr<Activation>& activation,
                              const std::shared_ptr<VirtualMachine>& vm) {
  // double objective_value = std::numeric_limits<double>::max();

  DLOG(INFO) << "Schedule the Activation[" << activation->get_id() << "] at VM[" << vm->get_id() << "]";
  google::FlushLogFiles(google::INFO);

  // Update activation VM information - needs to be first than updating VM
  auto my_activation = activations_[activation->get_id()];
  my_activation->SetVm(virtual_machines_[vm->get_id()]);

  //

  // Allocate Activation
  activation_allocations_[activation->get_id()] = vm->get_id();
//  std::cout << ordering_.size() << std::endl;
  ordering_.push_back(activation->get_id());

  // 1. Calculates the makespan
  double makespan = CalculateMakespanAndAllocateOutputFiles(activation, vm);

  makespan_ = std::max<double>(makespan_, makespan);

  // Update auxiliary structures (queue_ and time_vector_)
  // This update is important for the cost calculation
  time_vector_[activation->get_id()] = makespan_;
  double queue_diff = makespan_ - execution_vm_queue_[vm->get_id()];

  // for (size_t i = 0ul; i < algorithm_->GetVirtualMachineSize(); ++i) {
  //   double time = allocation_vm_queue_[i];

  //   if (time > makespan_) {
  //     std::shared_ptr<VirtualMachine> virtual_machine = algorithm_->GetVirtualMachinePerId(i);
  //     double diff = time - makespan_;

  //     cost_ += diff * virtual_machine->get_cost();
  //     std::cout << "Buga buga buga buga!!!!!!" << std::endl;
  //   }
  // }

  execution_vm_queue_[vm->get_id()] = makespan_;
  allocation_vm_queue_[vm->get_id()] = std::max<double>(makespan_, allocation_vm_queue_[vm->get_id()]);

  // 2. Calculates the cost contribution of the activation execution at the virtual machine

  DLOG(INFO) << "Accumulate the Virtual Machine Cost of the scheduled activation";
  google::FlushLogFiles(google::INFO);

  // Accumulate the Virtual Machine cost
  cost_ += queue_diff * vm->get_cost();

  // 3. Calculates the security exposure

  DLOG(INFO) << "Accumulate the activation Exposure of the scheduled activation";
  google::FlushLogFiles(google::INFO);

  // Accumulate the activation exposure

  for (size_t j = 0; j < activation->get_requirements().size(); ++j) {
    size_t virtual_machine_id = activation_allocations_[activation->get_id()];

    // If the activation is allocated
    if (virtual_machine_id != std::numeric_limits<size_t>::max()) {
      std::shared_ptr<VirtualMachine> virtual_machine = algorithm_->GetVirtualMachinePerId(virtual_machine_id);

      if (activation->GetRequirementValue(j) > virtual_machine->GetRequirementValue(j)) {
        security_exposure_ += activation->GetRequirementValue(j) - virtual_machine->GetRequirementValue(j);
      }
    }
  }

  DLOG(INFO) << "makespan: " << makespan_;
  DLOG(INFO) << "cost: " << cost_;
  DLOG(INFO) << "security_exposure: " << security_exposure_;
  google::FlushLogFiles(google::INFO);

  if (makespan_ == std::numeric_limits<double>::max()
      || cost_ == std::numeric_limits<double>::max()
      || security_exposure_ == std::numeric_limits<double>::max()) {
    objective_value_ = std::numeric_limits<double>::max();
    return objective_value_;
  }

  objective_value_ = algorithm_->get_alpha_time() * (makespan_ / algorithm_->get_makespan_max())
      + algorithm_->get_alpha_budget() * (cost_ / algorithm_->get_budget_max())
      + algorithm_->get_alpha_security() * (security_exposure_
          / algorithm_->get_maximum_security_and_privacy_exposure());

  return objective_value_;
}  // double Solution::ScheduleTask(...)

int Solution::ComputeTasksHeights(size_t node) {
  int min = std::numeric_limits<int>::max();
  if (task_height_[node] != -1)
    return task_height_[node];
  if (node != algorithm_->get_id_target()) {
    for (auto i: algorithm_->GetSuccessors(node)) {
      int value = ComputeTasksHeights(i);
      min = std::min(value, min);
    }
  } else {
    task_height_[node] = algorithm_->get_height()[node];
    return task_height_[node];
  }
  std::uniform_int_distribution<> dis(algorithm_->get_height()[node], min - 1);
  task_height_[node] = dis(generator());
  return task_height_[node];
}  // int Solution::ComputeHeightSoft(int node) {

// =========== Begin of local search functions  ========= //

// N1 - Swap-vm
bool Solution::localSearchN1() {
  DLOG(INFO) << "Executing localSearchN1 local search ...";
//    Chromosome old_ch(ch);
//    Solution old_solution = solution;
//    for (int i = 0; i < data->size; i++) {
//        for (int j = i + 1; j < data->size; j++) {
//            if (ch.allocation[i] != ch.allocation[j]) {
//                //do the swap
//                iter_swap(ch.allocation.begin() + i, ch.allocation.begin() + j);
//                ch.computeFitness();
//                if (ch.fitness < old_ch.fitness) {
//                    return ch;
//                }
//                //return element
//                iter_swap(ch.allocation.begin() + i, ch.allocation.begin() + j);
//            }
//        }
//    }
//    std::vector<size_t> old_task_allocations = activation_allocations_;
  double best_known_objective_value = objective_value_;

  for (size_t i = 1; i < algorithm_->GetTaskSize() - 2; i++) {
    for (size_t j = i + 1; j < algorithm_->GetTaskSize() - 1; j++) {
      if (activation_allocations_[i] != activation_allocations_[j]) {
        // Do the swap
        iter_swap(activation_allocations_.begin() + static_cast<long int>(i),
                  activation_allocations_.begin() + static_cast<long int>(j));
        ObjectiveFunction(false, false);
        DLOG(INFO) << "... localSearchN1 : " << objective_value_ << " < " << best_known_objective_value
                   << std::endl;
        if (objective_value_ < best_known_objective_value) {
          DLOG(INFO) << "... localSearchN1 : " << objective_value_ << " < " << best_known_objective_value
                     << std::endl;
          DLOG(INFO) << "... ending localSearchN1 local search";
//                    best_known_objective_value = new_objective_value;
          return true;
        }
        // Return elements
        iter_swap(activation_allocations_.begin() + static_cast<long int>(i),
                  activation_allocations_.begin() + static_cast<long int>(j));
        ObjectiveFunction(false, false);
//        objective_value_ = best_known_objective_value;
      }
    }
  }
  DLOG(INFO) << "... ending localSearchN1 local search";
//    return old_ch;
  return false;
}  // bool Solution::localSearchN1() {

// N2 - Swap position
bool Solution::localSearchN2() {
  DLOG(INFO) << "Executing localSearchN2 local search ...";
//    Chromosome old_ch(ch);
  // for each task, do
//    for (int i = 0; i < data->task_size; i++) {
//        auto task_i = ch.ordering[i];
//        for (int j = i + 1; j < data->task_size; j++) {
//            auto task_j = ch.ordering[j];
//            if (ch.height_soft[task_i] == ch.height_soft[task_j]) {
//                //do the swap
//                iter_swap(ch.ordering.begin() + i, ch.ordering.begin() + j);
//                ch.computeFitness(false, true);
//                if (ch.fitness < old_ch.fitness) {
//                    return ch;
//                }
//                //return elements
//                iter_swap(ch.ordering.begin() + i, ch.ordering.begin() + j);
//            } else
//                break;
//        }
//    }

  double best_known_objective_value = objective_value_;

  // for each task, do
  for (size_t i = 0; i < algorithm_->GetTaskSize(); i++) {
    auto task_i = ordering_[i];
    for (size_t j = i + 1; j < algorithm_->GetTaskSize(); j++) {
      auto task_j = ordering_[j];
      if (task_height_[task_i] == task_height_[task_j]) {
        // Do the swap
        iter_swap(ordering_.begin() + static_cast<long int>(i), ordering_.begin() + static_cast<long int>(j));
        ObjectiveFunction(false, false);
        DLOG(INFO) << "new objective value " << objective_value_ << " i " << i << " j " << j << std::endl;
        if (objective_value_ < best_known_objective_value) {
          DLOG(INFO) << "... localSearchN2 : " << objective_value_ << " < " << best_known_objective_value
                     << std::endl;
          DLOG(INFO) << "... ending localSearchN2 local search";
          return true;
        }
        // Return elements
        iter_swap(ordering_.begin() + static_cast<long int>(i), ordering_.begin() + static_cast<long int>(j));
//        objective_value_ = best_known_objective_value;
        ObjectiveFunction(false, false);
      } else {
        break;
      }
    }
  }

  DLOG(INFO) << "... ending localSearchN2 local search";
//    return old_ch;
  return false;
}  // bool Solution::localSearchN2() {

// N3 = Move-1 Element
bool Solution::localSearchN3() {
  DLOG(INFO) << "Executing localSearchN3 local search ...";
//    Chromosome old_ch(ch);

  double best_known_objective_value = objective_value_;
//
  for (size_t i = 0; i < algorithm_->GetTaskSize(); ++i) {
    size_t old_vm = activation_allocations_[i];
    for (size_t j = 0; j < algorithm_->GetVirtualMachineSize(); j++) {
      if (old_vm != j) {
        activation_allocations_[i] = j;
        ObjectiveFunction(false, false);
        DLOG(INFO) << "new objective value " << objective_value_ << " i " << i << " j " << j << std::endl;
        if (objective_value_ < best_known_objective_value) {
          DLOG(INFO) << "... localSearchN3 : " << objective_value_ << " < " << best_known_objective_value
                     << std::endl;
          DLOG(INFO) << "... ending localSearchN3 local search";
          return true;
        }
      }
    }
    activation_allocations_[i] = old_vm;
    ObjectiveFunction(false, false);
//    objective_value_ = best_known_objective_value;
  }
  DLOG(INFO) << "... ending localSearchN3 local search";
//    return old_ch;
  return false;
}  // bool Solution::localSearchN3() {

// =========== End of local search functions  ========= //
