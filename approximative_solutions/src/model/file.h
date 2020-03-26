/**
 * \file src/model/File.h
 * \brief Contains the \c File class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \date 2020
 *
 * This header file contains the \c File class.
 */

#ifndef SRC_MODEL_FILE_H_
#define SRC_MODEL_FILE_H_

#include <string>
#include <vector>

#include <sstream>
#include <iterator>

#include <limits>

class File {
 public:

  virtual ~File() { }

  /// Parametrised constructor
  explicit File(const size_t id, const std::string name, const double size, const size_t place) :
    id_(id), name_(name), size_(size), place_(place) { }

  /// Parametrised constructor
  explicit File(const size_t id, const std::string name, const double size) :
    id_(id), name_(name), size_(size) { place_ = std::numeric_limits<size_t>::max(); }

  /// Getter for _id
  size_t get_id() const { return id_; }

  /// Getter for _name
  const std::string &get_name() const { return name_; }

  /// Getter for _size
  double get_size() const { return size_; }

  /// Getter for _place
  size_t get_place() const { return place_; }

  /// Getter for _static
  virtual bool IsStatic() { return false; };

  friend std::ostream& operator<<(std::ostream& os, const File& a) {
    return a.write(os);
  }

 protected:
  std::ostream& write(std::ostream& os) const {
    return os << "File[_id: " << id_ << ", "
      << "_name: " << name_ << ", "
      << "_size: " << size_ << "]";
  };

  //  std::ostream& write(std::ostream& os) const {
  //   return os << "File[_id: " << a.id_ << ", "
  //     << "_name: " << a.name_ << ", "
  //     << "_size: " << a.size_ << "]";

  // }

  size_t id_;

  std::string name_;

	double size_;

  size_t place_;
};  // end of class File

#endif  // SRC_MODEL_FILE_H_
