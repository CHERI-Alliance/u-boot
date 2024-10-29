#!/bin/sh
# SPDX-License-Identifier: GPL-2.0
#
# llvm-objcopy-version llvm-objcopy-command
#
# Prints the llvm-objcopy version of `llvm-objcopy-command' in a canonical 6-digit form
# such as `180100' for llvm 18.1.0
#

objcopy="$*"

if [ ${#objcopy} -eq 0 ]; then
	echo "Error: No objcopy specified."
	printf "Usage:\n\t$0 <llvm-objcopy-command>\n"
	exit 1
fi

version_string=$($objcopy --version | tail -n +3 | head -1 | \
	sed -e 's/(.*)//; s/[^0-9.]*\([0-9.]*\).*/\1/')

MAJOR=$(echo $version_string | cut -d . -f 1)
MINOR=$(echo $version_string | cut -d . -f 2)
MICRO=$(echo $version_string | cut -d . -f 3)

printf "%02d%02d%02d\\n" $MAJOR $MINOR $MICRO
