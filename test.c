#include <stdio.h>
#include <string.h>

int main() {
   char str[21]; // Espacio para 20 caracteres + '\0'

   printf("Escribe algo: ");
   if (fgets(str, sizeof(str), stdin)) {
       
       // Buscamos el salto de línea en la cadena
       if (strchr(str, '\n')) {
           printf("Texto guardado: %s", str);
       } else {
           // Si NO hay '\n', es porque la entrada superó los 20 caracteres
           printf("Error: El string es muy largo (max 20).\n");
           
           // Opcional: Limpiar el búfer para que no afecte al siguiente scanf/fgets
           int c;
           while ((c = getchar()) != '\n' && c != EOF);
       }
   }

   return 0;
}
