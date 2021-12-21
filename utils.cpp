//
// Created by xiaomaotou31 on 2021/12/21.
//
#include "defs.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include "inode.h"
struct inode;
char c_path[2048] = "/";//结尾不加斜线！
char p_path[2048] = "/";//结尾不加斜线！
void cd(char *temp_path)
{
    char path[2048];
    strcpy(path, temp_path);
    if(strcmp(path, "/") != 0 && path[strlen(path) - 1] == '/')
        path[strlen(path) - 1] = 0;
    if(path[0] == '.' && path[1] != '.')
    {
        strcat(c_path, path + 1);
    }
    else if(path[0] == '/')
    {
        strcpy(c_path, path);
    }
    if(strstr(path, "..") == path)
    {
        if(strcmp(c_path, p_path) == 0)
        {
            printf("无法返回上级目录\n");
            return;
        }
        strcpy(c_path, p_path);
        strcat(c_path, path + 2);
    }
    if(strcmp(c_path, "/") == 0)
        strcpy(p_path, "/");
    char * r_path_char = strrchr(c_path, '/');
    bzero(p_path, sizeof(p_path));
    for(int i = 0; i + c_path != r_path_char; i++)
    {
        p_path[i] = c_path[i];
    }
    if(strlen(p_path) == 0)
        strcpy(p_path, "/");
}
void ls(char *path)
{
    inode *ip = get_inode(0);
    int offset = sizeof(dirent);
    dirent dir;
    bzero(&dir, sizeof(dir));
    ip = dirlookup(ip, path, 0);
    for(int i = 0; i < ip->size; i += offset)
    {
        readi(ip, (uint64_t)&dir, i, offset);
        if(strlen(dir.name) != 0)
        {
            printf("%s\n", dir.name);
        }
    }
}
void mkdir(char *path)
{
    char r_path[2048];
    bzero(r_path, sizeof(r_path));
    if(strstr(path, "..") == path)
    {
        strcpy(r_path, p_path);
        strcat(r_path, path + 2);
    }
    else if(path[0] == '.')
    {
        strcpy(r_path, c_path);
        strcat(r_path, path + 1);
    }
    else if(path[0] == '/')
    {
        strcpy(r_path, path);
    }
    else
    {
        strcpy(r_path, c_path);
        strcat(r_path, path);
    }

    inode *ip = get_inode(0);
    ip = dirlookup(ip, r_path, 1);
    if(ip == NULL)
    {
        printf("mkdir error\n");
    }
}