#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "entities.h"
#include "update.h"

int main()
{
	Sb_entity* sbe_v;
	int v_size;
	int i;
	if ((sbe_v = fetch_from_datafile(&v_size)) == NULL){
		update();
		sbe_v = fetch_from_datafile(&v_size);
	}
	for (i = 0; i < v_size; i++){
		printf("Name: %s\nLocation: %s\nFiles: %s\nVersion: %s\nDownload: %s\nDonwload 64: %s\nMD5: %s\nMD5_64: %s\nRequires: %s\nShort Desc: %s\n\n", sbe_v[i].name, sbe_v[i].location, sbe_v[i].files, sbe_v[i].version, sbe_v[i].download, sbe_v[i].download_x86_64, sbe_v[i].md5sum, sbe_v[i].md5sum_x86_64, sbe_v[i].requires, sbe_v[i].short_desc);
	}
	return 0;
}

