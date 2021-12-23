//
// Created by xiaomaotou31 on 2021/12/21.
//

#ifndef OS_CROUSE_WORK_DEFS_H
#define OS_CROUSE_WORK_DEFS_H


#include "inode.h"
#include "superblock.h"
#include <cstdlib>
#include <sys/types.h>
#include <iostream>
#define VIRDISK_SIZE 1024 * 1024 * 20
#define BLOCK_SIZE 1024
#define DMAP_START (BLOCK_SIZE)
#define INODE_START (VIRDISK_SIZE / 8 + 1)
#define DATA_START (INODE_START + BLOCK_SIZE * 20)
#define INODE_SIZE 64


#define T_DIR 1
#define T_FILE 2

extern uint64_t vir_start_ad;
extern char c_path[2048];
extern char p_path[2048];
extern superblock *spb;
//init.cpp
uint64_t init();


//fs.cpp
inode* ialloc(char type);
int readi(inode *ip, uint64_t dst, uint off, uint n);
int writei(inode *ip, uint64_t src, uint off, uint n);
inode *get_inode(ushort inum);
inode* dirlookup(inode *dp, char *name, int create_new, int parent, int type);
void remove_file(char *name);

//utils.cpp
void ls(char *path);
void mkdir(char *path);
void cd(char *path);
void rm(char *path);
void sysinfo();
inode *create_new_file(char *path, char *buffer);
void touch(char *path, char *buffer);
#endif //OS_CROUSE_WORK_DEFS_H