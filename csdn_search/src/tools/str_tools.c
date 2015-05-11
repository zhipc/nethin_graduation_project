#include <ctype.h>
#include <stdlib.h>
#include "str_tools.h"

char* r_trim(const char* str)
{
	char* ret = NULL;

	size_t index = 0;
	
	if(str == NULL)
		return ret;

	index = strlen(str) + 1;

	while(index --)
	{
		if(!isspace(str[index]))
			break;
	}
	
	ret = calloc(index + 1, sizeof(char));
	if(ret == NULL)
	{
		return ret;
	}

	if(memcpy((void*)ret, (void*)str, index) == NULL)
	{
		free((void*)ret);
		return NULL;
	}

	ret[index] = 0;

	return ret;
}

char* l_trim(const char* str)
{
        char* ret = NULL;

	size_t index = 0;
        size_t str_len = 0;
	size_t ret_len = 0;

        if(str == NULL)
                return ret;

        str_len = strlen(str);

        while(str_len > index)
        {
                if(!isspace(str[index]))
                        break;
		index ++;
        }

	ret_len = str_len - index;
        ret = calloc(ret_len, sizeof(char));
	if(ret == NULL)
	{
		return ret;
	}

        if(memcpy((void*)ret, (void*)(str + index), ret_len) == NULL)
        {
                free((void*)ret);
                return NULL;
        }

        return ret;
}

char* trim(const char* str)
{
	char* l_ret = NULL;
	char* r_ret = NULL;
	
	l_ret = l_trim(str);
	if(l_ret == NULL)
	{
		return NULL;
	}

	r_ret = r_trim(l_ret);
	free(l_ret);

	return r_ret;
}
