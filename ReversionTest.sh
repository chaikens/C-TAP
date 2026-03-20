#!/usr/bin/bash

./C-TAP.sh

echo "Reversion test: C-TAP.sh finished. Diffing results:"


NAME="DroneShort1"

if diff OLDRESULTS/$NAME.int RESULTS/$NAME.int > RESULTS/$NAME.int.diff
then
    echo good for $NAME.int
    rm RESULTS/$NAME.int.diff
else
    echo "DIFFERENCE! OLDRESULTS/$NAME.int RESULTS/$NAME.int" RESULTS/$NAME.int.diff
fi

if diff OLDRESULTS/$NAME.out RESULTS/$NAME.out > RESULTS/$NAME.out.diff
then
    echo good for $NAME.out
    rm RESULTS/$NAME.out.diff
else
    echo "DIFFERENCE! OLDRESULTS/$NAME.out RESULTS/$NAME.out"
fi

if diff OLDRESULTS/$NAME.MOV RESULTS/$NAME.MOV > /dev/null
then
    echo good for $NAME.MOV
else
    echo "DIFFERENCE! OLDRESULTS/$NAME.MOV RESULTS/$NAME.MOV"
fi


