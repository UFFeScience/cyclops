/**
 * \file src/model/activation_execution_data.cc
 * \brief Contains the \c ActivationExecutionData class implementation
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo.raps@gmail.com\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2024
 *
 * This source file contains the \c ActivationExecutionData class implementation
 */

#include "activation_execution_data.h"

ActivationExecutionData::ActivationExecutionData(size_t activations_size, size_t vms_size)
        : activation_finish_time_(activations_size),
          vm_finish_time_(vms_size, 0ul),
          vm_allocation_time_(vms_size, 0ul) {

}

size_t ActivationExecutionData::get_activation_finish_time() const {
    return activation_finish_time_;
}

void ActivationExecutionData::set_activation_finish_time(size_t time) {
    activation_finish_time_ = time;
}

size_t ActivationExecutionData::get_vm_finish_time(size_t vm_id) const {
    return vm_finish_time_[vm_id];
}

void ActivationExecutionData::set_vm_finish_time(size_t vm_id, size_t time) {
    vm_finish_time_[vm_id] = time;
}

size_t ActivationExecutionData::get_vm_allocation_time(size_t vm_id) const {
    return vm_allocation_time_[vm_id];
}

void ActivationExecutionData::set_vm_allocation_time(size_t vm_id, size_t time) {
    vm_allocation_time_[vm_id] = time;
}
