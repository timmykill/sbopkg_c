#ifndef ENTITIES_H_
#define ENTITIES_H_

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

#endif
