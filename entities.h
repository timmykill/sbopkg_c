#ifndef ENTITIES_H_
#define ENTITIES_H_

/* Constants */
#define SB_URL "https://slackbuilds.org/slackbuilds"
#define SB_VER "14.2"
#define SB_FILE "SLACKBUILDS.TXT"
#define TMPDIR "/tmp/sbo_c/"

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

void free_sbe(SbEntity* sbe);

#endif
