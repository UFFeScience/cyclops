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

#include "src/model/virtual_machine.h"
#include "src/model/bucket.h"

#include <vector>

class Provider {
public:
  Provider(size_t id) : id_(id) {}

  /// Getter for _id
  size_t get_id() const { return id_; }

  /// Adds a input file
  void AddVirtualMachine(const VirtualMachine &virtual_machine) {
    virtual_machines_.push_back(virtual_machine);
  }

  /// Adds a output file
  void AddBucket(const Bucket &bucket) {
    buckets_.push_back(bucket);
  }
private:
	size_t id_;

	std::vector<VirtualMachine> virtual_machines_;

	std::vector<Bucket> buckets_;
};

#endif  // SRC_MODEL_PROVIDER_H_
