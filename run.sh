#!/bin/bash
if [[ "$1" = "gdb" ]]; then
    qemu-system-x86_64 -s -S -hda  Image/x64BareBonesImage.qcow2 -m 512 -d int
else
    qemu-system-x86_64 -soundhw pcspk -hda Image/x64BareBonesImage.qcow2 -m 512 
fi
