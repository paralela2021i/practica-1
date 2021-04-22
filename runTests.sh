#!/bin/bash
touch results.txt
for imagen in 720 1080 4k
do
echo "imagen: ${imagen}.jpg" >> results.txt
for hilos in {1,2,4,8,16}
do
echo "hilo(s): ${hilos}" >> results.txt
for pasada in {1..10}
do
./filtroPosix "${imagen}.jpg" "filtered_image_${imagen}.jpg" $hilos >> results.txt
done
done
done
