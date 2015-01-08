/*
 * libexfat-tools.c
 *
 *  Created on: 8 janv. 2015
 *      Author: ludo6431
 */

#include "libexfat-tools.h"

#include <exfat.h>
#include <exfatfs.h>
#include <stdlib.h>
#include <string.h>

struct exfat *exfatx_patch(const struct exfat *ef, struct exfat_super_block *new_sb){
	struct exfat *new = (struct exfat *)malloc(sizeof(*new));
	if(!new){
		return new;
	}
	memcpy(new, ef, sizeof(*new));

	new->sb = new_sb;
	new->ro = 1;
	new->zero_cluster = malloc(CLUSTER_SIZE(*new->sb));
	if(!new->zero_cluster){
		free(new);
		return NULL;
	}
	new->root->start_cluster = le32_to_cpu(new->sb->rootdir_cluster);
	new->root->fptr_cluster = new->root->start_cluster;

	// TODO rootdir_size, cache

	return new;
}
