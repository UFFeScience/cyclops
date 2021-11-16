#!/bin/bash

if [ -z "$1" ] ; then
  TASK_AND_FILES=Sipht_100.xml.dag
else
  TASK_AND_FILES=$1
fi

if [ -z "$2" ] ; then
  CLUSTER=cluster2.vcl
else
  CLUSTER=$2
fi

if [ -z "$3" ] ; then
  CONFLICT_GRAPH=Sipht_100.xml.scg
else
  CONFLICT_GRAPH=$3
fi

if [ -z "$4" ] ; then
  ALPHA_TIME=0.3
else
  ALPHA_TIME=$4
fi

if [ -z "$5" ] ; then
  ALPHA_COST=0.3
else
  ALPHA_COST=$5
fi

if [ -z "$6" ] ; then
  ALPHA_SECURITY=0.4
else
  ALPHA_SECURITY=$6
fi

if [ -z "$7" ] ; then
  ITERATION_NUMBER=100
else
  ITERATION_NUMBER=$7
fi

if [ -z "$8" ] ; then
  ALLOCATION_EXPERIMENTS=1
else
  ALLOCATION_EXPERIMENTS=$8
fi

cd ..

PROG=./bin/wf_security_greedy.x

export GLOG_log_dir=`pwd`/log

$PROG --tasks_and_files `pwd`/input/tasks_and_files/$TASK_AND_FILES \
  --cluster `pwd`/input/clouds/$CLUSTER \
  --conflict_graph `pwd`/input/conflict_graph/$CONFLICT_GRAPH \
  --algorithm greedy_randomized_constructive_heuristic \
  --alpha_time $ALPHA_TIME \
  --alpha_budget $ALPHA_COST \
  --alpha_security $ALPHA_SECURITY \
  --number_of_iteration $ITERATION_NUMBER \
  --number_of_allocation_experiments $ALLOCATION_EXPERIMENTS
cd shell

# cd ..
# #echo  GLOG_log_dir=`pwd`/log
# export GLOG_log_dir=`pwd`/log
# #export OMP_NUM_THREADS=4
# ./bin/wf_security_greedy.x --tasks_and_files `pwd`/input/tasks_and_files/Sipht_100.xml.dag \
#   --cluster `pwd`/input/clouds/cluster2.vcl \
#   --conflict_graph `pwd`/input/conflict_graph/Sipht_100.xml.scg \
#   --algorithm greedy_randomized_constructive_heuristic \
#   --alpha_time 0.3 \
#   --alpha_budget 0.3 \
#   --alpha_security 0.4 \
#   --number_of_iteration 100 \
#   --number_of_allocation_experiments 1
# cd shell
