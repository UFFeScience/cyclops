/**
 * \file src/model/VirutalMachine.h
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

class VirtualMachine {
 public:
  VirtualMachine(int id,
                 std::string name,
                 double slowdown,
                 double storage,
                 double cost,
                 double bandwidth,
                 int type_id) :
		id_(id),
    name_(name),
    slowdown_(slowdown),
    storage_(storage),
    cost_(cost),
    bandwidth_(bandwidth),
    type_id_(type_id){}

  /// Getter for _id
  int getId() const { return id_; }

  /// Getter for _name
  const std::string &get_name() const { return name_; }

  /// Getter for _slowdown
  double get_slowdown() const { return slowdown_; }

  /// Getter for _storage
  double get_storage() const { return storage_; }

  /// Getter for _cost
  double get_cost() const { return cost_; }

  /// Getter for _bandwidth
  double get_bandwith() const { return bandwidth_; }

  /// Getter for _typeId
  int get_type_id() const { return type_id_; }

 private:
	int id_;

	std::string name_;

	double slowdown_;

  double storage_;

  double cost_;

  double bandwidth_;

	int type_id_;

};

#endif  // SRC_MODEL_VIRTUAL_MACHINE_H_
