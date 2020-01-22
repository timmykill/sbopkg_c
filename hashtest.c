#include <stdio.h>
#include "utils.h"

int main(int argc, char** argv)
{
	printf("%d\n", hash(argv[1]));
	return 0;
}
