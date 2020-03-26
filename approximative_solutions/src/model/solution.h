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

  // Data* data;

  std::unordered_map<int, std::vector<std::string>> scheduler;

  std::unordered_map<size_t, std::vector<size_t>> vm_queue;

  // Solution() { }

  // Solution(std::shared_ptr<Algorithm> algorithm);
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
  virtual ~Solution() = default;

  void ComputeFitness(bool check_storage = true, bool check_sequence = false);

  inline bool checkTaskSeq(size_t);

  inline bool checkFiles();
    // // Compute the fitness of Solution
    // void computeFitness(bool check_storage = true, bool check_sequence = false) {

    //     fill(time_vector.begin(), time_vector.end(), -1);
    //     fill(start_time_vector.begin(), start_time_vector.end(), -1);

    //     std::vector<double> queue(algorithm_->get_vm_size(), 0);

    //     // if (check_storage && !checkFiles()) {
    //     //     std::cerr << "check file error" << std::endl;
    //     //     throw;
    //     // }

    //     scheduler.clear();
    //     vm_queue.clear();
    //     // compute makespan
    //     for (auto id_task : ordering) {//for each task, do
    //         if (id_task != algorithm_->get_id_source() && id_task != algorithm_->get_id_target()) {//if is not root or sink than

    //             if (check_sequence && !checkTaskSeq(id_task)) {
    //                 std::cerr << "Encode error - Chrom: Error in the precedence relations." << std::endl;
    //                 throw;
    //             }

    //             // Load Vm
    //             auto vm = algorithm_->get_vm_map().find(allocation[id_task])->second;
    //             auto task = algorithm_->get_task_map().find(id_task)->second;

    //             // update vm queue
    //             auto f_queue = vm_queue.insert(std::make_pair(vm.get_id(), std::vector<int>()));
    //             f_queue.first->second.push_back(task.get_id());

    //             // update scheduler
    //             auto f_scheduler = scheduler.insert(std::make_pair(vm.id, std::vector<string>()));
    //             f_scheduler.first->second.push_back(task.tag);

    //             // Compute Task Times
    //             // auto start_time = ST(task, vm, queue);
    //             // auto read_time = RT(task, vm);
    //             // auto run_time = std::ceil(task.base_time * vm.slowdown);  // Seconds
    //             // auto write_time = WT(task, vm);
    //             // auto finish_time = start_time + read_time + run_time + write_time;

    //             // Update structures
    //             // time_vector[id_task] = finish_time;
    //             // start_time_vector[id_task] = start_time;
    //             // queue[vm.id] = finish_time;

    //         } else {  // Root and sink tasks
    //             if (id_task == algorithm_->get_id_source())
    //                 time_vector[static_cat<size_t>(id_task)] = 0;
    //             else {  //sink task
    //                 double max_value = 0.0;
    //                 // for (auto tk : data->prec.find(id_task)->second)
    //                 //     max_value = std::max(max_value, time_vector[tk]);
    //                 time_vector[id_task] = max_value;
    //             }
    //         }
    //     }

    //     fitness = time_vector[algorithm_->get_id_target()];
    // }

    /*crossover*/
    // Chromosome crossover(Chromosome partner) {
    //     Chromosome chr(partner);
    //     uniform_int_distribution<> dis_ordering(0, data->task_size - 1);
    //     uniform_int_distribution<> dis_allocation(0, data->size - 1);

    //     int point_ordering = dis_ordering(engine_chr);//crossover point to ordering list
    //     int point_allocation = dis_allocation(engine_chr);//crossover point to allocation list
    //     //allocation crossover (Single point crossover)
    //     for (int i = 0; i < point_allocation; i++) {
    //         chr.allocation[i] = allocation[i];
    //     }
    //     //ordering crossover
    //     vector<bool> aux(data->task_size, false);
    //     chr.ordering.clear();
    //     //ordering crossover first part self -> chr
    //     for (auto i = 0; i < point_ordering; i++) {
    //         chr.ordering.push_back(ordering[i]);
    //         aux[ordering[i]] = true;
    //     }

    //     //Ordering crossover second part partner -> chr
    //     for (auto i = 0; i < data->task_size; i++) {
    //         if (!aux[partner.ordering[i]])
    //             chr.ordering.push_back(partner.ordering[i]);
    //     }
    //     return chr;
    // }

    //Mutation on allocation chromosome
    // void mutate(double prob) {
    //     uniform_int_distribution<> idis(0, data->vm_size - 1);
    //     for (int i = 0; i < data->size; i++) {
    //         if (((float) random() / (float) RAND_MAX) <= prob) {
    //             allocation[i] = idis(engine_chr);
    //         }
    //     }
    // }

  void print();
  std::ostream& write(std::ostream& os) const;

    // Compute distance between two solutions
    // int getDistance(const Chromosome &chr) {
    //     int distance = 0;

    //     // Compute the distance based on position
    //     for (int i = 0; i < data->size; i++) {
    //         if (chr.allocation[i] != allocation[i])
    //             distance += 1;
    //     }

    //     // Compute the distance based on swaps required
    //     vector<int> aux_ordering(ordering);
    //     for (int i = 0; i < data->task_size; i++) {
    //         if (chr.ordering[i] != aux_ordering[i]) {
    //             distance += 1;

    //             for (int j = i + 1; j < data->task_size; j++)
    //                 if (chr.ordering[i] == aux_ordering[j])
    //                     iter_swap(aux_ordering.begin() + i, aux_ordering.begin() + j);

    //         }
    //     }
    //     return distance;
    // }


  friend std::ostream& operator<<(std::ostream& os, const Solution& a) {
    return a.write(os);
  }

private:
  int ComputeHeightSoft(size_t node);

  void Encode();

  inline double TaskStartTime(Task task, VirtualMachine vm, const std::vector<double>& queue);

  inline double TaskReadTime(Task task, VirtualMachine vm);

  inline double TaskWriteTime(Task task, VirtualMachine vm);

  inline double FileTransferTime(File file, VirtualMachine vm1, VirtualMachine vm2);
};

#endif  // SRC_MODEL_SOLUTION_H_
