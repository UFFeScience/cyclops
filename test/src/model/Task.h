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

#include "src/model/File.h"

class Task {
 public:
  /// Parametrised constructor
  explicit Task(const int id, const std::string tag, const std::string name, const double time) :
    _id(id), _tag(tag), _name(name), _time(time) { }

  /// Getter for _id
  int getId() const { return _id; }

  /// Getter for _tag
  const std::string &getTag() const { return _tag; }

  /// Getter for _name
  const std::string &getName() const { return _name; }

  /// Getter for _size
  double getTime() const { return _time; }

  /// Adds a input file
  void addInputFile(const File &file) { _inputFiles.push_back(file); }

  /// Adds a output file
  void addOutputFile(const File &file) { _outputFiles.push_back(file); }

  friend std::ostream& operator<<(std::ostream& strm, const Task& a) {

    std::ostringstream oss;

    oss << "\nInput Files: { \n";
    if (!a._inputFiles.empty()) {
      // Convert all but the last element to avoid a trailing ","
      for (File o : a._inputFiles) {
        oss << "\tFile[_id: " << o.getId() << ", "
            << "_name: " << o.getName() << ", "
            << "_size: " << o.getSize() << "]\n";
      }
      // std::copy(a._inputFiles.begin(), a._inputFiles.end() - 1,
      //     std::ostream_iterator<int>(oss, ","));

      // // Now add the last element with no delimiter
      // oss << a._inputFiles.back();
    }
    oss << "}, Output Files: {\n";
    if (!a._outputFiles.empty()) {
      // Convert all but the last element to avoid a trailing ","
      for (File o : a._outputFiles) {
        oss << "\tFile[_id: " << o.getId() << ", "
            << "_name: " << o.getName() << ", "
            << "_size: " << o.getSize() << "]\n";
      }
      // std::copy(a._outputFiles.begin(), a._outputFiles.end() - 1,
      //     std::ostream_iterator<int>(oss, ","));

      // // Now add the last element with no delimiter
      // oss << a._outputFiles.back();
    }
    oss << "}";
    // std::cout << oss.str() << std::endl;

    return strm << "Task[_id: " << a._id << ", "
      << "_tag: " << a._tag << ", "
      << "_name: " << a._name << ", "
      << "_time: " << a._time << ", "
      << oss.str() << "] ";
  }

 private:
  int _id;

  std::string _tag;

  std::string _name;

  double _time;

  std::vector<File> _inputFiles;

  std::vector<File> _outputFiles;
};  // end of class Task

#endif  // SRC_MODEL_TASK_H_
