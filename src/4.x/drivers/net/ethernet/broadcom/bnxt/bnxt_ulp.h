/* Broadcom NetXtreme-C/E network driver.
 *
 * Copyright (c) 2016-2017 Broadcom Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 */

#ifndef BNXT_ULP_H
#define BNXT_ULP_H

#include <linux/version.h>

#define BNXT_ROCE_ULP	0
#define BNXT_OTHER_ULP	1
#define BNXT_MAX_ULP	2

#define BNXT_MIN_ROCE_CP_RINGS	2
#define BNXT_MIN_ROCE_STAT_CTXS	1

struct hwrm_async_event_cmpl;
struct bnxt;

/* Change BNXT_ULP_VERSION in bnxt.h if any structure or API exposed to
 * bnxt_re is modified.
 */
struct bnxt_ulp_ops {
	/* async_notifier() cannot sleep (in BH context) */
	void (*ulp_async_notifier)(void *, struct hwrm_async_event_cmpl *);
	void (*ulp_stop)(void *);
	void (*ulp_start)(void *);
	void (*ulp_sriov_config)(void *, int);
	void (*ulp_shutdown)(void *);
};

struct bnxt_msix_entry {
	u32	vector;
	u32	ring_idx;
	u32	db_offset;
};

struct bnxt_fw_msg {
	void	*msg;
	int	msg_len;
	void	*resp;
	int	resp_max_len;
	int	timeout;
};

struct bnxt_ulp {
	void		*handle;
	struct bnxt_ulp_ops __rcu *ulp_ops;
	unsigned long	*async_events_bmap;
	u16		max_async_event_id;
	u16		msix_requested;
	atomic_t	ref_count;
};

struct bnxt_en_dev {
	struct net_device *net;
	struct pci_dev *pdev;
	u32 flags;
	#define BNXT_EN_FLAG_ROCEV1_CAP		0x1
	#define BNXT_EN_FLAG_ROCEV2_CAP		0x2
	#define BNXT_EN_FLAG_ROCE_CAP		(BNXT_EN_FLAG_ROCEV1_CAP | \
						 BNXT_EN_FLAG_ROCEV2_CAP)
	const struct bnxt_en_ops	*en_ops;
	struct bnxt_ulp			ulp_tbl[BNXT_MAX_ULP];
};

struct bnxt_en_ops {
	int (*bnxt_register_device)(struct bnxt_en_dev *, int,
				    struct bnxt_ulp_ops *, void *);
	int (*bnxt_unregister_device)(struct bnxt_en_dev *, int);
	int (*bnxt_request_msix)(struct bnxt_en_dev *, int,
				 struct bnxt_msix_entry *, int);
	int (*bnxt_free_msix)(struct bnxt_en_dev *, int);
	int (*bnxt_send_fw_msg)(struct bnxt_en_dev *, int,
				struct bnxt_fw_msg *);
	int (*bnxt_register_fw_async_events)(struct bnxt_en_dev *, int,
					     unsigned long *, u16);
};

static inline bool bnxt_ulp_registered(struct bnxt_en_dev *edev, int ulp_id)
{
	if (edev && rcu_access_pointer(edev->ulp_tbl[ulp_id].ulp_ops))
		return true;
	return false;
}

void bnxt_subtract_ulp_resources(struct bnxt *bp, int ulp_id);
void bnxt_ulp_stop(struct bnxt *bp);
void bnxt_ulp_start(struct bnxt *bp);
void bnxt_ulp_sriov_cfg(struct bnxt *bp, int num_vfs);
void bnxt_ulp_shutdown(struct bnxt *bp);
void bnxt_ulp_async_events(struct bnxt *bp, struct hwrm_async_event_cmpl *cmpl);
struct bnxt_en_dev *bnxt_ulp_probe(struct net_device *dev);
#endif
