#include <strings.h>
#include "entities.h"


int sbecmp(const void * a, const void * b)
{
	return strcmp(
		((Sb_entity*)a)->name,
		((Sb_entity*)b)->name
	);
}
