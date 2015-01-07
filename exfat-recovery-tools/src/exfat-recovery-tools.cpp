//============================================================================
// Name        : exfat-recovery-tools.cpp
// Author      : Ludovic Lacoste
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>

using namespace std;

extern "C" {
#include "exfat.h"
}

int main(int argc, char *argv[]) {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

	exfat ef;
	int ret;

	ret = exfat_mount(&ef, argv[1], "ro");
	if(ret){
		exfat_error("can't exfat_mount");
		exit(1);
	}

	cout << "ok" << endl;

	exfat_unmount(&ef);
	return 0;
}
