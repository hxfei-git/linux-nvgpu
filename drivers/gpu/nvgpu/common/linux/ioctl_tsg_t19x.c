/*
 * GV11B TSG IOCTL Handler
 *
 * Copyright (c) 2017, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#include <linux/types.h>
#include <uapi/linux/nvgpu.h>

#include "gk20a/gk20a.h"

#include "gv11b/fifo_gv11b.h"
#include "gv11b/subctx_gv11b.h"
#include "ioctl_tsg_t19x.h"
#include "common/linux/os_linux.h"

static int gv11b_tsg_ioctl_bind_channel_ex(struct gk20a *g,
	struct tsg_gk20a *tsg, struct nvgpu_tsg_bind_channel_ex_args *arg)
{
	struct nvgpu_os_linux *l = nvgpu_os_linux_from_gk20a(g);
	struct gk20a_sched_ctrl *sched = &l->sched_ctrl;
	struct channel_gk20a *ch;
	struct gr_gk20a *gr = &g->gr;
	int err = 0;

	nvgpu_log(g, gpu_dbg_fn | gpu_dbg_sched, "tsgid=%u", tsg->tsgid);

	nvgpu_mutex_acquire(&sched->control_lock);
	if (sched->control_locked) {
		err = -EPERM;
		goto mutex_release;
	}
	err = gk20a_busy(g);
	if (err) {
		nvgpu_err(g, "failed to power on gpu");
		goto mutex_release;
	}

	ch = gk20a_get_channel_from_file(arg->channel_fd);
	if (!ch) {
		err = -EINVAL;
		goto idle;
	}

	if (arg->tpc_pg_enabled && (!tsg->t19x.tpc_num_initialized)) {
		if ((arg->num_active_tpcs > gr->max_tpc_count) ||
				!(arg->num_active_tpcs)) {
			nvgpu_err(g, "Invalid num of active TPCs");
			err = -EINVAL;
			goto ch_put;
		}
		tsg->t19x.tpc_num_initialized = true;
		tsg->t19x.num_active_tpcs = arg->num_active_tpcs;
		tsg->t19x.tpc_pg_enabled = true;
	} else {
		tsg->t19x.tpc_pg_enabled = false;
		nvgpu_log(g, gpu_dbg_info, "dynamic TPC-PG not enabled");
	}

	if (arg->subcontext_id < g->fifo.t19x.max_subctx_count) {
		ch->t19x.subctx_id = arg->subcontext_id;
	} else {
		err = -EINVAL;
		goto ch_put;
	}

	nvgpu_log(g, gpu_dbg_info, "channel id : %d : subctx: %d",
				ch->chid, ch->t19x.subctx_id);

	/* Use runqueue selector 1 for all ASYNC ids */
	if (ch->t19x.subctx_id > CHANNEL_INFO_VEID0)
		ch->t19x.runqueue_sel = 1;

	err = ch->g->ops.fifo.tsg_bind_channel(tsg, ch);
ch_put:
	gk20a_channel_put(ch);
idle:
	gk20a_idle(g);
mutex_release:
	nvgpu_mutex_release(&sched->control_lock);
	return err;
}

int t19x_tsg_ioctl_handler(struct gk20a *g, struct tsg_gk20a *tsg,
			unsigned int cmd, u8 *buf)
{
	int err = 0;

	nvgpu_log(g, gpu_dbg_fn, "t19x_tsg_ioctl_handler");

	switch (cmd) {
	case NVGPU_TSG_IOCTL_BIND_CHANNEL_EX:
	{
		err = gv11b_tsg_ioctl_bind_channel_ex(g, tsg,
			(struct nvgpu_tsg_bind_channel_ex_args *)buf);
		break;
	}

	default:
		nvgpu_err(g, "unrecognized tsg gpu ioctl cmd: 0x%x",
			   cmd);
		err = -ENOTTY;
		break;
	}
	return err;
}
