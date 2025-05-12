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
    explicit ActivationExecutionData(size_t activations_size, size_t vms_size);

    ///
    [[nodiscard]] double get_activation_finish_time() const;

    ///
    void set_activation_finish_time(double time);

    ///
    [[nodiscard]] double get_vm_finish_time(size_t) const;

    ///
    void set_vm_finish_time(size_t vm_id, double time);

    ///
    [[nodiscard]] double get_vm_allocation_time(size_t vm_id) const;

    ///
    void set_vm_allocation_time(size_t vm_id, double time);
private:
    /// Makespan for each task
    double activation_finish_time_;

    /// Final time of each Virtual Machine
    std::vector<double> vm_finish_time_;

    /// Total allocation time needed for each VM
    std::vector<double> vm_allocation_time_;
};


#endif //WF_SECURITY_ACTIVATION_EXECUTION_DATA_H
