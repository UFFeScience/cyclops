/**
 * \file src/model/activation.h
 * \brief Contains the \c Activation class declaration
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo.raps@gmail.com\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2024
 *
 * This header file contains the \c Activation class
 */

#ifndef APPROXIMATE_SOLUTIONS_SRC_MODEL_ACTIVATION_H_
#define APPROXIMATE_SOLUTIONS_SRC_MODEL_ACTIVATION_H_


#include <string>
#include <utility>
#include <vector>

#include <sstream>
#include <iterator>

#include <memory>
#include <src/solution/algorithm.h>

#include "src/model/file.h"
#include "virtual_machine.h"

/// Forward declaration of the class Algorithm, needed because of the circular reference
class Algorithm;

/**
 * \class Activation task.h "src/model/task.h"
 * \brief Represents the task of the model
 */
class Activation {
public:
    /// Parametrised constructor
    explicit Activation(const size_t id, std::string tag, std::string name, const double time) :
            id_(id), tag_(std::move(tag)), name_(std::move(name)), execution_time_(time) {}

    ~Activation() = default;

    /// Getter for id_
    [[nodiscard]] size_t get_id() const { return id_; }

    /// Getter for tag_
    [[nodiscard]] const std::string &get_tag() const { return tag_; }

    /// Getter for name_
    [[nodiscard]] const std::string &get_name() const { return name_; }

    /// Getter for execution_time_
    [[nodiscard]] double get_time() const { return execution_time_; }

    /// Getter for input_files_
    [[nodiscard]] std::vector<std::shared_ptr<File>> get_input_files() const { return input_files_; }

    /// Getter for output_files_
    [[nodiscard]] std::vector<std::shared_ptr<File>> get_output_files() const { return output_files_; }

    /// Getter for requirements_
    [[nodiscard]] std::vector<int> get_requirements() const { return requirements_; }

    /// Adds a input file
    void AddInputFile(const std::shared_ptr<File> &file) {
        input_files_.push_back(file);
    }

    /// Adds a output file
    size_t AddOutputFile(const std::shared_ptr<File> &file) {
        size_t index = output_files_.size();
        output_files_.push_back(file);
        return index;
    }

    /// Adds a requirement value
    void AddRequirement(int requirement) { requirements_.push_back(requirement); }

    /// Get a requirement value
    [[nodiscard]] int GetRequirementValue(size_t requirement_id) const { return requirements_[requirement_id]; }

    // Redefining operators

    /// Equal operator
    bool operator==(const Activation &rhs) const {
        return rhs.get_id() == id_;
    }

    /// Difference operator
    bool operator!=(const Activation &rhs) const {
        return rhs.get_id() != id_;
    }

    /// Concatenate operator
    friend std::ostream &operator<<(std::ostream &strm, const Activation &a) {
        std::ostringstream oss;
        // TODO input files
        oss << "\nInput Files { \n";
//        if (!a.input_files_.empty()) {
//            // Convert all but the last element to avoid a trailing ","
//            for (File *o: a.input_files_) {
//                oss << "\tFile[ID " << o->get_id() << ", name " << o->get_name() << ", size " << o->get_size() << "]\n";
//            }
//        }
        // TODO output files
        oss << "}, Output Files {\n";
//        if (!a.output_files_.empty()) {
//            // Convert all but the last element to avoid a trailing ","
//            for (File *o: a.output_files_) {
//                oss << "\tFile[ID " << o->get_id() << ", name " << o->get_name() << ", size " << o->get_size() << "]\n";
//            }
//        }

        oss << "}, Requirements: {\n";
        if (!a.requirements_.empty()) {
            // Convert all but the last element to avoid a trailing ","
            for (size_t r = 0ul; r < a.requirements_.size(); ++r) {
                oss << "Requirement[ID " << r << ", value " << a.requirements_[r] << "]\n";
            }
        }
        oss << "}";

        return strm << "Activation[ID " << a.id_ << ", TAG " << a.tag_ << ", name " << a.name_
                    << ", execution time " << a.execution_time_ << ", " << oss.str() << "] ";
    }

private:
    /// The id of the task
    size_t id_;

    /// The tag of the task
    std::string tag_;

    /// The name of the task
    std::string name_;

    /// The execution_time_ necessary to execute this task in a default machine
    double execution_time_;

    /// The input files necessary to execute this task
    std::vector<std::shared_ptr<File>> input_files_;

    /// The output files produced by this task when executed
    std::vector<std::shared_ptr<File>> output_files_;

    /// A vector containing the requirement values
    std::vector<int> requirements_;

    ///
    std::vector<std::shared_ptr<Activation>> successors_{};

    ///
    std::vector<std::shared_ptr<Activation>> predecessors_{};

    ///
    std::shared_ptr<VirtualMachine> vm_;
};


#endif  // APPROXIMATE_SOLUTIONS_SRC_MODEL_ACTIVATION_H_
