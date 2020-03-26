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

#include <iterator>
#include <vector>
#include <glog/logging.h>

#include "src/model/file.h"

class StaticFile : public File {
 public:
  /// Parametrised constructor
  explicit StaticFile(const size_t id, const std::string name, const double size) :
    File(id, name, size) { }

  ~StaticFile() { DLOG(INFO) << "deleting static file " << id_; }

  /// Adds a vm
  void AddVm(size_t vmId) { vms_.push_back(vmId); }

  /// Getter the first virtual machine from the list vms_
  size_t GetFirstVm() const { return vms_[0]; }

  /// Getter for _static
  bool IsStatic() { return true; };

  friend std::ostream& operator<<(std::ostream& os, const StaticFile& a) {
    return a.write(os);
  }

 private:
  std::ostream& write(std::ostream& os) const {
    std::ostringstream oss;

    if (!vms_.empty()) {
      // Convert all but the last element to avoid a trailing ","
      std::copy(vms_.begin(), vms_.end() - 1,
          std::ostream_iterator<int>(oss, ","));

      // Now add the last element with no delimiter
      oss << vms_.back();
    }

    // std::cout << oss.str() << std::endl;

    return os << "StaticFile[_id: " << id_ << ", "
      << "_name: " << name_ << ", "
      << "_size: " << size_ << ", "
      << "_vms: " << oss.str() << "]";
  }

  std::vector<size_t> vms_;
};  // end of class StaticFile

#endif  // SRC_MODEL_STATIC_FILE_H_
