/**
 * \file src/solution/algorithm.cc
 * \brief Contains the \c Configuration class methods
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo.raps@gmail.com\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2024
 *
 * This source file contains the methods from the \c Configuration class that start the mode the
 * approximate solution
 */

#include "src/solution/algorithm.h"

#include <utility>
#include "src/solution/grch.h"
#include "src/solution/grasp.h"
#include "src/solution/cplex.h"
#include "src/solution/hea.h"
#include "src/solution/heft.h"
#include "src/solution/min_min.h"

// Constructors

Algorithm::Algorithm(std::shared_ptr<Configuration> configuration) : configuration_(std::move(configuration)) {}

// Public methods

/**
 * The \c ReturnAlgorithm() returns an object derived from \c Configuration depending on the
 * \c algorithm parameter.
 *
 * \param[in] algorithm  Name of the \c Configuration object to be instantiated
 * \retval    object     Instance of an \c Configuration object
 */
std::shared_ptr<Algorithm> Algorithm::ReturnAlgorithm(
        const std::string& algorithm,
        std::shared_ptr<Configuration> configuration
) {
    DLOG(INFO) << "ReturnAlgorithm: " << algorithm;
    std::cout << "ReturnAlgorithm: " << algorithm << std::endl;

    if (algorithm == "grch") {
        return std::make_shared<Grch>(configuration);
    } else if (algorithm == "cplex") {
        return std::make_shared<Cplex>(configuration);
    } else if (algorithm == "grasp") {
        configuration->set_check_for_hard_constraints(false);
        return std::make_shared<Grasp>(configuration);
    } else if (algorithm == "heft") {
        configuration->set_check_for_hard_constraints(false);
        return std::make_shared<Heft>(configuration);
    } else if (algorithm == "min-min") {
        configuration->set_check_for_hard_constraints(false);
        return std::make_shared<MinMin>(configuration);
    } else if (algorithm == "hea") {
        configuration->set_check_for_hard_constraints(false);
        return std::make_shared<Hea>(configuration);
    } else {
        std::fprintf(stderr, "Please select a valid algorithm.\n");
        std::exit(-1);
    }
}
