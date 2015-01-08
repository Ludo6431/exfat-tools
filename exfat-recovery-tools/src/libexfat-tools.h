/*
 * libexfat-tools.h
 *
 *  Created on: 8 janv. 2015
 *      Author: ludo6431
 */

#ifndef LIBEXFAT_TOOLS_H_
#define LIBEXFAT_TOOLS_H_

#include "exfat.h"

struct exfat *exfatx_patch(const struct exfat *ef, struct exfat_super_block *new_sb);

#endif /* LIBEXFAT_TOOLS_H_ */
