/**
 * Copyright 2021 Larthia, University of Pisa. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#ifdef NETHUNS_SOCKET
#undef NETHUNS_SOCKET
#endif

#define NETHUNS_SOCKET NETHUNS_SOCKET_XDP

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#include "xdp.h"

#include <bpf/bpf.h>
#include <bpf/libbpf.h>

#include <xdp/libxdp.h>
#include <xdp/xsk.h>

#include "xsk_ext.h"

#include "../api.h"

struct xsk_umem_info *
xsk_configure_umem(
	  struct nethuns_socket_xdp *sock
	, void *buffer
	, size_t size
	, size_t frame_size)
{
	struct xsk_umem_info *umem;
	struct xsk_umem_config cfg = {
		/* We recommend that you set the fill ring size >= HW RX ring size +
		 * AF_XDP RX ring size. Make sure you fill up the fill ring
		 * with buffers at regular intervals, and you will with this setting
		 * avoid allocation failures in the driver. These are usually quite
		 * expensive since drivers have not been written to assume that
		 * allocation failures are common. For regular sockets, kernel
		 * allocated memory is used that only runs out in OOM situations
		 * that should be rare.
		 */
		.fill_size = (sock->base.rx_ring.mask + 1) * 2,
		.comp_size = sock->base.tx_ring.mask + 1,
		.frame_size = frame_size,
		.frame_headroom = XSK_UMEM__DEFAULT_FRAME_HEADROOM,
		.flags = 0 // opt_umem_flags -> HUGEFPAGE?
	};

	int ret;

#ifdef DEBUG
	printf("buffer %p size %ld (%lx) frame_size %ld (%lx)\n",
			buffer, size, size, frame_size, frame_size);
#endif

	umem = calloc(1, sizeof(*umem));
	if (!umem) {
        nethuns_perror(nethuns_socket(sock)->errbuf, "xsk_config_umem: could not allocate memory");
		return NULL;
    }

	ret = xsk_umem__create(&umem->umem, buffer, size, &umem->fq, &umem->cq, &cfg);
	if (ret) {
        nethuns_perror(nethuns_socket(sock)->errbuf, "xsk_config_umem: could not create umem");
        return NULL;
    }

	umem->buffer = buffer;
	return umem;
}

struct xsk_socket_info *
xsk_configure_socket(struct nethuns_socket_xdp *sock)
{
	struct xsk_socket_config cfg;
	struct xsk_socket_info *xsk;
	unsigned int ret;
    unsigned int i;
    unsigned int idx;

	xsk = calloc(1, sizeof(*xsk));
	if (!xsk)
		return NULL;

	xsk->umem = sock->umem;
	cfg.rx_size = sock->base.rx_ring.mask + 1;
	cfg.tx_size = sock->base.tx_ring.mask + 1;

	nethuns_fprintf(stderr, "RX size: %d\n", cfg.rx_size);
	nethuns_fprintf(stderr, "TX size: %d\n", cfg.tx_size);

	cfg.libbpf_flags = nethuns_socket(sock)->opt.xdp_prog != NULL
						? XSK_LIBBPF_FLAGS__INHIBIT_PROG_LOAD
						: 0;

	cfg.xdp_flags = sock->xdp_flags;
	cfg.bind_flags = 0;

	ret = xsk_socket__create(&xsk->xsk, nethuns_socket(sock)->devname, nethuns_socket(sock)->queue, sock->umem->umem,
			sock->rx ? &xsk->rx : NULL,
			sock->tx ? &xsk->tx : NULL,
			&cfg);

	if (ret) {
        nethuns_perror(nethuns_socket(sock)->errbuf, "xsk_config: could not create socket");
		goto err;
	}

	ret = xsk_ring_prod__reserve(&xsk->umem->fq, cfg.rx_size * 2, &idx);

	if (ret != (cfg.rx_size * 2)) {
        nethuns_perror(nethuns_socket(sock)->errbuf, "xsk_config: could not reserve slots in fill ring");
		goto err;
	}

	for (i = 0; i < (cfg.rx_size * 2); i ++) {
		*xsk_ring_prod__fill_addr(&xsk->umem->fq, idx) = rx_frame2(sock, idx);
		idx++;
	}

	xsk_ring_prod__submit(&xsk->umem->fq, cfg.rx_size * 2);

	return xsk;
err:
	free(xsk);
	return NULL;
}

int
xsk_enter_into_map(struct nethuns_socket_xdp *sock, int queue)
{
	struct bpf_map *map;
	int xdp_map;

	// Load the xsk map. A set entry in this map means that an active AF_XDP socket is bound to the corresponding device and queue (see nethuns_bind_xdp).
	if (!nethuns_socket(sock)->opt.xsk_map_name)
		nethuns_socket(sock)->opt.xsk_map_name = "xsk_map";

	map = bpf_object__find_map_by_name(sock->obj, nethuns_socket(sock)->opt.xsk_map_name);
	xdp_map = bpf_map__fd(map);
	if (xdp_map < 0) {
        nethuns_perror(nethuns_socket(sock)->errbuf, "xsk_enter_into_map: couldn't find xsk map: %s", strerror(xdp_map));
		return -1;
	}

	int fd = xsk_socket__fd(sock->xsk->xsk);
	int key, ret;

	key = queue;
	ret = bpf_map_update_elem(xdp_map, &key, &fd, 0);
	if (ret) {
        nethuns_perror(nethuns_socket(sock)->errbuf, "xsk_enter_into_map: bfp_map_update_elem");
		return -1;
	}

	return 0;
}
