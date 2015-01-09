/*
 * exfat-action-hashes.c
 *
 *  Created on: 8 janv. 2015
 *      Author: ludo6431
 */

#include "files-action-mp4chunks.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "byteorder.h"

typedef struct{
	be32_t sz;
	union{
		char nm[4];
		uint32_t id;
	};
} atom_header_t;

static int handle_atom(int fdin, off_t o, atom_header_t *ah){
	uint32_t sz = be32_to_cpu(ah->sz);

	printf("ATOM: %c%c%c%c (%08x) (sz:%u)\n", ah->nm[0], ah->nm[1], ah->nm[2], ah->nm[3], ah->id, sz);

	switch(ah->id){
	default:
		// TODO
		break;
	}

	return 0;
}

static int read_atom_level(int fdin, off_t o){
	atom_header_t ah;
	ssize_t ret;

	do{
		ret = pread(fdin, &ah, sizeof(ah), o);
		if(ret == 0){
			break;
		}
		else if(ret < sizeof(ah)){
			return 1;
		}

		if(handle_atom(fdin, o, &ah)){
			return 1;
		}

		o += sizeof(ah) + be32_to_cpu(ah.sz);
	}while(1);

	return 0;
}

// discovers mp4 chunks
void action_mp4chunks(FILE *fin) {
	int ret;

	// try to walk the file...
	if((ret=read_atom_level(fileno(fin), 0))){
		fprintf(stderr, "read_atom returned: %i\n", ret);
	}
}
