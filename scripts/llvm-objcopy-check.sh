#!/bin/sh
# SPDX-License-Identifier: GPL-2.0
#
# llvm-objcopy-check llvm-objcopy-command
#
# Prints y if `llvm-objcopy-command' is llvm otherwise n
#

objcopy="$*"

if [ ${#objcopy} -eq 0 ]; then
	echo "Error: No objcopy specified."
	printf "Usage:\n\t$0 <llvm-objcopy-command>\n"
	exit 1
fi

$objcopy --version | head -1 | \
	grep -q llvm && echo y || echo n
