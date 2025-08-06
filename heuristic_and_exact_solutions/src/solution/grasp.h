/**
 * \file src/solution/greedy_randomized_constructive_heuristic.h
 * \brief Contains the \c Grasp class declaration
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo.raps@gmail.com\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2024
 *
 * This header file contains the \c Grasp class that construct
 * several \c Solution objects by selecting tasks randomly inside a restrict candidate list
 */

#ifndef APPROXIMATE_SOLUTIONS_SRC_SOLUTION_GRASP_H_
#define APPROXIMATE_SOLUTIONS_SRC_SOLUTION_GRASP_H_


#include "src/solution/algorithm.h"
#include "src/solution/configuration.h"
#include "grch.h"

class Grasp : public Algorithm {
public:
    // Constructors

    ///
    Grasp(std::shared_ptr<Configuration> configuration) : Algorithm(configuration) {}

    // Destructor

    ///
    virtual ~Grasp() = default;

    ///
    [[nodiscard]] std::string get_name() const override { return name_; }

    ///
    void localSearch(Solution &);

    ///
    void Run() override;
private:
    std::string name_ = "grasp";

    double lsn_time_1 = 0.0;  // Total Elapsed Time of Local Search Neighborhood 1
    size_t lsn_noi_1 = 0ul;  // Total Number of Improvements made by Local Search Neighborhood 1

    double lsn_time_2 = 0.0;  // Total Elapsed Time of Local Search Neighborhood 2
    size_t lsn_noi_2 = 0ul;  // Total Number of Improvements made by Local Search Neighborhood 2

    double lsn_time_3 = 0.0;  // Total Elapsed Time of Local Search Neighborhood 3
    size_t lsn_noi_3 = 0ul;  // Total Number of Improvements made by Local Search Neighborhood 3
};


#endif  // APPROXIMATE_SOLUTIONS_SRC_SOLUTION_GRASP_H_
