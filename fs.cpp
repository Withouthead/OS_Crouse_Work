//
// Created by xiaomaotou31 on 2021/12/21.
//
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cassert>
#include "defs.h"
#include "inode.h"

//extern uint64_t vir_start_ad;


uint64_t get_real_addr(uint va)
{
    return vir_start_ad + va;
}

inode* find_free_inode()
{

    int inode_size = sizeof(inode);
    for(int i = INODE_START; i < DATA_START; i += inode_size)
    {
        uint64_t addr = get_real_addr(i);
        inode *temp = (inode *) addr;
        if (temp->type == 0) {
            memset(temp, 0, sizeof(&temp));
            temp->inum = (i - INODE_START) / inode_size;
            return temp;
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
            memset((void *)(vir_start_ad + (DATA_START  + (i - DMAP_START) * BLOCK_SIZE)), 0, BLOCK_SIZE);
            return (DATA_START  + (i - DMAP_START) * BLOCK_SIZE);
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
    return (inode *)get_real_addr(inum * sizeof(inode) + INODE_START);
}
int readi(inode *ip, uint64_t dst,uint off, uint n)
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

        uint64_t block_ad = bmap(ip, bn);
        memcpy((void *)dst, (void *)(block_ad + off % BLOCK_SIZE), m);
    }
    return tot;
}
int writei(inode *ip, uint64_t src, uint off, uint n)
{
    uint tot = 0, m = 0;
    for(tot = 0; tot < n; tot += m, off += m, src += m)
    {
        uint bn = off / BLOCK_SIZE;
        m = std::min(n - tot, BLOCK_SIZE - off % BLOCK_SIZE);
//        uint64_t test = bmap(ip, bn);
        memcpy((void *)(bmap(ip, bn) + off % BLOCK_SIZE), (void *)src, m);
    }
    if(off > ip->size)
        ip->size = off;
    return tot;
}
inode* dirlookup(inode *dp, char *name, int create_new=0)
{
    assert(dp && dp->type == T_DIR);
    dirent de{};
    char *p = strtok(name, "/");
    while(p != NULL)
    {
        bzero(&de, sizeof(de));
        for(int i = 0; i < dp->size; i += sizeof(de))
        {
            readi(dp, (uint64_t)&de, i, sizeof(de));
            if(strcmp(de.name, p) == 0)
            {
                dp = get_inode(de.inum);
                break;
            }
        }
        if(strcmp(de.name, p) != 0)
        {
            if(create_new == 0)
                return NULL;
            else
            {
                inode *new_node = ialloc(T_DIR);
                dirent new_de{};
                strcpy(new_de.name, p);
                new_de.inum = new_node->inum;
                writei(dp, (uint64_t)&new_de, dp->size, sizeof(new_de));
                dp = new_node;
            }
        }
        p = strtok(NULL, "/");
    }
    return dp;
}

int bfree(uint va)//va是虚拟地址而不是真实地址
{
//    va = (va / BLOCK_SIZE) * BLOCK_SIZE; // round一下
    uint bnum = (va - DATA_START)  / BLOCK_SIZE;
    char *b = (char *)get_real_addr(DMAP_START + bnum);
    *b = 0;
    bzero((void *)get_real_addr(va), BLOCK_SIZE);
    return 1;
}
int ifree(ushort inum)
{
    inode *node = get_inode(inum);
    if(node->nlink > 0)
        return 1;
    node->size = 0;
    node->type = 0;
    for(int i = 0; i < NDIRECT; i++)
    {
        if(node->addrs[i] != 0)
            bfree(node->addrs[i]);
        node->addrs[i] = 0;
    }
    for(int i = NDIRECT; i < NINDIRECT; i++)
    {
        if(node->addrs[i] != 0)
        {
            int p_size = sizeof(uint);
            uint *v_addr = (uint *)get_real_addr(node->addrs[NDIRECT]);
            for(int k = 0; k < BLOCK_SIZE / p_size; k += p_size)
            {
                if(v_addr[k] != 0)
                {
                    bfree(v_addr[k]);
                    v_addr[k] = 0;
                }
            }
            bfree(node->addrs[i]);
            node->addrs[i] = 0;
        }
    }
    return 1;
}
