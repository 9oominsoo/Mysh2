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
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <signal.h>
#include <pthread.h>
#include <sys/un.h>
#include <assert.h>
#include <sys/stat.h>

#define BUFF_SIZE 1024
#define PATH "tpf_unix_sock.socket"
#define READ 0
#define WRITE 1
#define WUNTRACED 2

void *client(char *command);


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
  },
  {
	"kill",
	do_kill,
	err_kill
  },
  {	
	"fg",
	do_fg,
	err_fg
  }	
};

struct command_entry* fetch_command(const char* command_name)
{

  if(strcmp(command_name, "pwd") == 0)
      return &commands[0];
  else if(strcmp(command_name, "cd") == 0)
      return &commands[1];
 // else if(strcmp(command_name, "kill") == 0)
 //	  return &commands[2];
  else if(strcmp(command_name, "fg") == 0)
	  return &commands[3];
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

void do_kill(char** argv)
{
	kill(atoi(argv[1]), SIGKILL);
}

void err_kill(int err_code)
{
	//
}

int do_fg(char **argv)
{
	wait(0);
}

void err_fg(int err_code)
{
	//
}

void zombie_handler(int signo)
{
	pid_t pid;
	int stat;
	while((pid=waitpid(-1,&stat,WNOHANG))>0)
		printf("child %d terminated normally\n",pid);
}
/*
void bg_process(int argc, char **argv)
{
	int fg_flag;
    int status;
    pid_t pid, wpid;
    int bg_flag=0;
    int bg_pid=0;
    char bg_command[1024];
    if(strcmp(argv[argc-1], "&") == 0){
        bg_flag=1;
        argv[argc-1] = NULL;
        argc --;
        strcpy(bg_command, argv[0]);
        for(int i=1; i<argc; i++){
            strcat(bg_command," ");
            strcat(bg_command, argv[i]);
        }
        printf("bg_commnad is: %s\n", bg_command);
    }
    else{
        strcpy(bg_command, argv);
        printf("bg_command is: %s\n", bg_command);
    }

    pid = fork();

    if(pid == 0){
        //child process
        if(fg_flag == 1)
        {
            waitpid(pid,&status,0);
            execvp(argv[0],argv);
            printf("foreground executed\n");
            fg_flag = 0;
        }
        else
        {
            if(execvp(bg_command, argv)<0){
				 fprintf(stderr, "command not found: %s\n", argv[0]);
                exit(EXIT_FAILURE);
            }exit(-1);
        }
    }
    else{
        //parent process
        if(bg_flag != 1){
            do{
                wpid = waitpid(pid, &status, WUNTRACED);
            }while(!WIFEXITED(status)&&!WIFSIGNALED(status));
        }
    }   
}       
*/
/*
void pipe_process(char *command, int category)
{
    //parse full command into two command ( | )
    int i=0;
    int RorW = category; //select child or parent execution
    char *path = (char *)malloc(sizeof(char)*strlen(command));
    char *send_command = (char *)malloc(sizeof(char)*125);
    char *receive_command = (char *)malloc(sizeof(char)*125);
    char *send[1];
    send[0] = NULL; send[1] = NULL; 
    char *receive[1];
    receive[0] = NULL; receive[1] = NULL;
    strcpy(path, command);
    pipe_parse(path, send_command, receive_command);
    printf("send command: %s\n", send_command);
    printf("receive command: %s\n", receive_command);
    //parse command[0] and command 
    char *pct;
    pct = strtok(send_command, " \n");
    while(pct != NULL && i<2)
    {
        send[i] = pct;
        pct=strtok(NULL," \n");
        i++;
    }
    i =0;
    pct = strtok(receive_command," \n");
    while(pct != NULL && i<2)
    {
        receive[i] = pct;
        pct=strtok(NULL," \n");
        i++;
    }

    //make command path
    make_path(send);
    make_path(receive);

    if(RorW == 1){
        //execute write(child) process
        if(execvp(send[0], send) == -1)                           
			   fprintf(stderr, " there is no writing command..\n");
    }else{
        //execute read(parent) process
        for(int z=0; z<2; z++)
            printf("receive command is : %s\n", receive[z]);
        if(execvp(receive[0], receive) == -1)
            fprintf(stderr, " there is no reading command..\n");
    }
    
}
*/
void _pipe(char *realcommand)
{
	char *server_com = (char *)malloc(sizeof(char)*25);
	char *exec_com[2];
	strcpy(server_com,realcommand);
	char *token = strtok(server_com, " |");
	token = strtok(NULL, " \n");
	token = strtok(NULL, " \n");
    exec_com[0] = token;
	token = strtok(NULL, " \n");
    exec_com[1] = token;


    int server_socket, client_socket, len, pf, status;
    char *command = (char *)malloc(sizeof(char)*125);
    command  = (char *)realcommand;
    struct sockaddr_un server_addr;
    struct sockaddr_un client_addr;
    int backlog=10;
    char *send_buf[1024];
    char *recv_buf[1024];
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    memset(&client_addr, 0, sizeof(struct sockaddr_un));
    memset(send_buf,0,1024);
    memset(recv_buf,0,1024);
    pthread_t cThread;
    int cThread_value;

    cThread_value = pthread_create(&cThread, NULL, client, command);
    if(cThread_value<0){
        printf("Thread create error\n");
        exit(0);
    }

    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    printf("--------------start-------------\n");
    if(server_socket == -1){
        printf("socket error\n");
        exit(1);
    }

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, PATH);
    len = sizeof(server_addr);

    unlink(PATH);
    pf = bind(server_socket, (struct sockaddr *) &server_addr, len);
    if(pf == -1){
        printf("bind error\n");
        close(server_socket);
        exit(1);    
    }                                      
	
	printf("waiting for listening to server ...\n");
    pf = listen(server_socket, backlog);
    if(pf == -1){
        printf("listen error\n");
        close(server_socket);
        exit(1);
    }

    printf("listening to client socket ...\n");
    client_socket = accept(server_socket, (struct sockaddr*) &client_addr, &len);
	while(1){
    	if(client_socket == -1){
       		printf("accept error\n");
       		close(server_socket);
        	close(client_socket);
        //	exit(1);
    	}else break;
	}

    pthread_join(cThread, NULL);
    
    printf("waiting to read ...\n");
    if(fork() == 0){
        close(READ);
        dup2(client_socket,READ);
        close(client_socket);
		execlp(exec_com[0], exec_com[0], exec_com[1], NULL);
        exit(0);
    }
    status;
    wait(&status);
    printf("-------------close-------------\n");
    close(server_socket);
    close(client_socket);
}

void *client(char *command)
{
	
	char *client_com = (char *)malloc(sizeof(char)*25);
	strcpy(client_com,command);
	char *token = strtok(client_com, " |");
	char *exec_com[1];
	exec_com[0] = token;

    int client_socket, pf, len, status;
    struct sockaddr_un server_addr;
    struct sockaddr_un client_addr;
    memset(&server_addr,0,sizeof(struct sockaddr_un));
    memset(&client_addr,0,sizeof(struct sockaddr_un));

    client_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if(client_socket == -1){
        printf("socket error\n");
        exit(1);
    }
    
    printf("creating client socket ...\n");
    printf("socket ID: %d\n",client_socket);

    client_addr.sun_family = AF_UNIX;
    strcpy(client_addr.sun_path, PATH);
    len=sizeof(client_addr);
   
    printf("waiting for connect to server ...\n");
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, PATH);
    while(1){
        pf=connect(client_socket, (struct sockaddr *) &server_addr, len);
        if(pf == -1){
            printf("connect error\n");
            close(client_socket);
            exit(1);
        }else break;
    }
    if(fork()==0){
        close(WRITE);                        
		dup2(client_socket,WRITE);
		execlp(exec_com[0], exec_com[0], NULL);
        exit(0);
    }
    wait(&status);
    close(client_socket);
    pthread_exit(NULL);
    return 0;
}


