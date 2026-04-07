#!/bin/bash
cd /proc/sys/kernel
echo core > core_pattern
echo 0 > core_uses_pid
