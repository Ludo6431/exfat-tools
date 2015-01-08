/*
 * exfat-action-hashes.c
 *
 *  Created on: 8 janv. 2015
 *      Author: ludo6431
 */

#include "exfat-action-hashes.h"

#include <byteorder.h>
#include <exfat.h>
#include <exfatfs.h>
#include <mutils/mglobal.h>
#include <mutils/mhash.h>
#include <mutils/mtypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

// build hashes of clusters
void action_hashes(struct exfat *ef, char *foutn) {
	const cluster_t cend = le32_to_cpu(ef->sb->cluster_count);
	const size_t csize = CLUSTER_SIZE(*ef->sb);
	MHASH td; // see http://linux.die.net/man/3/mhash
	unsigned char hash[16]; /* enough size for MD5 */
	char* buffer = (char*) malloc(csize);
	if (!buffer) {
		exfat_error("can't malloc");
		exit(1);
	}

	// init output file
	FILE *fout = fopen(foutn, "wb+");
	if (!fout) {
		exfat_error("can't open \"%s\" file for writing", foutn);
		exit(1);
	}

	// iterate through clusters
	for (cluster_t c = EXFAT_FIRST_DATA_CLUSTER; c < cend; c++) {
		off_t o = exfat_c2o(ef, c);

		// read entire cluster
		if (exfat_pread(ef->dev, buffer, csize, o) != csize) {
			exfat_warn("can't read entire cluster #%u, o:%lu", c, o);
			continue;
		}

		// reinit hash lib
		td = mhash_init(MHASH_MD5);
		if (td == MHASH_FAILED) {
			exfat_error("can't reset mhash md5");
			exit(1);
		}
		mhash(td, buffer, csize);
		mhash_deinit(td, hash);

		// append hash to output file
		fprintf(fout, "c#%u;", c);
		for (int i = 0; i < mhash_get_block_size(MHASH_MD5); i++) {
			fprintf(fout, "%.2x", hash[i]);
		}
		fprintf(fout, "\n");
	}

	fclose(fout);
	free(buffer);
}
