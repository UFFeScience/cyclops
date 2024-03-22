/**
 * \file src/model/bucket.h
 * \brief Contains the \c Bucket class declaration
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo.raps@gmail.com\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2024
 *
 * This header file contains the \c Bucket class
 */

#ifndef APPROXIMATIVE_SOLUTIONS_SRC_MODEL_BUCKET_H_
#define APPROXIMATIVE_SOLUTIONS_SRC_MODEL_BUCKET_H_

#include <glog/logging.h>

#include <string>
#include <utility>

#include "src/model/storage.h"

/**
 * \class Bucket bucket.h "src/model/bucket.h"
 * \brief Represents the bucket; a storage that do not execute task like Virtual Machines
 */
class Bucket : public Storage {
public:
    /// Parameterized constructor with full member initialisation
    Bucket(size_t id,
           std::string name,
           double storage,
           double cost,
           double bandwidth,
           int type_id,
           size_t numberOfIntervals)
            : Storage(id, std::move(name), storage, cost, bandwidth, type_id),
              number_of_GB_per_cost_intervals_(numberOfIntervals) {}

    /// Default destructor
    ~Bucket() override = default;

    /// Getter for the upper limit of the storage of the specified interval
    [[nodiscard]] double get_storage_of_the_interval(size_t interval) const {
        if (interval == 0) {
            return 0.0;
        } else if (interval == 1) {
            return storage_;
        } else {
            exit(1);
        }
    }

    /// Getter for numberOfIntervals_ (not used for now)
    [[nodiscard]] size_t get_number_of_GB_per_cost_intervals() const { return number_of_GB_per_cost_intervals_; }

    /// Concatenate operator
    friend std::ostream &operator<<(std::ostream &os, const Bucket &a) {
        return a.write(os);
    }

private:
    /// Print the bucket in the output stream
    std::ostream &write(std::ostream &os) const {
        std::ostringstream oss;

        oss << "requirements_: {";
        if (!requirements_.empty()) {
            // Convert all but the last element to avoid a trailing ","
            for (size_t r = 0ul; r < requirements_.size(); ++r) {
                oss << "Requirement[id_: " << r << ", value_: " << requirements_[r] << "]\n";
            }
        }
        oss << "}";

        return os << "Bucket[id_: " << id_
                  << ", name_: " << name_
                  << ", storage_: " << storage_
                  << ", cost_: " << cost_
                  << ", bandwidth_: " << bandwidth_
                  << ", type_id_: " << type_id_
                  << ", numberOfIntervals_: " << number_of_GB_per_cost_intervals_
                  << ", " << oss.str()
                  << "]";
    }

    /// Number of the GBs/Cost intervals
    size_t number_of_GB_per_cost_intervals_;
};  // end of class Bucket

#endif  // APPROXIMATIVE_SOLUTIONS_SRC_MODEL_BUCKET_H_
