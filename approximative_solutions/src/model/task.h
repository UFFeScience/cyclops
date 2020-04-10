/**
 * \file src/model/Task.h
 * \brief Contains the \c Task class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \date 2020
 *
 * This header file contains the \c Task class.
 */

#ifndef SRC_MODEL_TASK_H_
#define SRC_MODEL_TASK_H_

#include <string>
#include <vector>

#include <sstream>
#include <iterator>

#include <memory>

#include "src/model/file.h"

class Task {
public:
  /// Parametrised constructor
  explicit Task(const size_t id, const std::string tag, const std::string name, const double time) :
    id_(id), tag_(tag), name_(name), time_(time) { }

  ~Task() { }

  /// Getter for _id
  size_t get_id() const { return id_; }

  /// Getter for _tag
  const std::string &get_tag() const { return tag_; }

  /// Getter for _name
  const std::string &get_name() const { return name_; }

  /// Getter for time_
  double get_time() const { return time_; }

  /// Getter for input_files_
  std::vector<File*> get_input_files() const { return input_files_; }
  // std::vector<std::shared_ptr<File>> get_input_files() const { return input_files_; }

  /// Getter for output_files_
  std::vector<File*> get_output_files() const { return output_files_; }
  // std::vector<std::shared_ptr<File>> get_output_files() const { return output_files_; }

  /// Getter for requirements_
  std::vector<double> get_requirements() const { return requirements_; }

  /// Adds a input file
  void AddInputFile(File* file) { input_files_.push_back(file); }
  // void AddInputFile(std::shared_ptr<File> file) { input_files_.push_back(file); }

  /// Adds a output file
  void AddOutputFile(File* file) { output_files_.push_back(file); }
  // void AddOutputFile(std::shared_ptr<File> file) { output_files_.push_back(file); }

  /// Adds a output file
  void AddRequirement(double requirement) { requirements_.push_back(requirement); }
  // void AddOutputFile(std::shared_ptr<File> file) { output_files_.push_back(file); }

  bool operator==(const Task &rhs) const {
    return rhs.get_id() == id_;
  }

  friend std::ostream& operator<<(std::ostream& strm, const Task& a) {

    std::ostringstream oss;

    oss << "\nInput Files: { \n";
    if (!a.input_files_.empty()) {
      // Convert all but the last element to avoid a trailing ","
      for (File* o : a.input_files_) {
      // for (std::shared_ptr<File> o : a.input_files_) {
        oss << "\tFile[id_: " << o->get_id() << ", "
            << "name_: " << o->get_name() << ", "
            << "size_: " << o->get_size() << "]\n";
      }
    }

    oss << "}, Output Files: {\n";
    if (!a.output_files_.empty()) {
      // Convert all but the last element to avoid a trailing ","
      for (File* o : a.output_files_) {
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
      // for (std::shared_ptr<File> o : a.output_files_) {
        oss << "Requirement[id_: " << r << ", value_: " << a.requirements_[r] << "]\n";
      }
    }
    oss << "}";

    return strm << "Task[id_: " << a.id_ << ", "
                << "tag_: " << a.tag_ << ", "
                << "name_: " << a.name_ << ", "
                << "time_: " << a.time_ << ", "
                << oss.str() << "] ";
  }

private:
  size_t id_;

  std::string tag_;

  std::string name_;

  double time_;

  std::vector<File*> input_files_;
  // std::vector<std::shared_ptr<File>> input_files_;

  std::vector<File*> output_files_;
  // std::vector<std::shared_ptr<File>> output_files_;

  std::vector<double> requirements_;
};  // end of class Task

#endif  // SRC_MODEL_TASK_H_
