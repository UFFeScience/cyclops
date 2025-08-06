/**
 * \file src/solution/hea.h
 * \brief Contains the \c Hea class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo.raps@gmail.com\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2025
 *
 * This header file contains the \c Hea class that defines the Hybrid Evolutionary Configuration methods.
 */

#ifndef APPROXIMATE_SOLUTIONS_SRC_SOLUTION_HEA_H_
#define APPROXIMATE_SOLUTIONS_SRC_SOLUTION_HEA_H_

#include "src/solution/algorithm.h"
#include "src/solution/configuration.h"
#include <list>
#include <algorithm>
#include <vector>

//struct SettingsStruct {
//    int num_chromosomes;                // Number of chromosomes
//    int num_generations;                // Number of generations
//    int num_elite_set;                  // Max size of Elite-set
//
//    float mutation_probability;         // Probability of mutation (for each gene)
//    float elitism_rate;                 // Rate of generated solutions
//    int howMany_elistism;
//    float alpha;                        // percentage of chromosomes send to local search procedures
//    float localSearch_probability;      // Probability of local search
//
//    float time_limit;                   // Run time limite in seconds
//    int print_gen;                      // What generation will be printed
//    bool verbose, start_heuristic;      // verbose = print type, start_heuristic = Initial Population type
//
//    long seed;                          // Random Seed
//
//    double delta = 0.0;                 // acceptance criteria for Elite-Set (based on distance)
//    double lambda;                      // read and write constant
//
//    SettingsStruct() {
//        // Default Settings
//        num_chromosomes = 50;
//        num_generations = 100;
//        num_elite_set = 25;
//
//        mutation_probability = 0.10;
//        elitism_rate = 0.10;
//        alpha = 0.30; // Local Search Rate
//        localSearch_probability = 0.50;
//        time_limit = 7200; // time limit 30 minutes
//
//        print_gen = 10;
//
//        verbose = false;
//        start_heuristic = true;
//
//        howMany_elistism = (int) ceil(num_chromosomes * elitism_rate);
//
//        lambda = 0.000;
//    }
//};

class Hea : public Algorithm {
public:
    // Constructors

    ///
    Hea(std::shared_ptr<Configuration> configuration) : Algorithm(configuration) {}

    // Destructor

    ///
    virtual ~Hea() = default;

    // Methods

    ///
    void HeaScheduling(Solution& best_solution);

    void DoNextPopulation(std::vector<Solution>& population);

    ///
    size_t TournamentSelection(std::vector<Solution>& population);

    size_t GetBest(std::vector<Solution>& population);

    ///
    Solution PathRelinking(std::vector<Solution> elite_set, Solution& dest);

    ///
    Solution start();

    ///
    void LocalSearchHea(std::vector<Solution>& population);

    ///
    [[nodiscard]] std::string get_name() const override { return name_; }

    ///
    void Run() override;
private:
    std::string name_ = "hea";

//    SettingsStruct *setting;
};

#endif  // APPROXIMATE_SOLUTIONS_SRC_SOLUTION_HEA_H_
