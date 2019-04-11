#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h> 
#include "entities.h"

/* constants */
#define SB_URL "https://slackbuilds.org/slackbuilds/14.2/SLACKBUILDS.TXT"
#define SB_FILE "SLACKBUILDS.TXT"
#define CACHE_FILE "repo.bin"

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

static int get_v_size(FILE* fp);
static int fetch_sb_list(FILE* fp, Sb_entity* sbe_v, size_t v_size);
static int get_line(FILE* fp, char* prefix, char* field, size_t max_size);

/* Needs error checking */
int update()
{
	FILE* fp;
	int v_size, v_l_size;
	Sb_entity* sbe_v;
	/* 
	Get file
	NOTE: when i know how to do it, ill make the http download natively
	*/
	system("curl '" SB_URL "' -o " SB_FILE);
	fp = fopen(SB_FILE, "r");
	v_size = get_v_size(fp);
	printf("%d\n", v_size);
	sbe_v = (Sb_entity*) malloc(sizeof(Sb_entity) * v_size);
	v_l_size = fetch_sb_list(fp, sbe_v, v_size);
	fclose(fp);
	/* Sort the array */
	qsort(sbe_v, v_l_size, sizeof(Sb_entity), sbecmp);
	/* write it in a bin file */
	fp = fopen(CACHE_FILE, "wb");
	fwrite(&v_l_size, sizeof(int), 1, fp); 
	fwrite(sbe_v, v_l_size, sizeof(Sb_entity), fp);
	fclose(fp);
	printf("Real size: %d Logical Size: %d\n", v_size, v_l_size);
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

static int get_v_size(FILE* fp)
{
	int tmp, size = 0;
	do {
		tmp = fgetc(fp);
		if (tmp == '\n')
			if ((tmp = fgetc(fp)) == '\n')
				size++;
	} while (tmp != EOF);
	rewind(fp);
	return size;
}

static int fetch_sb_list(FILE* fp, Sb_entity* sbe_v, size_t v_size)
{
	int logic_size = 0;
	int err = 0;
	int tolerable_err = -1;
	int tot_err;
	Sb_entity sbe_tmp;
	while(v_size > logic_size){
		tot_err = 0;
		err = get_line(fp, NAME_PREF, sbe_tmp.name, SBE_NAME_MAX);
		if (err < tolerable_err) break;
		else tot_err += err;
		err = get_line(fp, LOCATION_PREF, sbe_tmp.location, SBE_LOCATION_MAX);
		if (err < tolerable_err) break;
		else tot_err += err;
		err = get_line(fp, FILES_PREF, sbe_tmp.files, SBE_FILES_MAX);
		if (err < tolerable_err) break;
		else tot_err += err;
		err = get_line(fp, VERSION_PREF, sbe_tmp.version, SBE_VERSION_MAX);
		if (err < tolerable_err) break;
		else tot_err += err;
		err = get_line(fp, DOWNLOAD_PREF, sbe_tmp.download, SBE_DOWNLOAD_MAX);
		if (err < tolerable_err) break;
		else tot_err += err;
		err = get_line(fp, DOWNLOAD_X86_64_PREF, sbe_tmp.download_x86_64, SBE_DOWNLOAD_MAX);
		if (err < tolerable_err) break;
		else tot_err += err;
		err = get_line(fp, MD5SUM_PREF, sbe_tmp.md5sum, SBE_MD5SUM_MAX);
		if (err < tolerable_err) break;
		else tot_err += err;
		err = get_line(fp, MD5SUM_X86_64_PREF, sbe_tmp.md5sum_x86_64, SBE_MD5SUM_MAX);
		if (err < tolerable_err) break;
		else tot_err += err;
		err = get_line(fp, REQUIRES_PREF, sbe_tmp.requires, SBE_REQUIRES_MAX);
		if (err < tolerable_err) break;
		else tot_err += err;
		err = get_line(fp, SHORT_DESC_PREF, sbe_tmp.short_desc, SBE_SHORT_DESC_MAX);
		if (err < tolerable_err) break;
		else tot_err += err;
		
		/* get trailing newline*/
		fgetc(fp);
		if (tot_err == 10)
			sbe_v[logic_size++] = sbe_tmp;
	}
	return logic_size;
}

static int get_line(FILE* fp, char* prefix, char* field, size_t max_size)
{
	int i;
	int tmp;
	size_t str_len = strlen(prefix);
	/* check prefix */
	for (i = 0; i < str_len; i++){
		tmp = fgetc(fp);
		if (tmp == EOF) {
			return -2;
		} else if (tmp == prefix[i]) {
			continue;
		} else {
			return -1;
		}
	}
	/* get field */
	for (i = 0; i < max_size; i++)
		if ((tmp = fgetc(fp)) == EOF)
			return -2;
		else if (tmp == '\n')
			break;
		else
			field[i] = tmp;
	/* if we hit max size, result is invalid */
	if (i == max_size){
		/* ignore till newline */
		do{
			tmp = fgetc(fp);
		} while (tmp != '\n');
		/* sent line ending */
		field[max_size-1] = '\0';
		return 0;
	} else {
		field[i] = '\0';
		return 1;
	}
}
