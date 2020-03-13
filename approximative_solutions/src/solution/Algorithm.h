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

#include "src/model/File.h"
#include "src/model/Requirement.h"
#include "src/model/Task.h"

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

  /**
   * \brief Read input files.
   */
  void readInputFiles(const std::string tasks_and_files,
                      const std::string cluster,
                      const std::string conflict_graph);

  /**
   * \brief Executes the algorithm.
   */
  virtual void run() = 0;

  /**
   * \brief Returns an object derived from Algorithm according to \c name parameter.
   */
  static std::unique_ptr<Algorithm> getAlgorithm(const std::string algorithm);

  std::unordered_map<int, std::vector<int>> reverse_map(std::unordered_map<int,
                                                        std::vector<int>> amap);

 private:

  int _static_file_size;

  int _dynamic_file_size;

  int _task_size;

  int _requirement_size;

  double _deadline;

  double _budget;

  int _file_size;

  int _size;

  int _id_source;

  int _id_target;

  double _period_hr;

  int _vm_size;

  std::vector<double> _storage_vet;  //storage of vm

  std::vector<Requirement> _requirements;

  // std::unordered_map<int, File> file_map;
  std::unordered_map<std::string, File> _file_map;

  // unordered_map<int, Task> task_map;
  std::unordered_map<std::string, Task> _task_map;

  // Workflow task Graphs
  std::unordered_map<int, std::vector<int>> succ;
  std::unordered_map<int, std::vector<int>> prec;



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
