/*
 * Copyright (c) 2018-2019, NVIDIA CORPORATION.  All rights reserved.
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

#ifndef NVGPU_GR_TU104_H
#define NVGPU_GR_TU104_H

#ifdef CONFIG_NVGPU_DEBUGGER

#include <nvgpu/types.h>

struct gk20a;

int gr_tu104_init_sw_bundle64(struct gk20a *g);
void gr_tu10x_create_sysfs(struct gk20a *g);
void gr_tu10x_remove_sysfs(struct gk20a *g);
int gr_tu104_get_offset_in_gpccs_segment(struct gk20a *g,
	enum ctxsw_addr_type addr_type, u32 num_tpcs, u32 num_ppcs,
	u32 reg_list_ppc_count, u32 *__offset_in_segment);
void gr_tu104_init_sm_dsm_reg_info(void);
void gr_tu104_get_sm_dsm_perf_ctrl_regs(struct gk20a *g,
	u32 *num_sm_dsm_perf_ctrl_regs, u32 **sm_dsm_perf_ctrl_regs,
	u32 *ctrl_register_stride);
int tu104_gr_update_smpc_global_mode(struct gk20a *g, bool enable);
#endif /* CONFIG_NVGPU_DEBUGGER */
#endif /* NVGPU_GR_TU104_H */
