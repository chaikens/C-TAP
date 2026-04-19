MOVIE_FILE=LOCALTESTMOVIE.mp4
#N884A6_ch1_main_20250218200000_20250218210000.mp4
moviebase=${MOVIE_FILE%.*}


/usr/bin/time -o ${moviebase}.time \
     -f 'real    %U\nuser  %E\nsys      %S' \
     ./Phase1aDoOneMovieHalfedDecimated.sh \
     $MOVIE_FILE &

xterm -geometry 132x30+0+0 -e tail -f ffmpeg.log &
xterm -geometry 180x30+0+180 -e tail -f $moviebase.int &

wait %/usr/bin/time

kill %2 %3
echo "To see progress logs, run"
echo "less ffmpeg.log, and/or"
echo "less ${moviebase}.int"

echo 
echo Time Report
cat ${moviebase}.time
echo 
echo In case of errors, use ./killLeftovers.sh to kill our residual processes.
echo


