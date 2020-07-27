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

#ifndef APPROXIMATIVE_SOLUTIONS_SRC_MODEL_VIRTUAL_MACHINE_H_
#define APPROXIMATIVE_SOLUTIONS_SRC_MODEL_VIRTUAL_MACHINE_H_

#include <glog/logging.h>

#include <string>

#include "src/model/storage.h"

class VirtualMachine : public Storage {
 public:
  VirtualMachine(size_t id,
                 std::string name,
                 double slowdown,
                 double storage,
                 double cost,
                 double bandwidth,
                 int type_id) :
    Storage(id, name, storage, cost, bandwidth, type_id),
    slowdown_(slowdown) {}

  ~VirtualMachine() {
    // DLOG(INFO) << "Deleting Virtual Machine " << id_;
  }

  /// Getter for slowdown_
  double get_slowdown() const { return slowdown_; }

  friend std::ostream& operator<<(std::ostream& os, const VirtualMachine& a) {
    return a.write(os);
  }

 private:
  std::ostream& write(std::ostream& os) const {
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
};

#endif  // APPROXIMATIVE_SOLUTIONS_SRC_MODEL_VIRTUAL_MACHINE_H_
