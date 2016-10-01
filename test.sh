#!/bin/bash
set -e
set -o xtrace

make memfs

mnt="`dirname $0`/testmount"

myname=`whoami`
myuid=`id --user`
mygid=`id --group`

function verify_chmod {
  chmod 0$1 "$2"
  perms=`stat --format="%a" "$2"`
  if [ "$1" != "$perms" ]; then
    echo "chmod(0$1, $2) actually set permissions to $perms"
    exit 1
  fi
}

function kill_ramfs {
  fusermount -u "$mnt"
}

# Create directory if necessary
if [ ! -d "$mnt" ]; then
  mkdir "$mnt"
fi

# Start memfs in a background process and give it a few seconds to initialize
./memfs -d -s -o default_permissions -o auto_unmount "$mnt" &
sleep 5

# Unmount file system on exit
trap kill_ramfs EXIT

# Verify that mounting succeeded
mountpoint "$mnt"

# Create and unlink a file
echo xyz > "$mnt/foo"
text=`cat "$mnt/foo"`
if [ "xyz" != "$text" ]; then
  echo "file content is '$text' but should be xyz"
  exit 1
fi

unlink "$mnt/foo"

# Try to create a directory
mkdir "$mnt/foo"

# Check that uid/gid have been set correctly
dir1uid=`stat --format="%u" "$mnt/foo"`
dir1gid=`stat --format="%g" "$mnt/foo"`

if [ "$myuid" != "$dir1uid" ]; then
  echo "uid is incorrect"
  exit 1
fi

if [ "$mygid" != "$dir1gid" ]; then
  echo "gid is incorrect"
  exit 1
fi

# chmod the directory
verify_chmod 740 "$mnt/foo"

# Create and unlink some files
for i in 1 2 3 4 5; do
  f="$mnt/.~\\$i"
  g="$mnt/foo/.~\\$i"
  dd if=/dev/urandom bs=1024 count=8 of="$f" status=none
  #find "$mnt"
  mv "$f" "$g"
  unlink "$g"
done

# Delete the directory
rmdir "$mnt/foo"

# Create random files
for i in 1 2 3 4 5 6 7 8 9 10; do
  dd if=/dev/urandom bs=1024 count=60 of=/tmp/random.b status=none
  cp /tmp/random.b "$mnt/random.b"
  diff /tmp/random.b "$mnt/random.b"
done
rm /tmp/random.b

# Try symlinks
echo baz > "$mnt/foo"
ln -s "foo" "$mnt/bar"

target=`readlink "$mnt/bar"`
if [ "$target" != "foo" ]; then
  echo "symlink should point to $mnt/foo, but target is $target"
  exit 1
fi

content=`cat "$mnt/bar"`
if [ "$content" != "baz" ]; then
  echo "reading $mnt/bar should return the contents of $mnt/foo, but returned $content"
  exit 1
fi

unlink "$mnt/foo"
if [ ! -L "$mnt/bar" ]; then
  echo "symlink should still exist"
  exit 1
fi

unlink "$mnt/bar"
if [ -L "$mnt/bar" ]; then
  echo "symlink should not exist anymore"
  exit 1
fi

