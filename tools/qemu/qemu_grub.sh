#!/bin/bash

# Create floppy disk using instructions at,
# http://wiki.osdev.org/Loopback_Device

# make ganoid
pushd ../../
make
popd

# mount qemu grub floppy disk and copy kernel
mkdir -p ./tmp
sudo mount qemu_kernel_grub.img -o loop ./tmp
sudo /bin/cp -f ../../ganoid-0.0.1 ./tmp/kernel.bin
sudo umount ./tmp
rm -fr ./tmp

# run qemu
qemu -fda qemu_kernel_grub.img -monitor stdio
