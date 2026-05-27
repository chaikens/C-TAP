#!/usr/bin/bash
echo
echo "---Test 1-----------------------------------------------------"
echo
echo ./C-TAP-jobDS1FullDecimatedFRAME.sh
./C-TAP-jobDS1FullDecimatedFRAME.sh
echo
echo "---Test 2-----------------------------------------------------"
echo
echo
echo ./C-TAP-jobDS1FullDecimatedPIPE.sh
./C-TAP-jobDS1FullDecimatedPIPE.sh
echo
echo "---Test 3-----------------------------------------------------"
echo
echo
echo ./C-TAP-jobDS1HalfDecimatedFRAME.sh
./C-TAP-jobDS1HalfDecimatedFRAME.sh
echo
echo "---Test 4-----------------------------------------------------"
echo
echo
echo ./C-TAP-jobDS1HalfDecimatedPIPE.sh
./C-TAP-jobDS1HalfDecimatedPIPE.sh
echo
echo
echo


