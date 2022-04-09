#!/bin/sh

KAFL_DIR=dependencies/kAFL
WORK_DIR=work

# Create Work Directory if it does not already exist.
mkdir -p $WORK_DIR

cd $KAFL_DIR

# Launch the Ubuntu Server VM with QEMU-PT
python3 kAFL-Fuzzer/kafl_info.py \
    -vm_dir ../../targets/linux/ubuntu_16_04_3_Server/snapshot/ \
	-work_dir ../../$WORK_DIR \
	-agent targets/linux_x86_64/bin/info/info \
	-mem 512 \
	-v --debug
