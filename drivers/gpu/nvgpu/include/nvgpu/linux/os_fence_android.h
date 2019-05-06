/*
 * Copyright (c) 2018, NVIDIA CORPORATION.  All rights reserved.
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

#ifndef __NVGPU_OS_FENCE_ANDROID_H__
#define __NVGPU_OS_FENCE_ANDROID_H__

struct gk20a;
struct nvgpu_os_fence;
struct sync_fence;
struct nvgpu_channel;

struct sync_fence *nvgpu_get_sync_fence(struct nvgpu_os_fence *s);

void nvgpu_os_fence_android_drop_ref(struct nvgpu_os_fence *s);

int nvgpu_os_fence_sema_fdget(struct nvgpu_os_fence *fence_out,
	struct nvgpu_channel *c, int fd);

void nvgpu_os_fence_init(struct nvgpu_os_fence *fence_out,
	struct gk20a *g, const struct nvgpu_os_fence_ops *fops,
	struct sync_fence *fence);

void nvgpu_os_fence_android_install_fd(struct nvgpu_os_fence *s, int fd);

int nvgpu_os_fence_syncpt_fdget(
	struct nvgpu_os_fence *fence_out,
	struct nvgpu_channel *c, int fd);

#endif /* __NVGPU_OS_FENCE_ANDROID_H__ */