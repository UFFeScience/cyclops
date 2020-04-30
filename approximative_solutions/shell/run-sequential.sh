#!/bin/bash

if [ -z "$1" ] ; then
  file=./input/danoint.mps.g.g
else
  file=$1
fi
if [ -z "$2" ] ; then
  maxiter=100
else
  maxiter=$2
fi
if [ -z "$3" ] ; then
  maxtime=30000
else
  maxtime=$3
fi
if [ -z "$4" ] ; then
  target=96
else
  target=$4
fi

prog=./bin/dmer_Sequential

$prog $file $maxiter $maxtime $target
