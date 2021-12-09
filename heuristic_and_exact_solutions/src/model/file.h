/**
 * \file src/model/file.h
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
#include <utility>
#include <vector>

#include <sstream>
#include <iterator>

#include <limits>
#include "storage.h"

/**
 * \class File file.h "src/model/file.h"
 * \brief It is a file abstraction used by the \c DynamicFile and the \c StaticFile
 */
class File {
 public:
  /// Parametrized constructor
  explicit File(const size_t id,
                std::string name,
                const double size)
      : id_(id),
        name_(std::move(name)),
        size_(size),
        size_in_MB_(size / 1000.0),
        size_in_GB_(size / 1000000.0) { }

  /// Default destructor
  virtual ~File() = default;

  /// Getter for the ID of the file
  [[nodiscard]] size_t get_id() const { return id_; }

  /// Getter for name of the file
  [[nodiscard]] const std::string &get_name() const { return name_; }

  /// Getter for size in KBs of the file
  [[nodiscard]] double get_size() const { return size_; }

  /// Getter for size in KBs of the file
  [[nodiscard]] double get_size_in_MB() const { return size_in_MB_; }

  /// Getter for size in KBs of the file
  [[nodiscard]] double get_size_in_GB() const { return size_in_GB_; }

  [[nodiscard]] size_t GetStorageId() const {
    auto s = storage_.lock();
    if (s) {
      return s->get_id();
    }
    return std::numeric_limits<size_t>::max();
  }

  [[nodiscard]] std::shared_ptr<Storage> GetStorage() const {
    auto s = storage_.lock();
    if (s) {
      return s;
    }
    return nullptr;
  }

  /// Concatenate operator
  friend std::ostream& operator<<(std::ostream& os, const File& a) {
    return a.write(os);
  }

 protected:
  virtual /// Print the File object to the output stream
  std::ostream& write(std::ostream& os) const {
    return os << "File[_id: " << id_ << ", "
              << "name_: " << name_ << ", "
              << "size_: " << size_ << "]";
  }

  /// The ID of the file
  size_t id_;

  /// The file name
  std::string name_;

  /// The file size in KB
  double size_;

  /// The file size in MB
  double size_in_MB_;

  /// The file size in GB
  double size_in_GB_;

  std::weak_ptr<Storage> storage_;
};  // end of class File

#endif  // APPROXIMATIVE_SOLUTIONS_SRC_MODEL_FILE_H_
