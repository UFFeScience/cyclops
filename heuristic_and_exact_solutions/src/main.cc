/**
 * \file src/main.cc
 * \brief Run the Wf-Security Greedy Solution executable.
 *
 * \author Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2024
 *
 * This source file contains the \c main() function that reads the execution mode from the
 * command-line and calls the appropriate methods.
 */

#include <glog/logging.h>
#include "src/solution/algorithm.h"

DEFINE_string(tasks_and_files, // NOLINT(cert-err58-cpp)
              "CyberShake_30.xml.dag",
              "Activation and Files configuration file");

DEFINE_string(cluster, // NOLINT(cert-err58-cpp)
              "cluster.vcl",
              "Clusters configuration file");

DEFINE_string(conflict_graph, // NOLINT(cert-err58-cpp)
              "CyberShake_30.xml.scg",
              "Conflict Graph configuration file");

DEFINE_string(algorithm, // NOLINT(cert-err58-cpp)
              "greedy",
              "Selected algorithm to solve the problem");

DEFINE_double(alpha_time, // NOLINT(cert-err58-cpp)
              0.4,
              "The weight of the time objective");

DEFINE_double(alpha_budget, // NOLINT(cert-err58-cpp)
              0.4,
              "The weight of the budget objective");

DEFINE_double(alpha_security, // NOLINT(cert-err58-cpp)
              0.2,
              "The weight of the security objective");

DEFINE_double(alpha_restrict_candidate_list, // NOLINT(cert-err58-cpp)
              0.5,
              "The threshold parameter to truncate the candidate list");

DEFINE_uint64(number_of_iteration, // NOLINT(cert-err58-cpp)
              100ul,
              "Number of attempts to build the solution");

DEFINE_uint64(number_of_allocation_experiments, // NOLINT(cert-err58-cpp)
              4ul,
              "Number of allocation experiments");

DEFINE_string(cplex_output_file, // NOLINT(cert-err58-cpp)
              "./temp/manual/cplex/not_applicable.lp",
              "Example of output model file name of the CPLEX");

/**
 * The \c main() function reads the loads the desired input files, applies the required algorithms
 * and writes output data files.
 */
int main(int argc, char **argv) {
    // Initialise Google's logging library
    ::google::InitGoogleLogging(argv[0]);

    gflags::SetUsageMessage("some usage message");
    gflags::SetVersionString("0.0.1");
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    DLOG(INFO) << "Starting ...";

    DLOG(INFO) << "Input File of the Tasks and Files: " << FLAGS_tasks_and_files;
    DLOG(INFO) << "Input File of the Cluster: " << FLAGS_cluster;
    DLOG(INFO) << "Input File of the Conflict Graph: " << FLAGS_conflict_graph;
    DLOG(INFO) << "Selected algorithm: " << FLAGS_algorithm;
    DLOG(INFO) << "Alpha Time weight: " << FLAGS_alpha_time;
    DLOG(INFO) << "Alpha Budget weight: " << FLAGS_alpha_budget;
    DLOG(INFO) << "Alpha Security weight: " << FLAGS_alpha_security;
    DLOG(INFO) << "Alpha Restrict Candidate List threshold: " << FLAGS_alpha_restrict_candidate_list;
    DLOG(INFO) << "Number of iteration: " << FLAGS_number_of_iteration;
    DLOG(INFO) << "Number of allocation experiments: " << FLAGS_number_of_allocation_experiments;
    DLOG(INFO) << "CPLEX output file: " << FLAGS_cplex_output_file;

    std::cout << "Input File of the Tasks and Files: " << FLAGS_tasks_and_files << std::endl;
    std::cout << "Input File of the Cluster: " << FLAGS_cluster << std::endl;
    std::cout << "Input File of the Conflict Graph: " << FLAGS_conflict_graph << std::endl;
    std::cout << "Selected algorithm: " << FLAGS_algorithm << std::endl;
    std::cout << "Alpha Time weight: " << FLAGS_alpha_time << std::endl;
    std::cout << "Alpha Budget weight: " << FLAGS_alpha_budget << std::endl;
    std::cout << "Alpha Security weight: " << FLAGS_alpha_security << std::endl;
    std::cout << "Alpha Restrict Candidate List threshold: " << FLAGS_alpha_restrict_candidate_list << std::endl;
    std::cout << "Number of iteration: " << FLAGS_number_of_iteration << std::endl;
    std::cout << "Number of allocation experiments: " << FLAGS_number_of_allocation_experiments << std::endl;
    std::cout << "CPLEX output file: " << FLAGS_cplex_output_file << std::endl;

    std::shared_ptr<Algorithm> algorithm = Algorithm::ReturnAlgorithm(FLAGS_algorithm);

    DLOG(INFO) << "... algorithm picked-up ...";

    algorithm->ReadInputFiles(FLAGS_tasks_and_files, FLAGS_cluster, FLAGS_conflict_graph);
    std::cout << "Number of Activation: " << algorithm->GetActivationSize() << " (including source and target)"
              << std::endl;
    std::cout << "Number of files: " << algorithm->GetFilesSize() << std::endl;
    algorithm->SetAlphas(FLAGS_alpha_time,
                         FLAGS_alpha_budget,
                         FLAGS_alpha_security,
                         FLAGS_alpha_restrict_candidate_list);
    algorithm->CalculateMaximumSecurityAndPrivacyExposure();
    algorithm->Run();

    DLOG(INFO) << "... ending.";
    gflags::ShutDownCommandLineFlags();

    return 0;
}
