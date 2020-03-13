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


class Bucket {
 public:
  Bucket(int id,
         std::string capacity,
         int numberOfIntervals) :
		_id(id),
    _capacity(capacity),
    _numberOfIntervals(numberOfIntervals) {}

  /// Getter for _id
  int getId() const { return _id; }

  // Getter for _capacity
  const std::string &getCapacity() const { return _capacity; }

  /// Getter for _numberOfIntervals
  int getNumberOfIntervals() const { return _numberOfIntervals; }

 private:
	int _id;

	std::string _capacity;

	int _numberOfIntervals;
};


#endif  // SRC_MODEL_BUCKET_H_
