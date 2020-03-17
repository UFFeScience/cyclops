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
  Bucket(int id,
         std::string capacity,
         int numberOfIntervals) :
		  id_(id),
      capacity_(capacity),
      numberOfIntervals_(numberOfIntervals) {}

  /// Getter for _id
  int getId() const { return id_; }

  // Getter for _capacity
  const std::string &getCapacity() const { return capacity_; }

  /// Getter for _numberOfIntervals
  int getNumberOfIntervals() const { return numberOfIntervals_; }

 private:
	int id_;

	std::string capacity_;

	int numberOfIntervals_;
};


#endif  // SRC_MODEL_BUCKET_H_
