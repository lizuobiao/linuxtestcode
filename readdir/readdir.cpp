#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <vector>
using namespace std;
/*
#include <dirent.h>
struct dirent
{
   long d_ino; // inode number 索引节点号
   off_t d_off; // offset to this dirent 在目录文件中的偏移
   unsigned short d_reclen; //length of this d_name 文件名 
   unsigned char d_type; // the type of d_name 文件类型
   char d_name [NAME_MAX+1]; // file name (null-terminated) 文件名，最长255字符
}*/
vector<string> FileList; 
int readFileList(char *basePath)
{
    DIR *dir;
    struct dirent *ptr;
    char base[1000];

    if ((dir=opendir(basePath)) == NULL)
    {
        perror("Open dir error...");
        exit(1);
    }

    while ((ptr=readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
            continue;
        else if(ptr->d_type == 8)    ///file
		{
			 FileList.push_back(ptr->d_name);
             printf("d_name:%s/%s\n",basePath,ptr->d_name);
		}
        else if(ptr->d_type == 10)    ///link file
             printf("d_name:%s/%s\n",basePath,ptr->d_name);
        else if(ptr->d_type == 4)    ///dir
        {
            memset(base,'\0',sizeof(base));
            strcpy(base,basePath);
            strcat(base,"/");
            strcat(base,ptr->d_name);
            readFileList(base);
        }
    }
     closedir(dir);
     return 1;
}
 
int main(void)
{
    DIR *dir;
    char basePath[1000];
 
    ///get the current absoulte path
    memset(basePath,'\0',sizeof(basePath));
    getcwd(basePath, 999);
    printf("the current dir is : %s\n",basePath);

   ///get the file list
    memset(basePath,'\0',sizeof(basePath));
    strcpy(basePath,"./test");
    readFileList(basePath);
	
	vector<string>::iterator v = FileList.begin();
   while( v != FileList.end()) {
      cout << "value of v = " << *v << endl;
      v++;
   }
    return 0;
}