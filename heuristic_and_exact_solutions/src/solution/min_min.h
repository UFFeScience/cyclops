/**
 * \file src/solution/min_min.h
 * \brief Contains the \c MinMin class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo.raps@gmail.com\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 *
 * This header file contains the \c MinMin class.
 */

#ifndef APPROXIMATE_SOLUTIONS_SRC_SOLUTION_MIN_MIN_H_
#define APPROXIMATE_SOLUTIONS_SRC_SOLUTION_MIN_MIN_H_

#include "src/solution/algorithm.h"
#include "src/solution/configuration.h"
#include <list>
#include <map>
#include <boost/range/join.hpp>
#include <iomanip>

struct ActivationAssignment {
    size_t activation_id;
    size_t vm_id;
    double completion_time;
};

class MinMin : public Algorithm {
public:
    // Constructors

    ///
    explicit MinMin(const std::shared_ptr<Configuration> configuration) : Algorithm(configuration) {}

    // Destructors

    virtual ~MinMin() = default;

    // Public Methods

    ///
    void MinMinScheduling(Solution& best_solution);

    ///
    void Run() override;

    ///
    [[nodiscard]] std::string get_name() const override { return name_; }

private:
    // Properties

    std::string name_ = "MinMin";

    // Methods

    ///
    double CalculateFinishTime(
            Solution& solution,
            size_t act_id,
            size_t vm_id,
            const std::vector<ActivationAssignment>& activations_assignments,
            const std::vector<double>& virtual_machines_available_time
    );

    ///
    ActivationAssignment FindMinMin(
            Solution& best_solution,
            const std::list<size_t>& available_activations,
            const std::vector<ActivationAssignment>& activations_assignments,
            const std::vector<double>& virtual_machines_available_time
    );
};

#endif  // APPROXIMATE_SOLUTIONS_SRC_SOLUTION_HEFT_H_
