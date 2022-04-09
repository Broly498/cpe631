#!/bin/sh

SCRIPT_DIRECTORY=$(dirname "$0")
VM_DIRECTORY=targets/linux/ubuntu_16_04_3_Server

# Download Ubuntu Server 16.04.3 LTS
mkdir -p $VM_DIRECTORY
wget -O $VM_DIRECTORY/ubuntu.iso https://old-releases.ubuntu.com/releases/16.04.3/ubuntu-16.04-server-amd64.iso

# Create Disk Image
qemu-img create -f qcow2 $VM_DIRECTORY/ram.qcow2 512
qemu-img create -f qcow2 $VM_DIRECTORY/linux.qcow2

# Create Snapshot Image
mkdir -p $VM_DIRECTORY/snapshot
qemu-img create -b $SCRIPT_DIRECTORY/$VM_DIRECTORY/linux.qcow2 -f qcow2 $VM_DIRECTORY/snapshot/overlay_0.qcow2
qemu-img create -b $SCRIPT_DIRECTORY/$VM_DIRECTORY/ram.qcow2 -f qcow2 $VM_DIRECTORY/snapshot/ram_0.qcow2

# Launch the Ubuntu Server ISO with QEMU.
qemu-system-x86_64 -machine q35 -cpu host -enable-kvm -m 512 -hda $VM_DIRECTORY/linux.qcow2 -cdrom $VM_DIRECTORY/ubuntu.iso -usbdevice tablet
