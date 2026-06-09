#!/bin/bash
if [ `cat /proc/sys/kernel/core_pattern`baby != corebaby ]
then
    cd /proc/sys/kernel
    if ! sudo echo core > core_pattern
       then
	   echo We just use sudo to make core dumps go into cwd, not important.
       fi
    sudo echo 0 > core_uses_pid
fi

