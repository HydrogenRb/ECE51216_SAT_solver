#!/usr/bin/env bash

unset PYTHONHOME
unset PYTHONPATH

export LANG=en_US.UTF-8
export LC_ALL=en_US.UTF-8

exec /opt/rh/gcc-toolset-11/root/bin/gdb "$@"