#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SB_FILE "SLACKBUILDS.TXT"
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

/* define max field lenght */
#define NAME_MAX 64
#define LOCATION_MAX 64
#define FILES_MAX 128
#define VERSION_MAX 32
#define DOWNLOAD_MAX 256
#define MD5SUM_MAX 256
#define REQUIRES_MAX 256
#define SHORT_DESC_MAX 1024

typedef struct {
	char name[NAME_MAX];
	char location[LOCATION_MAX];
	char files[FILES_MAX];
	char version[VERSION_MAX];
	char download[DOWNLOAD_MAX];
	char download_x86_64[DOWNLOAD_MAX];
	char md5sum[MD5SUM_MAX];	
	char md5sum_x86_64[MD5SUM_MAX];
	char requires[REQUIRES_MAX];
	char short_desc[SHORT_DESC_MAX];
} Sb_entity;

int fetch_sb_list(FILE* fp, Sb_entity* sb_v, size_t v_size);
int get_line(FILE* fp, char* prefix, char* field, size_t max_size);
int get_v_size(FILE* fp);



int main()
{
	FILE* fp;
	int v_size, v_l_size;
	int i;
	Sb_entity* sbe_v;
	fp = fopen(SB_FILE, "r");
	v_size = get_v_size(fp);
	sbe_v = (Sb_entity*) malloc(sizeof(Sb_entity) * v_size);
	v_l_size = fetch_sb_list(fp, sbe_v, v_size);
	printf("Real size: %d Logical Size: %d\n", v_size, v_l_size);
	for (i = 0; i < v_l_size; i++){
		printf("Name: %s\nLocation: %s\nFiles: %s\nVersion: %s\nDownload: %s\nDonwload 64: %s\nMD5: %s\nMD5_64: %s\nRequires: %s\nShort Desc: %s\n\n", sbe_v[i].name, sbe_v[i].location, sbe_v[i].files, sbe_v[i].version, sbe_v[i].download, sbe_v[i].download_x86_64, sbe_v[i].md5sum, sbe_v[i].md5sum_x86_64, sbe_v[i].requires, sbe_v[i].short_desc);
	}
	return 0;
}

int get_v_size(FILE* fp)
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

int fetch_sb_list(FILE* fp, Sb_entity* sbe_v, size_t v_size)
{
	int logic_size = 0;
	int err = 0;
	int tolerable_err = -1;
	int tot_err;
	Sb_entity sbe_tmp;
	while(v_size > logic_size){
		tot_err = 0;
		err = get_line(fp, NAME_PREF, sbe_tmp.name, NAME_MAX);
		if (err < tolerable_err) break;
		else tot_err += err;
		err = get_line(fp, LOCATION_PREF, sbe_tmp.location, LOCATION_MAX);
		if (err < tolerable_err) break;
		else tot_err += err;
		err = get_line(fp, FILES_PREF, sbe_tmp.files, FILES_MAX);
		if (err < tolerable_err) break;
		else tot_err += err;
		err = get_line(fp, VERSION_PREF, sbe_tmp.version, VERSION_MAX);
		if (err < tolerable_err) break;
		else tot_err += err;
		err = get_line(fp, DOWNLOAD_PREF, sbe_tmp.download, DOWNLOAD_MAX);
		if (err < tolerable_err) break;
		else tot_err += err;
		err = get_line(fp, DOWNLOAD_X86_64_PREF, sbe_tmp.download_x86_64, DOWNLOAD_MAX);
		if (err < tolerable_err) break;
		else tot_err += err;
		err = get_line(fp, MD5SUM_PREF, sbe_tmp.md5sum, MD5SUM_MAX);
		if (err < tolerable_err) break;
		else tot_err += err;
		err = get_line(fp, MD5SUM_X86_64_PREF, sbe_tmp.md5sum_x86_64, MD5SUM_MAX);
		if (err < tolerable_err) break;
		else tot_err += err;
		err = get_line(fp, REQUIRES_PREF, sbe_tmp.requires, REQUIRES_MAX);
		if (err < tolerable_err) break;
		else tot_err += err;
		err = get_line(fp, SHORT_DESC_PREF, sbe_tmp.short_desc, SHORT_DESC_MAX);
		if (err < tolerable_err) break;
		else tot_err += err;
		
		/* get trailing newline*/
		fgetc(fp);
		if (tot_err == 10)
			sbe_v[logic_size++] = sbe_tmp;
	}
	return logic_size;
}

int get_line(FILE* fp, char* prefix, char* field, size_t max_size)
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
