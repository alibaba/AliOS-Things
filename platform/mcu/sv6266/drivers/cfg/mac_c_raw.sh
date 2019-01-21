#!/bin/bash

xxd -i mac.txt  | head -n -2 | tail -n +2 > mac_raw.h
