#ifndef __FILELIST__
#define __FILELIST__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <sys/inotify.h>

using namespace std;

typedef struct
{
	int flag;
	string filepath;
}file_path_stu;
	
class Filelist{
    public:
		static Filelist* getInstance();
		string get_filepath(void);
		int clear_filepath(string filepath);
		
	private:
		Filelist(const char* path);
		~Filelist();
		//prevent copy
		Filelist(const Filelist&);
		Filelist& operator=(const Filelist&);
		
		static Filelist* instance;
		string path_;
		string file_name;
		vector<file_path_stu> Filevect;
		
		int readFilevect(char *basePath);
		int watch_inotify_events(int fd);
		
		pthread_t file_monitor_pthreadid;
		static void *file_monitor_thread(void *param);
};

#endif