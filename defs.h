//
// Created by xiaomaotou31 on 2021/12/21.
//

#ifndef OS_CROUSE_WORK_DEFS_H
#define OS_CROUSE_WORK_DEFS_H



#define VIRDISK_SIZE 1024 * 1024 * 20
#define BLOCK_SIZE 1024
#define DMAP_START (BLOCK_SIZE)
#define INODE_START (BLOCK_SIZE * 2)
#define DATA_START (BLOCK_SIZE * 22)
#define NDIRECT 12
#define NINDIRECT 1
#define DIRSIZE 14

#define T_DIR 1
#define T_FILE 2

extern uint64_t vir_start_ad;
struct inode;
//init.cpp
uint64_t init();


//fs.cpp
inode* ialloc(char type);
int readi(inode *ip, uint64_t dst, uint off, uint n);
int writei(inode *ip, uint64_t src, uint off, uint n);
inode *get_inode(int inum);
#endif //OS_CROUSE_WORK_DEFS_H