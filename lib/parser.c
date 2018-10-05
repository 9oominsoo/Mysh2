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
#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int findend(char* input)
{
    char *ptr = strchr(input,'\"');
    if(ptr)
        return 1;
    else 
        return 0; 
}         
         
void parse_command(const char* input,
                   int* argc, char*** argv)
{       
    int i=0;
    char *two="\""; //큰 따옴표
    char* letter = (char*)malloc(sizeof(char)*(strlen(input)+1));// const 변수를 옮기기 위한 변수
    strcpy(letter, input);
    char *store[20] = {0,};// 큰 따옴표가 끝날때까지 임시로 저장해줄 배열
    char *ptr=strtok(letter," \n");
    *argv = (char**)malloc(sizeof(char*)*(20));
    while(ptr != NULL){ 
        (*argv)[i] = (char*)malloc(sizeof(char)*(150));
        if(strncmp(ptr,two,1) == 0)
        {
            strcat(store,ptr+1);
            strcat(store," ");
            ptr=strtok(NULL," \n");
            while(findend(ptr+1) ==0)
            {
                strcat(store,ptr);
                strcat(store," ");
                ptr=strtok(NULL," \n");
            }
            char del[20] = {0,};
            strncpy(del,ptr,strlen(ptr)-1);
            strcat(store,del);
            strcpy((*argv)[i],store);
            ptr = strtok(NULL," \n");
        }
        else
       {
            strcpy((*argv)[i],ptr);
            ptr = strtok(NULL," \n");
        }
        i++;
    }
    (*argv)[i] = NULL;
    *argc=i;
    free(letter);
}

