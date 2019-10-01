#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "entities.h"
#include "update.h"
#include "search_install.h"


int main(int argc, char* argv[])
{
	SbEntity* sbe_v;
	size_t v_size;
	int i;
	if (argc < 2){
		fprintf(stderr, "Usage: %s <search|info|update|install> <pkg>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	/* per evitare tutti questi stringcmp usare hash*/
	if (!strcmp("update", argv[1])){
		update();
	} else if ((sbe_v = fetch_from_datafile(&v_size)) == NULL){
		fprintf(stderr, "No bin repo found\nTry running update first\n");
		exit(EXIT_FAILURE);
	} else if (!strcmp("search", argv[1])){
		search(argv[2], sbe_v, v_size);	
		free_sbe_v(sbe_v, v_size);	
	} else if (!strcmp("info", argv[1])){
		info(argv[2], sbe_v, v_size);
	} else if (!strcmp("install", argv[1])){
		install(argv[2], sbe_v, v_size);
	}
	return 0;
}

