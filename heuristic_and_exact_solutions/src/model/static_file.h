/**
 * \file src/model/static_file.h
 * \brief Contains the \c StaticFile class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2021
 *
 * This header file contains the \c StaticFile class.
 */

#ifndef APPROXIMATE_SOLUTIONS_SRC_MODEL_STATIC_FILE_H_
#define APPROXIMATE_SOLUTIONS_SRC_MODEL_STATIC_FILE_H_

#include <glog/logging.h>

#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

#include "src/model/file.h"

class StaticFile : public File {
public:
    /// Parametrised constructor
    explicit StaticFile(const size_t id, const std::string &name, const double size) :
            File(id, name, size) {}

    ~StaticFile() override = default;

    /// Adds a vm
    void AddVm(size_t vmId) { vms_.push_back(vmId); }

    /// Getter the first virtual machine from the list vms_
    [[nodiscard]] size_t GetFirstVm() const { return vms_[0]; }

    friend std::ostream &operator<<(std::ostream &os, const StaticFile &a) {
        return a.Write(os);
    }

private:
    std::ostream &Write(std::ostream &os) const override {
        return os << "StaticFile[id " << id_ << ", name " << name_ << ", size_in_GB " << size_in_GB_ << "]";
    }

    std::vector<size_t> vms_;
};

#endif  // APPROXIMATE_SOLUTIONS_SRC_MODEL_STATIC_FILE_H_
