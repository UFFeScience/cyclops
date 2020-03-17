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
    _id(id), _name(name), _size(size), _place(place) { }

  /// Parametrised constructor
  explicit File(const int id, const std::string name, const double size) :
    _id(id), _name(name), _size(size) { _place = -1; }

  /// Getter for _id
  int getId() const { return _id; }

  /// Getter for _name
  const std::string &getName() const { return _name; }

  /// Getter for _size
  double getSize() const { return _size; }

  /// Getter for _place
  int getPlace() const { return _place; }

  /// Getter for _static
  virtual bool isStatic() { return false; };

  // friend std::ostream& operator<<(std::ostream& strm, const File& a){
  //   return strm << "File[_id: " << a._id << ", "
  //     << "_name: " << a._name << ", "
  //     << "_size: " << a._size << "]";
  // }

 protected:

  int _id;

  std::string _name;

	double _size;

  int _place;
};  // end of class File

#endif  // SRC_MODEL_FILE_H_
