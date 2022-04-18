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
	-agent ../../bin/futexTarget.exe \
	-mem 512 \
	-work_dir ../../$WORK_DIR \
	-funky \
	-D \
	-ip0 0xffffffff81100000-0xffffffff81103370 -v --purge -vv --debug
