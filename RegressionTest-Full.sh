#!/usr/bin/bash

NAME=DroneShort1Full

OLD_DIR=RESULTS-GOOD-OLD

C_TAPpgm=./C-TAP-${NAME}.sh

eval $C_TAPpgm

echo "Reversion test: ${C_TAPpgm} finished. Diffing results:"


if diff $OLD_DIR/$NAME.int RESULTS/$NAME.int > RESULTS/$NAME.int.diff
then
    echo good for $NAME.int
    rm RESULTS/$NAME.int.diff
else
    echo "DIFFERENCE! OLDRESULTS/$NAME.int RESULTS/$NAME.int"
    echo "see RESULTS/$NAME.int.diff"
fi

if diff $OLD_DIR/$NAME.out RESULTS/$NAME.out > RESULTS/$NAME.out.diff
then
    echo good for $NAME.out
    rm RESULTS/$NAME.out.diff
else
    echo "DIFFERENCE! OLDRESULTS/$NAME.out RESULTS/$NAME.out"
    echo "see RESULTS/$NAME.out.diff"
fi

if [ -r $OLD_DIR/$NAME.MOV ]
then
    if diff $OLD_DIR/$NAME.MOV RESULTS/$NAME.MOV > /dev/null
       then
	   echo good for $NAME.MOV
    else
	echo "DIFFERENCE! OLDRESULTS/$NAME.MOV RESULTS/$NAME.MOV"
	echo "different movie files."
    fi
else
    #for our friends who have started from the repository
    echo Movies "(here $OLD_DIR/$NAME.MOV)" are not archived because of their size.
fi



