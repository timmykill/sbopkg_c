#ifndef ENTITIES_H_
#define ENTITIES_H_

/* Constants */
#define SB_URL "https://slackbuilds.org/slackbuilds"
#define SB_VER "14.2"
#define SB_FILE "SLACKBUILDS.TXT"
#define TMPDIR "/tmp/sbo_c/"

/* define max field lenght */
#define SBE_NAME_MAX 64
#define SBE_LOCATION_MAX 64
#define SBE_FILES_MAX 128
#define SBE_VERSION_MAX 32
#define SBE_DOWNLOAD_MAX 256
#define SBE_MD5SUM_MAX 256
#define SBE_REQUIRES_MAX 256
#define SBE_SHORT_DESC_MAX 1024

typedef struct {
	char name[SBE_NAME_MAX];
	char location[SBE_LOCATION_MAX];
	char files[SBE_FILES_MAX];
	char version[SBE_VERSION_MAX];
	char download[SBE_DOWNLOAD_MAX];
	char download_x86_64[SBE_DOWNLOAD_MAX];
	char md5sum[SBE_MD5SUM_MAX];	
	char md5sum_x86_64[SBE_MD5SUM_MAX];
	char requires[SBE_REQUIRES_MAX];
	char short_desc[SBE_SHORT_DESC_MAX];
} Sb_entity;

typedef struct {
	char* name;
	size_t name_size;
	char* location;
	size_t location_size;
	char* files;
	size_t files_size;
	char* version;
	size_t version_size;
	char* download;
	size_t download_size;
	char* download_x86_64;
	size_t download_x86_64_size;
	char* md5sum;
	size_t md5sum_size;
	char* md5sum_x86_64;
	size_t md5sum_x86_64_size;
	char* requires;
	size_t requires_size;
	char* short_desc;
	size_t short_desc_size;
} SbEntity;


int sbecmp(const void * a, const void * b);
int new_sbecmp(const void * a, const void * b);

void free_sbe(SbEntity* sbe);

#endif
