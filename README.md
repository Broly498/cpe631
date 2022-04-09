# cpe631
This repository contains the source code for the final project of CPE-631 (Advanced Computer Architecture).
The purpose of the project is to demonstrate the kernel-fuzzing capabilities of several tools.
In particular, the project utilizes QEMU/KVM and Intel PT to fuzz OS kernels.

## System Requirements:
 - Ubuntu 18.04.6 LTS (Bionic Beaver)
 - 5th generation Intel x86 Processor or later (Broadwell Architecture)

## What is Kernel Fuzzing?
 - Fuzzing is an automated testing process whereby random input is provided to a software application.
 - Program performance is continuously monitored for abnormal behavior such as crashes, stalls, exceptions, memory leaks, memory corruption, etc.
 - Abnormal program behavior and the offending inputs are recorded in order to aid software testers when debugging the abnormal software behavior.
 - Fuzz testing is implemented in the software-development pipelines of major companies such as: Google, Facebook, Amazon, and many more.
 - Fuzzing allows developers to find bugs and address them prior to deployment.
 - Fuzzing can also be used by open-source communities to find and report previously unknown bugs to the maintainers of the codebases.
 - Fuzz testing can also be used by mallicious actors to find unknown bugs and leverage them to develop exploits.
 - Kernel fuzzing focuses on open-source and closed-source OS kernels.
 - Since monitoring and collecting information about kernel performance during execution of software requires that the host system not fail due to an inproper input, it can be seen that implementing a fuzz-testing suite is far more complex than stand-alone user-space fuzz testing.

## What is QEMU/KVM?
 - QEMU is a machine emulator that is used to run guest OS systems on a machine.
 - KVM is a type 1 hypervisor that contains physical CPU virtualization extensions.
 - QEMU/KVM is used in order to emulate different OS systems on host machines with QEMU while maintaining efficiency through the use of KVM hardware extensions.

## What is Intel PT?
 - Intel Processor Trace (Intel PT) is an extension of the Intel Architecture that collects information about software execution such as control flow, execution modes/timings and formats them into highly compressed binary packets.
 - Intel PT can be used by kernel fuzzers to continuously monitor the instruction flow of guest OS systems.
 - Kernel fuzzers such as kAFL monitor incoming instruction packets corresponding to virtualized targets and use these results to record unique crashes and system states prior to abnormal kernel behavior.

## How to Build the Project
 - This project utilizes several Git submodules which must first be initialized prior to building any of the tools.
 
 1.  Run "./initializeSubmodules.sh" - This will initialize the appropriate submodules and checkout the appropriate branches.
 2.  Run "./installDependencies.sh" - This will build all of the submodule dependencies and install the appropriate packages on the system (Note, this will install a custome Linux Kernel named kAFL+).

## How to Build the Sample Ubuntu Server 16.0.4.3 LTS Fuzzer Harness (KAFL)
 1.  Run "./install_Ubuntu_16_04_3_server.sh" - This will download the sample Ubuntu Server 16.04.3 LTS image, create the virtual disk images, and launch the interactive installation process.
 2.  INSIDE QEMU TERMINAL: Once the installation process has completed, the GRUB bootloader should appear in the QEMU terminal. Select the installed image and login.
 3.  INSIDE QEMU TERMINAL: Run "sudo apt install build-essential" - This will install the appropriate build tools inside of the VM.
 4.  INSIDE QEMU TERMINAL: Run "git clone https://github.com/IntelLabs/kAFL.git && git checkout legacy" - This will clone the KAFL repository inside of the VM and checkout the appropriate branch.
 5.  INSIDE QEMU TERMINAL: Run "cd kAFL/targets/linux_x86_64 && bash compile.sh" - This will compile the Loader agent inside of the VM (The loader agent performs KAFL handshakes between the guest and the host).
 6.  INSIDE QEMU TERMINAL: Run "sudo shutdown" - This will shutdown the VM.
 7.  Once the VM has shutdown and the QEMU terminal is no longer active, run "./launch_Ubuntu_16_04_03_server.sh" - This will launch the VM using QEMU-PT using a snapshot disk overlay.
 8.  INSIDE QEMU-PT TERMINAL: Select the installed image from the GRUB bootloader and login.
 9.  INSIDE QEMU-PT TERMINAL: Run "cd kAFL/tests/test_cases/simple/linux_x86-64 && chmod u+x load.sh && sudo ./load.sh" - This will install sample drivers which contain vulnerabilities.
 10. INSIDE QEMU-PT TERMINAL: Run "cd ~/kAFL && ./install.sh targets" - This will compile the loader agent inside of the VM (NOTE: Errors may appear on the terminal, but that is normal).
 11. INSIDE QEMU-PT TERMINAL: Run "sudo targets/linux_x86_64/bin/loader/loader" - This will create a QEMU-PT snapshot of the VM state which will be automatically loaded during the beginning of the kernel fuzzing process.
 12. Run "./retreiveUbuntu_16_04_3_server_AddressRanges.sh" - This will print out the address ranges of the vulnerable drivers that were installed inside of the VM. The address ranges are used by KAFL to monitor for potential anomolous behavior.
 13. Record the start and end address of one of the vulnerable drivers.
 14. Run "./fuzz_Ubuntu_16_04_3_server.sh" - This will fuzz the vulnerable driver that was installed inside of the VM. It should be noted that the address ranges in this script may not match the ones that were printed in step 13. This is due to the fact that the VM snapshot image is different from the one that was created for this tutorial; therefore, the address ranges should be updated if they do not match.

## How to interpret results
