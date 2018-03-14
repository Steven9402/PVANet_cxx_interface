#include "file.h"
namespace steven
{
    filebase::filebase()
    {

    }

    void filebase::readFileList(char *basePath,std::vector<std::string>& folders,std::vector<std::string>& files)
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
                {//printf("d_name:%s/%s\n",basePath,ptr->d_name);
                files.push_back(std::string(ptr->d_name));}
            else if(ptr->d_type == 10)    ///link file
                {//printf("d_name:%s/%s\n",basePath,ptr->d_name);
                files.push_back(std::string(ptr->d_name));}
            else if(ptr->d_type == 4)    ///dir
            {

                folders.push_back(ptr->d_name);

                //memset(base,'\0',sizeof(base));
                //strcpy(base,basePath);
                //strcat(base,"/");
                //strcat(base,ptr->d_name);
                //result.push_back(std::string(ptr->d_name));
                //readFileList(base);
                //std::cout<<"base:"<<std::string(base)<<std::endl;
            }
        }
        closedir(dir);
    }
}

