#!/bin/bash
    cd /proc/sys/kernel
    if ! sudo echo core > core_pattern
       then
	   echo We just use sudo to make core dumps go into cwd, not important.
	   exit 0
       fi
    sudo echo 0 > core_uses_pid


