#!/bin/bash
set -e

dir=${1:?"Usage: $0 <mountpoint>"}

if [ ! -d "$dir" ]; then
  mkdir -p "$dir"
fi

make
gdb --args ./memfs -d -s -o default_permissions -o auto_unmount "$dir"

