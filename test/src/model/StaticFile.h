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

#include "src/model/File.h"

class StaticFile : public File {
 public:
  /// Parametrised constructor
  explicit StaticFile(const int id, const std::string name, const double size) :
    File(id, name, size) { }

  /// Adds a vm
  void addVm(int vmId) { _vms.push_back(vmId); }

  /// Getter for _place
  int getFirstVm() const { return _vms[0]; }

  /// Getter for _static
  bool isStatic() { return true; };

  friend std::ostream& operator<<(std::ostream& strm, const StaticFile& a) {

    std::ostringstream oss;

    if (!a._vms.empty()) {
      // Convert all but the last element to avoid a trailing ","
      std::copy(a._vms.begin(), a._vms.end() - 1,
          std::ostream_iterator<int>(oss, ","));

      // Now add the last element with no delimiter
      oss << a._vms.back();
    }

    // std::cout << oss.str() << std::endl;

    return strm << "StaticFile[_id: " << a._id << ", "
      << "_name: " << a._name << ", "
      << "_size: " << a._size << ", "
      << "_vms: " << oss.str() << "]";
  }

 private:
  std::vector<int> _vms;
};  // end of class StaticFile

#endif  // SRC_MODEL_STATIC_FILE_H_
