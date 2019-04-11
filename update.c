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
#define SHORT_DESC_PREF "SLACKBUILD SHORT DESCRIPTION:  "

static int get_v_size(MemoryStruct* mp);
static int fetch_sb_list(MemoryStruct* mp, Sb_entity* sbe_v, size_t v_size);
static char get_line(MemoryStruct* mp, int* f_i, char* prefix, char* field, size_t max_size);

/* Needs error checking */
int update()
{
	FILE* fp;
	int v_size, v_l_size;
	Sb_entity* sbe_v;
	printf("Getting Slackbuilds from %s\n", SB_TXT_URL);
	MemoryStruct* mp = curl_from_url(SB_TXT_URL);
	v_size = get_v_size(mp);
	sbe_v = (Sb_entity*) malloc(sizeof(Sb_entity) * v_size);
	v_l_size = fetch_sb_list(mp, sbe_v, v_size);
	free(mp->memory);
	free(mp);
	/* Sort the array */
	qsort(sbe_v, v_l_size, sizeof(Sb_entity), sbecmp);
	/* write it in a bin file */
	fp = fopen(CACHE_FILE, "wb");
	fwrite(&v_l_size, sizeof(int), 1, fp); 
	fwrite(sbe_v, v_l_size, sizeof(Sb_entity), fp);
	fclose(fp);
	printf("Found: %d Could load: %d\n", v_size, v_l_size);
	return 1;
}


Sb_entity* fetch_from_datafile(int* v_size)
{
	FILE* fp;
	Sb_entity* sbe_v;
	fp = fopen(CACHE_FILE, "rb");
	if (fp == NULL)
		return NULL;
	fread(v_size, sizeof(int), 1, fp);
	printf("Data size: %d\n", *v_size);
	sbe_v = (Sb_entity*) malloc(sizeof(Sb_entity) * * v_size);
	fread(sbe_v, sizeof(Sb_entity), * v_size, fp);
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

static int fetch_sb_list(MemoryStruct* mp, Sb_entity* sbe_v, size_t v_size)
{
	int logic_size = 0, file_index = 0;
	char err;
	Sb_entity sbe_tmp;
	while(v_size > logic_size){
		err = 0;
		err |= get_line(mp, &file_index, NAME_PREF, sbe_tmp.name, SBE_NAME_MAX);
		err |= get_line(mp, &file_index, LOCATION_PREF, sbe_tmp.location, SBE_LOCATION_MAX);
		err |= get_line(mp, &file_index, FILES_PREF, sbe_tmp.files, SBE_FILES_MAX);
		err |= get_line(mp, &file_index, VERSION_PREF, sbe_tmp.version, SBE_VERSION_MAX);
		err |= get_line(mp, &file_index, DOWNLOAD_PREF, sbe_tmp.download, SBE_DOWNLOAD_MAX);
		err |= get_line(mp, &file_index, DOWNLOAD_X86_64_PREF, sbe_tmp.download_x86_64, SBE_DOWNLOAD_MAX);
		err |= get_line(mp, &file_index, MD5SUM_PREF, sbe_tmp.md5sum, SBE_MD5SUM_MAX);
		err |= get_line(mp, &file_index, MD5SUM_X86_64_PREF, sbe_tmp.md5sum_x86_64, SBE_MD5SUM_MAX);
		err |= get_line(mp, &file_index, REQUIRES_PREF, sbe_tmp.requires, SBE_REQUIRES_MAX);
		err |= get_line(mp, &file_index, SHORT_DESC_PREF, sbe_tmp.short_desc, SBE_SHORT_DESC_MAX);
		file_index++;
		if (!err){
			sbe_v[logic_size++] = sbe_tmp;
		} else if ((err == 2) || (err == 3)){
			break;	
		}
	}
	return logic_size;
}

/*
	Ret codes:
		0 : OK
		1 : Ignore field
		2 : Hit size limit
*/
static char get_line(MemoryStruct* mp, int* f_i, char* prefix, char* field, size_t max_size)
{
	char tmp = 0;
	size_t str_len = strlen(prefix);
	int i = 0;
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
