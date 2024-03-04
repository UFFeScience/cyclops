#!/usr/bin/env bash

ALGORITHM=grasp

INSTANCE=Synthetic_015_C

PROGRAM=/home/wsl/git/Wf-Security/heuristic_and_exact_solutions/bin/wf_security_greedy.x
TASK_AND_FILES=/home/wsl/git/Wf-Security/heuristic_and_exact_solutions/input/tasks_and_files/$INSTANCE.dag
CLUSTER=/home/wsl/git/Wf-Security/heuristic_and_exact_solutions/temp/clouds/4_1_4_16_2_4_heterogeneous_cloud.vcl
CONFLICT_GRAPH=/home/wsl/git/Wf-Security/heuristic_and_exact_solutions/input/conflict_graph/$INSTANCE.scg
NUMBER_OF_INTERATION=100
ALPHA_TIME=0.9
ALPHA_BUDGET=0.05
ALPHA_SECURITY=0.05
ALPHA_RESTRICT_CANDIDATE_LIST=0.3
NUMBER_OF_ALLOCATION_EXPERIMENTS=4
LOG_DIR=/home/wsl/git/Wf-Security/heuristic_and_exact_solutions/log/manual/grch/$ALPHA_TIME_$ALPHA_BUDGET_$ALPHA_SECURITY/1_1_4_2_1_4_homogeneous_cloud/0/$INSTANCE/

mkdir -p $LOG_DIR

valgrind --leak-check=full $PROGRAM --tasks_and_files $TASK_AND_FILES --cluster $CLUSTER --conflict_graph \
    $CONFLICT_GRAPH --algorithm $ALGORITHM --alpha_time $ALPHA_TIME --alpha_budget $ALPHA_BUDGET --alpha_security \
    $ALPHA_SECURITY --alpha_restrict_candidate_list=$ALPHA_RESTRICT_CANDIDATE_LIST --number_of_iteration $NUMBER_OF_INTERATION --number_of_allocation_experiments \
    $NUMBER_OF_ALLOCATION_EXPERIMENTS --log_dir $LOG_DIR --logbuflevel=-1
