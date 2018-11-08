/**********************************************************************
 * Copyright (C) Jaewon Choi <jaewon.james.choi@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTIABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 *********************************************************************/

#include "fs.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

char* findbin(const char*path)
{
    char *newpath=(char*)malloc(sizeof(char)*(150));
    memset(newpath, 0, 150);
    char *ptr= strstr(path,"/bin");
    if(ptr == NULL){
        strcat(newpath,"/bin/");
        strcat(newpath,path);
        return newpath;
    }
}

char* findusr(const char*path)
{
    char*newpath=(char*)malloc(sizeof(char)*(150));
    memset(newpath, 0, 150);
    char *ptr= strstr(path,"/usr/bin");
    if(ptr ==NULL){
        strcat(newpath,"/usr/bin/");
        strcat(newpath,path);
        return newpath;
    }
}

int does_exefile_exists(const char* path)
{
  // TODO: Fill it!
  struct stat minfo;
  int *pathtemp;//findbin , findusr 함수 메모리 할당하는 포인터 변수
  int return_stat;
  if((return_stat = stat(path,&minfo)) == -1){    // 기존 path가 존재하지 않는 경우 
      char *binpath=(char*)malloc(sizeof(char)*(150));
      memset(binpath, 0, 150);
      pathtemp = findbin(path);
      strncpy(binpath, pathtemp,strlen(pathtemp)); //binpath 생성
      free(pathtemp);
      return_stat = stat(binpath,&minfo);
      free(binpath);
      if(return_stat == -1)
      {
        char *usrpath=(char*)malloc(sizeof(char)*(150));
        memset(usrpath, 0, 150);
        pathtemp = findusr(path);
        strncpy(usrpath,pathtemp,strlen(pathtemp));
        free(pathtemp);
        return_stat = stat(usrpath,&minfo);
        free(usrpath);
        if(return_stat == -1){
            return 0;
       }
        if(S_ISREG(minfo.st_mode)){
            return 1;
        }
        else if(S_ISDIR(minfo.st_mode)){
            return 1;
        }
        else{
            return 0;
        }
      }
      if(S_ISREG(minfo.st_mode)){
          return 1;
      }
      else if(S_ISDIR(minfo.st_mode)){
          return 1;
      }
      else
          return 0;
 }

    if(S_ISREG(minfo.st_mode)){
        return 1;
    }
    else if(S_ISDIR(minfo.st_mode)){
        return 1;
    }
    else
        return 0;

}

char make_path(char **argv)
{
    char *path = malloc(strlen("/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"));
    strcpy(path, "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin");
    char *token;
    token = strtok(path,":");
    while(token!=NULL)
    {
        char *realpath = (char *)malloc(sizeof(char) *25);
        memset(realpath, 0, 25);

        strcat(realpath, token);
        strcat(realpath, "/");
        strcat(realpath, argv);
        if(!access(realpath,X_OK)){
            strcpy(argv[0], realpath);
            fprintf(stderr,"make_path output is : %s\n", argv[0]);
            return realpath;
        }
        token = strtok(NULL,":");
    }
    free(path);
}                                                 

