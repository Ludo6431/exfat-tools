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
		le32_t id;
	};
} atom_header_t;

#define ATOM_ID_ftyp (0x70797466)
typedef struct{
	char major_brand[4];
	uint8_t minor_version[4];
	char compatible_brands[][4];
} atom_ftyp;
#define ATOM_ID_moov (0x766f6f6d)
#define ATOM_ID_mdat (0x7461646d)
#define ATOM_ID_mvhd (0x6468766d)
#define ATOM_ID_udta (0x61746475)
#define ATOM_ID_trak (0x6b617274)
#define ATOM_ID_free (0x65657266)
#define ATOM_ID_tkhd (0x64686b74)
#define ATOM_ID_edts (0x73746465)
#define ATOM_ID_mdia (0x6169646d)
#define ATOM_ID_mdhd (0x6468646d)
#define ATOM_ID_hdlr (0x726c6468)
#define ATOM_ID_minf (0x666e696d)
#define ATOM_ID_vmhd (0x64686d76)
#define ATOM_ID_dinf (0x666e6964)
#define ATOM_ID_stbl (0x6c627473)
#define ATOM_ID_smhd (0x64686d73)

static int read_atom_level(int fdin, off_t o, size_t sz, int level);

static int handle_atom(int fdin, off_t o, int level, atom_header_t *ah){
#define PREFIX() do{ int i; for(i = 0; i < 4*level; i++) putchar(' '); }while(0)

	uint32_t sz = be32_to_cpu(ah->sz);
	ssize_t ret;
	char *buffer = NULL;
	int i;
	int errstat = 0;

	PREFIX(); printf("%c%c%c%c (0x%08x) (off:%#lx ; sz:%u)\n", ah->nm[0], ah->nm[1], ah->nm[2], ah->nm[3], le32_to_cpu(ah->id), o, sz);

	switch(le32_to_cpu(ah->id)){
	case ATOM_ID_ftyp:{
		uint32_t a_sz = sz - sizeof(*ah);
		buffer = (char *)realloc(buffer, a_sz);
		if(!buffer){
			fprintf(stderr, "couldn't realloc data (sz: %u)\n", a_sz);
			errstat = 1;
			break;
		}
		ret = pread(fdin, buffer, a_sz, o + sizeof(*ah));
		if(ret < a_sz){
			fprintf(stderr, "couldn't pread (err: %li)\n", ret);
			errstat = 1;
			break;
		}
		atom_ftyp *a = (atom_ftyp *)buffer;
		int nb_brands = (a_sz - 8) / 4;

		PREFIX(); printf("  Major brand   : %c%c%c%c\n", a->major_brand[0], a->major_brand[1], a->major_brand[2], a->major_brand[3]);
		PREFIX(); printf("  Minor version : %hhu%hhu%hhu%hhu\n", a->minor_version[0], a->minor_version[1], a->minor_version[2], a->minor_version[3]);
		for(i = 0; i < nb_brands; i++){
			PREFIX(); printf("  Compat brand#%i: %c%c%c%c\n", i, a->compatible_brands[i][0], a->compatible_brands[i][1], a->compatible_brands[i][2], a->compatible_brands[i][3]);
		}
		break;
	}
	case ATOM_ID_moov:
	case ATOM_ID_udta:
	case ATOM_ID_trak:
	case ATOM_ID_mdia:
	case ATOM_ID_minf:
	case ATOM_ID_dinf:
	case ATOM_ID_stbl:
	case ATOM_ID_edts:
		read_atom_level(fdin, o + sizeof(*ah), sz - sizeof(*ah), level + 1);
		break;
	default:
		// TODO
		break;
	}

	if(buffer){
		free(buffer);
	}

	return errstat;
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
