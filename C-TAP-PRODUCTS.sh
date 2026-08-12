#!/usr/bin/bash

set -u

r_dir=$1 #RESULTS-<jobname> dir with run numbered files
r_basename=$(basename $r_dir)

#echo $r_basename

p_par=$2 #Where to create PRODS-<jobname> dirs with cleanly named product files
#echo $p_par

p_basename=${r_basename/RESULTS/PRODS}
#echo $p_basename
mkdir  -p ${p_par}

#Worry if we already did this
if ! mkdir  ${p_par}/$p_basename
then
    echo Somethings Wrong..cleanly renamed copying should be done only
    echo once.  If intentional, delete old ${p_par}/${p_basename} and rerun.
    exit
fi

destd=$(realpath ${p_par}/$p_basename)
#echo $destd

#echo Writing clean FLIR product dir ${destd} of $r_dir


pushd $r_dir > /dev/null
logs=$(ls *.log.0)
#echo $logs
#echo
for log in $logs
do
    m=${log/.log.0/}
#    echo
#    echo $m
#    echo 
#    ls ${m}*
#    echo
    mlogs=$(ls ${m}.log.*)
#    echo $mlogs
    logn=$( ls ${m}.log.* | sed s/${m}.log.// | sort -n | tail -n 1)
#    echo
#    echo $logn
#    echo
    lslogn=$(ls ${m}*.$logn)
    nrs=$(echo $lslogn | wc -w)
    if [ $nrs = 8 ]
    then
	echo Good log $log Will copy.
#	echo Goodie!
#	pwd
#	echo ${m}.int.$logn
	
	cp ${m}.int.$logn ${destd}/${m}.int
	cp ${m}.out.$logn ${destd}/${m}.out
	cp ${m}.MOV.$logn ${destd}/${m}.MOV
	cp ${m}.cmds.$logn ${destd}/${m}.cmds
	cp ${m}.times.$logn ${destd}/${m}.times
	cp ${m}.log.$logn ${destd}/${m}.log
    else
	echo
	echo
	echo SOMETHINGS WRONG WITH $log
	echo NO COPYING DONE
	echo CHECK and HANDLE MANUALLY
	echo
	echo
    fi
done





