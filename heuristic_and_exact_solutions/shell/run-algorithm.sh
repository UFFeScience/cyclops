#!/bin/bash

if [ -z "$1" ] ; then
  TASK_AND_FILES=3_toy_5_A.dag
else
  TASK_AND_FILES=$1
fi

if [ -z "$2" ] ; then
  CLUSTER=cluster2.vcl
else
  CLUSTER=$2
fi

if [ -z "$3" ] ; then
  CONFLICT_GRAPH=3_toy_5_A.scg
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
  OUTPUT_FILE=3_toy_5_A
else
  OUTPUT_FILE=$7
fi

cd ..

PROG=./bin/wf_security_greedy.x

#export GLOG_log_dir=`pwd`/log

$PROG --tasks_and_files `pwd`/input/tasks_and_files/$TASK_AND_FILES \
  --cluster `pwd`/input/clouds/$CLUSTER \
  --conflict_graph `pwd`/input/conflict_graph/$CONFLICT_GRAPH \
  --algorithm cplex \
  --alpha_time $ALPHA_TIME \
  --alpha_budget $ALPHA_COST \
  --alpha_security $ALPHA_SECURITY \
  --cplex_output_file `pwd`/output/$OUTPUT_FILE.lp \
  --minloglevel=3
#  --log_dir=`pwd`/log
cd shell

# TODO: Make more flexible, receive the algorithm name, and create the name of the log file and directory