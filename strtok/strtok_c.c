#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int stringtoarraysubdelim(char* str,const char* delim,const char* subdelim,char **out)
{
   char *str1, *str2, *token, *subtoken;
   char *saveptr1, *saveptr2;
   int j;
   
   for (j = 1, str1 = str; ; j++, str1 = NULL) 
   {
	   token = strtok_r(str1, delim, &saveptr1);
	   if (token == NULL)
		   break;
	   printf("%d: %s\n", j, token);

	   for (str2 = token; ; str2 = NULL) {
		   subtoken = strtok_r(str2, subdelim, &saveptr2);
		   if (subtoken == NULL)
			   break;
		   printf(" --> %s\n", subtoken);
	   }
   }
}

int stringtoarray(char* str,const char* delim,char **out)
{
   char *str1,*token;
   char *saveptr1;
   int j;
   
   for (j = 0, str1 = str; ; j++, str1 = NULL) 
   {
	   token = strtok_r(str1, delim, &saveptr1);
	   if (token == NULL)
		   break;
	   out[j] = token;
	   printf("%d: %s\n", j, token);
   }
   return j;
}

int main(int argc, char *argv[])
{
/*   if (argc != 4) {
	   fprintf(stderr, "Usage: %s string delim subdelim\n",
			   argv[0]);
	   exit(EXIT_FAILURE);
   }
   递归二次分割
   //   stringtoarraysubdelim(argv[1],argv[2],argv[3],NULL);
   */
   char* delim = " ,.-";
   char str[] = "- This, a sample string.";
   char *saveptr[10];
   int num = 0,i;
   
   printf("Splitting string \"%s\" into tokens:\n", str);
   num = stringtoarray(str,delim,saveptr);
   for(i = 0;i < num;i++)
        printf("saveptr = %s\n",saveptr[i]);
//   printf("str: %s\n", str);
   

   exit(EXIT_SUCCESS);
}