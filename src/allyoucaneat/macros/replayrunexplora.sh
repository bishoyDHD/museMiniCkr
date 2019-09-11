#!/bin/bash
cd ~monitor-olymp/bin/ ; ./file-monitor `ls {/data/tests,/data/fall2012}/run_$1_*.zebra.gz`
