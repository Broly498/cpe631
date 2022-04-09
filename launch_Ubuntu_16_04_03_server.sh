#!/bin/sh

KAFL_DIR=dependencies/kAFL
VM_DIR=targets/linux/ubuntu_16_04_3_Server/snapshot

# Launch the Ubuntu Server VM with QEMU-PT

$KAFL_DIR/qemu-5.0.0/x86_64-softmmu/./qemu-system-x86_64 \
	-hda $VM_DIR/overlay_0.qcow2 \
	-machine q35 -serial mon:stdio -net none -enable-kvm -m 512
