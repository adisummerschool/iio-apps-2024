#!/bin/bash

max=10
for (( i=1; i <= $max; ++i ))
do
    iio_attr -u ip:10.76.84.107 -c iio:device0 voltage0 raw $i
    sleep 1
done
