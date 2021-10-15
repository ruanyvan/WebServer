#!/bin/bash
pkexec /usr/bin/gdb "$@" 

# 当运行 0--1024的端口需要root权限，因为此为公共端口，调试时所以需要此脚本