/**
 * \file src/model/storage.h
 * \brief Contains the \c Storage class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2020
 *
 * This header file contains the \c Storage class.
 */

#ifndef APPROXIMATIVE_SOLUTIONS_SRC_MODEL_STORAGE_H_
#define APPROXIMATIVE_SOLUTIONS_SRC_MODEL_STORAGE_H_

#include <string>
#include <vector>

class Storage {
 public:
  Storage(size_t id,
         std::string name,
         double storage,
         double cost,
         double bandwidth,
         int type_id)
      : id_(id),
        name_(name),
        storage_(storage),
        cost_(cost),
        bandwidth_(bandwidth),
        bandwidth_GBps_(bandwidth / 8.0),
        type_id_(type_id) { }

  virtual ~Storage() = default;

  /// Getter for id_
  size_t get_id() const { return id_; }

  /// Getter for name_
  const std::string &get_name() const { return name_; }

  /// Getter for storage_
  double get_storage() const { return storage_; }

  /// Getter for cost_
  double get_cost() const { return cost_; }

  /// Getter for bandwidth_
  double get_bandwidth() const { return bandwidth_; }

  /// Getter for bandwidth_
  double get_bandwidth_GBps() const { return bandwidth_GBps_; }

  /// Getter for bandwidth_
  double get_type_id() const { return type_id_; }

  /// Adds a requiremnt value
  void AddRequirement(double requirement) { requirements_.push_back(requirement); }

  /// Get a requiremnt value
  int GetRequirementValue(size_t requirement_id) const { return requirements_[requirement_id]; }

  bool operator==(const Storage &rhs) const {
    return rhs.get_id() == id_;
  }

 protected:
  size_t id_;

  std::string name_;

  double storage_;

  double cost_;

  /// Bandwidth in Gbps
  double bandwidth_;

  /// Bandwidth in GBps
  double bandwidth_GBps_;

  int type_id_;

  std::vector<int> requirements_;
};

#endif  // APPROXIMATIVE_SOLUTIONS_SRC_MODEL_STORAGE_H_
