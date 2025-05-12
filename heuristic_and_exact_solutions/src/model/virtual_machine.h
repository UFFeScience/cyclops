/**
 * \file src/model/virtual_machine.h
 * \brief Contains the \c VirtualMachine class declaration
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo.raps@gmail.com\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2024
 *
 * This header file contains the \c VirtualMachine class
 */

#ifndef APPROXIMATE_SOLUTIONS_SRC_MODEL_VIRTUAL_MACHINE_H_
#define APPROXIMATE_SOLUTIONS_SRC_MODEL_VIRTUAL_MACHINE_H_


#include <glog/logging.h>

#include <string>
#include <utility>
#include <vector>

#include "src/model/activation.h"
#include "src/model/storage.h"

/// Forward declaration of the class Algorithm, needed because of the circular reference
class Activation;

class VirtualMachine : public Storage {
public:
    ///
    VirtualMachine(size_t id,
                   std::string name,
                   double slowdown,
                   double storage,
                   double cost,
                   double bandwidth_in_Gbps,
                   int type_id) :
            Storage(id, std::move(name), storage, bandwidth_in_Gbps, type_id),
            slowdown_(slowdown),
            cost_(cost / 3600.0) {}


    ~VirtualMachine() override = default;

    /// Getter for slowdown_
    [[nodiscard]] double get_slowdown() const {
        return slowdown_;
    }

    /// Getter for cost_
    [[nodiscard]] double get_cost() const { return cost_; }

    ///
    friend std::ostream &operator<<(std::ostream &os, const VirtualMachine &a) {
        return a.write(os);
    }

protected:
    ///
    std::ostream &write(std::ostream &os) const {
        std::ostringstream oss;

        oss << "requirements_: {";
        if (!requirements_.empty()) {
            // Convert all but the last element to avoid a trailing ","
            for (size_t r = 0ul; r < requirements_.size(); ++r) {
                oss << "Requirement[id_: " << r << ", value_: " << requirements_[r] << "]\n";
            }
        }
        oss << "}";

        return os << "Virtual Machine[id_: " << id_
                  << ", name_: " << name_
                  << ", slowdown_: " << slowdown_
                  << ", storage_: " << storage_
                  << ", cost_: " << cost_
                  << ", bandwidth_: " << bandwidth_
                  << ", type_id_: " << type_id_
                  << ", " << oss.str()
                  << "]";
    }

    ///
    double slowdown_;

    /// Cost
    double cost_;
};


#endif  // APPROXIMATE_SOLUTIONS_SRC_MODEL_VIRTUAL_MACHINE_H_
