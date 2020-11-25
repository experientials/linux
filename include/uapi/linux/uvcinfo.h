/* SPDX-License-Identifier: ((GPL-2.0+ WITH Linux-syscall-note) OR MIT) */
/*
 * Rockchip preisp driver
 * Copyright (C) 2020 Rockchip Electronics Co., Ltd.
 */

#ifndef _UAPI_RKUVCINFO_H
#define _UAPI_RKUVCINFO_H

#include <linux/types.h>

struct uvc_sensor_info {
	u64 start;
	u64 ff; /* first frame time */
	int on;
};

struct uvc_csi_info {
	u64 sof[4];
	u64 eof[4];
	u64 t[4];
	unsigned int csi_sequence[4];
	int on[4];
	unsigned int stream_sequence[4];
	unsigned int sequence_map[4];
	u64 done_time[4];
	u64 vct[4];
};

struct uvc_isp_info {
	u64 sof;
	u64 eof;
	u64 t;
};

struct uvc_ispp_info {
	u64 sof;
	u64 eof;
	u64 t;
};

struct uvc_gadget_info {
	u64 sof;
	u64 eof;
	u32 bytesused, t_bytesused;
	u64 t;
};

struct uvc_info {
	struct uvc_sensor_info si;
	struct uvc_csi_info ci;
	struct uvc_isp_info ispi;
	struct uvc_ispp_info isppi;
	struct uvc_gadget_info gi;
};

extern struct uvc_info uvcinfo;

extern void si_first_frame_uvcinfo(void);
extern void si_uvcinfo(int on);
extern void cif_frame_uvcinfo(int sof, unsigned int sequence, int vc);
extern void cif_vb2_uvcinfo(int vc, unsigned int sequence, u64 time);
extern void isp_uvcinfo(int sof);
extern void ispp_uvcinfo(int sof);
extern void gadget_uvcinfo(int sof);
extern void gadget_bytesused_uvcinfo(unsigned int bytesused);


#endif /* _UAPI_RKUVCINFO_H */
