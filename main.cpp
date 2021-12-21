#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <cstring>
#include "defs.h"
int main() {
    init();
    inode *ip = get_inode(0);
    char temp[10];
    bzero(temp, sizeof(temp));
    readi(ip, (uint64_t)temp, 0, FILENAME_MAX);
    std::cout << temp;

}
