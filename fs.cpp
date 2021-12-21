//
// Created by xiaomaotou31 on 2021/12/21.
//
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cassert>
#include "defs.h"

//extern uint64_t vir_start_ad;
struct dirent
{
    ushort inum;
    char name[DIRSIZE];
};

struct inode //64B
{
    char type;
    int nlink;
    int size;
    uint addrs[NDIRECT + NINDIRECT];
};

uint64_t get_real_addr(uint va)
{
    return vir_start_ad + va;
}

inode* find_free_inode()
{

    int inode_size = sizeof(inode);
    for(int i = INODE_START; i < DATA_START; i++)
    {
        for(int j = 0; j < BLOCK_SIZE; j += inode_size)
        {
            uint64_t addr = get_real_addr(i * BLOCK_SIZE + j);
            inode *temp = (inode *)addr;
            if(temp->type == 0) {
                memset(temp, 0, sizeof(&temp));
                return temp;
            }
        }
    }
    throw "There are no free inode!";
    return NULL;
}


uint64_t find_free_block()
{
    for(int i = DMAP_START; i < INODE_START; i++)
    {
        uint64_t addr = get_real_addr(i);
        char *b = (char *)(addr);
        if(*b == 0)
        {
            *b = 1;
            memset((void *)(vir_start_ad + DATA_START + (BLOCK_SIZE * i)), 0, BLOCK_SIZE);
            return DATA_START + (BLOCK_SIZE * i);
        }
    }
    throw "There is no free block!";
}
uint64_t balloc()
{
    uint64_t new_block = find_free_block();
    return new_block;
}
inode* ialloc(char type)
{
    inode *new_inode = find_free_inode();
    new_inode->type = type;
    return new_inode;
}

uint64_t bmap(inode *ip, uint bn)
{
    assert(bn < NDIRECT + NINDIRECT);
    uint64_t addr = vir_start_ad;
    if(bn < NDIRECT)
    {
        if(ip->addrs[bn] == 0)
        {
            ip->addrs[bn] = balloc();
        }
        addr += ip->addrs[bn];
        return addr;
    }
    bn -= NDIRECT;
    uint *v_addr;
    assert(bn < BLOCK_SIZE / sizeof(uint));
    if(ip->addrs[NDIRECT] == 0)
        ip->addrs[NDIRECT] = balloc();
    v_addr = (uint *)get_real_addr(ip->addrs[NDIRECT]);
    if(v_addr[bn] == 0)
        v_addr[bn] = balloc();
    addr = addr + v_addr[bn];
    return addr;

}
inode *get_inode(ushort inum)
{
    return (inode *)get_real_addr((INODE_START * BLOCK_SIZE) + inum);
}
int readi(inode *ip, uint64_t dst, uint off, uint n)
{
    uint tot, m;
    assert(!(off > ip->size || off + n < off));
    if(off + n > ip->size)
    {
        n = ip->size - off;
    }
    for(tot = 0; tot < n; tot += m, off += m, dst += m)
    {
        uint bn = off / BLOCK_SIZE;
        m = std::min(n - tot, BLOCK_SIZE - off % BLOCK_SIZE);
        memcpy((void *)dst, (void *)(bmap(ip, bn) + off % BLOCK_SIZE), m);
    }
    return tot;
}
int writei(inode *ip, uint64_t src, uint off, uint n)
{
    uint tot, m;
    for(tot = 0; tot < n; tot += m, off += m, src += m)
    {
        uint bn = off / BLOCK_SIZE;
        m = std::min(n - tot, BLOCK_SIZE - off % BLOCK_SIZE);
        memcpy((void *)(bmap(ip, bn) + off % BLOCK_SIZE), (void *)src, m);
    }
    if(off > ip->size)
        ip->size = off;
    return tot;
}
inode* dirlookup(inode *dp, char *name)
{
    assert(dp && dp->type == T_DIR);
    dirent de{};
    char *p = strtok(name, "/");
    while(p != NULL)
    {
        for(int i = 0; i < dp->size; i += sizeof(de))
        {
            readi(dp, (uint64_t)&de, i, sizeof(de));
            if(strcmp(de.name, name) == 0)
            {
                dp = get_inode(de.inum);
            }
        }
        p = strtok(name, "/");
    }
    return dp;
}