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
		_id(id),
    _name(name),
    _slowdown(slowdown),
    _storage(storage),
    _cost(cost),
    _bandwidth(bandwidth),
    _typeId(type_id){}

  /// Getter for _id
  int getId() const { return _id; }

  /// Getter for _name
  const std::string &getName() const { return _name; }

  /// Getter for _slowdown
  double getSlowdown() const { return _slowdown; }

  /// Getter for _storage
  double getStorage() const { return _storage; }

  /// Getter for _cost
  double getCost() const { return _cost; }

  /// Getter for _bandwidth
  double getBandwith() const { return _bandwidth; }

  /// Getter for _typeId
  int getTypeId() const { return _typeId; }

 private:
	int _id;

	std::string _name;

	double _slowdown;

  double _storage;

  double _cost;

  double _bandwidth;

	int _typeId;

};

#endif  // SRC_MODEL_VIRTUAL_MACHINE_H_
