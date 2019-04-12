#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "entities.h"
#include "search_install.h"
#include "curl_helper.h"

void new_get_files(char* tmpdir, SbEntity* sbe);
void get_files(char* tmpdir, Sb_entity* sbe);
char* make_tmpdir(char* pkgname);

void new_search(char* s, SbEntity* sbe_v, size_t v_size)
{
	int i;
	for (i = 0; i < v_size; i++){
		if (strstr(sbe_v[i].name, s) != NULL)
			printf("Name: %s\n%s\n\n", sbe_v[i].name, sbe_v[i].short_desc);
		}
}


void search(char* s, Sb_entity* sbe_v, int v_size)
{
	int i;
	for (i = 0; i < v_size; i++)
		if (strstr(sbe_v[i].name, s) != NULL)
			printf("Name: %s\n%s\n\n", sbe_v[i].name, sbe_v[i].short_desc);
}

static SbEntity* new_locate(char* s, SbEntity* sbe_v, size_t v_size)
{
	SbEntity *sbe_tmp, *sbe;
	printf("v_size: %zu\n", v_size);
	sbe_tmp = (SbEntity*) bsearch(s, sbe_v, v_size, sizeof(SbEntity), new_sbecmp);
	if (sbe_tmp == NULL)
		return NULL;
	sbe = (SbEntity*) malloc(sizeof(SbEntity));
	memcpy(sbe, sbe_tmp, sizeof(SbEntity));
	return sbe;
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

void new_install(char* s, SbEntity* sbe_v, size_t v_size)
{
	SbEntity* sbe;
	char* tmpdir;
	sbe = new_locate(s, sbe_v, v_size);
	/* memory leak here */
	if (sbe == NULL){
		printf("%s not found\n", s);
		return;
	}
	printf("Installing: %s\nLocation: %s\n", sbe->name, sbe->location);	
	tmpdir = make_tmpdir(sbe->name);
	new_get_files(tmpdir, sbe);
}


void install(char* s, Sb_entity* sbe_v, int v_size)
{
	Sb_entity* sbe;
	char* tmpdir;
	sbe = locate(s, sbe_v, v_size);
	free(sbe_v);
	if (sbe == NULL){
		printf("%s not found", s);
		return;
	}
	printf("Installing: %s\nLocation: %s\n", sbe->name, sbe->location);	
	tmpdir = make_tmpdir(sbe->name);
	get_files(tmpdir, sbe);
}


void new_get_files(char* tmpdir, SbEntity* sbe)
{
	int i, j;
	char tmpfile[SBE_FILES_MAX];
	char *url;
	size_t tmpdir_len = strlen(tmpdir);
	size_t url_len = strlen(SB_URL "/" SB_VER);
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

void get_files(char* tmpdir, Sb_entity* sbe)
{
	int i, j;
	char tmpfile[SBE_FILES_MAX];
	char *url;
	size_t tmpdir_len = strlen(tmpdir);
	size_t url_len = strlen(SB_URL "/" SB_VER);
	tmpdir = (char*) realloc(tmpdir, tmpdir_len + SBE_FILES_MAX);
	url = (char*) malloc( url_len + strlen(sbe->location) + SBE_FILES_MAX);
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

char* make_tmpdir(char* pkgname)
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
