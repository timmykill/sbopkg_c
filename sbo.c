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
		printf("Usage: %s <search|update|install> <pkg>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	/* per evitare tutti questi stringcmp usare hash*/
	if (!strcmp("update", argv[1])){
		new_update();
	} else if ((sbe_v = new_fetch_from_datafile(&v_size)) == NULL){
		printf("No bin repo found\nTry running update first\n");
		exit(EXIT_FAILURE);
	} else if (!strcmp("search", argv[1])){
		new_search(argv[2], sbe_v, v_size);	
	} else if (!strcmp("install", argv[1])){
		new_install(argv[2], sbe_v, v_size);
	}
	return 0;
}
