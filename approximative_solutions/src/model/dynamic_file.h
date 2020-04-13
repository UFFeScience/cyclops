/**
 * \file src/model/DynamicFile.h
 * \brief Contains the \c DynamicFile class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2020
 *
 * This header file contains the \c DynamicFile class.
 */

#ifndef APPROXIMATIVE_SOLUTIONS_SRC_MODEL_DYNAMIC_FILE_H_
#define APPROXIMATIVE_SOLUTIONS_SRC_MODEL_DYNAMIC_FILE_H_

#include <glog/logging.h>

#include <string>

#include "src/model/file.h"

/**
 * \class DynamicFile dynamic_file.h "src/model/dynamic_file.h"
 * \brief Represents the Dynamic File; a file produced by the executaion of a \c Task
 */
class DynamicFile : public File {
 public:
  /// Parametrized constructor
  explicit DynamicFile(const size_t id, const std::string name, const double size) :
    File(id, name, size) { }

  /// Default destructor
  ~DynamicFile() = default;

  /// Concatenator operator
  friend std::ostream& operator<<(std::ostream& os, const DynamicFile& a) {
    return a.write(os);
  }

 private:
  /// Writes the Dynamic File object to the output stream
  std::ostream& write(std::ostream& os) const {
    return os << "DynamicFile[id_: " << id_
              << ", name_: " << name_
              << ", size_: " << size_ << "]";
  }
};  // end of class DynamicFile

#endif  // APPROXIMATIVE_SOLUTIONS_SRC_MODEL_DYNAMIC_FILE_H_
