/**
 * \file src/model/virtual_machine.h
 * \brief Contains the \c VirtualMachine class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \date 2020
 *
 * This header file contains the \c VirtualMachine class.
 */

#ifndef SRC_MODEL_VIRTUAL_MACHINE_H_
#define SRC_MODEL_VIRTUAL_MACHINE_H_

#include <string>
#include <glog/logging.h>
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
    return os << "Virtual Machine[id_: " << id_
        << ", name_: " << name_
        << ", slowdown_: " << slowdown_
        << ", storage_: " << storage_
        << ", cost_: " << cost_
        << ", bandwidth_: " << bandwidth_
        << ", type_id_: " << type_id_
        << "]";
  }

	double slowdown_;
};

#endif  // SRC_MODEL_VIRTUAL_MACHINE_H_
