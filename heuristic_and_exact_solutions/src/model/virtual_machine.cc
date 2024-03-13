/**
 * \file src/model/virtual_machine.cc
 * \brief Contains the \c VirtualMachine class definition
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2021
 *
 * This source file contains the \c VirtualMachine class definition
 */

#include "src/model/virtual_machine.h"

void VirtualMachine::AddActivation(std::shared_ptr<Activation> activation) {
    DLOG(INFO) << "Adding the activation [" << activation->get_id() << "] to the VM [" << id_ << "] ...";
    activation_list_.push_back(activation);
    DLOG(INFO) << ".. activation added to the VM";
}
