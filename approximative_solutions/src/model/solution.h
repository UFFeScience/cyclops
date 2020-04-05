/**
 * \file src/model/solution.h
 * \brief Contains the \c Solution class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \date 2020
 *
 * This header file contains the \c Solution class.
 */

#ifndef SRC_MODEL_SOLUTION_H_
#define SRC_MODEL_SOLUTION_H_

#include <iostream>
#include <vector>
#include <list>
#include <limits>
#include <algorithm>
#include <random>

#include "src/solution/algorithm.h"

class Algorithm;

struct vm_time {
  std::vector<std::pair<double, double>> life;
  double last;
  double static_vm = false;
  double storage = 0.0;
  int count_file = 0;
};

class Solution {

public:

  Algorithm* algorithm_;

  std::vector<size_t> allocation;

  std::vector<int> height_soft_;

  std::vector<double> time_vector;

  std::vector<double> start_time_vector;

  std::vector<size_t> ordering;

  double fitness;

  double lambda_;

  double transfer_size;

  std::unordered_map<int, std::vector<std::string>> scheduler;

  std::unordered_map<size_t, std::vector<size_t>> vm_queue;

  Solution(Algorithm* algorithm);

  Solution(const Solution &other)
      : algorithm_(other.algorithm_),
        allocation(other.allocation),
        height_soft_(other.height_soft_),
        time_vector(other.time_vector),
        start_time_vector(other.start_time_vector),
        ordering(other.ordering),
        fitness(other.fitness),
        lambda_(other.lambda_),
        transfer_size(other.transfer_size),
        scheduler(other.scheduler),
        vm_queue(other.vm_queue) {}


  //virtual ~Solution();
  ~Solution() { algorithm_ = nullptr; }

  // Compute the fitness of Solution
  void ComputeFitness(bool check_storage = true, bool check_sequence = false);

  inline bool CheckTaskSequence(size_t);

  inline bool checkFiles();

  void print();

  std::ostream& write(std::ostream& os) const;

  friend std::ostream& operator<<(std::ostream& os, const Solution& a) {
    return a.write(os);
  }

private:
  int ComputeHeightSoft(size_t node);

  void Encode();

  inline double TaskStartTime(Task task, VirtualMachine vm, const std::vector<double>& queue);

  inline double TaskReadTime(Task task, VirtualMachine vm);

  inline double TaskWriteTime(Task task, VirtualMachine vm);

  inline double FileTransferTime(File file, Storage vm1, Storage vm2);
};

#endif  // SRC_MODEL_SOLUTION_H_
