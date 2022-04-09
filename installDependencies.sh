#!/bin/sh

KAFL_DIR=dependencies/kAFL

# Install stand-alone QEMU Utilities
sudo apt install qemu-system-x86
sudo apt install qemu-utils

# Install KAFL Modules

cd $KAFL_DIR
./install.sh deps  # Check platform and install dependencies
./install.sh perms # Allow current user to control KVM (/dev/kvm)
./install.sh qemu  # Download, patch, and build QEMU
./install.sh linux # Download, patch, and build Linux

# Install KAFL+ Linux Kernel

cd linux-5.11.4
# optionally set 'MODULES=dep' in /etc/initramfs-tools/initramfs.conf
sudo make INSTALL_MOD_STRIP=1 modules_install
sudo make install

# Compile Agents on the Host
cd ../targets/linux_x86_64
bash ./compile.sh
