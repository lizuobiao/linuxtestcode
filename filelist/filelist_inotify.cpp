#include "filelist.h"

#define PICTUREPATH  "./picture/upload/"


Filelist::Filelist(const char* path)
{
	DIR *dir;
 //   char basePath[128];
	
	path_ = path;
	
 /* memset(basePath,'\0',sizeof(basePath));
    getcwd(basePath, 999);
    printf("the current dir is : %s\n",basePath);*/

    //get the file list
//    memset(basePath,'\0',sizeof(basePath));
//    strcpy(basePath,path_.c_str());
    readFilevect(path_.c_str());

	pthread_create(&file_monitor_pthreadid, NULL, file_monitor_thread,this);
	pthread_detach(file_monitor_pthreadid);
}

Filelist* Filelist::instance = new Filelist(PICTUREPATH);
Filelist* Filelist::getInstance()
{
	return instance;
}

int Filelist::readFilevect(const char *basePath)
{
    DIR *dir;
    struct dirent *ptr;
    char base[1000];
	string path;
	file_path_stu file_path;
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
			 file_name = ptr->d_name;
			 path = path_+file_name;
			 file_path.filepath = path;
			 file_path.flag     = 0;
			 Filevect.push_back(file_path);
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
            readFilevect(base);
        }
    }
     closedir(dir);
     return 1;
}

void Filelist::set_filepath(string filepath)
{
		file_path_stu file_path;
		file_path.flag = 0;
		file_path.filepath = filepath;
		Filevect.push_back(file_path);
}

string Filelist::get_filepath(void)
{
	string filepath = "";
	for(int i = 0; i < Filevect.size(); i++)
	{
//      	cout << "erase value of vec [" << i << "] = " << Filevect[i].filepath << endl;

		if(Filevect[i].flag == 0)
		{
			Filevect[i].flag = 1;
			filepath = Filevect[i].filepath;
			break;
		}
    }
	
	return filepath;
}

int Filelist::clear_filepath(string filepath)
{
	for(vector<file_path_stu>::iterator it=Filevect.begin(); it!=Filevect.end(); )    //小括号里不需写  ++it
	{
	   if((*it).filepath == filepath)
	   {
	        (*it).flag = 0;
			Filevect.erase(it);
	        break;
	   }
	   else
	   {
	        ++it;
	   }
	}
	return 1;
}

int Filelist::watch_inotify_events(int fd)
{
	char event_buf[512] = {0};
	int ret;
	int event_pos = 0;
	int event_size = 0;
	char *p = NULL;
	string path;
	file_path_stu file_path;
	std::ostringstream oss;
	
	struct inotify_event *event;
	
	/*读事件是否发生，没有发生就会阻塞*/
	ret = read(fd, event_buf, sizeof(event_buf));
	
	/*如果read的返回值，小于inotify_event大小出现错误*/
	if(ret < (int)sizeof(struct inotify_event))
	{
		printf("counld not get event!\n");
		return -1;
	}
	
	/*因为read的返回值存在一个或者多个inotify_event对象，需要一个一个取出来处理*/
	while( ret >= (int)sizeof(struct inotify_event) )
	{
		event = (struct inotify_event*)(event_buf + event_pos);
		if(event->len)
		{
//			memset((char *)event->name,0,128);
			p = strstr((char *)event->name,"filepart");
			file_name = event->name;
			path = path_+file_name;
			if(event->mask & IN_CREATE)
			{
	//			printf("create file: %s\n",event->name);			
				if(p == NULL)
				{	
					 file_path.flag = 0;
					 file_path.filepath = path;
			 		 Filevect.push_back(file_path);
//					 oss << "cp " << file_path.filepath << " /mnt/";		
//					 system(oss.str().c_str());
				}
			}else if(event->mask & IN_MOVED_TO)
			{
//					printf("IN_MOVED_FROM!\n");
					printf("IN_MOVED_FROM file: %s\n",event->name);	
					if(p == NULL)
					{	
						 file_path.flag = 0;
						 file_path.filepath = path;
				 		 Filevect.push_back(file_path);
//						 oss << "cp " << file_path.filepath << " /mnt/";		
//					 	 system(oss.str().c_str());
					}
			}
			else
			{
				for(vector<file_path_stu>::iterator it=Filevect.begin(); it!=Filevect.end(); )    //小括号里不需写  ++it
				{
				   if((*it).filepath == path)
				   {
				        it  =  Filevect.erase(it);       //以迭代器为参数，删除元素3，并把数据删除后的下一个元素位置返回给迭代器。  
				        break;
				   }
				   else
				   {
				        ++it;
				   }
				}
				
//				printf("delete file: %s\n",event->name);
			}
		}
		
		/*event_size就是一个事件的真正大小*/
		event_size = sizeof(struct inotify_event) + event->len;
		ret -= event_size;
		event_pos += event_size;
	}
	
	return 0;
}
 
void*  Filelist::file_monitor_thread(void *param)
{
	
	int InotifyFd;
	int ret;
	Filelist *filelist = reinterpret_cast<Filelist*>(param);
	/*inotify初始化*/
	InotifyFd = inotify_init();
	if( InotifyFd == -1)
	{
		printf("inotify_init error!\n");
		return NULL;
	}
	/*添加watch对象*/
	ret = inotify_add_watch(InotifyFd, PICTUREPATH, IN_CREATE |  IN_DELETE | IN_MOVED_TO);
	
	/*处理事件*/
	while(1)
	{
		filelist->watch_inotify_events(InotifyFd);
//		DLS_PrintInfo("picture file number : %d",filelist->Filevect.size());
	/*	for(int i = 0; i < filelist->Filevect.size(); i++)
		{
	      cout <<"                "<<filelist->Filevect[i].filepath << endl;
	    }*/
	}
 
    /*删除inotify的watch对象*/
	if (inotify_rm_watch(InotifyFd, ret) == -1) 
	{
		printf("notify_rm_watch error!\n");
		close(InotifyFd);
		return NULL;
	}
	
	/*关闭inotify描述符*/
	close(InotifyFd);
	return NULL;
}



int main(void)
{
	Filelist *filelist = Filelist::getInstance();

	while(1);
	
    return 0;
}









