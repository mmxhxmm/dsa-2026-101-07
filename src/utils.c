#include "../lib/common.h"

/*
    @param:     maximum size of the string to read
    @def:       reads from stdin a string with the correct length
    @return:    char pointer to fisrt character of string 
*/
char    *input_str(int max)
{
    char    *str = (char *)malloc((max + 1) * sizeof(char));
    
    if (str == NULL)
        return NULL; 

    if (fgets(str, max + 1, stdin))
    {
        char *p = strchr(str, '\n');
        
        if (p != NULL) {
            *p = '\0';
            return str;
        } else {
            printf("Error: El string supera los %d caracteres.\n", max);
            
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            
            free(str);
            return NULL;
        }
    }
    
    free(str);
    return NULL;
}

int input_int()
{
	char	*temp = input_str(10);
	int		value;

	if (temp == NULL)
		return -1;
    
    value = atoi(temp);
    free(temp);

    return value;
}