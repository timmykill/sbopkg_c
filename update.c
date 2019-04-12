#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h> 
#include "entities.h"
#include "curl_helper.h"

/* constants */
#define CACHE_FILE "repo.bin"
#define SB_TXT_URL SB_URL "/" SB_VER "/" SB_FILE

/* define file prefixes */
#define NAME_PREF "SLACKBUILD NAME: "
#define LOCATION_PREF "SLACKBUILD LOCATION: "
#define FILES_PREF "SLACKBUILD FILES: "
#define VERSION_PREF "SLACKBUILD VERSION: "
#define DOWNLOAD_PREF "SLACKBUILD DOWNLOAD: "
#define DOWNLOAD_X86_64_PREF "SLACKBUILD DOWNLOAD_x86_64: "
#define MD5SUM_PREF "SLACKBUILD MD5SUM: "
#define MD5SUM_X86_64_PREF "SLACKBUILD MD5SUM_x86_64: "
#define REQUIRES_PREF "SLACKBUILD REQUIRES: "
#define SHORT_DESC_PREF "SLACKBUILD SHORT DESCRIPTION: "

static size_t fetch_sb_list(MemoryStruct* mp, SbEntity* sbe_v, size_t v_size);
static int get_line_lenght(MemoryStruct* mp, size_t fi, size_t pref_size);
static int get_v_size(MemoryStruct* mp);
static char get_line(MemoryStruct* mp, size_t* f_i, char* prefix, char* field, size_t max_size);

int update()
{
	FILE* fp;
	size_t v_size,  v_l_size;
	int i;
	SbEntity* sbe_v;
	printf("Getting Slackbuilds from %s\n", SB_TXT_URL);
	MemoryStruct* mp = curl_from_url(SB_TXT_URL);
	v_size = get_v_size(mp);
	printf("Got size: %zu\n", v_size);
	sbe_v = (SbEntity*) malloc(sizeof(SbEntity) * v_size);
	if (sbe_v == NULL){
		printf("Not enought memory\n");
		return 0;
	}
	v_l_size = fetch_sb_list(mp, sbe_v, v_size);
	free(mp->memory);
	/* Sort the array */
	qsort(sbe_v, v_l_size, sizeof(SbEntity), sbecmp);
	/* write it in a bin file */
	fp = fopen(CACHE_FILE, "wb");
	fwrite(&v_l_size, sizeof(size_t), 1, fp); 
	fwrite(sbe_v, sizeof(SbEntity), v_l_size, fp);
	for (i = 0; i < v_l_size; i++){
		fwrite(sbe_v[i].name, 1, sbe_v[i].name_size, fp);
		fwrite(sbe_v[i].location, 1, sbe_v[i].location_size, fp);
		fwrite(sbe_v[i].files, 1, sbe_v[i].files_size, fp);
		fwrite(sbe_v[i].version, 1, sbe_v[i].version_size, fp);
		fwrite(sbe_v[i].download, 1, sbe_v[i].download_size, fp);
		fwrite(sbe_v[i].download_x86_64, 1, sbe_v[i].download_x86_64_size, fp);
		fwrite(sbe_v[i].md5sum, 1, sbe_v[i].md5sum_size, fp);
		fwrite(sbe_v[i].md5sum_x86_64, 1, sbe_v[i].md5sum_x86_64_size, fp);
		fwrite(sbe_v[i].requires, 1, sbe_v[i].requires_size, fp);
		fwrite(sbe_v[i].short_desc, 1, sbe_v[i].short_desc_size, fp);
	}	
	fclose(fp);
	printf("Found: %zu Could load: %zu\n", v_size, v_l_size);
	return 1;
}

SbEntity* fetch_from_datafile(size_t* v_size)
{
	FILE* fp;
	int i;
	SbEntity* sbe_v;
	fp = fopen(CACHE_FILE, "rb");
	if (fp == NULL)
		return NULL;
	fread(v_size, sizeof(size_t), 1, fp);
	sbe_v = (SbEntity*) malloc(sizeof(SbEntity) * *v_size);
	fread(sbe_v, sizeof(SbEntity), * v_size, fp);
	for (i = 0; i < *v_size; i++){
		sbe_v[i].name = (char*) malloc(sbe_v[i].name_size);
		fread(sbe_v[i].name, 1, sbe_v[i].name_size, fp);
		sbe_v[i].location = (char*) malloc(sbe_v[i].location_size);
		fread(sbe_v[i].location, 1, sbe_v[i].location_size, fp);
		sbe_v[i].files = (char*) malloc(sbe_v[i].files_size);
		fread(sbe_v[i].files, 1, sbe_v[i].files_size, fp);
		sbe_v[i].version = (char*) malloc(sbe_v[i].version_size);
		fread(sbe_v[i].version, 1, sbe_v[i].version_size, fp);
		sbe_v[i].download = (char*) malloc(sbe_v[i].download_size);
		fread(sbe_v[i].download, 1, sbe_v[i].download_size, fp);
		sbe_v[i].download_x86_64 = (char*) malloc(sbe_v[i].download_x86_64_size);
		fread(sbe_v[i].download_x86_64, 1, sbe_v[i].download_x86_64_size, fp);
		sbe_v[i].md5sum = (char*) malloc(sbe_v[i].md5sum_size);
		fread(sbe_v[i].md5sum, 1, sbe_v[i].md5sum_size, fp);
		sbe_v[i].md5sum_x86_64 = (char*) malloc(sbe_v[i].md5sum_x86_64_size);
		fread(sbe_v[i].md5sum_x86_64, 1, sbe_v[i].md5sum_x86_64_size, fp);
		sbe_v[i].requires = (char*) malloc(sbe_v[i].requires_size);
		fread(sbe_v[i].requires, 1, sbe_v[i].requires_size, fp);
		sbe_v[i].short_desc = (char*) malloc(sbe_v[i].short_desc_size);
		fread(sbe_v[i].short_desc, 1, sbe_v[i].short_desc_size, fp);
	}
	fclose(fp);
	return sbe_v;
}

static int get_v_size(MemoryStruct* mp)
{
	int f_i, size = 0;
	while (f_i < mp->size)
		if (mp->memory[f_i++] == '\n' && mp->memory[f_i++] == '\n' )
			size++;
	return size;
}

static size_t fetch_sb_list(MemoryStruct* mp, SbEntity* sbe_v, size_t v_size)
{
	size_t logic_size = 0;
	size_t file_index = 0;
	char err;
	SbEntity sbe_tmp;
	while(v_size > logic_size){
		err = 0;
		sbe_tmp.name_size = get_line_lenght(mp, file_index, strlen(NAME_PREF));
		sbe_tmp.name = malloc(sbe_tmp.name_size);
		err |= get_line(mp, &file_index, NAME_PREF, sbe_tmp.name, sbe_tmp.name_size);
		
		sbe_tmp.location_size = get_line_lenght(mp, file_index, strlen(LOCATION_PREF));
		sbe_tmp.location = malloc(sbe_tmp.location_size);
		err |= get_line(mp, &file_index, LOCATION_PREF, sbe_tmp.location, sbe_tmp.location_size);
		
		sbe_tmp.files_size = get_line_lenght(mp, file_index, strlen(FILES_PREF));
		sbe_tmp.files = malloc(sbe_tmp.files_size);
		err |= get_line(mp, &file_index, FILES_PREF, sbe_tmp.files, sbe_tmp.files_size);
		
		sbe_tmp.version_size = get_line_lenght(mp, file_index, strlen(VERSION_PREF));
		sbe_tmp.version = malloc(sbe_tmp.version_size);
		err |= get_line(mp, &file_index, VERSION_PREF, sbe_tmp.version, sbe_tmp.version_size);
		
		sbe_tmp.download_size = get_line_lenght(mp, file_index, strlen(DOWNLOAD_PREF));
		sbe_tmp.download = malloc(sbe_tmp.download_size);
		err |= get_line(mp, &file_index, DOWNLOAD_PREF, sbe_tmp.download, sbe_tmp.download_size);
		
		sbe_tmp.download_x86_64_size = get_line_lenght(mp, file_index, strlen(DOWNLOAD_X86_64_PREF));
		sbe_tmp.download_x86_64 = malloc(sbe_tmp.download_x86_64_size);
		err |= get_line(mp, &file_index, DOWNLOAD_X86_64_PREF, sbe_tmp.download_x86_64, sbe_tmp.download_x86_64_size);
		
		sbe_tmp.md5sum_size = get_line_lenght(mp, file_index, strlen(MD5SUM_PREF));
		sbe_tmp.md5sum = malloc(sbe_tmp.md5sum_size);
		err |= get_line(mp, &file_index, MD5SUM_PREF, sbe_tmp.md5sum, sbe_tmp.md5sum_size);
		
		sbe_tmp.md5sum_x86_64_size = get_line_lenght(mp, file_index, strlen(MD5SUM_X86_64_PREF));
		sbe_tmp.md5sum_x86_64 = malloc(sbe_tmp.md5sum_x86_64_size);
		err |= get_line(mp, &file_index, MD5SUM_X86_64_PREF, sbe_tmp.md5sum_x86_64, sbe_tmp.md5sum_x86_64_size);
		
		sbe_tmp.requires_size = get_line_lenght(mp, file_index, strlen(REQUIRES_PREF));
		sbe_tmp.requires = malloc(sbe_tmp.requires_size);
		err |= get_line(mp, &file_index, REQUIRES_PREF, sbe_tmp.requires, sbe_tmp.requires_size);
		
		sbe_tmp.short_desc_size = get_line_lenght(mp, file_index, strlen(SHORT_DESC_PREF));
		sbe_tmp.short_desc = malloc(sbe_tmp.short_desc_size);
		err |= get_line(mp, &file_index, SHORT_DESC_PREF, sbe_tmp.short_desc, sbe_tmp.short_desc_size);
		
		file_index++; /*skip newline*/
		if (!err){
			sbe_v[logic_size++] = sbe_tmp;
		} else if ((err == 2) || (err == 3)){
			break;	
		}
	}
	return logic_size;
}

static int get_line_lenght(MemoryStruct* mp, size_t fi, size_t pref_size)
{
	int size = 1 - pref_size;
	while (mp->size > fi && mp->memory[fi] != '\n'){
		size++;
		fi++;
	}
	return size;
}

/*
	Ret codes:
		0 : OK
		1 : Ignore field
		2 : Hit size limit
*/
static char get_line(MemoryStruct* mp, size_t* f_i, char* prefix, char* field, size_t max_size)
{
	char tmp = 0;
	size_t str_len = strlen(prefix);
	int i = 0;
	/* check field not  null*/
	if (field == NULL || prefix == NULL){
		printf("Something very wrong:\n");
		return 2;
	}
	/* check prefix */
	if ((*f_i)+str_len >= mp->size)
		return 2;
	else if (!strcmp(mp->memory + *f_i, prefix))
		return 1;
	(*f_i) += str_len;
	/* get field */
	while (
		(*f_i) < mp->size	
		&& (tmp = mp->memory[(*f_i)++]) != '\n'
		&& i < max_size 
		)
		field[i++] = tmp;

	/* if we hit max size, result is invalid */
	if (i == max_size){
		/* ignore till newline */
		while ((tmp = mp->memory[(*f_i)++]) != '\n');
		/* sent line ending */
		field[max_size-1] = '\0';
		return 1;
	} else {
		field[i] = '\0';
		return 0;
	}
}
