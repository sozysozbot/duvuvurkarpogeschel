#!/bin/sh -eu

cp kernel/lang/asciilip.txt kernel/lang/hankaku.txt
if [ "${ALPHABET-}" = "latin" ]; then
  echo "Using Latin Alphabet";
  cp kernel/lang/asciilat.txt kernel/lang/hankaku.txt
fi

make ${MAKE_OPTS:-} -C kernel kernel.elf

for MK in $(ls apps/*/Makefile)
do
  APP_DIR=$(dirname $MK)
  APP=$(basename $APP_DIR)
  make ${MAKE_OPTS:-} -C $APP_DIR $APP
done

if [ "${1:-}" = "run" ]
then
  MIKANOS_DIR=$PWD $HOME/osbook/devenv/run_mikanos.sh
fi
