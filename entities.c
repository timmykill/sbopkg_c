#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "entities.h"


int sbecmp(const void * a, const void * b)
{
	return strcmp(
		((SbEntity*)a)->name,
		((SbEntity*)b)->name
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
