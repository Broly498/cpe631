#!/bin/sh

KAFL_DIR=dependencies/kAFL
WORK_DIR=work
SEED_DIR=seeds

# Create Work Directory if it does not already exist.
mkdir -p $WORK_DIR

# Create Seed Directory if it does not already exist.
mkdir -p $SEED_DIR

cd $KAFL_DIR

# Launch the Ubuntu Server Fuzzing Job with QEMU-PT
python3 kAFL-Fuzzer/kafl_fuzz.py \
    -vm_dir ../../targets/linux/ubuntu_16_04_3_Server/snapshot/ \
	-seed_dir ../../$SEED_DIR/ \
	-agent targets/linux_x86_64/bin/fuzzer/kafl_vuln_test \
	-mem 512 \
	-work_dir ../../$WORK_DIR \
	-funky \
	-ip0 0xffffffffc0246000-0xffffffffc024a000 -v --purge -vv --debug
