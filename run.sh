#!/bin/bash


echo "cd os && make " $1 | singularity shell xv6-tools_buildx-latest.sif
