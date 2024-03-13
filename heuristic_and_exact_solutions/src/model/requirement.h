/**
 * \file src/model/requirement.h
 * \brief Contains the \c Requirement class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2020
 *
 * This header file contains the \c Requirement class.
 */

#ifndef APPROXIMATIVE_SOLUTIONS_SRC_MODEL_REQUIREMENT_H_
#define APPROXIMATIVE_SOLUTIONS_SRC_MODEL_REQUIREMENT_H_

#include <string>
#include <iostream>
#include <fstream>

/**
 * \class Requirement requirement.h "src/model/requirement.h"
 * \brief Represents the requirement needed for the execution of the \c Activation in a Virtual Machine
 */
class Requirement {
public:
    /// Parametrised constructor
    explicit Requirement(const size_t id, const double max_value) : id_(id), max_value_(static_cast<int>(max_value)) {}

    /// Getter for ID of the \c Requirement
    [[nodiscard]] size_t get_id() const { return id_; }

    /// Getter for maximum integer value that a specific Requirement can have
    [[nodiscard]] int get_max_value() const { return max_value_; }

    // Concatenate operator
    friend std::ostream &operator<<(std::ostream &os, const Requirement &a) {
        return a.write(os);
    }

private:
    /// Print the \c Requirement object to the output stream
    std::ostream &write(std::ostream &os) const {
        return os << "Requirement[_id: " << id_ << ", " << "_max_value: " << max_value_ << "]";
    }

    /// The ID of the \c Requirement
    size_t id_;

    /// The maximum value that a \c Requirement can have
    int max_value_;
};  // end of class Requirement

#endif  // APPROXIMATIVE_SOLUTIONS_SRC_MODEL_REQUIREMENT_H_
