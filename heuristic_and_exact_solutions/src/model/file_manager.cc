/**
 * \file src/model/file_manager.cc
 * \brief Contains the \c FileManager class implementation
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo.raps@gmail.com\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2024
 *
 * This source file contains the \c FileManager class implementation
 */

#include "file_manager.h"

FileManager::FileManager(size_t files_size, size_t storages_size, const std::shared_ptr<ConflictGraph> &conflict_graph)
        : total_conflict_(0ul),
          file_allocations_(files_size, std::numeric_limits<size_t>::max()),
          storages_conflict_(storages_size, 0ul),
          files_distribution_(storages_size, std::list<size_t>()),
          conflict_graph_(conflict_graph) {
}

size_t FileManager::get_file_allocation(size_t file_id) const {
    return file_allocations_[file_id];
}

void FileManager::set_file_allocation(size_t file_id, size_t storage_id) {

    DLOG(INFO) << "file_id " << file_id << " storage_id " << storage_id;

    if (file_allocations_[file_id] != std::numeric_limits<size_t>::max()) {
        LOG(FATAL) << "Reassign is not permitted";
    }

    file_allocations_[file_id] = storage_id;
//    auto sum_of_conflicts = 0ul;
    auto sum_of_conflicts = storages_conflict_[storage_id];
    auto previous_sum = storages_conflict_[storage_id];
#ifndef NDEBUG
    auto it = std::find(files_distribution_[storage_id].begin(), files_distribution_[storage_id].end(), file_id);
    if (it != files_distribution_[storage_id].end()) {
        LOG(FATAL) << "Trying to assign the file more than one time";
    }
#endif
    int file_conflict;
    for (auto stored_file_id: files_distribution_[storage_id]) {
        file_conflict = conflict_graph_->ReturnConflict(file_id, stored_file_id);
        DLOG(INFO) << "file_conflict " << file_conflict;
        if (file_conflict < 0) {
            LOG(FATAL) << "Not acceptable file assign with hard-constrain conflict";
        }
        sum_of_conflicts += static_cast<size_t>(file_conflict);
    }
    storages_conflict_[storage_id] = sum_of_conflicts;
    total_conflict_ += (sum_of_conflicts - previous_sum);
    files_distribution_[storage_id].push_back(file_id);
}

bool FileManager::ChangeFileAllocation(size_t file_id, size_t new_storage_id) {

    auto old_storage_id = file_allocations_[file_id];
    if (old_storage_id == new_storage_id) {
        LOG(FATAL) << "Same storage is not permitted";
    }
    auto it = std::find(files_distribution_[old_storage_id].begin(), files_distribution_[old_storage_id].end(),
            file_id);
    if (it != files_distribution_[old_storage_id].end()) {
        // Accumulate old vm conflict between files
        int file_conflict;
        auto sum_of_conflicts_old_vm = 0ul;
        for (auto stored_file_id: files_distribution_[old_storage_id]) {
            if (file_id != stored_file_id) {
                file_conflict = conflict_graph_->ReturnConflict(file_id, stored_file_id);
                if (file_conflict < 0) {
                    LOG(FATAL) << "Makes no sense, would not be a hard-conflict here";
                }
                sum_of_conflicts_old_vm += static_cast<size_t>(file_conflict);
            }
        }
        // Accumulate old vm conflict between files
        auto sum_of_conflicts_new_vm = 0ul;
        for (auto stored_file_id: files_distribution_[new_storage_id]) {
            file_conflict = conflict_graph_->ReturnConflict(file_id, stored_file_id);
            if (file_conflict < 0) {
                return false;
            }
            sum_of_conflicts_new_vm += static_cast<size_t>(file_conflict);
        }
        storages_conflict_[old_storage_id] -= sum_of_conflicts_old_vm;
        storages_conflict_[new_storage_id] += sum_of_conflicts_new_vm;
        total_conflict_ += (sum_of_conflicts_new_vm - sum_of_conflicts_old_vm);
        // Moving file from between storages
        files_distribution_[old_storage_id].erase(it);
        files_distribution_[new_storage_id].push_back(file_id);
        file_allocations_[file_id] = new_storage_id;
    } else {
        LOG(FATAL) << "File ID not found";
    }
    return true;
}

bool FileManager::FileHasHardConstraintsAgainstVmFiles(size_t file_id, size_t vm_id) {
    int file_conflict;
    for (auto stored_file_id: files_distribution_[vm_id]) {
        if (file_id != stored_file_id) {
            file_conflict = conflict_graph_->ReturnConflict(file_id, stored_file_id);
            if (file_conflict < 0) {
                return true;
            }
        } else {
            LOG(FATAL) << "Makes no sense, the file is already assign to a VM";
        }
    }
    return false;
}

size_t FileManager::get_file_privacy_exposure() const {

    DLOG(INFO) << "total_conflict_: " << total_conflict_;
    return total_conflict_;
}
