#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "entities.h"
#include "update.h"
#include "search_install.h"
#include "utils.h"
/*
 * HASHES:
 * update = 643
 * search = 630
 * info = 428
 * install = 759
*/


int main(int argc, char* argv[])
{
	SbEntity* sbe_v;
	size_t v_size;
	int i, h;
	if (argc < 2){
		fprintf(stderr, "Usage: %s <search|info|update|install> <pkg>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	/* per evitare tutti questi stringcmp usare hash*/
	h = hash(argv[1]); 
	if (h == 643 && !strcmp("update", argv[1])){
		update();
	} else if ((sbe_v = fetch_from_datafile(&v_size)) == NULL){
		fprintf(stderr, "No bin repo found\nTry running update first\n");
		exit(EXIT_FAILURE);
	}
	switch(h){
		case 630:
			if (!strcmp("search", argv[1])){
				search(argv[2], sbe_v, v_size);	
				free_sbe_v(sbe_v, v_size);
			}
			break;
		case 428:
			if (!strcmp("info", argv[1]))
				info(argv[2], sbe_v, v_size);
			break;
		case 759:
			if (!strcmp("install", argv[1]))
				install(argv[2], sbe_v, v_size);
			break;
	}
	return 0;
}

