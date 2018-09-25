#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

main()
{
  FILE * fp = fdopen(0, "w+");
  fprintf(fp, "%s\n", "hello!");
  fclose(fp);
}
