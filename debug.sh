#!/bin/bash
set -e
make
gdb --args ./memfs -d -s -o default_permissions -o auto_unmount testmount
