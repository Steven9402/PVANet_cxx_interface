#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <vector>
#include "stdlib.h"
//#include <stdio.h>
#include <iostream>
namespace steven {
class filebase{
public:
    filebase();
    void readFileList(char *basePath,std::vector<std::string>& folders,std::vector<std::string>& files);
};

}



