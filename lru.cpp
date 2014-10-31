/***************************************************************************
*
* Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
*
**************************************************************************/



/**
* @file lru.cpp
* @author zhangzhao01(com@baidu.com)
* @date 2014/10/11 20:26:07
* @brief
* 
**/
#include<iostream>
using namespace std;


//hash_map define
typedef struct hashmap* hashmap_t;
struct hashmap_node
{
    int key;
    void* value;
};

struct hashmap
{
    unsigned int  size;
    struct hashmap_node* table[0];
  //  struct hashmap_node** table;
   // struct hashmap_node** table;
};

//table = new hashmap_node*[size];

hashmap_t hashmap_create(unsigned int size)
{
    if(size<0)
    {
        //避免内存泄露
        return NULL;
    }
    //hashmap_t map = (hashmap_t)calloc(1,sizeof(*map)+size*sizeof(struct hashmap_node));
    struct hashmap* map = (struct hashmap*)calloc(1,sizeof(*map)+size*sizeof(struct hashmap_node));
    map->size = size;
   // map->table = (struct hashmap_node**)malloc(size);
    //memset(map->table,0,size);
   
    for(int i=0;i<size;i++)
    {
        struct hashmap_node* new_hashmap_node = (struct hashmap_node* )calloc(1,sizeof(*new_hashmap_node));
        map->table[i] = new_hashmap_node;
    }
    return map;
}

int
hashmap_add(hashmap_t map,int key,void* value)
{
    if(map==NULL)
        return -1;
    int i,count;
    unsigned int size = map->size;
    for(i=key%size,count=0;count<size;i=(i+1)%size,count++)
    {
        if(map->table[i]->value == NULL)
        {
            map->table[i]->key = key;
            map->table[i]->value = value;
            return 0;
        }
        if(map->table[i]->key == key)
        {
            //insert the same element
            return -1;
        }
   
    }
    //hashtable is full
    return -2;
}

void*
hashmap_find(hashmap_t map,int key)
{
    if(map == NULL)
    {
        return NULL;
    }
    int i,count;
    unsigned int size = map->size;
    for(i=key%size,count=0;count<size;i=(i+1)%size,count++)
    {
        if(map->table[i]->value != NULL && map->table[i]->key == key)
        {
            return map->table[i]->value;

        }
    }
    return NULL;
}

void *
hashmap_remove(hashmap_t map,int key)
{
    int i,count;
    int size = map->size;
    void* value = NULL;
    for(i=key%size,count=0;count<size;i=(i+1)%size,count++)
    {
        if(map->table[i]->value != NULL && map->table[i]->key == key)
        {
            value = map->table[i]->value;
            map->table[i]->value = NULL;
            map->table[i]->key = 0;
            return value;
        }
    }
    return NULL;
}

int
hashmap_destory(hashmap_t map)
{
    free(map);
    return 0;
}


/////////////////////////////////////////////////
typedef struct dblist* dblist_t;

struct dblist_node
{
    int key;
    void* value;
    struct dblist_node* prev;
    struct dblist_node* next;
      
};


struct dblist
{
    unsigned long used;
    unsigned long total;
    struct dblist_node* head;
    struct dblist_node* tail;
};

struct dblist_node*
dbnode_create(int key,void* value)
{
    struct dblist_node* newNode = (struct dblist_node*)calloc(1,sizeof(*newNode));
    if(newNode == NULL)
    {
        return NULL;
    }
    newNode->key = key;
    newNode->value = value;
    newNode->prev = NULL;
    newNode->next = NULL;
    return newNode;
}

dblist_t
dblist_create(unsigned int size)
{
    dblist_t list = (dblist_t)calloc(1,sizeof(*list));
    if(list == NULL || size <0)
    {
        return NULL;
    }
    list->total = size;
    list->used = 0;
    struct dblist_node* head = (struct dblist_node*)calloc(1,sizeof(*head));
    struct dblist_node* tail = (struct dblist_node*)calloc(1,sizeof(*tail));
    list->head = head;
    list->tail = tail;

    list->head->next = NULL;
    list->tail->next = NULL;

    return list;   
}
struct dblist_node*
dblist_tail(dblist_t list)
{
    if(list == NULL)
    {
        return NULL;
    }
    return list->tail->next;
}

int
dblist_addhead(dblist_t list,struct dblist_node* node)
{
    if(list == NULL || node == NULL)
    {
        return -1;
    }
    if(list->used ==0)
    {
        //firt node
        node->next = NULL;
        list->head->next = list->tail->next = node;
    }
    else
    {
        node->next = list->head->next;
        list->head->next->prev = node; 
        list->head->next = node;
   
    }
    node->prev = NULL;
    list->used++;
   
    return 0;
}


int
dblist_removeNode(dblist_t list,struct dblist_node* node)
{
    if(list == NULL)
    {
        return -1;
    }
    struct dblist_node* nextNode = node->next;
    struct dblist_node* prevNode = node->prev;
   
    //tail
    if(node == list->tail->next)
    {
        node->prev->next = NULL;
        list->tail->next = node->prev;
    }
    //head
  else if(node == list->head->next)
    {
        cout<<"aaaaaaaaa"<<endl;
        node->prev->next = NULL;
        list->head->next = list->tail->next = NULL;
    }
    else
    {  
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
    list->used--;
    node->next = NULL;
    node->prev = NULL;
    free(node);
    return 0;
}

///////////////////////////////////
void*
lru_put(dblist_t list,hashmap_t map,int key,void* value)
{
    //if(list == NULL || map == NULL || key == NULL)
    if(list == NULL || map == NULL )
    {  
        return NULL;
    }
    struct dblist_node* node = (struct dblist_node*)hashmap_find(map,key);
   
    if(node == NULL)
    {
        //create the node
        node = (struct dblist_node*)dbnode_create(key,value);
        node->prev = NULL;
        node->next = NULL;

        //update the hashmap
        hashmap_add(map,key,node);
        if(list->used >= list->total)
        {
            //delete the tail node
            struct dblist_node*  tailNode = dblist_tail(list);
            hashmap_remove(map,tailNode->key);
            dblist_removeNode(list,tailNode);
        }
                  
    }
    else
    {
        //if exist ,so update the value
        node->value = value;
        dblist_removeNode(list,node);
    }
    //take the node to head
    dblist_addhead(list,node);
    return node;
}

void print_hashmap(hashmap_t map)
{
    for(int i=0;i<map->size;i++)
    {
        cout<<map->table[i]->key<<endl;
    }
}

void print_list(dblist_t list)
{
    if(list == NULL || list->head == NULL)
    {
        return ;
    }
    struct dblist_node* node =list->head->next;
    while(node)
    {
        cout<<node->key<<endl;
        node = node->next;
    }
}
struct dblist_node*
lru_get(dblist_t list,hashmap_t map,int key)
{
    //if(list == NULL || map == NULL || key == NULL)
    if(list == NULL || map == NULL )
    {
        return NULL;
    }
    struct dblist_node* node = (struct dblist_node*)hashmap_find(map,key);
    if(node == NULL)
    {
        //没有找到
        return NULL;
    }
    else
    {
        //调整dblist
        int key = node->key;
        void* value = node->value;

        dblist_removeNode(list,node);
        node = dbnode_create(key,value);
        dblist_addhead(list,node);

    }
    return node;
}

int lru_del(dblist_t list,hashmap_t map,int key)
{
    if(list == NULL || map == NULL)
    {
        return -1;
    }
    struct dblist_node* node = (struct dblist_node*)hashmap_find(map,key);
    struct dblist_node*  tailNode = dblist_tail(list);
    dblist_removeNode(list,node);
    hashmap_remove(map,key);
    return 0;

}

int main()
{
    hashmap_t map = hashmap_create(10);
    dblist_t list = dblist_create(3);
    char aa[3] = "aa";
    char bb[3] = "bb";
    char cc[3]= "cc";
    char dd[3]= "dd";
    lru_put(list,map,1,aa);
    lru_put(list,map,2,bb);
    lru_put(list,map,3,cc);
    lru_put(list,map,4,dd);
    //lru_put(list,map,5,&dd);
    //lru_put(list,map,6,&dd);
    //lru_put(list,map,7,&dd);
    //cout<<"======================="<<endl;
    //print_hashmap(map);
   print_list(list);
   cout<<"============="<<endl;
   struct dblist_node* node = lru_get(list,map,3);
   if(node != NULL)
   {
       cout<<"key is "<<node->key<<",value is "<<(char*)node->value<<endl;
   }
   else
   {
    cout<<"not found the node"<<endl;
   }
   print_list(list);
   cout<<"==============="<<endl;
   node = lru_get(list,map,4);
   print_list(list);
    cout<<"============="<<endl;
  lru_put(list,map,7,&dd);
  print_list(list);
  cout<<"============"<<endl;
  node = lru_get(list,map,3);
  cout<<"key is "<<node->key<<",value is "<<(char*)node->value<<endl;

  print_list(list);
    cout<<"=============="<<endl;
    lru_del(list,map,7);
    print_list(list);
    cout<<"=============="<<endl;
   print_hashmap(map);
//  node = lru_get(list,map,3);
  // print_list(list);
   // lru_put(list,map,2,&bb);

    //char* value = (char*)lru_get(list,map,1);
    //cout<<value<<endl;
}




/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
