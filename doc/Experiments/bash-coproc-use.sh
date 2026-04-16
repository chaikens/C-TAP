#!/bin/bash
#executing the script fails too!
#These work when entered line-by-line,
# but NOT when this file is shell sourced!
coproc MP { bash ; }
cat <& ${MP[0]}
# put that in the background
cat >& ${MP[1]}
# enter shell commands, results get printed
# if you give the cat EOF with ^d, you get back the
# shell of course but you can repeat the
# cat >& ${MP[1]}
# and continue to use the coprocess.
#
# So give exit command to stop it all.



