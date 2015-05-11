#include <stdio.h>
#include "../tools/str_tools.h"

int main(void)
{
	char str[] = " hello world ";
	char* ptr = NULL;
	ptr = l_trim(str);
	printf("%s\n", ptr);
	free(ptr);
	ptr = r_trim(str);
	printf("%s\n", ptr);
	free(ptr);
	ptr = trim(str);
	printf("%s\n", ptr);
	free(ptr);
	ptr = NULL;
	return 0;
}
