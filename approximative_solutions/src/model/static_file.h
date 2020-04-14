/**
 * \file src/model/static_file.h
 * \brief Contains the \c StaticFile class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2020
 *
 * This header file contains the \c StaticFile class.
 */

#ifndef APPROXIMATIVE_SOLUTIONS_SRC_MODEL_STATIC_FILE_H_
#define APPROXIMATIVE_SOLUTIONS_SRC_MODEL_STATIC_FILE_H_

#include <glog/logging.h>

#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

#include "src/model/file.h"

class StaticFile : public File {
 public:
  /// Parametrised constructor
  explicit StaticFile(const size_t id, const std::string name, const double size) :
    File(id, name, size) { }

  ~StaticFile() = default;

  /// Adds a vm
  void AddVm(size_t vmId) { vms_.push_back(vmId); }

  /// Getter the first virtual machine from the list vms_
  size_t GetFirstVm() const { return vms_[0]; }

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

    return os << "StaticFile[id_: " << id_
              << ", name_: " << name_
              << ", size_: " << size_
              << ", vms_: " << oss.str() << "]";
  }

  std::vector<size_t> vms_;
};  // end of class StaticFile

#endif  // APPROXIMATIVE_SOLUTIONS_SRC_MODEL_STATIC_FILE_H_
