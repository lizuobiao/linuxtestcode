#include "filelist_data.h"
#define PICTUREPATH  "./picture/upload/"
#define FILELISTNAME  "test2"

Filelist::Filelist(const char* path)
{
    readFilevect();
}

Filelist* Filelist::instance = new Filelist(PICTUREPATH);
Filelist* Filelist::getInstance()
{

	return instance;
}

void Filelist::readFilevect(void)
{
	int fd;
	int ret;
	char p[MAXLEN];
	fd=open(FILELISTNAME,O_CREAT|O_RDONLY,00700);
    if(fd<0)
    {
        perror("open");
		return;
    }
	while(1)
	{
		ret=read(fd,p,MAXLEN);
		if(ret<0)
		{
			perror("read");
			break;
		}
		if(ret==0)
		{
			printf("-------read over------\n");
			break;
		}
		if(ret>0)
		{
			set_data(p);
		}
	}
	close(fd);
}

void Filelist::write_to_File(void)
{
	size_t size;
	FILE *fp = fopen(FILELISTNAME,"wb");
    for(int i = 0; i < Filevect.size(); i++)
	{
	   size = fwrite(Filevect[i].data,1,MAXLEN,fp);   //依次将结构体数组写到文件
//	   cout << "erase value of vec [" << i << "] = " << Filevect[i].data << endl;
    }
	fclose(fp);
}

void Filelist::set_data(string data)
{
	file_path_stu file_path;
	file_path.flag = 0;
//	file_path.data = (char*)data.c_str();
	strcpy(file_path.data, data.c_str());
	Filevect.push_back(file_path);
}

void Filelist::set_data_writefile(string data)
{
	set_data(data);
	write_to_File();
}

string Filelist::get_data(void)
{
	string data = "";
	for(int i = 0; i < Filevect.size(); i++)
	{
//      	cout << "erase value of vec [" << i << "] = " << Filevect[i].data << endl;

		if(Filevect[i].flag == 0)
		{
			Filevect[i].flag = 1;
			data = Filevect[i].data;
			break;
		}
    }
	
	return data;
}

int Filelist::clear_data_writefile(string data)
{
	clear_data(data);
	write_to_File();
}

int Filelist::clear_data(string data)
{
	for(vector<file_path_stu>::iterator it=Filevect.begin(); it!=Filevect.end(); )    //小括号里不需写  ++it
	{
	   if((*it).data == data)
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

void Filelist::ergodic_data(void)
{
	for(int i = 0; i < Filevect.size(); i++)
	{
       cout << "erase value of vec [" << i << "] = " << Filevect[i].data << endl;
    }
}

int main(void)
{
	Filelist *filelist = Filelist::getInstance();
	filelist->set_data_writefile("/mnt/bfe7b477-88ff-4b63-9a3e-b3160604f3ed/0101/0101_11.jpg");

	while(1);
    return 0;
}








