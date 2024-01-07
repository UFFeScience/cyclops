/**
 * \file src/solution/heft.h
 * \brief Contains the \c Grch class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2021
 *
 * This header file contains the \c Grch class that construct
 * several \c Solution objects by selecting tasks randomly inside a restrict candidate list.
 */

#ifndef APPROXIMATE_SOLUTIONS_SRC_SOLUTION_HEFT_H_
#define APPROXIMATE_SOLUTIONS_SRC_SOLUTION_HEFT_H_

#include "src/solution/algorithm.h"
#include <list>
#include <map>
#include <boost/range/join.hpp>
#include <iomanip>


/* Event represents the start and end time of a task(id)*/
struct Event {
    size_t id = 0ul;
    double start = 0;
    double end = 0;
};

class Heft : public Algorithm {
public:
    ///
    Heft() = default;

    double RankUpward(size_t activation_id, std::vector<double> &communication_plus_max_successor_rank_cache);

    double ComputeAverageCommunicationCost(size_t activation_id_i, size_t activation_id_j);

    double CommunicationCostStatic(size_t activation_id, size_t vm_id);

    double CommunicationCostOfDynamicFiles(size_t activation_id_i, size_t activation_id_j, size_t vm_id_i, size_t vm_id_j);

    static std::vector<size_t> intersection(std::vector<size_t> v1, std::vector<size_t> v2);

    double ComputeAverageComputationCost(size_t activation_id);

    double ComputationCost(size_t activation_id, size_t vm_id);

    double StartTime(size_t activation_id,
                     size_t vm_id,
                     std::vector<size_t> activation_on,
                     std::map<size_t, std::vector<Event>> orders,
                     std::vector<double> end_time);

    void Allocate(size_t activation_id,
                  std::vector<size_t> &activation_allocation,
                  std::vector<size_t> vm_keys,
                  std::map<size_t, std::vector<Event>> &orders,
                  std::vector<double> &end_time);

    ///
    [[nodiscard]] std::string GetName() const override { return name_; }

    ///
    void Run() override;

private:

    std::string name_ = "Heft";
};

#endif  // APPROXIMATE_SOLUTIONS_SRC_SOLUTION_HEFT_H_
