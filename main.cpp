#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <cstring>
#include "defs.h"

int main() {
    init();
//    bzero((void *)vir_start_ad, VIRDISK_SIZE);
//    spb->free_inode_num = (DATA_START - INODE_START) / INODE_SIZE - 1;
//    spb->free_block_num = ((VIRDISK_SIZE) / BLOCK_SIZE - 1 - 2560 - 20);
//    spb->all_block_num = spb->free_block_num;
//    spb->all_inode_num = (DATA_START - INODE_START) / INODE_SIZE;
//    spb->inode_size = INODE_SIZE;
//    spb->block_size = BLOCK_SIZE;
//    spb->virdis_size = VIRDISK_SIZE;
    inode * temp = get_inode(0);
    temp->type = T_DIR;
    char command[2048];
    while(1)
    {
        printf("xiaomaotou31: %s $",c_path);
        fgets(command, sizeof(command), stdin);
        char *p = strtok(command, " \n");
        if(strcmp(p, "quit") == 0)
        {
            break;
        }
        else if(strcmp(p, "ls") == 0)
        {
            p = strtok(NULL, " ");
            ls(p);
        }
        else if(strcmp(p, "cd") == 0)
        {
            p = strtok(NULL, " ");
            cd(p);
        }
        else if(strcmp(p, "mkdir") == 0)
        {
            p = strtok(NULL, " ");
            mkdir(p);
        }
        else if(strcmp(p, "rm") == 0)
        {
            p = strtok(NULL, " ");
            rm(p);
        }
        else if(strcmp(p, "sysinfo") == 0)
        {
            sysinfo();
        }
        else
            printf("There is no command named %s\n", command);
    }
    ls(c_path);
}
