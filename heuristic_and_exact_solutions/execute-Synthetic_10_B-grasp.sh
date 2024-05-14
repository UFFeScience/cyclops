#!/usr/bin/env bash

ALGORITHM=grch

#INSTANCE=Synthetic_015_C
#INSTANCE=wf-montage-050d
INSTANCE=Sipht_100

PROGRAM=/home/wsl/git/Wf-Security/heuristic_and_exact_solutions/bin/wf_security_greedy.x
TASK_AND_FILES=/home/wsl/git/Wf-Security/heuristic_and_exact_solutions/input/tasks_and_files/$INSTANCE.dag
CLUSTER=/home/wsl/git/Wf-Security/heuristic_and_exact_solutions/temp/clouds/4_1_4_64_2_4_heterogeneous_cloud.vcl
CONFLICT_GRAPH=/home/wsl/git/Wf-Security/heuristic_and_exact_solutions/input/conflict_graph/$INSTANCE.scg
NUMBER_OF_INTERATION=20
ALPHA_TIME=0.9
ALPHA_BUDGET=0.05
ALPHA_SECURITY=0.05
ALPHA_RESTRICT_CANDIDATE_LIST=0.7
NUMBER_OF_ALLOCATION_EXPERIMENTS=4
LOG_DIR=/home/wsl/git/Wf-Security/heuristic_and_exact_solutions/log/manual/$ALGORITHM/"$ALPHA_TIME"_"$ALPHA_BUDGET"_"$ALPHA_SECURITY"_"$ALPHA_RESTRICT_CANDIDATE_LIST"/1_1_4_2_1_4_homogeneous_cloud/0/$INSTANCE/

mkdir -p $LOG_DIR
# --main-stacksize=16777216000
time valgrind --main-stacksize=16777216000 --leak-check=full $PROGRAM --tasks_and_files $TASK_AND_FILES --cluster $CLUSTER --conflict_graph \
    $CONFLICT_GRAPH --algorithm $ALGORITHM --alpha_time $ALPHA_TIME --alpha_budget $ALPHA_BUDGET --alpha_security \
    $ALPHA_SECURITY --alpha_restrict_candidate_list=$ALPHA_RESTRICT_CANDIDATE_LIST --number_of_iteration $NUMBER_OF_INTERATION --number_of_allocation_experiments \
    $NUMBER_OF_ALLOCATION_EXPERIMENTS --log_dir $LOG_DIR --logbuflevel=-1


#time /usr/local/bin/perf record -g $PROGRAM --tasks_and_files $TASK_AND_FILES --cluster $CLUSTER --conflict_graph \
#    $CONFLICT_GRAPH --algorithm $ALGORITHM --alpha_time $ALPHA_TIME --alpha_budget $ALPHA_BUDGET --alpha_security \
#    $ALPHA_SECURITY --alpha_restrict_candidate_list=$ALPHA_RESTRICT_CANDIDATE_LIST --number_of_iteration $NUMBER_OF_INTERATION --number_of_allocation_experiments \
#    $NUMBER_OF_ALLOCATION_EXPERIMENTS --log_dir $LOG_DIR --logbuflevel=-1

#time /usr/local/bin/perf record -g  /home/wsl/git/Wf-Security/heuristic_and_exact_solutions/bin/wf_security_greedy.x --tasks_and_files=/home/wsl/git/Wf-Security/heuristic_and_exact_solutions/input/tasks_and_files/Sipht_100.dag --cluster=/home/wsl/git/Wf-Security/heuristic_and_exact_solutions/temp/clouds/1_1_4_16_1_4_homogeneous_cloud.vcl --conflict_graph=/home/wsl/git/Wf-Security/heuristic_and_exact_solutions/input/conflict_graph/Sipht_100.scg --algorithm=grch --alpha_time=0.9 --alpha_budget=0.05 --alpha_security=0.05 --alpha_restrict_candidate_list=0.3 --number_of_iteration=20 --number_of_allocation_experiments=4 --log_dir=/home/wsl/git/Wf-Security/heuristic_and_exact_solutions/log/20240319110826/grch/0.9_0.05_0.05_0.3/1_1_4_16_1_4_homogeneous_cloud/0/Sipht_100/ --logbuflevel=-1

#/home/wsl/git/Wf-Security/heuristic_and_exact_solutions/bin/wf_security_greedy.x --tasks_and_files /home/wsl/git/Wf-Security/heuristic_and_exact_solutions/input/tasks_and_files/wf-montage-050d.dag --cluster /home/wsl/git/Wf-Security/heuristic_and_exact_solutions/temp/clouds/4_1_4_64_2_4_heterogeneous_cloud.vcl --conflict_graph /home/wsl/git/Wf-Security/heuristic_and_exact_solutions/input/conflict_graph/wf-montage-050d.scg --algorithm grch --alpha_time 0.9 --alpha_budget 0.05 --alpha_security 0.05 --alpha_restrict_candidate_list=0.7 --number_of_iteration 20 --number_of_allocation_experiments 4 --log_dir /home/wsl/git/Wf-Security/heuristic_and_exact_solutions/log/manual/grch/0.9_0.05_0.05_0.7/1_1_4_2_1_4_homogeneous_cloud/0/wf-montage-050d/ --logbuflevel=-1