#pragma once

#include <pthread.h>

#ifndef _GNU_SOURCE 
#define _GNU_SOURCE
#endif

#include <search.h>

struct nethuns_net_info {
    int promisc_refcnt;
};

struct nethuns_global
{
    pthread_mutex_t m;
    struct hsearch_data netinfo;
};

extern struct nethuns_global __nethuns_global;

extern void __attribute__((constructor)) nethuns_global_init();
extern void __attribute__((destructor)) nethuns_global_fini();

static inline
void nethuns_lock_netinfo()
{
    pthread_mutex_lock(&__nethuns_global.m);
}

static inline
void nethuns_unlock_netinfo()
{
    pthread_mutex_unlock(&__nethuns_global.m);
}

struct nethuns_net_info *nethuns_lookup_netinfo(const char *);
struct nethuns_net_info *nethuns_create_netinfo(const char *);