/* Broadcom NetXtreme-C/E network driver.
 *
 * Copyright (c) 2016-2017 Broadcom Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 */

#ifndef BNXT_VFR_H
#define BNXT_VFR_H

#ifdef CONFIG_VF_REPS

#define	MAX_CFA_CODE			65536

void bnxt_vf_reps_destroy(struct bnxt *bp);
void bnxt_vf_reps_close(struct bnxt *bp);
void bnxt_vf_reps_open(struct bnxt *bp);
void bnxt_vf_rep_rx(struct bnxt *bp, struct sk_buff *skb);
struct net_device *bnxt_get_vf_rep(struct bnxt *bp, u16 cfa_code);

static inline u16 bnxt_vf_rep_get_fid(struct net_device *dev)
{
	struct bnxt_vf_rep *vf_rep = netdev_priv(dev);
	struct bnxt *bp = vf_rep->bp;

	return bp->pf.vf[vf_rep->vf_idx].fw_fid;
}

int bnxt_dl_eswitch_mode_get(struct devlink *devlink, u16 *mode);
int bnxt_dl_eswitch_mode_set(struct devlink *devlink, u16 mode);

#else

static inline void bnxt_vf_reps_destroy(struct bnxt *bp)
{
}

static inline void bnxt_vf_reps_close(struct bnxt *bp)
{
}

static inline void bnxt_vf_reps_open(struct bnxt *bp)
{
}

static inline void bnxt_vf_rep_rx(struct bnxt *bp, struct sk_buff *skb)
{
}

static inline struct net_device *bnxt_get_vf_rep(struct bnxt *bp, u16 cfa_code)
{
	return NULL;
}

static inline u16 bnxt_vf_rep_get_fid(struct net_device *dev)
{
	return 0;
}
#endif /* CONFIG_VF_REPS */
#endif /* BNXT_VFR_H */
