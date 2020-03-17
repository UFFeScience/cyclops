/**
 * \file src/model/Requirement.h
 * \brief Contains the \c Requirement class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \date 2020
 *
 * This header file contains the \c Requirement class. There are two types of requirements:
 *
 *  1.
 *  2.
 *
 *
 */

#ifndef SRC_MODEL_REQUIREMENT_H_
#define SRC_MODEL_REQUIREMENT_H_

#include <string>
#include <iostream>
#include <fstream>

class Requirement {
 public:
  /// Parametrised constructor
  explicit Requirement(const int id, const int max_value) : _id(id), _max_value(max_value) { }

  /// Getter for _id
  int getId() const { return _id; }

  /// Getter for _max_value
  int getMaxValue() const { return _max_value; }

  // std::ostream& operator<<(const Requirement &a) {
  //   return "Requirement(" << std::to_string(a._max_value) << ")";
  // }

  friend std::ostream& operator<<(std::ostream& strm, const Requirement& a){
    return strm << "Requirement[_id: " << a._id << ", " << "_max_value: " << a._max_value << "]";
  }

 private:
  int _id;
  int _max_value;
};  // end of class Requirement

#endif  // SRC_MODEL_REQUIREMENT_H_
