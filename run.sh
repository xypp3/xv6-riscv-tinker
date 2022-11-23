#!/bin/bash


echo "cd os && make -j qemu && exit" | singularity shell xv6-tools_buildx-latest.sif
