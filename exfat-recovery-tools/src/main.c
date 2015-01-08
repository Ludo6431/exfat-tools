//============================================================================
// Name        : exfat-recovery-tools.cpp
// Author      : Ludovic Lacoste
// Version     :
// Copyright   :
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <exfat.h>
#include <exfatfs.h>
#include <stdlib.h>
#include <string.h>

#include "exfat-action-hashes.h"
#include "libexfat-tools.h"

int main(int argc, char *argv[]) {
	exfat_debug("!!!Hello World!!!");

	struct exfat ef;
	struct exfat *efp;
	int ret;

	if (argc < 2) {
		exfat_error("Usage:\n\t%s <device or file>", argv[0]);
		exit(1);
	}

	ret = exfat_mount(&ef, argv[1], "ro");
	if (ret) {
		exfat_error("can't exfat_mount");
		exit(1);
	}

	exfat_debug("Original:");
	exfat_print_info(ef.sb, exfat_count_free_clusters(&ef));

	// patch exfat superblock for my specific usage (64GB µSD initially formatted by a GoPro, but reformatted by mistake)
	{
		struct exfat_super_block *new_sb = (struct exfat_super_block *)malloc(sizeof(*new_sb));
		if(!new_sb){
			exfat_error("can't alloc new_sb");
			exit(1);
		}
		memcpy(new_sb, ef.sb, sizeof(*new_sb));

		new_sb->sector_start 			= cpu_to_le64(    32768);
		new_sb->sector_count 			= cpu_to_le64(124702720);
		new_sb->fat_sector_start 		= cpu_to_le32(    16384);
		new_sb->fat_sector_count 		= cpu_to_le32(    16384);
		new_sb->cluster_sector_start 	= cpu_to_le32(    32768);
		new_sb->cluster_count 			= cpu_to_le32(   121748);
		new_sb->rootdir_cluster 		= cpu_to_le32(        4);
		new_sb->sector_bits 			=                     9;
		new_sb->spc_bits 				=                    10;

		efp = exfatx_patch(&ef, new_sb);
		if(!efp){
			exfat_error("can't patch exfat");
			exit(1);
		}
	}

	exfat_debug("Patched:");
	exfat_print_info(efp->sb, exfat_count_free_clusters(efp));

	action_hashes(efp, "hashes-patched.txt");

	exfat_debug("ok");

	exfat_unmount(&ef);
	return 0;
}
