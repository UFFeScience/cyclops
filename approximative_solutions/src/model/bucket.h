/**
 * \file src/model/Bucket.h
 * \brief Contains the \c Bucket class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \date 2020
 *
 * This header file contains the \c Bucket class.
 */

#ifndef SRC_MODEL_BUCKET_H_
#define SRC_MODEL_BUCKET_H_

#include <string>

class Bucket {
 public:
  Bucket(size_t id,
         std::string capacity,
         size_t numberOfIntervals) :
		  id_(id),
      capacity_(capacity),
      numberOfIntervals_(numberOfIntervals) {}

  /// Getter for _id
  size_t getId() const { return id_; }

  // Getter for _capacity
  const std::string &getCapacity() const { return capacity_; }

  /// Getter for _numberOfIntervals
  size_t getNumberOfIntervals() const { return numberOfIntervals_; }

  friend std::ostream& operator<<(std::ostream& os, const Bucket& a) {
    return a.write(os);
  }
 private:
  std::ostream& write(std::ostream& os) const {
    return os << "Bucket[id_: " << id_
        << ", capacity_: " << capacity_
        << ", numberOfIntervals_: " << numberOfIntervals_
        << "]";
  }

	size_t id_;

	std::string capacity_;

	size_t numberOfIntervals_;
};


#endif  // SRC_MODEL_BUCKET_H_
