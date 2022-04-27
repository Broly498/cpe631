# cpe631
This repository contains the source code for the final project of CPE-631 (Advanced Computer Architecture).
The purpose of the project is to demonstrate the kernel-fuzzing capabilities of several tools.
In particular, the project utilizes QEMU/KVM and Intel PT to fuzz OS kernels.

# Contents
 - include/     - This directory contains all of the header files for the CPE-631 fuzzer harnesses.
 - src/         - This directory contains all of the source files for the CPE-631 fuzzer harness.
 - dependencies - This directory contains submoduledependencies to external tools. It contains the forked kAFL repository code once submomdules are initialized.

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
 4.  INSIDE QEMU TERMINAL: Run "git clone https://github.com/IntelLabs/kAFL.git" - This will clone the KAFL repository inside of the VM.
 5.  INSIDE QEMU TERMINAL: Run "cd kAFL && git checkout legacy && cd targets && bash compile.sh" - This will checkout the appropriate kAFL branch and compile the Loader agent inside of the VM (The loader agent performs KAFL handshakes between the guest and the host).
 6.  INSIDE QEMU TERMINAL: Using preferred editor, modify "/etc/default/grub" and replace "GRUB_CMDLINE_LINUX_DEFAULT=""" with "GRUB_CMDLINE_LINUX_DEFAULT="rw console=ttyS0 nokaslr oops=panic nopti mitigations=off spectre_v2=off""
 7.  INSIDE QEMU TERMINAL: Run "sudo update-grub" - This will commit the Grub bootloader changes.
 8.  INSIDE QEMU TERMINAL: Run "sudo shutdown" - This will shutdown the VM.
 9.  Once the VM has shutdown and the QEMU terminal is no longer active, run "./launch_Ubuntu_16_04_03_server.sh" - This will launch the VM using QEMU-PT using a snapshot disk overlay.
 10.  INSIDE QEMU-PT TERMINAL: Select the installed image from the GRUB bootloader and login.
 11.  INSIDE QEMU-PT TERMINAL: Run "cd kAFL/tests/test_cases/simple/linux_x86-64 && chmod u+x load.sh && sudo ./load.sh" - This will install sample drivers which contain vulnerabilities.
 12. INSIDE QEMU-PT TERMINAL: Run "cd ~/kAFL && ./install.sh targets" - This will compile the loader agent inside of the VM.
 13. INSIDE QEMU-PT TERMINAL: Run "sudo targets/linux_x86_64/bin/loader/loader" - This will create a QEMU-PT snapshot of the VM state which will be automatically loaded during the beginning of the kernel fuzzing process.
 14. Run "./retreiveUbuntu_16_04_3_server_AddressRanges.sh" - This will print out the address ranges of the vulnerable drivers that were installed inside of the VM. The address ranges are used by KAFL to monitor for potential anomolous behavior.
 15. Record the start and end address of one of the vulnerable drivers.
 16. Run "./fuzz_Ubuntu_16_04_3_server.sh" - This will fuzz the vulnerable driver that was installed inside of the VM. It should be noted that the address ranges in this script may not match the ones that were printed in step 13. This is due to the fact that the VM snapshot image is different from the one that was created for this tutorial; therefore, the address ranges should be updated if they do not match.

## Visualizing Results
 1. Run "./launchFuzzerGuiUtility.sh" - This will start the KAFL Fuzzer GUI. This GUI lists various metrics relating to the fuzzer job. The GUI is capable of providing results in real-time or from a previously executed fuzzer job. The script assumes that the work/ directory contains the outputs of the current/previous fuzzer job.
 2. Examine the output directory of the current/previous fuzzer job. In this case, the work/ directory should contain the outputs of the Ubuntu fuzzer job. The directory work/corpus will contain several subdirectories that contain the fuzzer inputs which caused anomolous behaviors.

## How to run the Mutex Fuzzer.
 1.  Follow steps 1-14 of the "How to Build the Sample Ubuntu Server 16.0.4.3 LTS Fuzzer Harness (KAFL)" section.
 2.  Record the start and end address of the "kernel_core". We use the kernel core address since we are directly fuzzing the Linux API using the Mutex harness.
 3. Run "./fuzzFutexTarget.sh" - This will fuzz the Linux API futex inside of the VM. It should be noted that the address ranges in this script may not match the ones that were printed in step 13. This is due to the fact that the VM snapshot image is different from the one that was created for this tutorial; therefore, the address ranges should be updated if they do not match.
