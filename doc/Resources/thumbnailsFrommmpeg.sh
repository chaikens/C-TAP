#from V. SubhashQuick Start Guide to FFmpeg (O'Reilly ebook)

#!/bin/bash
# BASH script to create a 3x3 thumbnail gallery for a video
# Accepts the pathname of the video as argument ($1)
###########################################################
# Floating point number functions by Mitch Frazier
# Adapted from
# https://www.linuxjournal.com/content/floating-point-math-bash
###########################################################
# Default scale used by float functions
float_scale=2
# Evaluate a floating point number expression.
function float_eval() {
  local stat=0
  local result=0.0
  if [[ $# -gt 0 ]]; then
    result=$(echo "scale=$float_scale; $*" | bc -q 2>/dev/null)
    stat=$?
    if [[ $stat -eq 0  &&  -z "$result" ]]; then
      stat=1
    fi
  fi
  echo $result
  return $stat
}
# Evaluate a floating point number conditional expression.
function float_cond() {
  local cond=0
  if [[ $# -gt 0 ]]; then
    cond=$(echo "$*" | bc -q 2>/dev/null)
    if [[ -z "$cond" ]]; then
      cond=0
    fi
    if [[ "$cond" != 0  &&  "$cond" != 1 ]]; then
      cond=0
    fi
  fi
  local stat=$((cond == 0))
  return $stat
}
###########################################################
# Floating point number functions end
# Prefix for images
FILE_NAME=${1%.*}
#echo $FILE_NAME
NUMBER_OF_THUMBNAILS=9
MOVIE="$1"
COUNTER=0
# Number of seconds
MOVIE_DURATION=$(ffprobe \
                   -show_entries "format=duration" \
                   -of "default=nokey=1:noprint_wrappers=1" \
                   -i $MOVIE 2> /dev/null)
#echo $MOVIE_DURATION
MOVIE_WIDTH=$(ffprobe -select_streams v:0 \
              -show_entries "stream=width:" \
              -print_format \
                "default=nokey=1:noprint_wrappers=1" \
              -i $MOVIE 2> /dev/null)
MOVIE_HEIGHT=$(ffprobe -select_streams v:0 \
              -show_entries "stream=height" \
              -print_format \
                "default=nokey=1:noprint_wrappers=1" \
              -i $MOVIE 2> /dev/null)
#echo "$MOVIE_WIDTH x $MOVIE_HEIGHT"
TW=$(float_eval "$MOVIE_WIDTH/3")
TH=$(float_eval "$MOVIE_HEIGHT/3")
THUMB_WIDTH=${TW%.*}
THUMB_HEIGHT=${TH%.*}
#echo "$THUMB_WIDTH x $THUMB_HEIGHT"
for i in $(seq $NUMBER_OF_THUMBNAILS)
do
  let COUNTER=COUNTER+1
  #echo $COUNTER
  LOCATION_FLOAT=$(float_eval \
            "($i-0.5)*$MOVIE_DURATION/$NUMBER_OF_THUMBNAILS")
  #echo $LOCATION_FLOAT
  LOCATION_INT=${LOCATION_FLOAT%.*}
  #echo $LOCATION_INT
  # Create the thumbnails
  ffmpeg -y -skip_frame nokey -ss $LOCATION_INT -i $MOVIE \
         -frames:v 1 \
         -s ${THUMB_WIDTH}x${THUMB_HEIGHT} \
         "${FILE_NAME}_${COUNTER}.jpg"
done
# Create the gallery
montage -density 96 -tile 3x3 -geometry +4+4 \
        -border 1 "${FILE_NAME}*.jpg" \
        "${FILE_NAME}-thumbnail.jpg“

