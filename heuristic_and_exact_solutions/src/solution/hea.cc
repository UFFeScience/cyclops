/**
 * \file src/solution/hea.cc
 * \brief Contains the \c Configuration class methods for the Hybrid Evolutionary Configuration
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo.raps@gmail.com\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 *
 * This source file contains the methods for the \c Hea class that start the Hybrid Evolutionary Configuration
 * methods
 */

#include <iomanip>
#include "src/solution/hea.h"
#include "src/solution/min_min.h"
#include "src/solution/heft.h"

//DECLARE_uint64(number_of_iteration);

constexpr size_t NUM_CHROMOSOMES = 50;
constexpr size_t NUM_ELITE_SET = 25;
constexpr size_t NUM_GENERATIONS = 100;
constexpr double MUTATION_PROBABILITY = 0.1;
constexpr size_t HOW_MANY_ELITISM = 5;
constexpr double ALPHA = 0.3;
constexpr double LOCAL_SEARCH_PROBABILITY = 0.5;

// Get the best chromosome
size_t Hea::GetBest(std::vector<Solution>& population) {
    Solution best = population[0];
    size_t pos = 0;
    for (size_t i{}; i < NUM_CHROMOSOMES; i++) {
        if (best.OptimizedComputeObjectiveFunction() > population[i].OptimizedComputeObjectiveFunction()) {
            best = population[i];
            pos = i;
        }
    }
    return pos;
}

// Tournament Selection
size_t Hea::TournamentSelection(std::vector<Solution>& population) {
    // We pick to chromosomes at random
    size_t a = my_rand<size_t>(0, population.size() - 1);
    size_t b = my_rand<size_t>(0, population.size() - 1);

    // Make sure they're not the same chromosome!
    while (b == a) {
        b = my_rand<size_t>(0, population.size() - 1);
    }

    // Now select the better of the two as our parent
    return population[a].get_objective_value() < population[b].get_objective_value() ? a : b;
}

// ========== Path Relinking ============ //
Solution Hea::PathRelinking(std::vector<Solution> elite_set, Solution& dest) {
    Solution best(dest);

    // For each chromosome on Elite Set, do:
    for (size_t i{}; i < elite_set.size(); i++) {
        Solution src = elite_set[i];

        src.ClearOrdering();

        for (size_t act_id : dest.get_ordering()) {
            src.AddOrdering(act_id);
        }

        for (size_t el{}; el < GetActivationsSize(); el++) {
            if (src.get_activation_allocation(el) != dest.get_activation_allocation(el)) {
                src.set_activation_allocation(el, dest.get_activation_allocation(el));
                src.OptimizedComputeObjectiveFunction();
                if (best.get_objective_value() > src.get_objective_value()) {
                    best = src;
                }
            }
        }

        for (size_t el{}; el < GetFilesSize(); el++) {
            if (src.get_file_allocation(el) != dest.get_file_allocation(el)) {
                src.change_file_allocation(el, dest.get_file_allocation(el));
                src.OptimizedComputeObjectiveFunction();
                if (best.get_objective_value() > src.get_objective_value()) {
                    best = src;
                }
            }
        }
    }

    return best;
}

// ========== Main Functions ========== //

void Hea::DoNextPopulation(std::vector<Solution>& population) {
    LOG(INFO) << "HEA:DoNextPopulation:Start";
    std::vector<Solution> children_pool;

    // === do offsprings === //
    for (size_t i{}; i < ceil(NUM_CHROMOSOMES / 2.0); i++) {
        // select our two parents with tournament Selection
        size_t posA, posB;
        posA = TournamentSelection(population);
        do {
            posB = TournamentSelection(population);
        } while (posA == posB);

        // get the parents
        auto parentA = population[posA];
        auto parentB = population[posB];

        // cross their genes
        auto child = parentA.Crossover(parentB);
        // mutate the child
        child.Mutate(MUTATION_PROBABILITY);
        // recompute fitness
        child.OptimizedComputeObjectiveFunction();
        // Add solution on children_pool
        children_pool.push_back(child);
    }

    // === update population === //

    // add all solutions to the children_pool
    children_pool.insert(children_pool.end(), population.begin(), population.end());

    // Delete old population
    population.clear();

    // Elitisme operator - the best is always on the population
    sort(children_pool.begin(), children_pool.end(), [&](const Solution &chr1, const Solution &chr2) {
        return chr1.get_objective_value() < chr2.get_objective_value();
    });

    for (size_t i{}; i < HOW_MANY_ELITISM; i++) {
        population.push_back(children_pool[0]);
        children_pool.erase(children_pool.begin());
    }

    // Selected the solutions to build the new population
    while (population.size() < static_cast<unsigned int>(NUM_CHROMOSOMES)) {
//        LOG(INFO) << "HEA:DoNextPopulation:Selecting population";
        auto pos = TournamentSelection(children_pool);
        population.push_back(Solution(children_pool[pos]));
        children_pool.erase(children_pool.begin() + static_cast<long>(pos));
    }

    random_shuffle(population.begin(), population.end());
    LOG(INFO) << "HEA:DoNextPopulation:End";
}

// Call all Local Search Functions
void Hea::LocalSearchHea(std::vector<Solution>& population) {
    size_t how_many = ALPHA * NUM_CHROMOSOMES;

    for (size_t j{}; j < how_many; j++) {
        auto ch_pos = TournamentSelection(population);
        population[ch_pos].HeaLocalSearchN1();
        population[ch_pos].HeaLocalSearchN2();
        population[ch_pos].HeaLocalSearchN3();
    }
}

void Hea::HeaScheduling(Solution& best_solution) {
    // Load input Files and the data structures used by the algorithms
    std::vector<Solution> population;
    std::vector<Solution> elite_set;

    double delta = (GetActivationsSize() + GetFilesSize()) / 4.0;

    // check distance (inner Function)
    auto check_distance = [&](Solution chr, const std::vector<Solution>& set) {
        for (auto set_ch : set) {
            if (static_cast<double>(chr.GetDistance(set_ch)) < delta) {
                return false;
            }
        }
        return true;
    };

    // == Start initial population == //
    MinMin minMinHeuristic{get_configuration()};
    Solution minMinChr{get_configuration()};
    minMinHeuristic.MinMinScheduling(minMinChr);

    Heft heftHeuristic{get_configuration()};
    Solution heftChr(get_configuration());
    heftHeuristic.HeftScheduling(heftChr);

    population.push_back(minMinChr);
    population.push_back(heftChr);

    double mut = 0.05;

    // 90% using the mutate procedure with variation
    size_t min_min_heft_mutated_population_number = std::ceil<size_t>(NUM_CHROMOSOMES * 0.9);
    for (size_t i{}; i < min_min_heft_mutated_population_number; i++) {
        Solution chr1(minMinChr);
        chr1.Mutate(mut);

        Solution chr2(heftChr);
        chr2.Mutate(mut);

        population.push_back(chr1);
        population.push_back(chr2);

        mut = mut > 1.0 ? 0.05 : mut + 0.05;
    }

    // 10% random solutions
    for (size_t i{}; i < NUM_CHROMOSOMES - min_min_heft_mutated_population_number; i++) {
        Solution chromosome{get_configuration()};
        chromosome.Encode();
        population.push_back(chromosome);
    }

    // Get the best solution from initial population
    Solution best(population[GetBest(population)]);

    // Do generation
    size_t i{};
    // start stop clock

    while (i < NUM_GENERATIONS) {
        // Do local Search ?

        float doit = (float) random() / (float) RAND_MAX;

        if (doit <= (LOCAL_SEARCH_PROBABILITY)) {
            LocalSearchHea(population);
        }

        // Update best
        auto pos = GetBest(population);

        if (best.get_objective_value() > population[pos].get_objective_value()) {
            best = population[pos];

            // Apply path Relinking
            if (!elite_set.empty())
                best = PathRelinking(elite_set, best);

            // Update Elite-set
            if (check_distance(best, elite_set))
                elite_set.push_back(best);  // Push all best' solutions on Elite-set

            // check elite set size
            if (elite_set.size() > static_cast<unsigned int>(NUM_ELITE_SET))
                elite_set.erase(elite_set.begin());

            // Apply Local Search

            best.HeaLocalSearchN1();
            best.HeaLocalSearchN2();
            best.HeaLocalSearchN3();

            population[pos] = best;

            i = 0;
        }

//        std::cerr << "Gen: " << i << " Fitness: " << best.get_objective_value() << std::endl;

        i += 1;

        DoNextPopulation(population);
    }

    // return the global best
    best_solution = best;
}

void Hea::Run() {
    DLOG(INFO) << "Executing Hybrid Evolutionary Configuration (HEA) ...";
    double time_s;

    get_configuration()->SetAlphas(1.0, 0.0, 0.0, 0.5);

    Solution best_solution(get_configuration());

    HeaScheduling(best_solution);

//    // Calculating the real ObjectiveFunction
//    best_solution.OptimizedComputeObjectiveFunction();

    DLOG(INFO) << "Scheduling done";
    time_s = double(clock() - t_start) / CLOCKS_PER_SEC;  // Processing time

    LOG(INFO) << best_solution;

    std::cout << std::fixed << std::setprecision(6)
              << best_solution.get_objective_value()
              << " " << best_solution.get_makespan()
              << " " << best_solution.get_cost()
              << " " << best_solution.get_security_exposure() / GetMaximumConfidentialityExposure()
              << " " << time_s
              << " " << 1
              << " " << 1
              << " " << 1
              << std::endl;

    DLOG(INFO) << "... ending Hybrid Evolutionary Configuration (HEA)";
}
