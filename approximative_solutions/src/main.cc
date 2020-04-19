/**
 * \file src/main.cc
 * \brief Run the Wf-Security Greedy Solution executable.
 *
 * \author Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2020
 *
 * This source file contains the \c main() function that reads the execution mode from the
 * command-line and calls the appropriate methods.
 */

#include "src/main.h"

#include <glog/logging.h>
#include <gflags/gflags.h>
#include <exception>
#include "src/solution/algorithm.h"

DEFINE_string(tasks_and_files,
              "CyberShake_30.xml.dag",
              "Task and Files configuration file");

DEFINE_string(cluster,
              "cluster.vcl",
              "Clusters configuration file");

DEFINE_string(conflict_graph,
              "CyberShake_30.xml.scg",
              "Conflict Graph configuration file");

DEFINE_string(algorithm,
              "greedy",
              "Selected algorithm to solve the problem");

DEFINE_double(alpha_time,
              0.4,
              "The weight of the time objective");

DEFINE_double(alpha_budget,
              0.4,
              "The weight of the budget objective");

DEFINE_double(alpha_security,
              0.2,
              "The weight of the security objective");

DEFINE_double(alpha_restrict_candidate_list,
              0.5,
              "The threshold parameter to truncate the candidate list");

DEFINE_uint64(number_of_iteration,
              100ul,
              "Number of attempts to build the solution");

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
  DLOG(INFO) << "Number_of_iteration: " << FLAGS_number_of_iteration;
  // google::FlushLogFiles(google::INFO);

  std::shared_ptr<Algorithm> algorithm = Algorithm::ReturnAlgorithm(FLAGS_algorithm);

  DLOG(INFO) << "... algorithm picked-up ...";

  algorithm->ReadInputFiles(FLAGS_tasks_and_files, FLAGS_cluster, FLAGS_conflict_graph);
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
