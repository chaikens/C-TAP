#!/usr/bin/bash

if ! make -C ../.. Phase1bPipeOpt
then
    echo Make failed
    exit 1
fi

rm DSH.verbose*

../../Phase1bPipeOpt  DroneShort1HalfDecimated.int.1  443 0.5 \
		      --verbose-file DSH.verbose \
		      --CamSett-file ./testCamSett.txt \
		      > ./DroneShort1HalfDecimated.out \
		      2>> ./DroneShort1HalfDecimated.log

echo Ran Phase1bPipeOpt ret code $?
echo .out file regression test:
echo Im not sure run.5 is the right one to compare with, check CamSett and EZ.
echo diff DroneShort1HalfDecimated.out DroneShort1HalfDecimated.out.5
diff DroneShort1HalfDecimated.out DroneShort1HalfDecimated.out.5
ret=$!
if ! $ret
then
    echo Regression Test failed
    exit 1
fi

for f in DSH.verbose*
do
    echo
    echo $f contains
    echo
    fold -s $f
    echo
    echo
done
