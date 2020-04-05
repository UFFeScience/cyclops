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
#include <glog/logging.h>
#include "src/model/storage.h"

class Bucket : public Storage {
public:
  Bucket(size_t id,
         std::string name,
         double storage,
         double cost,
         double bandwidth,
         int type_id,
         size_t numberOfIntervals) :
		  Storage(id, name, storage, cost, bandwidth, type_id),
      numberOfIntervals_(numberOfIntervals) {}

  ~Bucket() {
    // DLOG(INFO) << "Deleting Bucket " << id_;
  }

  /// Getter for _numberOfIntervals
  size_t getNumberOfIntervals() const { return numberOfIntervals_; }

  friend std::ostream& operator<<(std::ostream& os, const Bucket& a) {
    return a.write(os);
  }
private:
  std::ostream& write(std::ostream& os) const {
    return os << "Bucket[id_: " << id_
        << ", name_: " << name_
        << ", storage_: " << storage_
        << ", cost_: " << cost_
        << ", bandwidth_: " << bandwidth_
        << ", type_id_: " << type_id_
        << ", numberOfIntervals_: " << numberOfIntervals_
        << "]";
  }

	size_t numberOfIntervals_;
};

#endif  // SRC_MODEL_BUCKET_H_
