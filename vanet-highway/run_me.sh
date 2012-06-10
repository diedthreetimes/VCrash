#!/bin/bash

cd ..
./waf --run 'vanet-highway-test --project=./vanet-highway/Examples/FourWayIntersection.xml --enablevehiclereceive=1'
