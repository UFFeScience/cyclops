/**
 * \file src/model/dynamic_file.h
 * \brief Contains the \c DynamicFile class declaration
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo.raps@gmail.com\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2024
 *
 * This header file contains the \c DynamicFile class
 */

#ifndef APPROXIMATE_SOLUTIONS_SRC_MODEL_DYNAMIC_FILE_H_
#define APPROXIMATE_SOLUTIONS_SRC_MODEL_DYNAMIC_FILE_H_


#include <glog/logging.h>

#include <string>
#include <utility>

#include "src/model/activation.h"
#include "src/model/file.h"

/// Forward declaration of the class Algorithm, needed because of the circular reference
class Activation;

/**
 * \class DynamicFile dynamic_file.h "src/model/dynamic_file.h"
 * \brief Represents the Dynamic File; a file produced by the execution of a \c Activation
 */
class DynamicFile : public File {
public:
    /// Parametrized constructor
    explicit DynamicFile(const size_t id, const std::string &name, const double size) :
            File(id, name, size) {}

    /// Default destructor
    ~DynamicFile() override = default;

    /// Getter for the parent task
    [[nodiscard]] std::weak_ptr<Activation> get_parent_task() const { return parent_task_; }

    /// Getter for the parent output file index
    [[nodiscard]] size_t get_parent_output_file_index() const { return parent_output_file_index_; }

    /// Setter for the parent task
    void set_parent_task(const std::shared_ptr<Activation> &parent_task) { parent_task_ = parent_task; }

    /// Setter for the parent output file index
    void set_parent_output_file_index(const size_t parent_output_file_index) {
        parent_output_file_index_ = parent_output_file_index;
    }

    /// Concatenate operator
    friend std::ostream &operator<<(std::ostream &os, const DynamicFile &a) {
        return a.Write(os);
    }

private:
    /// Writes the Dynamic File object to the output stream
    std::ostream &Write(std::ostream &os) const override {
        return os << "DynamicFile[id " << id_ << ", name " << name_ << ", size " << size_ << "]";
    }

    /// The \c Activation that generated this file
    std::weak_ptr<Activation> parent_task_;

    /// The index position of this generated file
    size_t parent_output_file_index_ = std::numeric_limits<size_t>::max();
};

#endif  // APPROXIMATE_SOLUTIONS_SRC_MODEL_DYNAMIC_FILE_H_
