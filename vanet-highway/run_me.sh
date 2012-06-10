#!/bin/bash

cd ..
./waf --run 'vcrash-sim --project=./vanet-highway/Examples/SimpleTwoLaneHighway.xml --enablevehiclereceive=1 --enabledevicetrace=1'
