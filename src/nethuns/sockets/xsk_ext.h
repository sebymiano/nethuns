// Copyright 2021 Larthia, University of Pisa. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#pragma once

#include <xdp/libxdp.h>
#include <xdp/xsk.h>

#include <stdint.h>

#define XSK_INVALID_UMEM_FRAME UINT64_MAX

struct xsk_umem_info {
	struct xsk_ring_prod fq;
	struct xsk_ring_cons cq;
	struct xsk_umem *umem;
	void *buffer;
};

struct xsk_socket_info {
	struct xsk_ring_cons rx;
	struct xsk_ring_prod tx;
	struct xsk_umem_info *umem;
	struct xsk_socket *xsk;

	uint32_t outstanding_tx;

	unsigned long rx_npkts;
	unsigned long tx_npkts;
	
	uint32_t umem_frame_free;
	uint64_t umem_frame_addr[];
};

struct nethuns_socket_xdp;

struct xsk_umem_info *
xsk_configure_umem(struct nethuns_socket_xdp *sock, void *buffer, size_t size, size_t frame_size);

int 
xsk_populate_fill_ring(struct nethuns_socket_xdp *sock, size_t frame_size);

struct xsk_socket_info *
xsk_configure_socket(struct nethuns_socket_xdp *sock);

int 
xsk_enter_into_map(struct nethuns_socket_xdp *sock, int queue);

static inline __u32 xsk_ring_prod__free(struct xsk_ring_prod *r)
{
	r->cached_cons = *r->consumer + r->size;
	return r->cached_cons - r->cached_prod;
}

static uint64_t  
__maybe_unused xsk_alloc_umem_frame(struct xsk_socket_info *xsk)
{
	uint64_t frame;
	if (xsk->umem_frame_free == 0)
		return XSK_INVALID_UMEM_FRAME;

	frame = xsk->umem_frame_addr[--xsk->umem_frame_free];
	xsk->umem_frame_addr[xsk->umem_frame_free] = XSK_INVALID_UMEM_FRAME;
	return frame;
}

static void 
__maybe_unused xsk_free_umem_frame(struct xsk_socket_info *xsk, uint64_t frame)
{
	// assert(xsk->umem_frame_free < XSK_NUM_FRAMES);
	xsk->umem_frame_addr[xsk->umem_frame_free++] = frame;
}

static uint64_t 
__maybe_unused xsk_umem_free_frames(struct xsk_socket_info *xsk)
{
	return xsk->umem_frame_free;
}
