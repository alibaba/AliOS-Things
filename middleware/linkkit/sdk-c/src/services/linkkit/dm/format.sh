#!/bin/bash

for file in ./*.c ./*.h ./client/* ./server/*
do
	astyle --options=${HOME}/astylerc $file
done
