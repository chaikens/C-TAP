#!/bin/bash -v
# run with -v so we see the commands

echo "This will demonstrate our 4 development cases"
###################################################
#                                                 #
#     Half Resolution .bmp extracted by ffmpeg    #
#                                                 #
###################################################
./C-TAP-jobDS1HalfDecimatedFRAME.sh

########################################################
#                                                      #
#     Half Resolution piped yuv extracted by ffmpeg    #
#  converted to .bmp by our code piped to Phase1a      #
#                                                      #
########################################################
./C-TAP-jobDS1HalfDecimatedPIPE.sh

###################################################
#                                                 #
#     FULL Resolution .bmp extracted by ffmpeg    #
#         Phase1b fails                           #
###################################################
./C-TAP-jobDS1FullDecimatedFRAME.sh

########################################################
#                                                      #
#     Full Resolution piped yuv extracted by ffmpeg    #
#  converted to .bmp by our code piped to Phase1a      #
#          Phase1b fails                               #
########################################################
./C-TAP-jobDS1FullDecimatedPIPE.sh

