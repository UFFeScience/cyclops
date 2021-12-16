/**
 * \file src/model/activation.cc
 * \brief Contains the \c Activation class definition
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2021
 *
 * This source file contains the \c Activation class definition
 */

#include "src/model/activation.h"
#include "static_file.h"

double Activation::ComputeFileTransferTime(const std::shared_ptr<File>& file,
                                           const std::shared_ptr<Storage>& storage1,
                                           const std::shared_ptr<Storage>& storage2,
                                           bool check_constraints) {
  double time = std::numeric_limits<double>::max();

//  DLOG(DEBUG) << "Compute the transfer time of File[" << file->get_id() << "] to/from VM["
//      << storage1->get_id() << "] to Storage[" << storage2->get_id() << "], check_constraints["
//      << check_constraints << "]";

  if (check_constraints) {
    for (size_t i = 0ul; i < algorithm_->GetFileSize(); ++i) {
//      size_t storage_id = file_allocations_[i];
      size_t storage_id = file->GetStorageId();

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

double Activation::ComputeReadTime(std::shared_ptr<VirtualMachine> vm) {
  DLOG(INFO) << "Compute Read Time of the Activation[" << id_ << "] at VM[" << vm->get_id() << "]";

  double read_time = 0.0;

  for (const auto& file: get_input_files()) {
//    size_t storage_id;

//    if (std::shared_ptr<StaticFile> static_file = std::dynamic_pointer_cast<StaticFile>(file)) {
////      storage_id = static_file->GetFirstVm();
//      storage_id = static_file->GetStorageId();
//    } else {
////      storage_id = file_allocations_[file->get_id()];
//      storage_id.
//    }

//    storage_id = file->GetStorageId();

//    std::shared_ptr<Storage> file_vm = algorithm_->GetStoragePerId(storage_id);
    std::shared_ptr<Storage> file_vm = file->GetStorage();

    // Ceil of File Transfer Time + File Size * lambda
    double one_file_read_time = ComputeFileTransferTime(file, file_vm, vm);

    if (one_file_read_time == std::numeric_limits<double>::max()) {
      DLOG(INFO) << "read_time: " << one_file_read_time;
      return one_file_read_time;
    } else {
      // read_time += std::ceil(one_file_read_time);
      read_time += one_file_read_time;
//      size_t storage_id = file_vm->get_id();

      // Update costs
//      if (storage_id < algorithm_->GetVirtualMachineSize() and storage_id != vm->get_id()) {
////        double diff = (aux_start_time + read_time) - allocation_vm_queue_[storage_id];
//        double diff = (start_time_ + read_time) - allocation_vm_queue_[storage_id];
//
//        if (diff > 0.0) {
//          std::shared_ptr<VirtualMachine> virtual_machine = algorithm_->GetVirtualMachinePerId(storage_id);
//
//          cost_ += diff * virtual_machine->get_cost();
//          allocation_vm_queue_[storage_id] = aux_start_time + read_time;
//          // std::cout << "diff: " << diff << " virtual_machine->get_cost(): " << virtual_machine->get_cost()
//          //           << " storage_id: " << storage_id << " vm: " << vm->get_id() << std::endl;
//        }
//      }
      // allocation_vm_queue_[storage_id] = std::max(aux_start_time
      //     + one_file_read_time, allocation_vm_queue_[storage_id]);
    }
  }  // for (std::shared_ptr<File> file : task.get_input_files()) {

  DLOG(INFO) << "read_time: " << read_time;

  return read_time;
}

void Activation::SetNextActivation(std::shared_ptr<Activation> activation) {
  DLOG(INFO) << "Setting the next activation [" << activation->get_id() << "] to the activation [" << id_ << "] ...";
//  auto p = next_vm_activation_.lock();
//  if (p) {
//    std::cout << "next_vm_activation_ " << *(p.get()) << std::endl;
//  }
//  std::cout << "activation " << *(activation.get()) << std::endl << std::endl;
  next_vm_activation_ = activation;
  DLOG(INFO) << "... next activation defined";
}

void Activation::SetVm(std::shared_ptr<VirtualMachine> vm) {
  DLOG(INFO) << "Setting the VM[" << vm->get_id() << "] to the Activation[" << id_ << "]";
  // Salve the VM
  allocated_vm_ = vm;

  // Update previous_vm_activation_
  auto last_vm_activation = vm->GetLastActivation().lock();
  if (last_vm_activation) {
    Activation a = *(last_vm_activation);
    Activation b = *(this);

//    std::cout << "a " << a << std::endl;
//    std::cout << "b " << b << std::endl << std::endl;

    if (a != b) {
      last_vm_activation->SetNextActivation(shared_from_this());
    }
  }

  // Update vm with this
//  DLOG(INFO) << "Antes do problema";
//  google::FlushLogFiles(google::INFO);
//  vm->AddActivation(shared_from_this());
//  DLOG(INFO) << "Depois do problema";
//  google::FlushLogFiles(google::INFO);
  // Update start time
  double max_prev_ft = 0.0;
//  auto predecessors = algorithm_->GetPredecessors(id_);
  std::for_each(predecessors_.begin(), predecessors_.end(), [&max_prev_ft, this](std::weak_ptr<Activation> wp) {
    auto activation = wp.lock();
    if (activation) {
      double ft = activation->GetFinishTime();
      max_prev_ft = std::max<double>(max_prev_ft, ft);
    }
  });
//  for (auto activation_id : algorithm_->GetPredecessors(id_)) {
//    auto activation = algorithm_->GetActivationPerId(activation_id);
//    double ft = activation->GetFinishTime();
//    max_prev_ft = std::max<double>(max_prev_ft, ft);
//  }
  double prev_vm_ft = 0.0;
  auto p = previous_vm_activation_.lock();
  if (p) {
    prev_vm_ft = p->GetFinishTime();
  }
  start_time_ = std::max<double>(prev_vm_ft, max_prev_ft);
  DLOG(INFO) << "start_time_ " << start_time_;

  // Calculate read input files time
  read_time_ = ComputeReadTime(vm);
  DLOG(INFO) << "start_time_ " << start_time_;

  // Calculate execution activation time
//  execution_time_ = ceil(execution_time_ * vm->get_slowdown());
  execution_time_ = ceil(execution_time_ * vm->get_slowdown());
  DLOG(INFO) << "execution_time_ " << start_time_;

  // Calculate write time
  write_time_ = ComputeWriteTime();
  DLOG(INFO) << "write_time_ " << start_time_;

  // Update finish time
  finish_time_ = read_time_ + execution_time_ + write_time_;
  DLOG(INFO) << "finish_time_ " << start_time_;
  DLOG(INFO) << "... VM was set on the activation";
}  // void Activation::SetVm(const std::shared_ptr<VirtualMachine>& vm) {
