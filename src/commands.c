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
#include "commands.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static struct command_entry commands[] =
{
  {
    "pwd",
    do_pwd,
    err_pwd
  },
  {
    "cd",
    do_cd,
    err_cd
  }
};

struct command_entry* fetch_command(const char* command_name)
{

  if(strcmp(command_name, "pwd") == 0)
      return &commands[0];
  else if(strcmp(command_name, "cd") == 0)
      return &commands[1];
  else
      return NULL;
}

int do_pwd(int argc, char** argv)
{
  char pwd[1024];
  getcwd(pwd,1024);
  printf("%s\n",pwd);
  fflush(stdout);
  return 0;
  if(getcwd(pwd,1024))
      return -1;
}

void err_pwd(int err_code)
{
  // TODO: Fill it.
}

int do_cd(int argc, char** argv)
{
  int return_stat;
  struct stat info;
  char *filename;

  if(argc != 2){
      if(argc == 1)
          chdir(getenv("HOME"));
      printf("input correct usage");
      exit(0);
  }
  if((return_stat = stat(argv[1],&info)) == -1)
      return 1;

  if(S_ISDIR(info.st_mode)){
      if(argc == 2){
          if(chdir(argv[1]))
              return 1;
      }
      return 0;
  }
  else{
      return 2;
  }

}

void err_cd(int err_code)
{
 int err = err_code;
  if(err ==1)
      fprintf(stderr, "cd: no such file or directory\n");
  else if(err ==2)
      fprintf(stderr, "cd: not a directory\n");
  else
      fprintf(stderr, "cd: Invalid input\n");

}
