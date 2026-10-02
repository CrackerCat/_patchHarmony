// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 dere3046
 */

/*
 * one place that decides how this module hooks things. the knobs below are the
 * whole policy: whether inline hooks run at all, which write path the engine
 * uses, and whether the engine may open a fixmap slot of its own. anything that
 * changes how we hook belongs here rather than at a call site
 */
#include "ds.h"
#include "hk_patch.h"

bool droid_lkm_inline_hooks_on;

static bool droid_lkm_inline_hook = true;
module_param_named(inline_hook, droid_lkm_inline_hook, bool, 0444);
MODULE_PARM_DESC(inline_hook, "install inline hooks, on by default");

/*
 * 0 keeps the engine on the kernel's own patch primitive, which owns the fixmap
 * slot, the frame and the locking. 1 hands the engine our own fixmap writer:
 * that store goes through an alias this module computes from its build headers,
 * and on the MTK device every engine write then faulted in hk_patch_slot_cb.
 * only pick 1 to reproduce that, never in production
 */
static uint droid_lkm_write_path;
module_param_named(write_path, droid_lkm_write_path, uint, 0444);
MODULE_PARM_DESC(write_path, "0 kernel patch primitive (default), 1 the module's own fixmap writer");


int droid_lkm_do_inline_hook(struct hk_inline *h, const char *sym,
			     const char *wrap)
{
	if (!droid_lkm_inline_hooks_on) {
		pr_warn_once("[droid_lkm] inline hooks are off (inline_hook=0), %s not hooked\n", sym);
		return -EOPNOTSUPP;
	}
	return hk_inline_hook(h, sym, wrap);
}

void droid_lkm_hook_policy_apply(void)
{
	droid_lkm_inline_hooks_on = droid_lkm_inline_hook;
	hk_patch_set_write(droid_lkm_write_path ? hk_write_fixmap : NULL);
	droid_lkm_info("hook policy: inline_hook=%d write_path=%u\n",
		       droid_lkm_inline_hooks_on, droid_lkm_write_path);
}
