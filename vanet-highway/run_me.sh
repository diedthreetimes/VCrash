#!/bin/bash

cd ..
./waf --run 'vanet-highway-test --project=./vanet-highway/Examples/SimpleTwoLaneHighway.xml --enablevehiclereceive=1'
