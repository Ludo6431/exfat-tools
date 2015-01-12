/*
 * exfat-action-hashes.c
 *
 *  Created on: 8 janv. 2015
 *      Author: ludo6431
 */

#include "files-action-hashes.h"

#include <mhash.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

// build hashes of clusters
void action_hashes(FILE *fin, size_t csize, char *foutn) {
	MHASH td; // see http://linux.die.net/man/3/mhash
	unsigned char hash[16]; /* enough size for MD5 */
	char* buffer = (char*) malloc(csize);
	if (!buffer) {
		fprintf(stderr, "can't malloc\n");
		exit(1);
	}
	int ret;

	// init input file
	ret = fseek(fin, 0, SEEK_END);
	if (ret) {
		fprintf(stderr, "can't fseek input file for hashes\n");
		exit(1);
	}
	long int finsz = ftell(fin);
	if (finsz < 0) {
		fprintf(stderr, "can't ftell input file for hashes\n");
		exit(1);
	}
	size_t cend = finsz/csize; // + (finsz%csize > 0); do not hash the last incomplete one

	// init output file
	FILE *fout = fopen(foutn, "wb+");
	if (!fout) {
		fprintf(stderr, "can't open \"%s\" file for writing\n", foutn);
		exit(1);
	}

	// iterate through clusters
	for (size_t c = 0; c < cend; c++) {
		off_t o = c * csize;

		// read entire cluster
		if (pread(fileno(fin), buffer, csize, o) != csize) {
			fprintf(stderr, "can't read entire cluster #%lu, o:%lu\n", c, o);
			continue;
		}

		// reinit hash lib
		td = mhash_init(MHASH_MD5);
		if (td == MHASH_FAILED) {
			fprintf(stderr, "can't reset mhash md5\n");
			exit(1);
		}
		mhash(td, buffer, csize);
		mhash_deinit(td, hash);

		// append hash to output file
		fprintf(fout, "c#%lu;", c);
		for (int i = 0; i < mhash_get_block_size(MHASH_MD5); i++) {
			fprintf(fout, "%.2x", hash[i]);
		}
		fprintf(fout, "\n");
	}

	fclose(fout);
	free(buffer);
}
