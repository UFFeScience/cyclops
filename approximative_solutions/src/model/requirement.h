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
  explicit Requirement(const int id, const int max_value) : id_(id), max_value_(max_value) { }

  /// Getter for _id
  int get_id() const { return id_; }

  /// Getter for _max_value
  int get_max_value() const { return max_value_; }

  // std::ostream& operator<<(const Requirement &a) {
  //   return "Requirement(" << std::to_string(a._max_value) << ")";
  // }

  // friend std::ostream& operator<<(std::ostream& strm, const Requirement& a);

  // friend std::ostream& operator<<(std::ostream&, const Requirement&);
  friend std::ostream& operator<<(std::ostream& os, const Requirement& a) {
    return a.write(os);
  }
private:
  std::ostream& write(std::ostream& os) const {
    return os << "Requirement[_id: " << id_ << ", " << "_max_value: " << max_value_ << "]";
  }

  int id_;

  int max_value_;
};  // end of class Requirement

// std::ostream& operator<<(std::ostream& os, const Requirement& a) {
//   return a.write(os);
// }

#endif  // SRC_MODEL_REQUIREMENT_H_
