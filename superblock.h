//
// Created by xiaomaotou31 on 2021/12/22.
//

#ifndef OS_CROUSE_WORK_SUPERBLOCK_H
#define OS_CROUSE_WORK_SUPERBLOCK_H
#include <iostream>
struct superblock
{


    uint virdis_size;
    uint block_size;
    uint all_inode_num;
    uint free_inode_num;
    uint all_block_num;
    uint free_block_num;
    uint inode_size;

};
#endif //OS_CROUSE_WORK_SUPERBLOCK_H
