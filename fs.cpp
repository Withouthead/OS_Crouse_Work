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
            printf("分配了第%d个空闲inode\n", temp->inum);
            spb->free_inode_num --;
            return temp;
        }
    }
    throw "There are no free inode!";
    return NULL;
}


uint64_t find_free_block()
{
    for(int i = DMAP_START; i < INODE_START; i++) {
        int k_size = sizeof(char) * 8;
        uint64_t addr = get_real_addr(i);
        char *b = (char *) (addr);
        for (int k = 0; k < k_size; k++) {

            char check = 1 << k;
            if ((*b & check) == 0) {
                *b |= check;
                memset((void *) (vir_start_ad + (DATA_START + ((i - DMAP_START) * k_size + k) * BLOCK_SIZE)), 0, BLOCK_SIZE);
                spb->free_block_num--;
                return (DATA_START + ((i - DMAP_START) * k_size + k) * BLOCK_SIZE);
            }
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
            printf("分配了第%u个空闲块给第%d个inode\n", ((ip->addrs[bn] - DATA_START) / BLOCK_SIZE), ip->inum);
        }
        addr += ip->addrs[bn];
        return addr;
    }
    bn -= NDIRECT;
    uint *v_addr;
    assert(bn < BLOCK_SIZE / sizeof(uint));
    if(ip->addrs[NDIRECT] == 0) {
        ip->addrs[NDIRECT] = balloc();
        printf("分配了第%u个空闲块给第%d个inode\n", ((ip->addrs[NDIRECT] - DATA_START) / BLOCK_SIZE), ip->inum);
    }
    v_addr = (uint *)get_real_addr(ip->addrs[NDIRECT]);
    if(v_addr[bn] == 0) {
        v_addr[bn] = balloc();
        printf("分配了第%u个空闲块给第%d个inode\n", ((ip->addrs[v_addr[bn]] - DATA_START) / BLOCK_SIZE), ip->inum);
    }
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
inode* dirlookup(inode *dp, char *name, int create_new=0, int parent=0, int type=-1)
{
    assert(dp && dp->type == T_DIR);
    dirent de{};
    char name_back_up[2048];
    strcpy(name_back_up, name);
    char *p = strtok(name_back_up, "/");
    inode *p_dp = get_inode(0);
    while(p != NULL)
    {
        bzero(&de, sizeof(de));
        for(int i = 0; i < dp->size; i += sizeof(de))
        {
            readi(dp, (uint64_t)&de, i, sizeof(de));
            if(strcmp(de.name, p) == 0)
            {

                inode* temp_inode = get_inode(de.inum);
                if(type != -1 && temp_inode->type != type)
                    continue;
                p_dp = dp;
                dp = temp_inode;
                break;
            }
        }
        if(strcmp(de.name, p) != 0 || (type != -1 && de.type != type))
        {
            if(create_new == 0 || type != T_DIR)
            {
                if(parent)
                    return p_dp;
                return NULL;
            }

            else
            {
                inode *new_node = ialloc(T_DIR);
                dirent new_de{};
                strcpy(new_de.name, p);
                new_de.inum = new_node->inum;
                new_de.type = T_DIR;
                writei(dp, (uint64_t)&new_de, dp->size, sizeof(new_de));
                p_dp = dp;
                dp = new_node;
            }
        }
        p = strtok(NULL, "/");
    }
    if(parent == 1)
        dp = p_dp;
    return dp;
}

int bfree(uint va)//va是虚拟地址而不是真实地址
{
//    va = (va / BLOCK_SIZE) * BLOCK_SIZE; // round一下
    int k_size = sizeof(char) * 8;
    uint bnum = (va - DATA_START)  / BLOCK_SIZE;
    printf("释放了第%d个磁盘块\n", bnum);
    char *b = (char *)get_real_addr(DMAP_START + bnum / k_size);
    *b &= ~(1 << (bnum % k_size));
    bzero((void *)get_real_addr(va), BLOCK_SIZE);
    spb->free_block_num ++;
    return 1;
}
int ifree(inode *node)
{
    if(node->nlink > 1) {
        node->nlink --;
        return 1;
    }
    node->nlink = 0;
    node->size = 0;
    node->type = 0;
    for(int i = 0; i < NDIRECT; i++)
    {
        if(node->addrs[i] != 0)
            bfree(node->addrs[i]);
        node->addrs[i] = 0;
    }
    for(int i = NDIRECT; i < NDIRECT + NINDIRECT; i++)
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
    printf("释放了第%d个inode\n", node->inum);
    node->inum = 0;
    spb->free_inode_num ++;
    return 1;
}

void remove_file(char *name) {
    inode *zero_dp = get_inode(0);
    inode *p_dp = dirlookup(zero_dp, name, 0, 1, T_DIR);
    if (p_dp == NULL)
        return;
    inode *ip = dirlookup(zero_dp, name, 0, 0, -1);
    if (ip == NULL)
        return;


    dirent de{};
    int offset = sizeof(de);
    if (ip->type == T_DIR && ip->size > 0) {
        for (int i = 0; i < ip->size; i += offset) {
            readi(ip, (uint64_t) &de, i, offset);
            if (strlen(de.name) > 0) {
                char path[2048];
                bzero(path, sizeof(path));
                strcpy(path, name);
                path[strlen(path)] = '/';
                strcat(path, de.name);
                remove_file(path);
                i = i - offset;
            }
        }
    }

    int flag = 0;
    for (int i = 0; i < p_dp->size; i += offset) {
        readi(p_dp, (uint64_t) &de, i, offset);
        if (de.inum == ip->inum) {
            bzero(&de, sizeof(de));
            writei(p_dp, (uint64_t) &de, i, offset);
            flag = 1;
            continue;
        }
        if (flag == 1) {
            writei(p_dp, (uint64_t) &de, i - offset, offset);
        }
    }
    p_dp->size -= offset;
    ifree(ip);
};

inode *create_new_file(char *path, char *buffer)
{
    inode* zero_inode = get_inode(0);
    inode *p_inode = dirlookup(zero_inode, path, 0, 1, T_DIR);
    char file_name[2048];
    char *r_char = strrchr(path, '/');
    strcpy(file_name, r_char + 1);
    if(p_inode == NULL)
        return NULL;
    dirent de;
    bzero(&de, sizeof(de));

    inode *file_inode = NULL;
    for(int i = 0; i < p_inode->size; i += sizeof(de))
    {
        readi(p_inode, (uint64_t)&de, i, sizeof(de));
        if(strcmp(de.name, file_name) == 0)
        {
            inode* temp_inode = get_inode(de.inum);
            if(temp_inode->type != T_FILE)
                continue;
            file_inode = temp_inode;
            break;
        }
    }
    if(file_inode != NULL && file_inode->type == T_FILE)
    {
        ifree(file_inode);
        file_inode = NULL;
    }
    if(file_inode == NULL || strcpy(de.name, file_name) != file_name || file_inode->type != T_FILE)
    {
        file_inode = ialloc(T_FILE);
        dirent new_de{};
        new_de.type = T_FILE;
        strcpy(new_de.name, file_name);
        new_de.inum = file_inode->inum;
        writei(p_inode, (uint64_t)&new_de, p_inode->size, sizeof(new_de));
        writei(file_inode, (uint64_t)buffer, 0, strlen(buffer));
    }

    return file_inode;
}