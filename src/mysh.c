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
#include "parser.h"
#include "utils.h"
#include "fs.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>

static int fg_flag=0;
int main()
{
  signal(SIGINT, SIG_IGN);
  signal(SIGTSTP, SIG_IGN);
  pid_t pid;
  pid = fork();
  if(pid == 0){
	pid_t childpid;
	childpid = fork();
	if(childpid == 0){
		exit(1);
	}
	else if(childpid > 0){
		sleep(1000);
		exit(0);
	}
	else{
		fprintf(stderr,"fail to fork\n");
  	}
  }
  while (1) {
 	char command_buffer[4096] = { 0, }; 
 	
	int fg_flag;
	int status;
	pid_t spid, wpid;
	int bg_flag=0;
	int bg_pid=0;
	char bg_command[1024];


    fgets(command_buffer,4096,stdin);

    if(strcmp(command_buffer, "\n") == 0){
	    do
	    {
			fprintf(stderr, "enter correct command...\n");
			fgets(command_buffer,4096,stdin);
		}while(strcmp(command_buffer,"\n") == 0);
	}

    int argc = -1;
    char** argv = NULL;
    parse_command(command_buffer, &argc, &argv);
	make_path(argv);
	
	 //check pipe process
    for(int i=0; i<argc; i++){
        if(strncmp(argv[i],"|",1) == 0){
            _pipe(command_buffer);
            printf("terminated process ..\n");
    	//	exit(1);
        }
    }
    
	 //check background process
	if(strncmp(argv[argc-1], "&",1) == 0){
        bg_flag=1;
        argv[argc-1] = NULL;
        argc --;

    	spid = fork();

   	    if(spid == 0){
        //child process
           	if(execvp(argv[0], argv)<0){
           	    fprintf(stderr, "command not found: %s\n", argv[0]);
           	    exit(EXIT_FAILURE);
           	}//exit(1);
    	}
		else {
			continue;
		}
        //parent process
	}

    assert(argv != NULL);
    if (strcmp(argv[0], "exit") == 0) {
      FREE_2D_ARRAY(argc, argv);
      break;
    }

    struct command_entry* comm_entry = fetch_command(argv[0]);

    if (comm_entry != NULL) {
      int ret = comm_entry->command_func(argc, argv);
      if (ret != 0) {
        comm_entry->err(ret);
      }
    } else if (does_exefile_exists(argv[0])) {
      pid_t pid;
      pid = fork();
      if(pid<0){
          exit(-1);
      }
      else if(pid == 0){
            if(execvp(argv[0],argv) == -1);
            {
                exit(EXIT_FAILURE);
            }
      }
      else{
          pid = wait(NULL);
      }

	} else {
      assert(comm_entry == NULL);
      fprintf(stderr, "%s: command not found.\n", argv[0]);
    }

    FREE_2D_ARRAY(argc, argv);
  }

  return 0;
}
