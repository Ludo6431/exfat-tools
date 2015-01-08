//============================================================================
// Name        : exfat-recovery-tools.cpp
// Author      : Ludovic Lacoste
// Version     :
// Copyright   :
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>

#include "files-action-hashes.h"

int main(int argc, char *argv[]) {
	int ret;

	if (argc < 2) {
		fprintf(stderr, "Usage:\n\t%s <device or file>\n", argv[0]);
		exit(1);
	}

	FILE *fin = fopen(argv[1], "rb+");
	if(!fin){
		fprintf(stderr, "can't open input file \"%s\"\n", argv[1]);
		exit(1);
	}

	action_hashes(fin, 512*1024, "file-hashes-patched.txt");

	fclose(fin);

	return 0;
}
