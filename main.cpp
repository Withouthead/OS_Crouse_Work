#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <cstring>
#include "defs.h"
int main() {
    init();
    bzero((void *)vir_start_ad, VIRDISK_SIZE);
    inode * temp = get_inode(0);
    temp->type = T_DIR;
    ls(c_path);
//    mkdir("/test");
    mkdir("/test/gfh");
    cd("/test/");
    ls(c_path);
}
