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

#include "src/model/file.h"

class Task {
public:
  /// Parametrised constructor
  explicit Task(const int id, const std::string tag, const std::string name, const double time) :
    id_(id), tag_(tag), name_(name), time_(time) { }

  /// Getter for _id
  int get_id() const { return id_; }

  /// Getter for _tag
  const std::string &get_tag() const { return tag_; }

  /// Getter for _name
  const std::string &get_name() const { return name_; }

  /// Getter for _size
  double get_time() const { return time_; }

  /// Adds a input file
  void AddInputFile(const File &file) { input_files_.push_back(file); }

  /// Adds a output file
  void AddOutputFile(const File &file) { output_files_.push_back(file); }

  friend std::ostream& operator<<(std::ostream& strm, const Task& a) {

    std::ostringstream oss;

    oss << "\nInput Files: { \n";
    if (!a.input_files_.empty()) {
      // Convert all but the last element to avoid a trailing ","
      for (File o : a.input_files_) {
        oss << "\tFile[_id: " << o.get_id() << ", "
            << "_name: " << o.get_name() << ", "
            << "_size: " << o.get_size() << "]\n";
      }
      // std::copy(a._inputFiles.begin(), a._inputFiles.end() - 1,
      //     std::ostream_iterator<int>(oss, ","));

      // // Now add the last element with no delimiter
      // oss << a._inputFiles.back();
    }
    oss << "}, Output Files: {\n";
    if (!a.output_files_.empty()) {
      // Convert all but the last element to avoid a trailing ","
      for (File o : a.output_files_) {
        oss << "\tFile[_id: " << o.get_id() << ", "
            << "_name: " << o.get_name() << ", "
            << "_size: " << o.get_size() << "]\n";
      }
      // std::copy(a._outputFiles.begin(), a._outputFiles.end() - 1,
      //     std::ostream_iterator<int>(oss, ","));

      // // Now add the last element with no delimiter
      // oss << a._outputFiles.back();
    }
    oss << "}";
    // std::cout << oss.str() << std::endl;

    return strm << "Task[_id: " << a.id_ << ", "
      << "_tag: " << a.tag_ << ", "
      << "_name: " << a.name_ << ", "
      << "_time: " << a.time_ << ", "
      << oss.str() << "] ";
  }

private:
  int id_;

  std::string tag_;

  std::string name_;

  double time_;

  std::vector<File> input_files_;

  std::vector<File> output_files_;
};  // end of class Task

#endif  // SRC_MODEL_TASK_H_
