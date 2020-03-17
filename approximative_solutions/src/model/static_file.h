/**
 * \file src/model/StaticFile.h
 * \brief Contains the \c StaticFile class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \date 2020
 *
 * This header file contains the \c StaticFile class.
 */

#ifndef SRC_MODEL_STATIC_FILE_H_
#define SRC_MODEL_STATIC_FILE_H_

#include <vector>

#include "src/model/file.h"

class StaticFile : public File {
 public:
  /// Parametrised constructor
  explicit StaticFile(const int id, const std::string name, const double size) :
    File(id, name, size) { }

  /// Adds a vm
  void addVm(int vmId) { vms_.push_back(vmId); }

  /// Getter for _place
  int getFirstVm() const { return vms_[0]; }

  /// Getter for _static
  bool isStatic() { return true; };

  friend std::ostream& operator<<(std::ostream& strm, const StaticFile& a) {

    std::ostringstream oss;

    if (!a.vms_.empty()) {
      // Convert all but the last element to avoid a trailing ","
      std::copy(a.vms_.begin(), a.vms_.end() - 1,
          std::ostream_iterator<int>(oss, ","));

      // Now add the last element with no delimiter
      oss << a.vms_.back();
    }

    // std::cout << oss.str() << std::endl;

    return strm << "StaticFile[_id: " << a.id_ << ", "
      << "_name: " << a.name_ << ", "
      << "_size: " << a.size_ << ", "
      << "_vms: " << oss.str() << "]";
  }

 private:
  std::vector<int> vms_;
};  // end of class StaticFile

#endif  // SRC_MODEL_STATIC_FILE_H_
