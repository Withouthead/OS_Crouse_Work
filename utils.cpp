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
void cd(char *path)
{

}
void ls(char *path)
{
    inode *ip = get_inode(0);
    int offset = sizeof(dirent);
    char name[FILENAME_MAX];
    for(int i = 0; i < ip->size; i += offset)
    {
        readi(ip, (uint64_t)name, i, offset);
        if(strlen(name) != 0)
        {
            printf("%s\n", name);
        }
    }
}
void mkdir(char *path)
{
    char r_path[2048];
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