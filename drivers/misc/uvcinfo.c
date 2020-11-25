// SPDX-License-Identifier: GPL-2.0
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uvcinfo.h>

struct uvc_info uvcinfo;

void si_first_frame_uvcinfo(void)
{
	uvcinfo.si.ff = ktime_get_ns();
}

void si_uvcinfo(int on)
{
	if (on)
		uvcinfo.si.start = ktime_get_ns();
	uvcinfo.si.on = !!on;
}

void cif_frame_uvcinfo(int sof, unsigned int sequence, int vc)
{
	static u64 last_sof;

	if (vc < 0 || vc >= 4)
		return;


	uvcinfo.ci.on[vc] = 1;

	if (sof)
		last_sof = ktime_get_ns();
	else {
		uvcinfo.ci.csi_sequence[vc] = sequence;
		uvcinfo.ci.sof[vc] = last_sof;
		uvcinfo.ci.eof[vc] = ktime_get_ns();
		uvcinfo.ci.t[vc] = uvcinfo.ci.eof[vc] - uvcinfo.ci.sof[vc];
	}
}

void cif_vb2_uvcinfo(int vc, unsigned int sequence, u64 time)
{
	static int count;
	static unsigned int last_sequence = 0xFFFFFFFF;

	if (vc < 0 || vc >= 4)
		return;


	if (last_sequence != sequence)
		count = 0;

	uvcinfo.ci.stream_sequence[count] = sequence;
	uvcinfo.ci.sequence_map[count] = uvcinfo.ci.csi_sequence[count];
	uvcinfo.ci.done_time[count] = time;
	uvcinfo.ci.vct[count] = uvcinfo.ci.done_time[count] - uvcinfo.ci.eof[count];
	count = (count + 1) % 4;
	last_sequence = sequence;
}

void isp_uvcinfo(int sof)
{
	if (sof)
		uvcinfo.ispi.sof = ktime_get_ns();
	else {
		uvcinfo.ispi.eof = ktime_get_ns();
		uvcinfo.ispi.t = uvcinfo.ispi.eof - uvcinfo.ispi.sof;
	}
}

void ispp_uvcinfo(int sof)
{
	if (sof)
		uvcinfo.isppi.sof = ktime_get_ns();
	else {
		uvcinfo.isppi.eof = ktime_get_ns();
		uvcinfo.isppi.t = uvcinfo.isppi.eof - uvcinfo.isppi.sof;
	}
}

void gadget_uvcinfo(int sof)
{
	if (sof)
		uvcinfo.gi.sof = ktime_get_ns();
	else {
		uvcinfo.gi.eof = ktime_get_ns();
		uvcinfo.gi.t = uvcinfo.gi.eof - uvcinfo.gi.sof;
		uvcinfo.gi.t_bytesused = uvcinfo.gi.bytesused;
	}
}

void gadget_bytesused_uvcinfo(unsigned int bytesused)
{
	uvcinfo.gi.bytesused = bytesused;
}

static int uvcinfo_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "vc on: %d %d %d %d\n",
			uvcinfo.ci.on[0],
			uvcinfo.ci.on[1],
			uvcinfo.ci.on[2],
			uvcinfo.ci.on[3]);
	seq_printf(m, "csi sequence: %u %u %u %u\n",
			uvcinfo.ci.csi_sequence[0],
			uvcinfo.ci.csi_sequence[1],
			uvcinfo.ci.csi_sequence[2],
			uvcinfo.ci.csi_sequence[3]);
	seq_printf(m, "csi transport delay: %llu %llu %llu %llu\n",
			uvcinfo.ci.t[0],
			uvcinfo.ci.t[1],
			uvcinfo.ci.t[2],
			uvcinfo.ci.t[3]);
	seq_printf(m, "stream sequence: %u %u %u %u\n",
			uvcinfo.ci.stream_sequence[0],
			uvcinfo.ci.stream_sequence[1],
			uvcinfo.ci.stream_sequence[2],
			uvcinfo.ci.stream_sequence[3]);
	seq_printf(m, "stream sequence map: %u %u %u %u\n",
			uvcinfo.ci.sequence_map[0],
			uvcinfo.ci.sequence_map[1],
			uvcinfo.ci.sequence_map[2],
			uvcinfo.ci.sequence_map[3]);
	seq_printf(m, "stream vb2 done: %llu %llu %llu %llu\n",
			uvcinfo.ci.vct[0],
			uvcinfo.ci.vct[1],
			uvcinfo.ci.vct[2],
			uvcinfo.ci.vct[3]);
	seq_printf(m, "usb transport bytes: %u\n", uvcinfo.gi.t_bytesused);
	seq_printf(m, "usb transport delay: %llu\n",
			uvcinfo.gi.t);
	return 0;
}

static int __init proc_uvcinfo_init(void)
{
	proc_create_single("uvcinfo", 0, NULL, uvcinfo_proc_show);
	return 0;
}
fs_initcall(proc_uvcinfo_init);
