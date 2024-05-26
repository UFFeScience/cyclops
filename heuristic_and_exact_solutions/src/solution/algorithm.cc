/**
 * \file src/solution/algorithm.cc
 * \brief Contains the \c Algorithm class methods
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo.raps@gmail.com\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2024
 *
 * This source file contains the methods from the \c Algorithm class that run the mode the
 * approximate solution
 */

#include "src/solution/algorithm.h"

#include <boost/algorithm/string.hpp>
#include <filesystem>
#include "src/solution/grch.h"
#include "src/solution/grasp.h"
#include "src/solution/cplex.h"
#include "heft.h"

Algorithm::Algorithm() {
    conflict_graph_ = std::make_shared<ConflictGraph>();
}

void Algorithm::ReadTasksAndFiles(const std::string &tasks_and_files_file,
                                  std::unordered_map<std::string, std::shared_ptr<File>> &file_map_per_name) {
    DLOG(INFO) << "Reading Activations and Files from input file [" + tasks_and_files_file + "]" ;

    if (!std::filesystem::exists(tasks_and_files_file)) {
        LOG(FATAL) << "Activations and Tasks input file could not be found in \"" << tasks_and_files_file << "\"!";
    }

    size_t task_size;
    size_t file_size;
    size_t requirement_size;
    std::unordered_map<std::string, std::shared_ptr<Activation>> task_map_per_name_;

    // Reading file
    std::string line;
    std::ifstream in_file(tasks_and_files_file);

    // Get number of tasks and number of files
    getline(in_file, line);
    DLOG(INFO) << "Head: " << line;
    std::vector<std::string> tokens;
    boost::split(tokens, line, boost::is_any_of(" "));

    static_file_size_ = stoul(tokens[0]);
    dynamic_file_size_ = stoul(tokens[1]);
    task_size = stoul(tokens[2]) + 2;  // Adding two tasks (source and target)
    requirement_size = stoul(tokens[3]);
    makespan_max_ = stod(tokens[4]);
    budget_max_ = stod(tokens[5]);
    file_size = static_file_size_ + dynamic_file_size_;

    DLOG(INFO) << "static_file_size_: " << static_file_size_;
    DLOG(INFO) << "dynamic_file_size_: " << dynamic_file_size_;
    DLOG(INFO) << "task_size_: " << task_size;
    DLOG(INFO) << "requirement_size_: " << requirement_size;
    DLOG(INFO) << "makespan_max_: " << makespan_max_;
    DLOG(INFO) << "budget_max_: " << budget_max_;
    DLOG(INFO) << "file_size_: " << file_size;

    getline(in_file, line);  // Reading blank line

    // Reading information about requirements
    for (size_t i = 0; i < requirement_size; i++) {
        getline(in_file, line);
        DLOG(INFO) << "Requirement: " << line;

        std::vector<std::string> strs;
        boost::split(strs, line, boost::is_any_of(" "));

        size_t id = stoul(strs[0]);
        double max_value = stod(strs[1]);
        Requirement my_requirement = Requirement(id, max_value);
        requirements_.push_back(my_requirement);
        DLOG(INFO) << my_requirement;
    }

    getline(in_file, line);  // reading blank line

    files_.reserve(file_size);
    file_map_per_name.reserve(file_size);

    // Reading information about static files
    for (size_t i = 0ul; i < static_file_size_; i++) {
        getline(in_file, line);
        DLOG(INFO) << "File: " << line;

        std::vector<std::string> strs;
        boost::split(strs, line, boost::is_any_of(" "));
        auto my_file_name = strs[0];
        auto my_file_size = stod(strs[1]);

        DLOG(INFO) << "file_name: " << my_file_name;
        DLOG(INFO) << "file_size: " << my_file_size;

        std::shared_ptr<File> my_staticFile = std::make_shared<StaticFile>(i, my_file_name, my_file_size);

        for (size_t j = 0; j < stoul(strs[2]); ++j) {
            auto msf = std::dynamic_pointer_cast<StaticFile>(my_staticFile);
            msf->AddVm(stoul(strs[3 + j]));
        }

        DLOG(INFO) << *my_staticFile;

        files_.push_back(my_staticFile);
        auto mfn = std::string(my_file_name);
        auto pair = std::make_pair(mfn, my_staticFile);
        file_map_per_name.insert(pair);
    }

    // Reading information about dynamic files
    for (size_t i = static_file_size_; i < static_file_size_ + dynamic_file_size_; i++) {
        getline(in_file, line);
        DLOG(INFO) << "File: " << line;

        std::vector<std::string> strs;
        boost::split(strs, line, boost::is_any_of(" "));
        const std::string my_file_name = strs[0];
        auto my_file_size = stod(strs[1]);

        DLOG(INFO) << "file_name: " << my_file_name;
        DLOG(INFO) << "file_size: " << my_file_size;

        std::shared_ptr<File> my_dynamicFile = std::make_shared<DynamicFile>(i, my_file_name, my_file_size);

        DLOG(INFO) << *my_dynamicFile;

        files_.push_back(my_dynamicFile);
        auto mfn = std::string(my_file_name);
        auto pair = std::make_pair(mfn, my_dynamicFile);
        file_map_per_name.insert(pair);
    }

    getline(in_file, line);  // reading blank line

    activations_.reserve(task_size);
    task_map_per_name_.reserve(task_size);

    // Reading information about tasks
    id_source_ = 0;
    id_target_ = task_size - 1;

    std::shared_ptr<Activation> source_task = std::make_shared<Activation>(id_source_, "source", "SOURCE", 0.0);
    std::shared_ptr<Activation> target_task = std::make_shared<Activation>(id_target_, "target", "TARGET", 0.0);

    for (size_t i = 0; i < requirement_size; ++i) {
        source_task->AddRequirement(0);
        target_task->AddRequirement(0);
    }

    activations_.push_back(source_task);

    for (size_t i = 1; i < task_size - 1; i++) {
        getline(in_file, line);
        DLOG(INFO) << "Activation: " << line;
        std::vector<std::string> strs;
        boost::split(strs, line, boost::is_any_of(" "));

        // Get task info
        auto tag = strs[0];
        std::string task_name = strs[1];
        auto base_time = stod(strs[2]);
        auto in_size = stoul(strs[3]);
        auto out_size = stoul(strs[4]);

        DLOG(INFO) << "tag: " << tag;
        DLOG(INFO) << "task_name: " << task_name;
        DLOG(INFO) << "base_time: " << base_time;
        DLOG(INFO) << "in_size: " << in_size;
        DLOG(INFO) << "out_size: " << out_size;

        std::shared_ptr<Activation> my_task = std::make_shared<Activation>(i, tag, task_name, base_time);

        // Adding requirement to the task
        for (size_t j = 5ul; j < 5ul + requirement_size; ++j) {
            auto requirement_value = stod(strs[j]);
            my_task->AddRequirement(static_cast<int>(requirement_value));
        }

        // Reading input files
        for (size_t j = 0; j < in_size; j++) {
            getline(in_file, line);
            DLOG(INFO) << "Input file: " << line;

            std::shared_ptr<File> my_file(file_map_per_name.find(line)->second);

            my_task->AddInputFile(my_file);
        }

        // Reading output files
        for (size_t j = 0; j < out_size; j++) {
            getline(in_file, line);
            DLOG(INFO) << "Output file: " << line;

            auto my_file(file_map_per_name.find(line)->second);

            auto index = my_task->AddOutputFile(my_file);

            if (auto dynamic_file = std::dynamic_pointer_cast<DynamicFile>(my_file)) {
                auto p = dynamic_file->get_parent_task().lock();
                if (p) {
                    LOG(FATAL) << my_file->get_name() << " already have a parent task!";
                }
                dynamic_file->set_parent_task(my_task);
                dynamic_file->set_parent_output_file_index(index);
            }
        }

        activations_.push_back(my_task);
        task_map_per_name_.insert(std::make_pair(tag, my_task));

        DLOG(INFO) << my_task;
    }

    getline(in_file, line);  // reading blank line

    // Update Source and Target tasks
    activations_.push_back(target_task);

    task_map_per_name_.insert(std::make_pair("source", source_task));
    task_map_per_name_.insert(std::make_pair("target", target_task));

    successors_.resize(task_size, std::vector<size_t>());

    std::vector<int> aux(task_size, -1);
    aux[id_source_] = 0;
    aux[id_target_] = 0;

    // Reading successors graph information
    for (size_t i = 0; i < task_size - 2; i++) {
        getline(in_file, line);  // Reading parent task

        DLOG(INFO) << "Parent: " << line;

        std::vector<std::string> strs;
        boost::split(strs, line, boost::is_any_of(" "));
        auto task_tag = strs[0];
        auto number_of_successors = stoi(strs[1]);

        std::vector<size_t> children;
        // Reading children task
        for (int j = 0; j < number_of_successors; j++) {
            getline(in_file, line);

            DLOG(INFO) << "Child: " << line;

            auto child_task = task_map_per_name_.find(line)->second;

            children.push_back(child_task->get_id());
            aux[child_task->get_id()] = 0;
        }

        // Target task
        if (number_of_successors == 0) {
            children.push_back(id_target_);
        }

        auto task = task_map_per_name_.find(task_tag)->second;

        successors_[task->get_id()] = children;
    }

    // Add synthetic source task
    for (size_t i = 0; i < task_size; i++) {
        // Add source
        if (aux[i] == -1) {
            // f_source.first->second.push_back(i);
            successors_[0].push_back(i);
        }
    }

    predecessors_.resize(task_size, std::vector<size_t>());

    for (auto i = 0ul; i < successors_.size(); ++i) {
        for (auto successor_id: successors_[i]) {
            predecessors_[successor_id].push_back(i);
        }
    }

    in_file.close();
}

void Algorithm::ReadCluster(const std::string &cluster) {
    DLOG(INFO) << "Reading Clusters from input file [" + cluster + "]" ;

    if (!std::filesystem::exists(cluster)) {
        LOG(FATAL) << "Cluster file could not be found \"" << cluster << "\"!";
    }

    size_t vm_size;

    // Reading file
    std::string line;
    std::ifstream in_cluster(cluster);
    std::vector<std::string> tokens;

    getline(in_cluster, line);

    DLOG(INFO) << "Head: " << line;

    boost::split(tokens, line, boost::is_any_of(" "));

    size_t number_of_requirements = stoul(tokens[1]);

    getline(in_cluster, line);  // ignore line

    size_t storage_id = 0ul;

    getline(in_cluster, line);
    DLOG(INFO) << "Provider: " << line;

    std::vector<std::string> strs1;
    boost::split(strs1, line, boost::is_any_of(" "));

    vm_size = stoul(strs1[4]);
    size_t bucket_size = stoul(strs1[5]);

    // Reading VMs information
    for (auto j = 0ul; j < vm_size; j++) {
        getline(in_cluster, line);
        DLOG(INFO) << "VM: " << line;

        std::vector<std::string> strs;
        boost::split(strs, line, boost::is_any_of(" "));

        int type_id = stoi(strs[0]);
        std::string vm_name = strs[1];
        double slowdown = stod(strs[2]);
        double storage = stod(strs[3]) * 1024;  // GB to MB
        double bandwidth = stod(strs[4]);
        double cost = stod(strs[5]);

        auto my_vm = std::make_shared<VirtualMachine>(storage_id, vm_name, slowdown, storage, cost, bandwidth, type_id);

        // Adding requirement to the task
        for (size_t l = 6ul; l < 6ul + number_of_requirements; ++l) {
            auto requirement_value = stod(strs[l]);
            my_vm->AddRequirement(requirement_value);
        }

        virtual_machines_.push_back(my_vm);
        storages_.push_back(my_vm);
        storage_id += 1;
        DLOG(INFO) << my_vm;
    }

    // Reading Buckets information
    for (auto j = 0ul; j < bucket_size; j++) {
        getline(in_cluster, line);
        DLOG(INFO) << "Bucket: " << line;

        std::vector<std::string> strs;
        boost::split(strs, line, boost::is_any_of(" "));

        int type_id = stoi(strs[0]);
        std::string name = strs[1];
        double storage = stod(strs[2]) * 1024;  // GB to MB
        double bandwidth = stod(strs[3]);
        size_t number_of_intervals = 1ul;
        double cost = stod(strs[4]);

        auto my_bucket = std::make_shared<Bucket>(storage_id, name, storage, cost, bandwidth, type_id,
                                                  number_of_intervals);

        // Adding requirement to the bucket
        for (size_t l = 5ul; l < 5ul + number_of_requirements; ++l) {
            auto requirement_value = stod(strs[l]);
            my_bucket->AddRequirement(requirement_value);
        }

        storages_.push_back(my_bucket);
        storage_id += 1;
        DLOG(INFO) << my_bucket;
        ++bucket_size_;
    }
    in_cluster.close();
}

void Algorithm::ReadConflictGraph(const std::string &conflict_graph,
                                  std::unordered_map<std::string, std::shared_ptr<File>> &file_map_per_name) {
    DLOG(INFO) << "Reading Conflict Graph from input file [" + conflict_graph + "]" ;
    std::string line;
    std::vector<std::string> tokens;

    std::ifstream in_conflict_graph(conflict_graph);

    if (!in_conflict_graph.is_open()) {
        LOG(FATAL) << "Conflict graph [" + conflict_graph + "] doesn't exist";
    }

    conflict_graph_->Redefine(GetFilesSize());

    // Reading conflict graph information
    while (getline(in_conflict_graph, line)) {
        DLOG(INFO) << "Conflict: " << line;

        std::vector<std::string> strs;
        boost::split(strs, line, boost::is_any_of(" "));
        auto first_file = strs[0];
        auto second_file = strs[1];
        auto conflict_value = stod(strs[2]);
        auto first_file_id = file_map_per_name.find(first_file)->second->get_id();
        auto second_file_id = file_map_per_name.find(second_file)->second->get_id();
        conflict_graph_->AddConflict(first_file_id, second_file_id, static_cast<int>(conflict_value));
    }

    DLOG(INFO) << "Finished reading Conflict Graph" ;

    in_conflict_graph.close();
}

/**
 * The the three input files.
 *
 * \param[in] tasks_and_files_file  Name of the Activation and Files input file
 * \param[in] cluster_file          Name of the Cluster input file
 * \param[in] conflict_graph_file   Name of the Conflict Graph input file
 */
void Algorithm::ReadInputFiles(const std::string &tasks_and_files_file,
                               const std::string &cluster_file,
                               const std::string &conflict_graph_file) {
    std::unordered_map<std::string, std::shared_ptr<File>> file_map_per_name;

    ReadTasksAndFiles(tasks_and_files_file, file_map_per_name);
    ReadCluster(cluster_file);
    ReadConflictGraph(conflict_graph_file, file_map_per_name);
    storage_vet_.resize(storages_.size(), 0.0);

    for (const std::shared_ptr<Storage> &storage: storages_) {
        // Storage* storage = storage_pair.second;
        storage_vet_[storage->get_id()] = storage->get_storage();
    }

    height_.resize(GetActivationSize(), -1);
    ComputeHeight(id_source_, 0);

    for (size_t i = 0; i < height_.size(); ++i) {
        DLOG(INFO) << "Height[" << i << "]: " << height_[i];
    }

    ComputeFileTransferMatrix();
}

/**
 * The \c ReturnAlgorithm() returns an object derived from \c Algorithm depending on the
 * \c algorithm parameter.
 *
 * \param[in] algorithm  Name of the \c Algorithm object to be instantiated
 * \retval    object     Instance of an \c Algorithm object
 */
std::shared_ptr<Algorithm> Algorithm::ReturnAlgorithm(const std::string &algorithm) {
    DLOG(INFO) << "ReturnAlgorithm: " << algorithm;
    std::cout << "ReturnAlgorithm: " << algorithm << std::endl;

    if (algorithm == "grch") {
        return std::make_shared<Grch>();
    } else if (algorithm == "cplex") {
        return std::make_shared<Cplex>();
    } else if (algorithm == "grasp") {
        return std::make_shared<Grasp>();
    } else if (algorithm == "heft") {
        return std::make_shared<Heft>();
    } else {
        std::fprintf(stderr, "Please select a valid algorithm.\n");
        std::exit(-1);
    }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
void Algorithm::ComputeHeight(size_t node, int n) {
    DLOG(INFO) << "Height " << n << " node " << node << " name " << activations_[node]->get_tag() << std::endl;

    if (height_[node] < n) {
        height_[node] = n;

        auto vet = successors_[node];
        for (auto j: vet) {
            ComputeHeight(j, n + 1);
        }
    }
}

void Algorithm::ComputeFileTransferMatrix() {
    for (const auto& file : files_) {
        file->PopulateFileTransferMatrix(storages_);
    }
}

void Algorithm::CalculateMaximumSecurityAndPrivacyExposure() {
    double maximum_task_exposure = 0.0;

    DLOG(INFO) << "Calculate the Maximum Security and Privacy Exposure";

    for (Requirement requirement: requirements_) {
        maximum_task_exposure += static_cast<double>(GetActivationSize())
                                 * static_cast<double>(requirement.get_max_value());
    }

    DLOG(INFO) << "task_exposure: " << maximum_task_exposure;

    auto maximum_privacy_exposure =
            static_cast<double>(conflict_graph_->get_maximum_of_soft_constraints());

    maximum_security_and_privacy_exposure_ = maximum_task_exposure + maximum_privacy_exposure;

    DLOG(INFO) << "maximum_security_and_privacy_exposure_: " << maximum_security_and_privacy_exposure_;
}
