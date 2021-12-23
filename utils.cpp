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
    inode *temp_inode;
    if((temp_inode = dirlookup(zero_inode, r_path, 0, 0, T_DIR)) == NULL || temp_inode->type != T_DIR)
    {
        printf("cd error: %s does not exist\n", r_path);
        return;
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
    ip = dirlookup(ip, r_path, 0, 0, T_DIR);
    if(ip == NULL)
    {
        printf("ls error: %s does not exist\n", path);
        return;
    }
    printf("name\t\ttype\tsize\n");
    for(int i = 0; i < ip->size; i += offset)
    {
        bzero(&dir, sizeof(dir));
        char file_name[20];
        bzero(file_name, sizeof(file_name));
        readi(ip, (uint64_t)&dir, i, offset);
        strcpy(file_name, dir.name);
        inode* chile_inode = dirlookup(ip, file_name, 0, 0, dir.type);
        if(strlen(dir.name) != 0)
        {
            printf("%s\t%u\t%uB\n", dir.name, chile_inode->type,chile_inode->size);
        }
    }
}
void mkdir(char *path)
{
    process_path(path);
    inode *ip = get_inode(0);
    ip = dirlookup(ip, r_path, 1, 0, T_DIR);
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
void touch(char *path, char *buffer)
{
    inode *zero_inode = get_inode(0);
    process_path(path);
    inode *p_inode = dirlookup(zero_inode, r_path, 0, 1, T_DIR);
    if(p_inode == NULL)
    {
        printf("touch error: %s does not exist\n", path);
        return;
    }
    create_new_file(r_path, buffer);
}
void sysinfo()
{
    printf("磁盘总大小:\t%uKB\n", spb->virdis_size);
    printf("磁盘剩余大小:\t%uKB\n", spb->free_block_num * spb->block_size);
    printf("块大小:\t%uKB\n", spb->block_size);
    printf("inode大小:\t%uKB\n", spb->inode_size);
    printf("块个数:\t%u\n", spb->all_block_num);
    printf("inode个数:\t%u\n", spb->all_inode_num);
    printf("剩余块个数:\t%u\n", spb->free_block_num);
    printf("剩余inode个数:\t%u\n", spb->free_inode_num);
}
