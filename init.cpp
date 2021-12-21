//
// Created by xiaomaotou31 on 2021/12/21.
//

#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include "defs.h"

uint64_t vir_start_ad;
uint64_t init()
{
    int vir_fd = open("./virtual_disk.gfh", O_RDWR);
    vir_start_ad = (uint64_t)mmap(NULL,VIRDISK_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,vir_fd,0);
    return vir_start_ad;
}