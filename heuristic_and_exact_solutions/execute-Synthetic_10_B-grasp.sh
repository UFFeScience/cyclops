#!/usr/bin/env bash

ALGORITHM=grch

INSTANCE=Synthetic_5_A

PROGRAM=/home/vagrant/Wf-Security/bin/wf_security_greedy.x
TASK_AND_FILES=/home/vagrant/Wf-Security/input/tasks_and_files/$INSTANCE.dag
CLUSTER=/home/vagrant/Wf-Security/temp/clouds/1_1_4_2_1_4_homogeneous_cloud.vcl
CONFLICT_GRAPH=/home/vagrant/Wf-Security/input/conflict_graph/$INSTANCE.scg
NUMBER_OF_INTERATION=100
ALPHA_TIME=0.9
ALPHA_BUDGET=0.05
ALPHA_SECURITY=0.05
NUMBER_OF_ALLOCATION_EXPERIMENTS=4
LOG_DIR=/home/vagrant/Wf-Security/log/manual/grch/$ALPHA_TIME_$ALPHA_BUDGET_$ALPHA_SECURITY/1_1_4_2_1_4_homogeneous_cloud/0/$INSTANCE/

mkdir -p $LOG_DIR

valgrind --leak-check=full $PROGRAM --tasks_and_files $TASK_AND_FILES --cluster $CLUSTER --conflict_graph \
    $CONFLICT_GRAPH --algorithm $ALGORITHM --alpha_time $ALPHA_TIME --alpha_budget $ALPHA_BUDGET --alpha_security \
    $ALPHA_SECURITY --number_of_iteration $NUMBER_OF_INTERATION --number_of_allocation_experiments \
    $NUMBER_OF_ALLOCATION_EXPERIMENTS --log_dir $LOG_DIR --logbuflevel=-1
