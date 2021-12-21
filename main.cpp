#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <cstring>
#include "defs.h"
int main() {
    init();
    bzero((void *)vir_start_ad, VIRDISK_SIZE);
}
