/**
 * \file src/model/activation_execution_data.h
 * \brief Contains the \c ActivationExecutionData class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo.raps@gmail.com\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2024
 *
 * This header file contains the \c ActivationExecutionData class.
 */

#ifndef WF_SECURITY_ACTIVATION_EXECUTION_DATA_H
#define WF_SECURITY_ACTIVATION_EXECUTION_DATA_H


#include <iostream>
#include <vector>

class ActivationExecutionData {
public:
    /// Constructor declaration
    explicit ActivationExecutionData(size_t, size_t);

    ///
    [[nodiscard]] size_t get_activation_finish_time() const;

    ///
    void set_activation_finish_time(size_t);

    ///
    [[nodiscard]] size_t get_vm_finish_time(size_t) const;

    ///
    void set_vm_finish_time(size_t, size_t);

    ///
    [[nodiscard]] size_t get_vm_allocation_time(size_t) const;

    ///
    void set_vm_allocation_time(size_t, size_t);
private:
    /// Makespan for each task
    size_t activation_finish_time_;

    /// Final time of each Virtual Machine
    std::vector<size_t> vm_finish_time_;

    /// Total allocation time needed for each VM
    std::vector<size_t> vm_allocation_time_;
};


#endif //WF_SECURITY_ACTIVATION_EXECUTION_DATA_H
