/**
 * \file src/model/Provider.h
 * \brief Contains the \c Provider class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \date 2020
 *
 * This header file contains the \c Provider class.
 */

#ifndef SRC_MODEL_PROVIDER_H_
#define SRC_MODEL_PROVIDER_H_

#include "src/model/VirtualMachine.h"
#include "src/model/Bucket.h"

#include <vector>

class Provider {
 public:
  Provider(int id) : _id(id) {}

  /// Getter for _id
  int getId() const { return _id; }

  /// Adds a input file
  void addVirtualMachine(const VirtualMachine &virtualMachine) {
    _virtualMachines.push_back(virtualMachine);
  }

  /// Adds a output file
  void addBucket(const Bucket &bucket) {
    _buckets.push_back(bucket);
  }

 private:
	int _id;

	std::vector<VirtualMachine> _virtualMachines;

	std::vector<Bucket> _buckets;
};


#endif  // SRC_MODEL_PROVIDER_H_
