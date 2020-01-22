#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdbool.h>
#include <ctype.h>

#include "entities.h"
#include "search_install.h"
#include "curl_helper.h"

static void get_files(char* tmpdir, SbEntity* sbe);
static char* make_tmpdir(char* pkgname);
static SbEntity* locate(char* s, SbEntity* sbe_v, size_t v_size);


void info(char* s, SbEntity* sbe_v, size_t v_size)
{
	SbEntity* sbe = locate(s, sbe_v, v_size);
	free_sbe_v(sbe_v, v_size);
	if (sbe == NULL){
		fprintf(stderr, "%s not found\n", s);
		return;
	}
	printf("Info:\n\tNome:\t%s\n\tLocation:\t%s\n\tVersion:\t%s\n\tDownload:\t%s\n\tDownload x86_64:\t%s\n\tDesc:\t%s\n", sbe->name, sbe->location, sbe->version, sbe->download, sbe->download_x86_64, sbe->short_desc);
}

void search(char* s, SbEntity* sbe_v, size_t v_size)
{
	int i;
	char * l_s, *tmp_name = NULL, *tmp;
	size_t tmp_name_s = 0;
	/* lower here the arg */
	l_s = malloc(strlen(s));
	for (i = 0 ; s[i] != '\0'; i++)
		l_s[i] = tolower(s[i]);
	l_s[i] = '\0';
	for (i = 0; i < v_size; i++){
		/* save the name somewhere else to not mess up the output */
		if (sbe_v[i].name_size > tmp_name_s){
			free(tmp_name);
			tmp_name_s = sbe_v[i].name_size;
			tmp_name = malloc(tmp_name_s);
		}
		memcpy(tmp_name, sbe_v[i].name, sbe_v[i].name_size);
		/* lower the name */
		for (tmp = tmp_name ; *tmp != '\0'; tmp++)
			*tmp = tolower(*tmp);
		if (strstr(tmp_name, l_s) != NULL)
			printf("Name: %s\n%s\n\n", sbe_v[i].name, sbe_v[i].short_desc);
	}
	free(l_s);
	free(tmp_name);
}

static SbEntity* locate(char* s, SbEntity* sbe_v, size_t v_size)
{
	SbEntity *sbe_tmp, *sbe, sbe_s;
	sbe_s.name = s;
	sbe_tmp = (SbEntity*) bsearch(&sbe_s, sbe_v, v_size, sizeof(SbEntity), sbecmp);
	if (sbe_tmp == NULL)
		return NULL;
	sbe = (SbEntity*) malloc(sizeof(SbEntity));
	sbecpy(sbe, sbe_tmp);
	return sbe;
}

void install(char* s, SbEntity* sbe_v, size_t v_size)
{
	SbEntity* sbe;
	char* tmpdir;
	sbe = locate(s, sbe_v, v_size);
	free_sbe_v(sbe_v, v_size);
	if (sbe == NULL){
		fprintf(stderr, "%s not found\n", s);
		return;
	}
	printf("Installing: %s\nLocation: %s\n", sbe->name, sbe->location);	
	tmpdir = make_tmpdir(sbe->name);
	get_files(tmpdir, sbe);
	free_sbe(sbe);
}

static void get_files(char* tmpdir, SbEntity* sbe)
{
	int i, j;
	char *tmpfile;
	char *url;
	size_t tmpdir_len = strlen(tmpdir);
	size_t url_len = strlen(SB_URL "/" SB_VER);
	tmpfile = malloc(sbe->files_size);	
	tmpdir = (char*) realloc(tmpdir, tmpdir_len + sbe->files_size);
	url = (char*) malloc( url_len + sbe->location_size + sbe->files_size);
	strcpy(url, SB_URL "/" SB_VER);
	if (tmpdir == NULL || url == NULL){
		printf("Errore nell'allocare memoria");
		return;
	}
	i = 0;
	do {
		j = 0;
		do {
			tmpfile[j++] = sbe->files[i++];
		} while (sbe->files[i] != '\0' && sbe->files[i] != ' ');
		/* skip space */
		if (sbe->files[i] == ' ')
			i++;
		/* add endline */
		tmpfile[j] = '\0';
		strcat(tmpdir, "/");
		strcat(tmpdir, tmpfile);
		strcat(url, sbe->location + 1); /* delete the first dot */
		strcat(url, "/");
		strcat(url, tmpfile);
		curl_to_file(url , tmpdir);
		tmpdir[tmpdir_len] = '\0';
		url[url_len] = '\0';
	} while (sbe->files[i] != '\0');	
	/* get the sources */
	/* tmpdir risks out of bound here */
	curl_to_file(sbe->download, strcat(tmpdir, strrchr(sbe->download,'/'))); 

}

static char* make_tmpdir(char* pkgname)
{
	int result;
	result = mkdir(TMPDIR, 0764);
	if (result < 0)
		printf("%s:%d\n", TMPDIR, errno);
	char* dir = (char*) malloc(strlen(pkgname) + strlen(TMPDIR));
	strcpy(dir, TMPDIR);
	strcat(dir, pkgname);
	result = mkdir(dir, 0764);
	if (result < 0)
		printf("%s:%d\n", dir, errno);
	return dir;
}
