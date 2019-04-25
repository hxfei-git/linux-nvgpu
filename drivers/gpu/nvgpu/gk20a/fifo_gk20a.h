/*
 * GK20A graphics fifo (gr host)
 *
 * Copyright (c) 2011-2019, NVIDIA CORPORATION.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#ifndef FIFO_GK20A_H
#define FIFO_GK20A_H

#include <nvgpu/types.h>
#include <nvgpu/kref.h>
#include <nvgpu/fifo.h>
#include <nvgpu/engines.h>
#include <nvgpu/runlist.h>

struct gk20a_debug_output;
struct nvgpu_semaphore;
struct channel_gk20a;
struct tsg_gk20a;

#define FIFO_INVAL_ENGINE_ID		(~U32(0U))
#define FIFO_INVAL_MMU_ID		(~U32(0U))
#define FIFO_INVAL_CHANNEL_ID		(~U32(0U))
#define FIFO_INVAL_TSG_ID		(~U32(0U))
#define FIFO_INVAL_RUNLIST_ID		(~U32(0U))
#define FIFO_INVAL_SYNCPT_ID		(~U32(0U))

/*
 * Number of entries in the kickoff latency buffer, used to calculate
 * the profiling and histogram. This number is calculated to be statistically
 * significative on a histogram on a 5% step
 */
#ifdef CONFIG_DEBUG_FS
#define FIFO_PROFILING_ENTRIES	16384U
#endif

enum {
	PROFILE_IOCTL_ENTRY = 0U,
	PROFILE_ENTRY,
	PROFILE_JOB_TRACKING,
	PROFILE_APPEND,
	PROFILE_END,
	PROFILE_IOCTL_EXIT,
	PROFILE_MAX
};

struct fifo_profile_gk20a {
	u64 timestamp[PROFILE_MAX];
};

struct fifo_gk20a {
	struct gk20a *g;
	unsigned int num_channels;
	unsigned int runlist_entry_size;
	unsigned int num_runlist_entries;

	unsigned int num_pbdma;
	u32 *pbdma_map;

	struct nvgpu_engine_info *engine_info;
	u32 max_engines;
	u32 num_engines;
	u32 *active_engines_list;

	/* Pointers to runlists, indexed by real hw runlist_id.
	 * If a runlist is active, then runlist_info[runlist_id] points
	 * to one entry in active_runlist_info. Otherwise, it is NULL.
	 */
	struct nvgpu_runlist_info **runlist_info;
	u32 max_runlists;

	/* Array of runlists that are actually in use */
	struct nvgpu_runlist_info *active_runlist_info;
	u32 num_runlists; /* number of active runlists */
#ifdef CONFIG_DEBUG_FS
	struct {
		struct fifo_profile_gk20a *data;
		nvgpu_atomic_t get;
		bool enabled;
		u64 *sorted;
		struct nvgpu_ref ref;
		struct nvgpu_mutex lock;
	} profile;
#endif
	struct nvgpu_mutex userd_mutex;
	struct nvgpu_mem *userd_slabs;
	u32 num_userd_slabs;
	u32 num_channels_per_slab;
	u32 userd_entry_size;
	u64 userd_gpu_va;

	unsigned int used_channels;
	struct channel_gk20a *channel;
	/* zero-kref'd channels here */
	struct nvgpu_list_node free_chs;
	struct nvgpu_mutex free_chs_mutex;
	struct nvgpu_mutex engines_reset_mutex;
	struct nvgpu_spinlock runlist_submit_lock;

	struct tsg_gk20a *tsg;
	struct nvgpu_mutex tsg_inuse_mutex;

	void (*remove_support)(struct fifo_gk20a *f);
	bool sw_ready;
	struct {
		/* share info between isrs and non-isr code */
		struct {
			struct nvgpu_mutex mutex;
		} isr;
		struct {
			u32 device_fatal_0;
			u32 channel_fatal_0;
			u32 restartable_0;
		} pbdma;
		struct {

		} engine;


	} intr;

	unsigned long deferred_fault_engines;
	bool deferred_reset_pending;
	struct nvgpu_mutex deferred_reset_mutex;

	u32 max_subctx_count;
	u32 channel_base;
};

int gk20a_init_fifo_reset_enable_hw(struct gk20a *g);
int gk20a_init_fifo_setup_hw(struct gk20a *g);
void gk20a_fifo_bar1_snooping_disable(struct gk20a *g);
int gk20a_fifo_init_pbdma_map(struct gk20a *g, u32 *pbdma_map, u32 num_pbdma);
u32 gk20a_fifo_get_runlist_timeslice(struct gk20a *g);
u32 gk20a_fifo_get_pb_timeslice(struct gk20a *g);

#ifdef CONFIG_DEBUG_FS
struct fifo_profile_gk20a *gk20a_fifo_profile_acquire(struct gk20a *g);
void gk20a_fifo_profile_release(struct gk20a *g,
	struct fifo_profile_gk20a *profile);
void gk20a_fifo_profile_snapshot(struct fifo_profile_gk20a *profile, int idx);
#else
static inline struct fifo_profile_gk20a *
gk20a_fifo_profile_acquire(struct gk20a *g)
{
	return NULL;
}
static inline void gk20a_fifo_profile_release(struct gk20a *g,
	struct fifo_profile_gk20a *profile)
{
}
static inline void gk20a_fifo_profile_snapshot(
		struct fifo_profile_gk20a *profile, int idx)
{
}
#endif

#endif /* FIFO_GK20A_H */
