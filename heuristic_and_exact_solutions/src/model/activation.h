/**
 * \file src/model/task.h
 * \brief Contains the \c Activation class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2020
 *
 * This header file contains the \c Activation class.
 */

#ifndef APPROXIMATE_SOLUTIONS_SRC_MODEL_ACTIVATION_H_
#define APPROXIMATE_SOLUTIONS_SRC_MODEL_ACTIVATION_H_

#include <string>
#include <utility>
#include <vector>

#include <sstream>
#include <iterator>

#include <memory>
#include <src/solution/algorithm.h>

#include "src/model/file.h"
#include "virtual_machine.h"

/// Foward declaration of the class Algorithm, needed because of the circular reference
class Algorithm;

/**
 * \class Activation task.h "src/model/task.h"
 * \brief Represents the task of the model
 */
class Activation : public std::enable_shared_from_this<Activation> {
 public:
  /// Parametrised constructor
  explicit Activation(const size_t id, std::string  tag, std::string  name, const double time, Algorithm* algorithm) :
      id_(id), tag_(std::move(tag)), name_(std::move(name)), execution_time_(time), algorithm_(algorithm) {}

  ~Activation() = default;

  inline double ComputeFileTransferTime(const std::shared_ptr<File>& file,
                                        const std::shared_ptr<Storage>& vm1,
                                        const std::shared_ptr<Storage>& vm2,
                                        bool check_constraints = false);

  /// Getter for id_
  [[nodiscard]] size_t get_id() const { return id_; }

  /// Getter for tag_
  [[nodiscard]] const std::string& get_tag() const { return tag_; }

  /// Getter for name_
  [[nodiscard]] const std::string& get_name() const { return name_; }

  /// Getter for execution_time_
  [[nodiscard]] double get_time() const { return execution_time_; }

  /// Getter for input_files_
//  std::vector<std::shared_ptr<File>> get_input_files() const { return input_files_; }
  [[nodiscard]] std::vector<std::shared_ptr<File>> get_input_files() const { return input_files_; }

  /// Getter for output_files_
//  std::vector<std::shared_ptr<File>> get_output_files() const { return output_files_; }
  [[nodiscard]] std::vector<std::shared_ptr<File>> get_output_files() const { return output_files_; }

  /// Getter for requirements_
  [[nodiscard]] std::vector<int> get_requirements() const { return requirements_; }

  /// Adds a input file
//  void AddInputFile(std::shared_ptr<File> file) { input_files_.push_back(file); }
  void AddInputFile(const std::shared_ptr<File>& file) { input_files_.push_back(file); }

  /// Adds a output file
  size_t AddOutputFile(const std::shared_ptr<File>& file) {
    size_t index = output_files_.size();
    output_files_.push_back(file);
    return index;
  }

  /// Adds a requirement value
  void AddRequirement(int requirement) { requirements_.push_back(requirement); }

  /// Get a requirement value
  [[nodiscard]] int GetRequirementValue(size_t requirement_id) const { return requirements_[requirement_id]; }

  ///
//  size_t GetFinishTime() const { return requirements_[requirement_id]; }

  double GetFinishTime() {
    return finish_time_;
  }

  double ComputeReadTime(std::shared_ptr<VirtualMachine>);

  double ComputeExecutionTime() {
    return 1.0;
  }

  double ComputeWriteTime() {
    return 1.0;
  }

  [[nodiscard]] std::weak_ptr<Activation> GetNextVmActivation() const {
    return next_vm_activation_;
//    auto p = next_vm_activation_.lock();
//    if (p) {
//      return p;
//    }
//    return nullptr;
  }

  void SetNextActivation(std::shared_ptr<Activation> activation);

  void SetVm(std::shared_ptr<VirtualMachine>);

  void SetPredecessors(std::vector<std::weak_ptr<Activation>> predecessors) {
    predecessors_ = predecessors;
  }

  void SetSuccessors_(std::vector<std::weak_ptr<Activation>> successors) {
    successors_ = successors;
  }

  // Redefining operators

  /// Equal operator
  bool operator==(const Activation& rhs) const {
//    std::cout << "Testing for equal activations" << std::endl;
    return rhs.get_id() == id_;
  }

  bool operator!=(const Activation& rhs) const {
//    std::cout << "Testing for different activations" << std::endl;
    return rhs.get_id() != id_;
  }
//
//  bool operator==(const Activation& rhs) {
//    std::cout << "Testing for NOT const" << std::endl;
//    return rhs.get_id() == id_;
//  }

  /// Concatenate operator
  friend std::ostream& operator<<(std::ostream& strm, const Activation& a) {
    std::ostringstream oss;

    oss << "\nInput Files: { \n";
    if (!a.input_files_.empty()) {
      // Convert all but the last element to avoid a trailing ","
      for (const std::shared_ptr<File>& o: a.input_files_) {
        // for (std::shared_ptr<File> o : a.input_files_) {
        oss << "\tFile[id_: " << o->get_id() << ", "
            << "name_: " << o->get_name() << ", "
            << "size_: " << o->get_size() << "]\n";
      }
    }

    oss << "}, Output Files: {\n";
    if (!a.output_files_.empty()) {
      // Convert all but the last element to avoid a trailing ","
      for (const std::shared_ptr<File>& o: a.output_files_) {
        // for (std::shared_ptr<File> o : a.output_files_) {
        oss << "\tFile[id_: " << o->get_id() << ", "
            << "name_: " << o->get_name() << ", "
            << "size_: " << o->get_size() << "]\n";
      }
    }

    oss << "}, Requirements: {\n";
    if (!a.requirements_.empty()) {
      // Convert all but the last element to avoid a trailing ","
      for (size_t r = 0ul; r < a.requirements_.size(); ++r) {
        oss << "Requirement[id_: " << r << ", value_: " << a.requirements_[r] << "]\n";
      }
    }
    oss << "}";

    return strm << "Activation[id_: " << a.id_ << ", "
                << "tag_: " << a.tag_ << ", "
                << "name_: " << a.name_ << ", "
                << "execution_time_: " << a.execution_time_ << ", "
                << oss.str() << "] ";
  }

 private:
  /// The id of the task
  size_t id_;

  /// The tag of the task
  std::string tag_;

  /// The name of the task
  std::string name_;

  /// The execution_time_ necessary to execute this task in a default machine
//  double execution_time_;

  /// The input files necessary to execute this task
//  std::vector<std::shared_ptr<File>> input_files_;
  std::vector<std::shared_ptr<File>> input_files_;

  /// The output files produced by this task when executed
//  std::vector<std::shared_ptr<File>> output_files_;
  std::vector<std::shared_ptr<File>> output_files_;

  /// A vector containing the requirement values
  std::vector<int> requirements_;

  ///
  std::weak_ptr<VirtualMachine> allocated_vm_;

//  ///
  std::weak_ptr<Activation> previous_vm_activation_;
//
//  ///
  std::weak_ptr<Activation> next_vm_activation_;

  ///
  double start_time_ = 0.0;

  ///
  double read_time_ = 0.0;

  ///
  double execution_time_ = 0.0;

  ///
  double write_time_ = 0.0;

  ///
  double finish_time_ = std::numeric_limits<double>::max();

  ///
  Algorithm* algorithm_ = nullptr;

  ///
  std::vector<std::weak_ptr<Activation>> predecessors_;

  ///
  std::vector<std::weak_ptr<Activation>> successors_;
};  // end of class Activation

#endif  // APPROXIMATE_SOLUTIONS_SRC_MODEL_ACTIVATION_H_
