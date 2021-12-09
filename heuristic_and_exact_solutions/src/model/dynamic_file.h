/**
 * \file src/model/dynamic_file.h
 * \brief Contains the \c DynamicFile class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2020
 *
 * This header file contains the \c DynamicFile class.
 */

#ifndef APPROXIMATIVE_SOLUTIONS_SRC_MODEL_DYNAMIC_FILE_H_
#define APPROXIMATIVE_SOLUTIONS_SRC_MODEL_DYNAMIC_FILE_H_

#include <glog/logging.h>

#include <string>
#include <utility>

#include "src/model/file.h"

/**
 * \class DynamicFile dynamic_file.h "src/model/dynamic_file.h"
 * \brief Represents the Dynamic File; a file produced by the executaion of a \c Activation
 */
class DynamicFile : public File {
 public:
  /// Parametrized constructor
  explicit DynamicFile(const size_t id, const std::string& name, const double size) :
    File(id, name, size) { }

  /// Default destructor
  ~DynamicFile() override = default;

  /// Getter for the parent task
//  Activation* get_parent_task() const { return parent_task_; }
  [[nodiscard]] std::shared_ptr<Activation> get_parent_task() const { return parent_task_; }

  /// Getter for the parent output file index
  [[nodiscard]] size_t get_parent_output_file_index() const { return parent_output_file_index_; }

  /// Setter for the parent task
//  void set_parent_task(Activation* parent_task) { parent_task_ = parent_task; }
  void set_parent_task(std::shared_ptr<Activation> parent_task) { parent_task_ = std::move(parent_task); }

  /// Setter for the parent output file index
  void set_parent_output_file_index(const size_t parent_output_file_index) {
    parent_output_file_index_ = parent_output_file_index;
  }

  /// Concatenator operator
  friend std::ostream& operator<<(std::ostream& os, const DynamicFile& a) {
    return a.write(os);
  }

 private:
  /// Writes the Dynamic File object to the output stream
  std::ostream& write(std::ostream& os) const override {
    return os << "DynamicFile[id_: " << id_
              << ", name_: " << name_
              << ", size_: " << size_ << "]";
  }


  /// The \c Activation that generated this file
  std::shared_ptr<Activation> parent_task_;

  /// The index position of this generated file
  size_t parent_output_file_index_ = std::numeric_limits<size_t>::max();
};  // end of class DynamicFile

#endif  // APPROXIMATIVE_SOLUTIONS_SRC_MODEL_DYNAMIC_FILE_H_
