#include <stdio.h>
#include <string.h>

/*
    @param:     maximum size of the string to read
    @def:       reads from stdin a string with the correct length
    @return:    a pointer of char
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