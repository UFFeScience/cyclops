/**
 * \file src/model/file.h
 * \brief Contains the \c File class declaration
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo.raps@gmail.com\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2024
 *
 * This header file contains the \c File class
 */

#ifndef APPROXIMATE_SOLUTIONS_SRC_MODEL_FILE_H_
#define APPROXIMATE_SOLUTIONS_SRC_MODEL_FILE_H_

#include <string>
#include <utility>
#include <vector>

#include <sstream>
#include <iterator>

#include <limits>
#include "storage.h"

/**
 * \class File file.h "src/model/file.h"
 * \brief It is a file abstraction used by the \c DynamicFile and the \c StaticFile
 */
class File {
public:
    /// Parametrized constructor
    explicit File(const size_t id,
                  std::string name,
                  const double size_in_MB)
            : id_(id),
              name_(std::move(name)),
              size_in_GB_(size_in_MB / 1000.0) {}

    /// Default destructor
    virtual ~File() = default;

    /// Getter for the ID of the file
    [[nodiscard]] size_t get_id() const { return id_; }

    /// Getter for name of the file
    [[nodiscard]] const std::string &get_name() const { return name_; }

    /// Getter for size in KBs of the file
    [[nodiscard]] double get_size_in_GB() const { return size_in_GB_; }

    /// Concatenate operator
    friend std::ostream &operator<<(std::ostream &os, const File &a) {
        return a.Write(os);
    }

protected:
    virtual /// Print the File object to the output stream
    std::ostream &Write(std::ostream &os) const {
        return os << "File[_id " << id_ << ", name " << name_ << ", size_in_GB " << size_in_GB_ << "]";
    }

    /// The ID of the file
    size_t id_;

    /// The file name
    std::string name_;

    /// The file size in GB
    double size_in_GB_;
};  // end of class File

#endif  // APPROXIMATE_SOLUTIONS_SRC_MODEL_FILE_H_
