// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 dere3046
 */

#ifndef DROID_LKM_CAPS_H
#define DROID_LKM_CAPS_H

#include <linux/types.h>

#define DROID_LKM_VERSION(_major, _minor, _patch) \
	((_major) * 65536 + (_minor) * 256 + (_patch))

/*
 * what the running kernel offers, decided once at load time before any hook is
 * installed. call sites ask these questions instead of testing a kernel
 * version, so one source serves every branch we ship without a conditional.
 */
struct droid_lkm_caps {
	unsigned int version;		/* utsname release, parsed */
	bool perm_takes_idmap;		/* inode_permission's first argument */
	void *idmap_none;		/* nop_mnt_idmap, or init_user_ns */
	bool mmap_takes_vm_flags;	/* do_mmap carries vm_flags */
	bool has_ns_count;		/* ns_common::count, absent on 5.10 */
	bool ctl_takes_table;		/* ctl_table_root::set_ownership has the table */
};

extern struct droid_lkm_caps droid_lkm_caps;

/* fills the struct, and refuses to load on a kernel this build cannot serve */
int droid_lkm_caps_init(void);
void droid_lkm_caps_report(void);

#endif
