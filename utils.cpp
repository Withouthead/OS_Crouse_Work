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
char r_path[2048];

int process_path(char *temp_path)
{
    if(temp_path == NULL)
    {
        strcpy(r_path, c_path);
        return 1;
    }
    char path[2048];
    strcpy(path, temp_path);
    if(path[strlen(path) - 1] == '/')
        path[strlen(path) - 1] = 0;
    char *n_char_pos = strrchr(path, '\n');
    if(n_char_pos != NULL)
        path[n_char_pos - path] = 0;

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
        if(strcmp(c_path, "/") != 0)
            r_path[strlen(r_path)] = '/';
        strcat(r_path, path);
    }
    return 1;
}
void cd(char *path)
{
    process_path(path);

    inode *zero_inode = get_inode(0);
    if(dirlookup(zero_inode, r_path, 0, 0) == NULL)
    {
        printf("cd error: %s does not exist\n", r_path);
    }
    strcpy(c_path, r_path);

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
    process_path(path);
    inode *ip = get_inode(0);
    int offset = sizeof(dirent);
    dirent dir;
    bzero(&dir, sizeof(dir));
    ip = dirlookup(ip, r_path, 0, 0);
    if(ip == NULL)
    {
        printf("ls error: %s does not exist\n", path);
    }
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
    process_path(path);
    inode *ip = get_inode(0);
    ip = dirlookup(ip, r_path, 1, 0);
    if(ip == NULL)
    {
        printf("mkdir error\n");
    }
}
void rm(char *path)
{
    if(strcmp(path, "/") == 0)
    {
        printf("path error: can't remove root\n");
        return;
    }
    process_path(path);
    remove_file(r_path);
}