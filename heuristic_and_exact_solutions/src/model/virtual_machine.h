/**
 * \file src/model/virtual_machine.h
 * \brief Contains the \c VirtualMachine class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2020
 *
 * This header file contains the \c VirtualMachine class.
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
    VirtualMachine(size_t id,
                   std::string name,
                   double slowdown,
                   double storage,
                   double cost,
                   double bandwidth,
                   int type_id) :
            Storage(id, std::move(name), storage, cost, bandwidth, type_id),
            slowdown_(slowdown) {}

    ~VirtualMachine() override = default;

    /// Getter for slowdown_
    [[nodiscard]] double get_slowdown() const { return slowdown_; }

    [[nodiscard]] std::weak_ptr<Activation> GetLastActivation() const {
        DLOG(INFO) << "Getting the last activation ...";
        std::weak_ptr<Activation> rv;
        if (!activation_list_.empty()) {
            rv = activation_list_.back();
            auto p = rv.lock();
            if (p) {
                DLOG(INFO) << "... last activation fetched";
                return p;
            }
//      return activation_list_.back();
        }
        DLOG(INFO) << "... last activation fetched";
        return rv;
    }

    [[nodiscard]] std::weak_ptr<Activation> GetFirstActivation() const {
        DLOG(INFO) << "Getting the first activation ...";
        std::weak_ptr<Activation> rv;
        if (!activation_list_.empty()) {
            rv = activation_list_.front();
            auto p = rv.lock();
            if (p) {
                DLOG(INFO) << "... first activation fetched";
                return p;
            }
//      return activation_list_.front();
        }
        DLOG(INFO) << "... first activation fetched";
        return rv;
    }

    void AddActivation(std::shared_ptr<Activation> activation);

    friend std::ostream &operator<<(std::ostream &os, const VirtualMachine &a) {
        return a.write(os);
    }

protected:
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

    double slowdown_;

    ///
    std::vector<std::shared_ptr<Activation>> activation_list_;
};

#endif  // APPROXIMATE_SOLUTIONS_SRC_MODEL_VIRTUAL_MACHINE_H_
