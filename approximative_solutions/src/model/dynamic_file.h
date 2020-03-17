/**
 * \file src/model/DynamicFile.h
 * \brief Contains the \c DynamicFile class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \date 2020
 *
 * This header file contains the \c DynamicFile class.
 */

#ifndef SRC_MODEL_DYNAMIC_FILE_H_
#define SRC_MODEL_DYNAMIC_FILE_H_

#include "src/model/file.h"

class DynamicFile : public File {
 public:
  /// Parametrised constructor
  explicit DynamicFile(const int id, const std::string name, const double size) :
    File(id, name, size) { }

  friend std::ostream& operator<<(std::ostream& strm, const DynamicFile& a) {
    return strm << "DynamicFile[_id: " << a.id_ << ", "
      << "_name: " << a.name_ << ", "
      << "_size: " << a.size_ << "]";
  }

  /// Getter for _static
  bool isStatic() { return false; };

 private:
};  // end of class DynamicFile

#endif  // SRC_MODEL_DYNAMIC_FILE_H_
