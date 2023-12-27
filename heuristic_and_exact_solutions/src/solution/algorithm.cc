/**
 * \file src/solution/algorithm.cc
 * \brief Contains the \c Algorithm class methods.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2020
 *
 * This source file contains the methods from the \c Algorithm class that run the mode the
 * approximate solution.
 */

#include "src/solution/algorithm.h"

#include <boost/algorithm/string.hpp>
#include <filesystem>
#include "src/model/static_file.h"
#include "src/model/dynamic_file.h"
#include "src/solution/grch.h"
#include "src/solution/min_min.h"
#include "src/solution/grasp.h"
#include "src/solution/cplex.h"
#include "heft.h"
#include "grch_hard.h"

//void Algorithm::ReadTasksAndFiles(const std::string &tasks_and_files,
//                                  std::unordered_map<std::string, File *> &file_map_per_name) {
void Algorithm::ReadTasksAndFiles(const std::string &tasks_and_files,
                                  std::unordered_map<std::string, std::shared_ptr<File>> &file_map_per_name) {
    DLOG(INFO) << "Reading Activations and Files from input file [" + tasks_and_files + "]" ;

    if (!std::filesystem::exists(tasks_and_files)) {
        DLOG(FATAL) << "Activations and Tasks input file could not be found in \"" << tasks_and_files << "\"!";
    }

//  double total_file = 0.0;
    size_t task_size;
    size_t file_size;
    size_t requirement_size;
//  std::unordered_map<std::string, Activation*> task_map_per_name_;
    std::unordered_map<std::string, std::shared_ptr<Activation>> task_map_per_name_;

    // DLOG(INFO) << "total_file: " << total_file;

    // Reading file
    std::string line;
    std::ifstream in_file(tasks_and_files);

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
    // tasks_plus_files_size_ = task_size_ + file_size_;  // Tasks + Files

    DLOG(INFO) << "static_file_size_: " << static_file_size_;
    DLOG(INFO) << "dynamic_file_size_: " << dynamic_file_size_;
    DLOG(INFO) << "task_size_: " << task_size;
    DLOG(INFO) << "requirement_size_: " << requirement_size;
    DLOG(INFO) << "makespan_max_: " << makespan_max_;
    DLOG(INFO) << "budget_max_: " << budget_max_;
    DLOG(INFO) << "file_size_: " << file_size;
    // DLOG(INFO) << "tasks_plus_files_size_: " << tasks_plus_files_size_;

    getline(in_file, line);  // Reading blank line

    // Start initial integer_id of elements
    // Tasks, dynamic files, and static files do share the same range of IDs
    // size_t initial_dynamic_file_id = task_size_;
    // size_t initial_static_file_id = task_size_ + dynamic_file_size_;

    // Reading information about requirements
    for (size_t i = 0; i < requirement_size; i++) {
        getline(in_file, line);
        DLOG(INFO) << "Requirement: " << line;
//    google::FlushLogFiles(google::INFO);

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

//    StaticFile* my_staticFile = new StaticFile(i, my_file_name, my_file_size);
//        std::shared_ptr<File> my_staticFile = std::make_shared<StaticFile>(i, my_file_name, my_file_size);
        std::shared_ptr<File> my_staticFile = std::make_shared<StaticFile>(i, my_file_name, my_file_size);
//        std::shared_ptr<StaticFile> my_staticFile(new StaticFile(i, my_file_name, my_file_size));

        for (size_t j = 0; j < stoul(strs[2]); ++j) {
            auto msf = std::dynamic_pointer_cast<StaticFile>(my_staticFile);
//            my_staticFile->AddVm(stoul(strs[3 + j]));
            msf->AddVm(stoul(strs[3 + j]));
        }

//    total_file += file_size;

        DLOG(INFO) << *my_staticFile;

        files_.push_back(my_staticFile);
//        file_map_per_name.insert(std::make_pair(my_file_name, my_staticFile.get()));
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

        // DynamicFile my_dynamicFile(i, file_name, file_size);
//    DynamicFile* my_dynamicFile = new DynamicFile(i, my_file_name, my_file_size);
//        std::shared_ptr<DynamicFile> my_dynamicFile = std::make_shared<DynamicFile>(i, my_file_name, my_file_size);
//        std::shared_ptr<File> my_dynamicFile = std::make_shared<DynamicFile>(i, my_file_name, my_file_size);
        std::shared_ptr<File> my_dynamicFile = std::make_shared<DynamicFile>(i, my_file_name, my_file_size);
//        std::shared_ptr<DynamicFile> my_dynamicFile(new DynamicFile(i, my_file_name, my_file_size));

//    total_file += file_size;

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

//  Activation* source_task = new Activation(id_source_, "source", "SOURCE", 0.0);
//  Activation* target_task = new Activation(id_target_, "target", "TARGET", 0.0);

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
//    google::FlushLogFiles(google::INFO);
        std::vector<std::string> strs;
        boost::split(strs, line, boost::is_any_of(" "));
        // get task info
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

        // reading input files
        for (size_t j = 0; j < in_size; j++) {
            getline(in_file, line);
            DLOG(INFO) << "Input file: " << line;

            std::shared_ptr<File> my_file(file_map_per_name.find(line)->second);

            my_task->AddInputFile(my_file);
        }

        // reading output files
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

//  DLOG(INFO) << "task_map_per_id_: ";
//  for (auto task : task_map_per_name_) {
//    DLOG(INFO) << "ID: " << task.first << ", " << task.second;
//  }

//  DLOG(INFO) << "task_map_per_name_: ";
//  for (auto task : task_map_per_name_) {
//    DLOG(INFO) << "Activation: " << task.second;
//  }

    getline(in_file, line);  // reading blank line

    // Update Source and Target tasks
    activations_.push_back(target_task);

    task_map_per_name_.insert(std::make_pair("source", source_task));
    task_map_per_name_.insert(std::make_pair("target", target_task));

    // auto f_source = successors_.insert(std::make_pair(id_source_, std::vector<size_t>()));
    successors_.resize(task_size, std::vector<size_t>());
    // successors_.insert(std::make_pair(id_target_, std::vector<size_t>()));

    std::vector<int> aux(task_size, -1);
    aux[id_source_] = 0;
    aux[id_target_] = 0;

    // Reading successors graph information
    for (size_t i = 0; i < task_size - 2; i++) {
        getline(in_file, line);  // Reading parent task

        DLOG(INFO) << "Parent: " << line;
//    google::FlushLogFiles(google::INFO);

        std::vector<std::string> strs;
        boost::split(strs, line, boost::is_any_of(" "));
        auto task_tag = strs[0];
        auto number_of_successors = stoi(strs[1]);

        std::vector<size_t> children;
        // Reading children task
        for (int j = 0; j < number_of_successors; j++) {
            getline(in_file, line);

            DLOG(INFO) << "Child: " << line;
//      google::FlushLogFiles(google::INFO);

            // auto child_id = key_map.find(line)->second;
            auto child_task = task_map_per_name_.find(line)->second;

            children.push_back(child_task->get_id());
            aux[child_task->get_id()] = 0;
        }

        // Target task
        if (number_of_successors == 0) {
            children.push_back(id_target_);
        }

        // auto task_id = key_map.find(task_tag)->second;
        auto task = task_map_per_name_.find(task_tag)->second;

        // successors_.insert(make_pair(task->get_id(), children));
        successors_[task->get_id()] = children;
    }

    // successors_.push_back(std::vector<size_t>());

    // Add synthetic source task
    for (size_t i = 0; i < task_size; i++) {
        // Add source
        if (aux[i] == -1) {
            // f_source.first->second.push_back(i);
            successors_[0].push_back(i);
        }
    }

    // ReverseMap
    // predecessors_ = ReverseMap(successors_);
    predecessors_.resize(task_size, std::vector<size_t>());

    for (size_t i = 0ul; i < successors_.size(); ++i) {
        for (auto val: successors_[i]) {
            // auto f = r_map.insert(std::make_pair(val, std::vector<size_t>(0)));
            // f.first->second.push_back(key.first);
            predecessors_[val].push_back(i);
        }
    }

    in_file.close();
}

void Algorithm::ReadCluster(const std::string &cluster) {
    DLOG(INFO) << "Reading Clusters from input file [" + cluster + "]" ;

    if (!std::filesystem::exists(cluster)) {
        DLOG(FATAL) << "Cluster file could not be found \"" << cluster << "\"!";
    }

//  double total_storage = 0.0;
    size_t vm_size;

    // DLOG(INFO) << "total_storage: " << total_storage;

    // Reading file
    std::string line;
    std::ifstream in_cluster(cluster);
    std::vector<std::string> tokens;

    getline(in_cluster, line);

    DLOG(INFO) << "Head: " << line;
//  google::FlushLogFiles(google::INFO);

    boost::split(tokens, line, boost::is_any_of(" "));

    // size_t number_of_providers = stoul(tokens[0]);
    size_t number_of_requirements = stoul(tokens[1]);

    getline(in_cluster, line);  // ignore line

    // size_t provider_id = 0ul;
    size_t storage_id = 0ul;
    // size_t bucket_id = 0ul;

    // for (size_t i = 0; i < number_of_providers_; ++i) {
    getline(in_cluster, line);
    DLOG(INFO) << "Provider: " << line;
//    google::FlushLogFiles(google::INFO);

    std::vector<std::string> strs1;
    boost::split(strs1, line, boost::is_any_of(" "));

//    period_hr_ = stod(strs1[2]);
    vm_size = stoul(strs1[4]);
    size_t bucket_size = stoul(strs1[5]);

    // Reading VMs information
    for (auto j = 0ul; j < vm_size; j++) {
        getline(in_cluster, line);
        DLOG(INFO) << "VM: " << line;
//      google::FlushLogFiles(google::INFO);

        std::vector<std::string> strs;
        boost::split(strs, line, boost::is_any_of(" "));

        int type_id = stoi(strs[0]);
        std::string vm_name = strs[1];
        double slowdown = stod(strs[2]);
        double storage = stod(strs[3]) * 1024;  // GB to MB
        double bandwidth = stod(strs[4]);
        double cost = stod(strs[5]);

//      VirtualMachine* my_vm = new VirtualMachine(storage_id,
        std::shared_ptr<VirtualMachine> my_vm = std::make_shared<VirtualMachine>(storage_id, vm_name, slowdown, storage,
                                                                                 cost, bandwidth, type_id);

        // Adding requirement to the task
        for (size_t l = 6ul; l < 6ul + number_of_requirements; ++l) {
            auto requirement_value = stod(strs[l]);
            my_vm->AddRequirement(requirement_value);
        }

        virtual_machines_.push_back(my_vm);
        storages_.push_back(my_vm);
        storage_id += 1;
//      total_storage += storage;
        DLOG(INFO) << my_vm;
    }

    // Reading Buckets information
    for (auto j = 0ul; j < bucket_size; j++) {
        getline(in_cluster, line);
        DLOG(INFO) << "Bucket: " << line;
//      google::FlushLogFiles(google::INFO);

        std::vector<std::string> strs;
        boost::split(strs, line, boost::is_any_of(" "));

        int type_id = stoi(strs[0]);
        std::string name = strs[1];
        double storage = stod(strs[2]) * 1024;  // GB to MB
        double bandwidth = stod(strs[3]);
        size_t number_of_intervals = 1ul;
        double cost = stod(strs[4]);

//      Bucket* my_bucket = new Bucket(storage_id,
        std::shared_ptr<Bucket>
                my_bucket = std::make_shared<Bucket>(storage_id, name, storage, cost, bandwidth, type_id,
                                                     number_of_intervals);

        // Adding requirement to the bucket
        for (size_t l = 5ul; l < 5ul + number_of_requirements; ++l) {
            auto requirement_value = stod(strs[l]);
            my_bucket->AddRequirement(requirement_value);
        }

        storages_.push_back(my_bucket);
        storage_id += 1;
//      total_storage += storage;
        DLOG(INFO) << my_bucket;
        ++bucket_size_;
        // google::FlushLogFiles(google::INFO);
    }
    // providers_.push_back(my_provider);
    // }
    in_cluster.close();
}  // void Algorithm::ReadCluster(std::string cluster) {

//void Algorithm::ReadConflictGraph(const std::string &conflict_graph,
//                                  std::unordered_map<std::string, File*> &file_map_per_name) {
void Algorithm::ReadConflictGraph(const std::string &conflict_graph,
                                  std::unordered_map<std::string, std::shared_ptr<File>> &file_map_per_name) {
    DLOG(INFO) << "Reading Conflict Graph from input file [" + conflict_graph + "]" ;
    std::string line;
    std::vector<std::string> tokens;

    std::ifstream in_conflict_graph(conflict_graph);

    if (!in_conflict_graph.is_open()) {
        LOG(FATAL) << "Conflict graph [" + conflict_graph + "] doesn't exist";
    }

    // conflict_graph_.redefine(size_, size_, 0.0);

    // conflict_graph_ = ConflictGraph(size_);
    conflict_graph_.Redefine(static_cast<int>(GetFilesSize()));

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
        // DLOG(INFO) << "firstFile: " << firstFile;
        // DLOG(INFO) << "secondFile: " << secondFile;
        // DLOG(INFO) << "conflictValue: " << conflictValue;
        // DLOG(INFO) << "firstId: " << firstId;
        // DLOG(INFO) << "secondId: " << secondId;
        // google::FlushLogFiles(google::INFO);
        // conflict_graph_(firstId, secondId) = conflictValue;
        // conflict_graph_(secondId, firstId) = conflictValue;
        conflict_graph_.AddConflict(first_file_id, second_file_id, static_cast<int>(conflict_value));
    }

//    DLOG(INFO) << "Conflict Graph: " << conflict_graph_;
    DLOG(INFO) << "Finished reading Conflict Graph" ;

    in_conflict_graph.close();
}

/**
 * The the three input files.
 *
 * \param[in] tasks_and_files  Name of the Activation and Files input file
 * \param[in] cluster          Name of the Cluster input file
 * \param[in] conflict_graph   Name of the Conflict Graph input file
 */
void Algorithm::ReadInputFiles(const std::string &tasks_and_files,
                               const std::string &cluster,
                               const std::string &conflict_graph) {
//  std::unordered_map<std::string, File*> file_map_per_name;
    std::unordered_map<std::string, std::shared_ptr<File>> file_map_per_name;

    ReadTasksAndFiles(tasks_and_files, file_map_per_name);
    ReadCluster(cluster);
    ReadConflictGraph(conflict_graph, file_map_per_name);
    storage_vet_.resize(storages_.size(), 0.0);
//  for (Storage* storage : storages_) {
    for (const std::shared_ptr<Storage> &storage: storages_) {
        // Storage* storage = storage_pair.second;
        storage_vet_[storage->get_id()] = storage->get_storage();
    }
    height_.resize(GetActivationSize(), -1);
    ComputeHeight(id_source_, 0);
    for (size_t i = 0; i < height_.size(); ++i) {
        DLOG(INFO) << "Height[" << i << "]: " << height_[i];
    }
    // google::FlushLogFiles(google::INFO);
    // Check if storage is enough
    // for (auto it : _file_map){
    //   // if (it.second.is_static){
    //   if (it.second.isStatic()) {
    //     // _storage_vet[it.second.getFirstVm()] -= it.second.size;
    //     StaticFile &sf = static_cast<StaticFile&>(it.second);
    //     _storage_vet[sf.getFirstVm()]
    //       -= it.second.getSize();
    //     if (_storage_vet[sf.getFirstVm()] < 0) {
    //       std::cerr << "Static file is bigger than the vm capacity" << std::endl;
    //       throw;
    //     }
    //   }
    // }

    // if (total_storage < total_file) {
    //   std::cerr << "Storage is not enough" << std::endl;
    //   throw;
    // }
}  // end of Algorithm::ReadInputFiles method

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
    } else if (algorithm == "grch_hard") {
        return std::make_shared<GrchHard>();
    } else if (algorithm == "min_min") {
        return std::make_shared<MinMin>();
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
}  // end of Algorithm::GetAlgorithm() method

//std::unordered_map<size_t, std::vector<size_t>> Algorithm::ReverseMap(
//    std::unordered_map<size_t, std::vector<size_t>> amap) {
//
//  std::unordered_map<size_t, std::vector<size_t>> r_map;
//
//  for (auto key : amap) {
//    for (auto val : amap.find(key.first)->second) {
//      auto f = r_map.insert(std::make_pair(val, std::vector<size_t>(0)));
//      f.first->second.push_back(key.first);
//    }
//  }
//
//  return r_map;
//}

void Algorithm::ComputeHeight(size_t node, int n) {
    // DLOG(INFO) << "node[" << node << "], n[" << n << "]";
    // google::FlushLogFiles(google::INFO);
    if (height_[node] < n) {
        height_[node] = n;
        // auto vet = successors_.find(node)->second;
        auto vet = successors_[node];
        for (auto j: vet) {
            // DLOG(INFO) << "vet[" << j << "]";
            // google::FlushLogFiles(google::INFO);

            ComputeHeight(j, n + 1);
        }
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
            static_cast<double>(conflict_graph_.get_maximum_of_soft_constraints());

    maximum_security_and_privacy_exposure_ = maximum_task_exposure + maximum_privacy_exposure;

    DLOG(INFO) << "maximum_security_and_privacy_exposure_: " << maximum_security_and_privacy_exposure_;
}  // void Algorithm::CalculateSecurityExposure() {ss
