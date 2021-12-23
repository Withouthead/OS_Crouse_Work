//
// Created by xiaomaotou31 on 2021/12/21.
//

#ifndef OS_CROUSE_WORK_INODE_H
#define OS_CROUSE_WORK_INODE_H



#define NDIRECT 12
#define NINDIRECT 1
#define DIRSIZE 14
#include <stdlib.h>
struct dirent
{
    ushort inum;
    char type;
    char name[DIRSIZE];
};

struct inode //64B
{
    char type;
    ushort inum;
    int nlink;
    ushort size;
    uint addrs[NDIRECT + NINDIRECT];
};
#endif //OS_CROUSE_WORK_INODE_H