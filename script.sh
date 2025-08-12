#!/bin/bash

make clean

make

./out/raytracing > out/image.ppm

# open image.ppm in Preview
open out/image.ppm