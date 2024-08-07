#!/bin/bash



max=10
for (( i=1; i <= $max; ++i ))
do
    raaw=$(iio_attr -u ip:10.76.84.107 -c xadc temp0 raw) 
    scalee=$(iio_attr -u ip:10.76.84.107 -c xadc temp0 scale)
    offsettt=$(iio_attr -u ip:10.76.84.107 -c xadc temp0 offset)

    temp=$(echo "($raaw + $offsettt) * ($scalee / 1000)" | bc -l)
    echo $temp
    sleep 1
done
