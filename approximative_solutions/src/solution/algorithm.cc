/**
 * \file src/exec_manager/execution.cc
 * \brief Contains the \c Algorithm class methods.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \date 2020
 *
 * This source file contains the methods from the \c Algorithm class that run the mode the
 * approximate solution.
 */

#include <boost/algorithm/string.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/bimap.hpp>
#include <iostream>
#include <fstream>
#include <glog/logging.h>
#include <gflags/gflags.h>
#include <unordered_map>
#include <vector>
#include <utility>
#include <sstream>

#include "src/model/file.h"
#include "src/model/static_file.h"
#include "src/model/dynamic_file.h"
#include "src/model/requirement.h"
#include "src/model/task.h"
#include "src/model/bucket.h"
#include "src/model/virtual_machine.h"
#include "src/solution/algorithm.h"
#include "src/solution/greedy_algorithm.h"

// DECLARE_string(tasks_and_files);
// DECLARE_string(cluster);
// DECLARE_string(conflict_graph);
// DECLARE_string(algorithm);

void Algorithm::ReadTasksAndFiles(std::string tasks_and_files) {
	double total_file = 0.0;

  DLOG(INFO) << "total_file: " << total_file;

  // Reading file
  std::string line;
  std::ifstream in_file(tasks_and_files);

  // Get number of tasks and number of files
  getline(in_file, line);
  DLOG(INFO) << "Head: " << line;
  std::vector<std::string> tokens;
  boost::split(tokens, line, boost::is_any_of(" "));

  static_file_size_ = stoi(tokens[0]);
  dynamic_file_size_ = stoi(tokens[1]);
  task_size_ = stoi(tokens[2]) + 2;  // Adding two tasks (source and target)
  requirement_size_ = stoi(tokens[3]);
  deadline_ = stod(tokens[4]);
  budget_ = stod(tokens[5]);
  file_size_ = static_file_size_ + dynamic_file_size_;
  size_ = task_size_ + dynamic_file_size_;

  DLOG(INFO) << "_static_file_size: " << static_file_size_;
  DLOG(INFO) << "_dynamic_file_size: " << dynamic_file_size_;
  DLOG(INFO) << "_task_size: " << task_size_;
  DLOG(INFO) << "_requirement_size: " << requirement_size_;
  DLOG(INFO) << "_deadline: " << deadline_;
  DLOG(INFO) << "_budget: " << budget_;
  DLOG(INFO) << "_file_size: " << file_size_;
  DLOG(INFO) << "_size: " << size_;

  getline(in_file, line); // Reading blank line

  // start initial integer_id of elements
  int id_task = 1;  // [1..._task_size)
  int id_dynamic_file = task_size_;  // [_task_size..._task_size + _dynamic_file_size)
  int id_static_file = task_size_ + dynamic_file_size_;  // _task_size + _dynamic_file_size)...

  // Reading information about requirements
  for (int i = 0; i < requirement_size_; i++) {
    getline(in_file, line);
    DLOG(INFO) << "Requirement: " << line;
    google::FlushLogFiles(google::INFO);

    std::vector<std::string> strs;
    boost::split(strs, line, boost::is_any_of(" "));

    int id = stoi(strs[0]);
    int max_value = stoi(strs[1]);
    Requirement my_requirement = Requirement(id, max_value);
    requirements_.push_back(my_requirement);
    DLOG(INFO) << my_requirement;
  }
  getline(in_file, line); // reading blank line

  // Reading information about static files
  for (int i = 0; i < static_file_size_; i++) {
    getline(in_file, line);
    DLOG(INFO) << "File: " << line;
    google::FlushLogFiles(google::INFO);

    std::vector<std::string> strs;
    boost::split(strs, line, boost::is_any_of(" "));
    auto file_name = strs[0];
    auto file_size = stod(strs[1]);

    DLOG(INFO) << "file_name: " << file_name;
    DLOG(INFO) << "file_size: " << file_size;

    int id = id_static_file;
    id_static_file += 1;
    // bool is_static = false;
    // auto place = -1;

    StaticFile my_staticFile(id, file_name, file_size);

    for (int j = 0; j < stoi(strs[2]); ++j) {
      my_staticFile.addVm(stoi(strs[3 + static_cast<unsigned long int>(j)]));
    }

    // auto my_file = File(id, file_name, file_size, is_static, place);
    total_file += file_size;
    DLOG(INFO) << my_staticFile;
    // key_map.insert(make_pair(file_name, id));
    // file_map.insert(make_pair(id, afile));
    file_map_.insert(std::make_pair(file_name, my_staticFile));
  }

  // Reading information about dynamic files
  for (int i = static_file_size_; i < file_size_; i++) {
    getline(in_file, line);
    DLOG(INFO) << "File: " << line;
    google::FlushLogFiles(google::INFO);

    std::vector<std::string> strs;
    boost::split(strs, line, boost::is_any_of(" "));
    const std::string file_name = strs[0];
    auto file_size = stod(strs[1]);

    DLOG(INFO) << "file_name: " << file_name;
    DLOG(INFO) << "file_size: " << file_size;

    int id = id_dynamic_file;
    id_dynamic_file += 1;
    // bool is_static = false;
    // auto place = -1;

    DynamicFile my_dynamicFile(id, file_name, file_size);

    // auto afile = File(file_name, id, file_size, is_static, place);
    total_file += file_size;
    DLOG(INFO) << my_dynamicFile;
    // key_map.insert(make_pair(file_name, id));
    // file_map.insert(make_pair(id, afile));
    file_map_.insert(std::make_pair(file_name, my_dynamicFile));
  }
  getline(in_file, line); //reading blank line

  //read tasks
  //cout << "Reading tasks" << endl;
  // Reading information about tasks
  for (int i = 0; i < task_size_ - 2; i++) {
    getline(in_file, line);
    DLOG(INFO) << "Task: " << line;
    google::FlushLogFiles(google::INFO);
    std::vector<std::string> strs;
    boost::split(strs, line, boost::is_any_of(" "));
    // get task info
    auto tag = strs[0];
    std::string task_name = strs[1];
    auto base_time = stod(strs[2]);
    auto in_size = stoi(strs[3]);
    auto out_size = stoi(strs[4]);
    // int current_task = id_task;
    id_task += 1;

    DLOG(INFO) << "tag: " << tag;
    DLOG(INFO) << "task_name: " << task_name;
    DLOG(INFO) << "base_time: " << base_time;
    DLOG(INFO) << "in_size: " << in_size;
    DLOG(INFO) << "out_size: " << out_size;

    Task my_taskFile(id_task, tag, task_name, base_time);

    // cout << taskId << " " << taskName << " " << taskTime << " " << inSize << " " << outSize << endl;
    // Add task in bi-map idToFile
    // std::vector<int> input;
    // std::vector<int> output;

    // reading input files
    for (int j = 0; j < in_size; j++){
      getline(in_file, line);
      DLOG(INFO) << "Input file: " << line;
      google::FlushLogFiles(google::INFO);

      // auto fileKey = key_map.find(line)->second;
      // input.push_back(fileKey);

      const File &my_file = file_map_.find(line)->second;
      my_taskFile.AddInputFile(my_file);
    }

    // reading output files
    for (int j = 0; j < out_size; j++){
      getline(in_file, line);
      DLOG(INFO) << "Output file: " << line;
      google::FlushLogFiles(google::INFO);

      // auto fileKey = key_map.find(line)->second;
      // update file
      // output.push_back(fileKey);

      const File &my_file = file_map_.find(line)->second;
      my_taskFile.AddOutputFile(my_file);
    }

    // Task atask(task_name, current_task, tag, base_time, input, output);

    // key_map.insert(make_pair(tag, current_task));
    // task_map.insert(make_pair(current_task, atask));

    task_map_.insert(make_pair(tag, my_taskFile));
    DLOG(INFO) << my_taskFile;
  }
  getline(in_file, line); // reading blank line

  // Update Source and Target tasks
  id_source_ = 0;
  id_target_ = id_task;
  id_task += 1;

  // key_map.insert(make_pair("root", id_source));
  // key_map.insert(make_pair("sink", id_target));

  Task sourceTask(id_source_, "source", "SOURCE", 0.0);
  Task targetTask(id_target_, "target", "TARGET", 0.0);

  task_map_.insert(std::make_pair("source", sourceTask));
  task_map_.insert(std::make_pair("target", targetTask));

  auto f_source = succ_.insert(std::make_pair(id_source_, std::vector<int>()));
  succ_.insert(std::make_pair(id_target_, std::vector<int>()));

  std::vector<int> aux(static_cast<unsigned long int>(task_size_), -1);
  aux[static_cast<unsigned long int>(id_source_)] = 0;
  aux[static_cast<unsigned long int>(id_target_)] = 0;

  // Reading successors graph informations
  for (int i = 0; i < task_size_ - 2; i++) {
    getline(in_file, line);  // Reading parent task
    DLOG(INFO) << "Parent: " << line;
    google::FlushLogFiles(google::INFO);

    std::vector<std::string> strs;
    boost::split(strs, line, boost::is_any_of(" "));
    auto task_tag = strs[0];
    auto number_of_successors = stoi(strs[1]);

    // auto task_id = key_map.find(task_tag)->second;
    auto task = task_map_.find(task_tag)->second;

    std::vector<int> children;
    // Reading children task
    for (int j = 0; j < number_of_successors; j++){
      getline(in_file, line);
      DLOG(INFO) << "Child: " << line;
      google::FlushLogFiles(google::INFO);

      // auto child_id = key_map.find(line)->second;
      auto child_task = task_map_.find(line)->second;
      children.push_back(child_task.get_id());
      aux[static_cast<unsigned long int>(child_task.get_id())] = 0;
    }

    // Target task
    if (number_of_successors == 0) {
      children.push_back(id_target_);
    }

    succ_.insert(make_pair(task.get_id(), children));
  }

  // Add synthetic source task
  for (int i = 0; i < task_size_; i++) {
    // Add source
    if (aux[static_cast<unsigned long int>(i)] == -1) {
      f_source.first->second.push_back(i);
    }
  }
  prec_ = reverse_map(succ_);
  in_file.close();
}

void Algorithm::ReadCluster(std::string cluster) {
  double total_storage = 0.0;
  DLOG(INFO) << "total_storage: " << total_storage;

  // Reading file
  std::string line;
  std::ifstream in_cluster(cluster);
  std::vector<std::string> tokens;

  getline(in_cluster, line);

  DLOG(INFO) << "Head: " << line;
  google::FlushLogFiles(google::INFO);

  boost::split(tokens, line, boost::is_any_of(" "));

  number_of_providers_ = stoi(tokens[0]);
  number_of_requirements_ = stoi(tokens[1]);

  getline(in_cluster, line);  // ignore line

  int provider_id = 0;

  for (int i = 0; i < number_of_providers_; ++i) {
    getline(in_cluster, line);
    DLOG(INFO) << "Provider: " << line;
    google::FlushLogFiles(google::INFO);

    std::vector<std::string> strs1;
    boost::split(strs1, line, boost::is_any_of(" "));

    period_hr_ = stod(strs1[2]);
    vm_size_ = stoi(strs1[4]);
    bucket_size_ = stoi(strs1[5]);
    storage_vet_.resize(static_cast<unsigned long int>(vm_size_), 0);

    Provider my_provider(provider_id++);

    int vm_id = 0;
    int bucket_id = 0;

    // Reading VMs information
    for (auto j = 0; j < vm_size_; j++) {
      getline(in_cluster, line);
      DLOG(INFO) << "VM: " << line;
      google::FlushLogFiles(google::INFO);

      std::vector<std::string> strs;
      boost::split(strs, line, boost::is_any_of(" "));

      int type_id = stoi(strs[0]);
      std::string vm_name = strs[1];
      double slowdown = stod(strs[2]);
      double storage = stod(strs[3]) * 1024; // GB to MB
      double bandwidth = stod(strs[4]);
      double cost = stod(strs[5]);

      VirtualMachine my_vm(vm_id, vm_name, slowdown, storage, cost, bandwidth, type_id);
      // _vm_map.insert(std::make_pair(vm_id, my_vm));
      my_provider.AddVirtualMachine(my_vm);
      storage_vet_[static_cast<unsigned long int>(vm_id)] = storage;
      vm_id += 1;
      total_storage += storage;
    }

    // Reading Buckets informations
    for (auto j = 0; j < bucket_size_; j++) {
      getline(in_cluster, line);
      DLOG(INFO) << "Bucket: " << line;
      google::FlushLogFiles(google::INFO);

      std::vector<std::string> strs;
      boost::split(strs, line, boost::is_any_of(" "));

      // <id-bucket> <capacidade> <numero de intervalos> [<limite superior> <valor contratado por intervalo>] <requirement-1> <requirement-2 (pode assumir 0 1 2{so bucket})>

      int bucketId = stoi(strs[0]);
      std::string bucketCapacity = strs[1];
      double bucketNumberOfIntervals = stod(strs[2]);
      // double storage = stod(strs[3]) * 1024; // GB to MB
      // double bandwidth = stod(strs[4]);
      // double cost = stod(strs[5]);

      Bucket my_bucket(bucketId, bucketCapacity, bucketNumberOfIntervals);
      my_provider.AddBucket(my_bucket);
      // _vm_map.insert(std::make_pair(vm_id, my_vm));
      // _storage_vet[static_cast<unsigned long int>(vm_id)] = storage;
      bucket_id += 1;
      // total_storage += storage;
    }
    providers_.push_back(my_provider);
  }
  in_cluster.close();
}

void Algorithm::ReadConflictGraph(std::string conflict_graph) {
  std::string line;
  std::vector<std::string> tokens;

  std::ifstream in_conflict_graph(conflict_graph);
  conflict_graph_.redefine(size_, size_);

  // Reading conflict graph informations
  while (getline(in_conflict_graph, line)) {
    DLOG(INFO) << "Conflict: " << line;
    google::FlushLogFiles(google::INFO);

    std::vector<std::string> strs;
    boost::split(strs, line, boost::is_any_of(" "));
    auto firstFile = strs[0];
    auto secondFile = strs[1];
    auto conflictValue = stoi(strs[2]);
    auto firstId = file_map_.find(firstFile)->second.get_id();
    auto secondId = file_map_.find(secondFile)->second.get_id();
    DLOG(INFO) << "firstFile: " << firstFile;
    DLOG(INFO) << "secondFile: " << secondFile;
    DLOG(INFO) << "conflictValue: " << conflictValue;
    DLOG(INFO) << "firstId: " << firstId;
    DLOG(INFO) << "secondId: " << secondId;
    google::FlushLogFiles(google::INFO);
    conflict_graph_(firstId, secondId) = conflictValue;
  }

  DLOG(INFO) << "Conflict Graph: " << conflict_graph_ << line;

  in_conflict_graph.close();
}

/**
 * The \c RunSetup() method calls \c DigitalRock methods involved in loading the greyscale cube and
 * calculating the greyscale histogram for later segmentation.
 *
 * \param[in] json_file_name  Name of JSON file containing setup-related parameters.
 */
void Algorithm::readInputFiles(const std::string tasks_and_files,
                               const std::string cluster,
                               const std::string conflict_graph) {
  ReadTasksAndFiles(tasks_and_files);
  ReadCluster(cluster);
  ReadConflictGraph(conflict_graph);


  // Check if storage is enough
  // for (auto it : _file_map){
  //   // if (it.second.is_static){
  //   if (it.second.isStatic()) {
  //     // _storage_vet[it.second.getFirstVm()] -= it.second.size;
  //     StaticFile &sf = static_cast<StaticFile&>(it.second);
  //     _storage_vet[static_cast<unsigned long int>(sf.getFirstVm())]
  //       -= it.second.getSize();
  //     if (_storage_vet[static_cast<unsigned long int>(sf.getFirstVm())] < 0) {
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
 * The \c getAlgorithm() returns an object derived from \c Algorithm depending on the
 * \c name parameter.
 *
 * \param[in] name      Name of the \c Algorithm object to be instantiated.
 * \retval    algorithm Instance of an \c Algorithm object.
 */
std::unique_ptr<Algorithm> Algorithm::getAlgorithm(const std::string algorithm) {
  if (algorithm == "greedy") {
    return std::make_unique<GreedyAlgorithm>();
  // } else if (name == "meta") {
  //   return std::make_unique<GRASPAlgorithm>();
  } else {
    std::fprintf(stderr, "Please select a valid algorithm.\n");
    std::exit(-1);
  }
}  // end of Algorithm::GetAlgorithm() method

std::unordered_map<int, std::vector<int>> Algorithm::reverse_map(std::unordered_map<int,
                                                            std::vector<int>> amap) {
  std::unordered_map<int, std::vector<int>> r_map;
  for(auto key : amap){
    for(auto val : amap.find(key.first)->second){
      auto f = r_map.insert(std::make_pair(val, std::vector<int>(0)));
      f.first->second.push_back(key.first);
    }
  }
  return r_map;
}
