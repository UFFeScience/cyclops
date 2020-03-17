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

class File {
 public:
  /// Parametrised constructor
  explicit File(const int id, const std::string name, const double size, const int place) :
    id_(id), name_(name), size_(size), place_(place) { }

  /// Parametrised constructor
  explicit File(const int id, const std::string name, const double size) :
    id_(id), name_(name), size_(size) { place_ = -1; }

  /// Getter for _id
  int get_id() const { return id_; }

  /// Getter for _name
  const std::string &get_name() const { return name_; }

  /// Getter for _size
  double get_size() const { return size_; }

  /// Getter for _place
  int get_place() const { return place_; }

  /// Getter for _static
  virtual bool isStatic() { return false; };

  // friend std::ostream& operator<<(std::ostream& strm, const File& a){
  //   return strm << "File[_id: " << a._id << ", "
  //     << "_name: " << a._name << ", "
  //     << "_size: " << a._size << "]";
  // }

 protected:

  int id_;

  std::string name_;

	double size_;

  int place_;
};  // end of class File

#endif  // SRC_MODEL_FILE_H_
