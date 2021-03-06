#ifndef __FILELIST__
#define __FILELIST__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <sys/inotify.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

#define FILEPATHLEN 64

typedef struct
{
	int flag;
	char filepath[FILEPATHLEN];
}file_path_stu;
	
class Filelist{
    public:
		static Filelist* getInstance(void);
		string get_filepath(void);
		int clear_filepath(string filepath);
		void set_filepath(string filepath);
		void ergodic_filepath(void);
		void set_filepath_writefile(string filepath);
		int clear_filepath_writefile(string filepath);
		
	private:
		Filelist(const char* path);
		~Filelist();
		//prevent copy
		Filelist(const Filelist&);
		Filelist& operator=(const Filelist&);
		
		static Filelist* instance;
		string file_name;
		vector<file_path_stu> Filevect;
		
		void readFilevect();
		void write_to_File(void);
};

#endif