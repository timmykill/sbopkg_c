#include <strings.h>
#include <stdlib.h>
#include "entities.h"


int new_sbecmp(const void * a, const void * b)
{
	return strcmp(
		((SbEntity*)a)->name,
		((SbEntity*)b)->name
	);
}

int sbecmp(const void * a, const void * b)
{
	return strcmp(
		((Sb_entity*)a)->name,
		((Sb_entity*)b)->name
	);
}

void free_sbe(SbEntity* sbe)
{
	free(sbe->name);
	free(sbe->location);
	free(sbe->files);
	free(sbe->version);
	free(sbe->download);
	free(sbe->download_x86_64);
	free(sbe->md5sum);
	free(sbe->md5sum_x86_64);
	free(sbe->requires);
	free(sbe->short_desc);
}
