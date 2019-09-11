#!/bin/bash

echo "*******************************"
time make -k -j 6 -f src/allyoucaneat/macros/MakeNearOnlineROOTfiles $@
echo "*******************************"
