/**
 * \file src/model/file_manager.h
 * \brief Contains the \c FileManager class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo.raps@gmail.com\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2024
 *
 * This header file contains the \c FileManager class.
 */

#ifndef WF_SECURITY_FILE_MANAGER_H
#define WF_SECURITY_FILE_MANAGER_H


#include <algorithm>
#include <iostream>
#include <glog/logging.h>
#include <limits>
#include <list>
#include <memory>
#include <string>
#include <vector>
#include "src/model/conflict_graph.h"

class FileManager {
public:
    /// Constructor declaration
    explicit FileManager(size_t, size_t, const std::shared_ptr<ConflictGraph> &);

    /// Pass the file id, and return the storage id where the file is located
    [[nodiscard]] size_t get_file_allocation(size_t) const;

    ///
    void set_file_allocation(size_t, size_t);

    ///
    bool ChangeFileAllocation(size_t file_id, size_t new_storage_id);

    ///
    bool FileHasHardConstraintsAgainstVmFiles(size_t, size_t);

    ///
    [[nodiscard]] size_t get_file_privacy_exposure() const;
private:
    ///
    size_t total_conflict_;

    /// Allocation of files in theirs storages
    std::vector<size_t> file_allocations_;

    ///
    std::vector<size_t> storages_conflict_;

    /// Each position in the vector represents a list of files stored in that storage
    std::vector<std::list<size_t>> files_distribution_;

    ///
    std::shared_ptr<ConflictGraph> conflict_graph_;
};


#endif //WF_SECURITY_FILE_MANAGER_H
