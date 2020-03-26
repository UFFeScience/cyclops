/**
 * \file src/solution/algorithm.h
 * \brief Contains the \c Algorithm class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \date 2020
 *
 * This header file contains the \c Algorithm class that handles different execution modes.
 */

#ifndef SRC_SOLUTION_ALGORITHM_H_
#define SRC_SOLUTION_ALGORITHM_H_

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

#include "src/model/file.h"
#include "src/model/requirement.h"
#include "src/model/task.h"
#include "src/model/virtual_machine.h"
#include "src/model/bucket.h"
#include "src/model/provider.h"
#include "src/model/solution.h"

#include "src/data_structure/matrix.h"

class Solution;

/**
 * \class Execution execution.h "src/solution/algorithm.h"
 * \brief Executes the appropriate routines.
 *
 * This class is responsible for calling the appropriate methods for read the input files and
 * execute a specific implementation.
 */

class Algorithm {
public:
  /**
   * \brief Parametrised constructor.
   */
  // Algorithm(const std::string workflow_file_name,
  //           const std::string cluster_file_name,
  //           const std::string conflict_graph_file_name)
  //   : _tasks_and_files(workflow_file_name),
  //     _cluster_file_name(cluster_file_name),
  //     _conflict_graph_file_name(conflict_graph_file_name) { }

  // Algorithm() { }
  virtual ~Algorithm();
  /**
   * \brief Read input files.
   */
  void ReadInputFiles(const std::string tasks_and_files,
                      const std::string cluster,
                      const std::string conflict_graph);


  /// Getter for _max_value
  size_t get_lambda() const { return lambda_; }

  /// Getter for _max_value
  size_t get_task_size() const { return task_size_; }

  /// Getter for _max_value
  size_t get_size() const { return size_; }

  /// Getter for _max_value
  size_t get_vm_size() const { return vm_size_; }

  /// Getter for _max_value
  size_t get_id_source() const { return id_source_; }

  /// Getter for _max_value
  size_t get_id_target() const { return id_target_; }

  /// Getter for _max_value
  std::unordered_map<size_t, VirtualMachine>& get_vm_map() { return vm_map_; }

  /// Getter for _max_value
  std::unordered_map<size_t, Task>& get_task_map_per_id() { return task_map_per_id_; }

  /// Getter for _max_value
  std::unordered_map<std::string, Task>& get_task_map_per_name() { return task_map_per_name_; }

  /// Getter for _max_value
  std::unordered_map<size_t, std::vector<size_t>>& get_successors() { return succ_; }

  /// Getter for _max_value
  std::unordered_map<size_t, std::vector<size_t>>& get_predecessors() { return prec_; }

  /// Getter for _max_value
  std::vector<double>& get_storage_vet() { return storage_vet_; }

  /// Getter for _max_value
  std::vector<int>& get_height() { return height_; }

  /// Getter for _max_value
  std::unordered_map<size_t, File*>& get_file_map_per_id() { return file_map_per_id_; }
  // std::unordered_map<size_t, std::shared_ptr<File>> get_file_map_per_id() const {
    // return file_map_per_id_;
  // }

  /// Getter for _max_value
  std::unordered_map<std::string, File*>& get_file_map_per_name_() {
    return file_map_per_name_;
  }
  // std::unordered_map<std::string, std::shared_ptr<File>> get_file_map_per_name_() const {
    // return file_map_per_name_;
  // }

  /// Getter for _max_value
  int get_file_size() const { return file_size_; }

  /**
   * \brief Executes the algorithm.
   */
  virtual void Run() = 0;

  /**
   * \brief Returns an object derived from Algorithm according to \c name parameter.
   */
  static std::shared_ptr<Algorithm> ReturnAlgorithm(const std::string algorithm);

  std::unordered_map<size_t, std::vector<size_t>> ReverseMap(
      std::unordered_map<size_t, std::vector<size_t>> amap);

protected:
  void ReadTasksAndFiles(std::string);

  void ReadCluster(std::string);

  void ReadConflictGraph(std::string);

  void ComputeHeight(size_t, int);

  size_t static_file_size_;

  size_t dynamic_file_size_;

  size_t task_size_;

  size_t requirement_size_;

  double deadline_;

  double budget_;

  size_t file_size_;

  size_t size_;

  size_t id_source_;

  size_t id_target_;

  double period_hr_;

  size_t vm_size_;

  std::vector<double> storage_vet_;  //storage of vm

  std::vector<Requirement> requirements_;

  // std::unordered_map<int, File> file_map;
  std::unordered_map<size_t, File*> file_map_per_id_;
  std::unordered_map<std::string, File*> file_map_per_name_;
  // std::unordered_map<size_t, std::shared_ptr<File>> file_map_per_id_;
  // std::unordered_map<std::string, std::shared_ptr<File>> file_map_per_name_;

  // unordered_map<int, Task> task_map;
  std::unordered_map<size_t, Task> task_map_per_id_;
  std::unordered_map<std::string, Task> task_map_per_name_;

  // Workflow task Graphs
  std::unordered_map<size_t, std::vector<size_t>> succ_;
  std::unordered_map<size_t, std::vector<size_t>> prec_;

  size_t number_of_providers_;

  size_t number_of_requirements_;

  std::unordered_map<size_t, VirtualMachine> vm_map_;

  int bucket_size_;

  std::vector<Provider> providers_;

  // ConflictGraph _conflictGraph;

  Matrix<double> conflict_graph_;

  std::vector<int> height_;

  double lambda_ =  0.000;  // read and write constant

  std::shared_ptr<Solution> solution_;

 	// int task_size, sfile_size, dfile_size, file_size, size, vm_size, id_sink, id_root;
	// double period_hr;

  /**
     * \brief The file name of the tasks and files.
     *
     * This file contains the description of files, tasks, and adjacencies of the tasks.
     */
  // std::string _tasks_and_files;

  /**
     * \brief The cluster file name.
     *
     * This file contains the public and private cluster definitions.
     */
  // std::string _cluster_file_name;

  /**
     * \brief The conflict graph file name.
     *
     * This file contains the conflict between from the files of consecutive tasks, and
     * task in the same hierarquical level.
     */
  // std::string _conflict_graph_file_name;
};  // end of class Algorithm

#endif  // SRC_SOLUTION_ALGORITHM_H_
