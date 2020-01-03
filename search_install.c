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
static char * stristr_opt(char *s1, char *s2);
static char * stristr_safe(char *s1, char *s2);
static char * stristr_monosafe(char *s1, char *s2);


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
	char * tmp;
	/* lower here the arg */
	for (tmp = s ; *tmp != '\0'; tmp++)
		*tmp = tolower(*tmp);
	for (i = 0; i < v_size; i++)
		if (stristr_monosafe(sbe_v[i].name, s) != NULL)
			printf("Name: %s\n%s\n\n", sbe_v[i].name, sbe_v[i].short_desc);
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

static char * stristr_opt(char *s1, char *s2)
{
	char *i;
	/* lower s1 */
	for (i = s1 ; *i != '\0'; i++)
		*i = tolower(*i);
	/* dont have to lower s2 */
	return strstr(s1, s2);
}

static char * stristr_monosafe(char *s1, char *s2)
{
	char *i, *tmp_s1;
	size_t s1_s = 1; //counting last null byte
	/* find size of s1 and s2 */
	for(i = s1; *i != '\0'; i++)
		s1_s++;
	tmp_s1 = malloc(sizeof(char) * s1_s);
	/* copy strings, to not mess up with the originals */
	memcpy(tmp_s1, s1, s1_s);
	/* lower s1 */
	for (i = tmp_s1 ; *i != '\0'; i++)
		*i = tolower(*i);
	i = strstr(tmp_s1, s2);
	free(tmp_s1);
	return i;
}
static char * stristr_safe(char *s1, char *s2)
{
	char *i, *tmp_s1, *tmp_s2;
	size_t s1_s = 1, s2_s = 1; //counting last null byte
	/* find size of s1 and s2 */
	for(i = s1; *i != '\0'; i++)
		s1_s++;
	for(i = s2; *i != '\0'; i++)
		s2_s++;
	tmp_s1 = malloc(sizeof(char) * s1_s);
	tmp_s2 = malloc(sizeof(char) * s2_s);
	/* copy strings, to not mess up with the originals */
	memcpy(tmp_s1, s1, s1_s);
	memcpy(tmp_s2, s2, s2_s);
	/* lower s1 */
	for (i = tmp_s1 ; *i != '\0'; i++)
		*i = tolower(*i);
	/* lower s2 */
	for (i = tmp_s2 ; *i != '\0'; i++)
		*i = tolower(*i);
	i = strstr(tmp_s1, tmp_s2);
	free(tmp_s1);
	free(tmp_s2);
	return i;
}

