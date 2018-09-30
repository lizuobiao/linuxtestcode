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

#define MAXLEN 64

typedef struct
{
	int flag;
	uint32_t data_len;
	char data[MAXLEN];
}file_path_stu;
	
class Filelist{
    public:
		static Filelist* getInstance(void);
		string get_data(void);
		int clear_data(string data);
		void set_data(string data);
		void ergodic_data(void);
		void set_data_writefile(string data);
		int clear_data_writefile(string data);
		
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