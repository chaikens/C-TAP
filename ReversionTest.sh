#!/usr/bin/bash

./C-TAP.sh

NAME="DroneShort1"

if diff OLDRESULTS/$NAME.int RESULTS/$NAME.int
then
    echo good for $NAME.int
else
    echo "DIFFERENCE! OLDRESULTS/$NAME.int RESULTS/$NAME.int"
fi

if diff OLDRESULTS/$NAME.out RESULTS/$NAME.out
then
    echo good for $NAME.out
else
    echo "DIFFERENCE! OLDRESULTS/$NAME.out RESULTS/$NAME.out"
fi

if diff OLDRESULTS/$NAME.MOV RESULTS/$NAME.MOV
then
    echo good for $NAME.MOV
else
    echo "DIFFERENCE! OLDRESULTS/$NAME.MOV RESULTS/$NAME.MOV"
fi


