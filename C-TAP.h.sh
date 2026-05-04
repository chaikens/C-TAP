# Do not execute me. I'm only code that another shell execution should include.
# My extension is .sh only so editors will help your with my syntax.


jobname=$(basename $0 | sed  -n 's/C-TAP-Job-\(.*\)\.sh/\1/p')

echo Your job\'s name is $jobname

mkdir ${jobname}

if [ ! $? ]
then
    echo Dir for your job .${jobname} already exists or failed to be made.
    exit 1;
fi

jobdir=${jobname}

echo -n Made ' '
ls -ld ${jobdir}



