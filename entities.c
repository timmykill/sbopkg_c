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

void free_sbe_v(SbEntity* sbe_v, size_t size)
{
	if (size > 0){
		free_sbe_v(sbe_v, size - 1);
		free_sbe(sbe_v + size);
	} else {
		free(sbe_v);
	}
}

void sbecpy(SbEntity* dest, SbEntity* source)
{
	dest->name = (char*) malloc(source->name_size);
	dest->name_size = source->name_size;
	memcpy(dest->name, source->name, source->name_size);
	dest->location = (char*) malloc(source->location_size);
	dest->location_size = source->location_size;
	memcpy(dest->location, source->location, source->location_size);
	dest->files = (char*) malloc(source->files_size);
	dest->files_size = source->files_size;
	memcpy(dest->files, source->files, source->files_size);
	dest->version = (char*) malloc(source->version_size);
	dest->version_size = source->version_size;
	memcpy(dest->version, source->version, source->version_size);
	dest->download = (char*) malloc(source->download_size);
	dest->download_size = source->download_size;
	memcpy(dest->download, source->download, source->download_size);
	dest->download_x86_64 = (char*) malloc(source->download_x86_64_size);
	dest->download_x86_64_size = source->download_x86_64_size;
	memcpy(dest->download_x86_64, source->download_x86_64, source->download_x86_64_size);
	dest->md5sum = (char*) malloc(source->md5sum_size);
	dest->md5sum_size = source->md5sum_size;
	memcpy(dest->md5sum, source->md5sum, source->md5sum_size);
	dest->md5sum_x86_64 = (char*) malloc(source->md5sum_x86_64_size);
	dest->md5sum_x86_64_size = source->md5sum_x86_64_size;
	memcpy(dest->md5sum_x86_64, source->md5sum_x86_64, source->md5sum_x86_64_size);
	dest->requires = (char*) malloc(source->requires_size);
	dest->requires_size = source->requires_size;
	memcpy(dest->requires, source->requires, source->requires_size);
	dest->short_desc = (char*) malloc(source->short_desc_size);
	dest->short_desc_size = source->short_desc_size;
	memcpy(dest->short_desc, source->short_desc, source->short_desc_size);
}
