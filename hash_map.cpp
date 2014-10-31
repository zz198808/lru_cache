/***************************************************************************
*
* Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
*
**************************************************************************/



/**
* @file hash_map.cpp
* @author zhangzhao01(com@baidu.com)
* @date 2014/07/22 14:49:30
* @brief
* 
**/
#include<iostream>
#include <string.h>
using namespace std;

typedef struct hashmap_node
{
    int key;
    void* value;
}hashmap_node_s;

typedef struct hashmap
{
    int size;
    hashmap_node_s table[0];
}hashmap_s;

typedef struct hashmap *hashmap_t;

hashmap_t hashmap_create(int size)
{
    hashmap_t map = NULL;
    map = (hashmap_t)malloc(sizeof(hashmap_s)+sizeof(hashmap_node_s)*size);
    map->size = size;
    memset(map->table,0,size*sizeof(hashmap_node_s));
    return map;
}

void* hashmap_find(hashmap_t map,int key)
{
    int i,count;
     //开放地址发解决冲突
    for(i=key%map->size,count=0;count<map->size;i=(i+1)%map->size,count++)
    {
        if(map->table[i].value!=NULL && map->table[i].key==key)
        {
            return map->table[i].value;
        }
    }
    return NULL;
}

int hashmap_add(hashmap_t map,int key,void* value)
{
    int i,count;
//开放地址发解决冲突，往下再进行一次hash
    for(i=key%map->size,count=0;count<map->size;i=(i+1)%map->size,count++)
    {
        if(map->table[i].value==NULL)
        {
            map->table[i].key = key;
            map->table[i].value = value;
            return 0;
        }
        else
        {
            if(map->table[i].key==key)
            {
                return -1;
            }
        }

    }
    return -1;
}


int main()
{
    hashmap_t map = hashmap_create(5);
    //string s[]={"aaa","bbb","ccc"};
    char* str1="aaa";
    char* str2="bbb";
    char* str3="ccc";
    hashmap_add(map,1,str1);
    hashmap_add(map,2,str2);
    hashmap_add(map,3,str3);
    cout<<(char*)hashmap_find(map,1)<<endl;
    cout<<(char*)hashmap_find(map,2)<<endl;
    cout<<(char*)hashmap_find(map,3)<<endl;
    return 0;
}

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
