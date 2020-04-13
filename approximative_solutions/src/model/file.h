/**
 * \file src/model/File.h
 * \brief Contains the \c File class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2020
 *
 * This header file contains the \c File class.
 */

#ifndef APPROXIMATIVE_SOLUTIONS_SRC_MODEL_FILE_H_
#define APPROXIMATIVE_SOLUTIONS_SRC_MODEL_FILE_H_

#include <string>
#include <vector>

#include <sstream>
#include <iterator>

#include <limits>

/**
 * \class File file.h "src/model/file.h"
 * \brief It is a file abstraction used by the \c DynamicFile and the \c StaticFile
 */
class File {
 public:
  /// Parametrized constructor
  explicit File(const size_t id,
                const std::string name,
                const double size)
      : id_(id),
        name_(name),
        size_(size) { }

  /// Default destructor
  virtual ~File() = default;

  /// Getter for the ID of the file
  size_t get_id() const { return id_; }

  /// Getter for name of the file
  const std::string &get_name() const { return name_; }

  /// Getter for size in MBs of the file
  double get_size() const { return size_; }

  /// Concatenate operator
  friend std::ostream& operator<<(std::ostream& os, const File& a) {
    return a.write(os);
  }

 protected:
  /// Print the File object to the output stream
  std::ostream& write(std::ostream& os) const {
    return os << "File[_id: " << id_ << ", "
              << "name_: " << name_ << ", "
              << "size_: " << size_ << "]";
  }

  /// The ID of the file
  size_t id_;

  /// The file name
  std::string name_;

  /// The file size in MBs
  double size_;
};  // end of class File

#endif  // APPROXIMATIVE_SOLUTIONS_SRC_MODEL_FILE_H_
