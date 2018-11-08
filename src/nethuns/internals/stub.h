#pragma once

#include "nethuns/internals/filepcap.h"

#ifdef NETHUNS_USE_TPACKET_V3

#include "nethuns/internals/tpacket_v3.h"

#define nethuns_open(...)           nethuns_open_tpacket_v3(__VA_ARGS__)
#define nethuns_close(...)          nethuns_close_tpacket_v3(__VA_ARGS__)
#define nethuns_bind(...)           nethuns_bind_tpacket_v3(__VA_ARGS__)
#define nethuns_fd(...)             nethuns_fd_tpacket_v3(__VA_ARGS__)
#define nethuns_recv(...)           nethuns_recv_tpacket_v3(__VA_ARGS__)
#define nethuns_flush(...)          nethuns_flush_tpacket_v3(__VA_ARGS__)
#define nethuns_send(...)           nethuns_send_tpacket_v3(__VA_ARGS__)
#define nethuns_fanout(...)         nethuns_fanout_tpacket_v3(__VA_ARGS__)

#define nethuns_tstamp_get_sec(...)     nethuns_tstamp_get_sec_tpacket_v3(__VA_ARGS__)
#define nethuns_tstamp_get_usec(...)    nethuns_tstamp_get_usec_tpacket_v3(__VA_ARGS__)
#define nethuns_tstamp_get_nsec(...)    nethuns_tstamp_get_nsec_tpacket_v3(__VA_ARGS__)
#define nethuns_tstamp_set_sec(...)     nethuns_tstamp_set_sec_tpacket_v3(__VA_ARGS__)
#define nethuns_tstamp_set_usec(...)    nethuns_tstamp_set_usec_tpacket_v3(__VA_ARGS__)
#define nethuns_tstamp_set_nsec(...)    nethuns_tstamp_set_nsec_tpacket_v3(__VA_ARGS__)

#define nethuns_snaplen(...)        nethuns_snaplen_tpacket_v3(__VA_ARGS__)
#define nethuns_len(...)            nethuns_len_tpacket_v3(__VA_ARGS__)
#define nethuns_rxhash(...)         nethuns_rxhash_tpacket_v3(__VA_ARGS__)
#define nethuns_vlan_tci(...)       nethuns_vlan_tci_tpacket_v3(__VA_ARGS__)
#define nethuns_dump_rings(...)     nethuns_dump_rings_tpacket_v3(__VA_ARGS__)
#define nethuns_get_stats(...)      nethuns_get_stats_tpacket_v3(__VA_ARGS__)

#elif defined(NETHUNS_USE_NETMAP)

#include "nethuns/internals/netmap.h"

#define nethuns_open(...)           nethuns_open_netmap(__VA_ARGS__)
#define nethuns_close(...)          nethuns_close_netmap(__VA_ARGS__)
#define nethuns_bind(...)           nethuns_bind_netmap(__VA_ARGS__)
#define nethuns_fd(...)             nethuns_fd_netmap(__VA_ARGS__)
#define nethuns_recv(...)           nethuns_recv_netmap(__VA_ARGS__)
#define nethuns_flush(...)          nethuns_flush_netmap(__VA_ARGS__)
#define nethuns_send(...)           nethuns_send_netmap(__VA_ARGS__)
#define nethuns_fanout(...)         nethuns_fanout_netmap(__VA_ARGS__)

#define nethuns_tstamp_get_sec(...)     nethuns_tstamp_get_sec_netmap(__VA_ARGS__)
#define nethuns_tstamp_get_usec(...)    nethuns_tstamp_get_usec_netmap(__VA_ARGS__)
#define nethuns_tstamp_get_nsec(...)    nethuns_tstamp_get_nsec_netmap(__VA_ARGS__)
#define nethuns_tstamp_set_sec(...)     nethuns_tstamp_set_sec_netmap(__VA_ARGS__)
#define nethuns_tstamp_set_usec(...)    nethuns_tstamp_set_usec_netmap(__VA_ARGS__)
#define nethuns_tstamp_set_nsec(...)    nethuns_tstamp_set_nsec_netmap(__VA_ARGS__)

#define nethuns_snaplen(...)        nethuns_snaplen_netmap(__VA_ARGS__)
#define nethuns_len(...)            nethuns_len_netmap(__VA_ARGS__)
#define nethuns_rxhash(...)         nethuns_rxhash_netmap(__VA_ARGS__)
#define nethuns_vlan_tci(...)       nethuns_vlan_tci_netmap(__VA_ARGS__)
#define nethuns_dump_rings(...)     nethuns_dump_rings_netmap(__VA_ARGS__)
#define nethuns_get_stats(...)      nethuns_get_stats_netmap(__VA_ARGS__)

#elif defined (NETHUNS_USE_DEVPCAP)

#include "nethuns/internals/devpcap.h"

#define nethuns_open(...)           nethuns_open_devpcap(__VA_ARGS__)
#define nethuns_close(...)          nethuns_close_devpcap(__VA_ARGS__)
#define nethuns_bind(...)           nethuns_bind_devpcap(__VA_ARGS__)
#define nethuns_fd(...)             nethuns_fd_devpcap(__VA_ARGS__)
#define nethuns_recv(...)           nethuns_recv_devpcap(__VA_ARGS__)
#define nethuns_flush(...)          nethuns_flush_devpcap(__VA_ARGS__)
#define nethuns_send(...)           nethuns_send_devpcap(__VA_ARGS__)
#define nethuns_fanout(...)         nethuns_fanout_devpcap(__VA_ARGS__)

#define nethuns_tstamp_get_sec(...)     nethuns_tstamp_get_sec_devpcap(__VA_ARGS__)
#define nethuns_tstamp_get_usec(...)    nethuns_tstamp_get_usec_devpcap(__VA_ARGS__)
#define nethuns_tstamp_get_nsec(...)    nethuns_tstamp_get_nsec_devpcap(__VA_ARGS__)
#define nethuns_tstamp_set_sec(...)     nethuns_tstamp_set_sec_devpcap(__VA_ARGS__)
#define nethuns_tstamp_set_usec(...)    nethuns_tstamp_set_usec_devpcap(__VA_ARGS__)
#define nethuns_tstamp_set_nsec(...)    nethuns_tstamp_set_nsec_devpcap(__VA_ARGS__)

#define nethuns_snaplen(...)        nethuns_snaplen_devpcap(__VA_ARGS__)
#define nethuns_len(...)            nethuns_len_devpcap(__VA_ARGS__)
#define nethuns_rxhash(...)         nethuns_rxhash_devpcap(__VA_ARGS__)
#define nethuns_vlan_tci(...)       nethuns_vlan_tci_devpcap(__VA_ARGS__)
#define nethuns_dump_rings(...)     nethuns_dump_rings_devpcap(__VA_ARGS__)
#define nethuns_get_stats(...)      nethuns_get_stats_devpcap(__VA_ARGS__)

#else

#error "Nethuns: socket type not specified!"

#endif
