#!/bin/bash

cd ..
./waf --run 'vcrash-sim --project=./vanet-highway/Examples/SimpleTwoLaneHighway.xml --enablevehiclereceive=1 --enabledevicetrace=1' --visualize

#./waf --run 'vcrash-sim' --command-template="gdb --args %s '--project=./vanet-highway/Examples/SimpleTwoLaneHighway.xml --enablevehiclereceive=1 --enabledevicetrace=1'"

#./waf --run 'vcrash-sim --project=./vanet-highway/Examples/SimpleTwoLaneHighway.xml --enablevehiclereceive=1 --enabledevicetrace=1'

