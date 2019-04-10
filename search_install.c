#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "entities.h"

void search(char* s, Sb_entity* sbe_v, int v_size)
{
	int i;
	for (i = 0; i < v_size; i++)
		if (strstr(sbe_v[i].name, s) != NULL)
			printf("Name: %s\n%s\n\n", sbe_v[i].name, sbe_v[i].short_desc);
}

static Sb_entity* locate(char* s, Sb_entity* sbe_v, int v_size)
{
	Sb_entity *sbe_tmp, *sbe;
	sbe_tmp = (Sb_entity*) bsearch(s, sbe_v, v_size, sizeof(Sb_entity), sbecmp);
	if (sbe_tmp == NULL)
		return NULL;
	sbe = (Sb_entity*) malloc(sizeof(Sb_entity));
	memcpy(sbe, sbe_tmp, sizeof(Sb_entity));
	return sbe;
}

void install(char* s, Sb_entity* sbe_v, int v_size)
{
	Sb_entity* sbe;
	sbe = locate(s, sbe_v, v_size);
	free(sbe_v);
	if (sbe == NULL){
		printf("%s not found", s);
		return;
	}
	printf("Installing: %s\nLocation: %s\n", sbe->name, sbe->location);
	
}
