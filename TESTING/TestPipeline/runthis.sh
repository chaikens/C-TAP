MOVIE_FILE=N884A6_ch1_main_20250218200000_20250218210000.mp4
moviebase=${MOVIE_FILE%.*}


/usr/bin/time -o ${moviebase}.time \
     ./Phase1aDoOneMovieTries.sh \
     $MOVIE_FILE &

xterm -geometry 125x30 -e tail -f ffmpeg.log &
xterm -geometry 250x30 -e tail -f $moviebase.int &

