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

#include <glog/logging.h>
#include "src/model/file.h"

class DynamicFile : public File {
public:
  /// Parametrised constructor
  explicit DynamicFile(const size_t id, const std::string name, const double size) :
    File(id, name, size) { }

  ~DynamicFile() { DLOG(INFO) << "deleting dynamic file " << id_; }

  /// Getter for _static
  bool isStatic() { return false; };

  friend std::ostream& operator<<(std::ostream& os, const DynamicFile& a) {
    return a.write(os);
  }

private:
  std::ostream& write(std::ostream& os) const {
    return os << "DynamicFile[id_: " << id_
              << ", name_: " << name_
              << ", size_: " << size_ << "]";
  }
};  // end of class DynamicFile

#endif  // SRC_MODEL_DYNAMIC_FILE_H_
