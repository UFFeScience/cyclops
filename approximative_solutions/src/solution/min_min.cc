// /**
//  * \file src/exec_manager/execution.cc
//  * \brief Contains the \c Algorithm class methods.
//  *
//  * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
//  * \date 2020
//  *
//  * This source file contains the methods from the \c Algorithm class that run the mode the
//  * approximate solution.
//  */

// #include "src/solution/min_min.h"

// /* Call MinMin */
// Chromosome minMinHeuristic(Data *data) {
//     list<int> task_list;
//     // start task list
//     for (auto info : data->task_map)
//         task_list.push_back(info.second.id);
//     task_list.sort([&](const int &a, const int &b) { return data->height[a] < data->height[b]; });

//     list<int> avail_tasks;

//     vector<double> ft_vector(data->size, 0);
//     vector<double> queue(data->vm_size, 0);
//     vector<int> file_place(data->size, 0);
//     list<int> task_ordering(0);

//     //the task_list is sorted by the height(t). While task_list is not empty do
//     while (!task_list.empty()) {
//       auto task = task_list.front();//get the first task
//       avail_tasks.clear();
//       while (!task_list.empty() && data->height[task] == data->height[task_list.front()]) {
//         //build list of ready tasks, that is the tasks which the predecessor was finish
//         avail_tasks.push_back(task_list.front());
//         task_list.pop_front();
//       }

//       schedule(data, avail_tasks, ft_vector, queue, file_place, task_ordering,
//                 setting->lambda);//Schedule the ready tasks
//     }

//     Chromosome minMin_chrom(data, setting->lambda);

//     for (int i = 0; i < data->size; i++) {
//       minMin_chrom.allocation[i] = file_place[i];
//     }
//     minMin_chrom.ordering.clear();

//     minMin_chrom.ordering.insert(minMin_chrom.ordering.end(), task_ordering.begin(), task_ordering.end());
//     minMin_chrom.computeFitness(true, true);

//     //if(setting->verbose)
//     //    cout << "MinMIn fitness: " << minMin_chrom.fitness << endl;

//     return minMin_chrom;
// }

// void MinMin::run() {
//   // Load input Files and the data structures used by the algorithms
//   Data *data = new Data(name_workflow, name_cluster);

//   vector<Chromosome> Population;
//   vector<Chromosome> Elite_set;

//   // Set Delta
//   setting->delta = data->size / 4.0;

//   // check distance (inner Function)
//   auto check_distance = [&](Chromosome chr, const vector<Chromosome> &Set) {
//     for (auto set_ch : Set) {
//       if (chr.getDistance(set_ch) < setting->delta) {
//           return false;
//       }
//     }
//     return true;
//   };

//   // == Start initial population == //
//   Chromosome minminChr(minMinHeuristic(data));
//   Chromosome heftChr(HEFT(data));

//   Population.push_back(minminChr);
//   Population.push_back(heftChr);

//   double mut = 0.05;

//   // 90% using the mutate procedure with variation
//   for (int i = 0; i < ceil(setting->num_chromosomes * 0.9); i++) {
//     Chromosome chr1(minminChr);
//     chr1.mutate(mut);
//     chr1.computeFitness();

//     Chromosome chr2(heftChr);
//     chr2.mutate(mut);
//     chr2.computeFitness();

//     Population.push_back(chr1);
//     Population.push_back(chr2);

//     mut = mut > 1.0 ? 0.05 : mut + 0.05;
//   }

//   // 10% random solutions
//   for (int i = 0; i < (setting->num_chromosomes * 0.10); i++) {
//       Population.push_back(Chromosome(data, setting->lambda));
//   }

//   // Get best solution from initial population
//   Chromosome best(Population[getBest(Population)]);

//   // Do generation
//   int i = 0;
//   // start stop clock

//   while (i < setting->num_generations) {
//     // Do local Search ?

//     float doit = (float) random() / (float) RAND_MAX;

//     if (doit <= (setting->localSearch_probability))
//       localSearch(Population, data);

//     // Update best
//     auto pos = getBest(Population);

//     if (best.fitness > Population[pos].fitness) {
//       best = Population[pos];

//       // Apply path Relinking
//       if (!Elite_set.empty()) {
//         best = pathRelinking(Elite_set, best, data);
//       }

//       // Update Elite-set
//       if (check_distance(best, Elite_set)) {
//         Elite_set.push_back(best);  // Push all best' solutions on Elite-set
//       }

//       // check elite set size
//       if (Elite_set.size() > static_cast<unsigned int>(setting->num_elite_set)) {
//         Elite_set.erase(Elite_set.begin());
//       }

//       // Apply Local Search

//       best = localSearchN1(data, best);
//       best = localSearchN2(data, best);
//       best = localSearchN3(data, best);

//       Population[pos] = best;

//       i = 0;
//     }

//     if (setting->verbose && (i % setting->print_gen) == 0) {
//       cout << "Gen: " << i << " Fitness: " << best.fitness / 60.0 << "(s)" << endl;
//     }

//     i += 1;

//     doNextPopulation(Population);
//   }

//   // return the global best
//   return best;
// }  // end of MinMin::run() method
