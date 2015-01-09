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

#define ATOM_ID_ftyp (0x70797466)
#define ATOM_ID_moov (0x766f6f6d)
#define ATOM_ID_mdat (0x7461646d)

static int read_atom_level(int fdin, off_t o, size_t sz, int level);

static int handle_atom(int fdin, off_t o, int level, atom_header_t *ah){
	uint32_t sz = be32_to_cpu(ah->sz);
	int i;

	for(i = 0; i < 4*level; i++) putchar(' ');
	printf("ATOM: %c%c%c%c (0x%08x) (sz:%u)\n", ah->nm[0], ah->nm[1], ah->nm[2], ah->nm[3], ah->id, sz);

	switch(ah->id){
	case ATOM_ID_moov:
		read_atom_level(fdin, o + sizeof(*ah), sz - sizeof(*ah), level + 1);
		break;
	default:
		// TODO
		break;
	}

	return 0;
}

static int read_atom_level(int fdin, off_t o, size_t sz, int level){
	atom_header_t ah;
	ssize_t ret;
	off_t o0 = o;

	while(o - o0 + sizeof(ah) <= sz){
		ret = pread(fdin, &ah, sizeof(ah), o);
		if(ret == 0){
			break;
		}
		else if(ret < sizeof(ah)){
			fprintf(stderr, "couldn't pread (err: %li)\n", ret);
			return 1;
		}

		if(handle_atom(fdin, o, level, &ah)){
			return 1;
		}

		o += be32_to_cpu(ah.sz);
	}

	return 0;
}

// discovers mp4 chunks
void action_mp4chunks(FILE *fin) {
	int ret;
	size_t fdinsz;

	if(fseek(fin, 0, SEEK_END)){
		fprintf(stderr, "can't fseek input file for mp4chunks\n");
		exit(1);
	}
	fdinsz = ftell(fin);

	// try to walk the file...
	if((ret=read_atom_level(fileno(fin), 0, fdinsz, 0))){
		fprintf(stderr, "read_atom returned: %i\n", ret);
	}
}
