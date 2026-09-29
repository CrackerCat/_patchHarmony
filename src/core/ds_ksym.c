// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 dere3046
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/pid.h>
#include <linux/pid_namespace.h>
#include <linux/sched/signal.h>
#include <linux/nsproxy.h>

#include "ds.h"
#include "ds_ksym.h"

struct droid_lkm_ksym droid_lkm_ks = { };
int *droid_lkm_ks_sysctl_overcommit_memory;
rwlock_t *droid_lkm_ks_tasklist_lock;
const struct proc_ns_operations *droid_lkm_ks_ipcns_operations;

static unsigned long __nocfi droid_lkm_resolve(const char *name)
{
	return droid_lkm_sym(name);
}

/* a symbol only one kernel generation carries, absence is expected */
#define DROID_LKM_THUNK_OPT(_field, _name, _type)                             \
	do {                                                                   \
		droid_lkm_ks._field = (_type)droid_lkm_resolve(_name);                \
	} while (0)

#define DROID_LKM_THUNK(_field, _name, _type)                                         \
	do {                                                                   \
		droid_lkm_ks._field = (_type)droid_lkm_resolve(_name);                       \
		if (!droid_lkm_ks._field)                                             \
			droid_lkm_warn("thunk missing: %s\n", _name);                 \
	} while (0)

int droid_lkm_ksym_init(void)
{
	int missing = 0;

	DROID_LKM_THUNK(proc_alloc_inum, "proc_alloc_inum", typeof(droid_lkm_ks.proc_alloc_inum));
	DROID_LKM_THUNK(proc_free_inum, "proc_free_inum", typeof(droid_lkm_ks.proc_free_inum));
	DROID_LKM_THUNK(disable_pid_allocation, "disable_pid_allocation",
		 typeof(droid_lkm_ks.disable_pid_allocation));
	DROID_LKM_THUNK(group_send_sig_info, "group_send_sig_info",
		 typeof(droid_lkm_ks.group_send_sig_info));
	DROID_LKM_THUNK(kernel_wait4, "kernel_wait4", typeof(droid_lkm_ks.kernel_wait4));
	DROID_LKM_THUNK(do_mmap, "do_mmap", typeof(droid_lkm_ks.do_mmap));
	DROID_LKM_THUNK(do_mmap_legacy, "do_mmap",
			typeof(droid_lkm_ks.do_mmap_legacy));
	DROID_LKM_THUNK_OPT(__do_munmap, "__do_munmap",
			    typeof(droid_lkm_ks.__do_munmap));
	DROID_LKM_THUNK_OPT(do_mas_munmap, "do_mas_munmap",
			    typeof(droid_lkm_ks.do_mas_munmap));
	DROID_LKM_THUNK_OPT(do_vmi_munmap, "do_vmi_munmap",
			    typeof(droid_lkm_ks.do_vmi_munmap));
	DROID_LKM_THUNK(wake_q_add, "wake_q_add", typeof(droid_lkm_ks.wake_q_add));
	DROID_LKM_THUNK(wake_q_add_safe, "wake_q_add_safe", typeof(droid_lkm_ks.wake_q_add_safe));
	DROID_LKM_THUNK(wake_up_q, "wake_up_q", typeof(droid_lkm_ks.wake_up_q));
	DROID_LKM_THUNK(schedule_hrtimeout_range, "schedule_hrtimeout_range",
		 typeof(droid_lkm_ks.schedule_hrtimeout_range));
	DROID_LKM_THUNK(get_timespec64, "get_timespec64", typeof(droid_lkm_ks.get_timespec64));
	DROID_LKM_THUNK(get_old_timespec32, "get_old_timespec32",
		 typeof(droid_lkm_ks.get_old_timespec32));
	DROID_LKM_THUNK(__percpu_counter_sum, "__percpu_counter_sum",
		 typeof(droid_lkm_ks.__percpu_counter_sum));
	DROID_LKM_THUNK(shmem_lock, "shmem_lock", typeof(droid_lkm_ks.shmem_lock));
	DROID_LKM_THUNK(shmem_kernel_file_setup, "shmem_kernel_file_setup",
		 typeof(droid_lkm_ks.shmem_kernel_file_setup));
	DROID_LKM_THUNK(shmem_unlock_mapping, "shmem_unlock_mapping",
		 typeof(droid_lkm_ks.shmem_unlock_mapping));
	DROID_LKM_THUNK(alloc_file_clone, "alloc_file_clone",
		 typeof(droid_lkm_ks.alloc_file_clone));
	DROID_LKM_THUNK(__mm_populate, "__mm_populate", typeof(droid_lkm_ks.__mm_populate));
	DROID_LKM_THUNK(switch_task_namespaces, "switch_task_namespaces",
		 typeof(droid_lkm_ks.switch_task_namespaces));
	DROID_LKM_THUNK(inode_permission, "inode_permission",
		 typeof(droid_lkm_ks.inode_permission));
	DROID_LKM_THUNK(mnt_want_write, "mnt_want_write",
		 typeof(droid_lkm_ks.mnt_want_write));
	DROID_LKM_THUNK(mnt_drop_write, "mnt_drop_write",
		 typeof(droid_lkm_ks.mnt_drop_write));
	DROID_LKM_THUNK(lookup_one_len, "lookup_one_len",
		 typeof(droid_lkm_ks.lookup_one_len));
	DROID_LKM_THUNK(get_tree_nodev, "get_tree_nodev",
		 typeof(droid_lkm_ks.get_tree_nodev));

	DROID_LKM_THUNK(security_ipc_permission, "security_ipc_permission",
		 typeof(droid_lkm_ks.security_ipc_permission));
	DROID_LKM_THUNK(security_mmap_file, "security_mmap_file",
		 typeof(droid_lkm_ks.security_mmap_file));
	DROID_LKM_THUNK(security_msg_msg_alloc, "security_msg_msg_alloc",
		 typeof(droid_lkm_ks.security_msg_msg_alloc));
	DROID_LKM_THUNK(security_msg_msg_free, "security_msg_msg_free",
		 typeof(droid_lkm_ks.security_msg_msg_free));
	DROID_LKM_THUNK(security_msg_queue_alloc, "security_msg_queue_alloc",
		 typeof(droid_lkm_ks.security_msg_queue_alloc));
	DROID_LKM_THUNK(security_msg_queue_free, "security_msg_queue_free",
		 typeof(droid_lkm_ks.security_msg_queue_free));
	DROID_LKM_THUNK(security_msg_queue_associate, "security_msg_queue_associate",
		 typeof(droid_lkm_ks.security_msg_queue_associate));
	DROID_LKM_THUNK(security_msg_queue_msgctl, "security_msg_queue_msgctl",
		 typeof(droid_lkm_ks.security_msg_queue_msgctl));
	DROID_LKM_THUNK(security_msg_queue_msgsnd, "security_msg_queue_msgsnd",
		 typeof(droid_lkm_ks.security_msg_queue_msgsnd));
	DROID_LKM_THUNK(security_msg_queue_msgrcv, "security_msg_queue_msgrcv",
		 typeof(droid_lkm_ks.security_msg_queue_msgrcv));
	DROID_LKM_THUNK(security_shm_alloc, "security_shm_alloc",
		 typeof(droid_lkm_ks.security_shm_alloc));
	DROID_LKM_THUNK(security_shm_free, "security_shm_free",
		 typeof(droid_lkm_ks.security_shm_free));
	DROID_LKM_THUNK(security_shm_associate, "security_shm_associate",
		 typeof(droid_lkm_ks.security_shm_associate));
	DROID_LKM_THUNK(security_shm_shmctl, "security_shm_shmctl",
		 typeof(droid_lkm_ks.security_shm_shmctl));
	DROID_LKM_THUNK(security_shm_shmat, "security_shm_shmat",
		 typeof(droid_lkm_ks.security_shm_shmat));
	DROID_LKM_THUNK(security_sem_alloc, "security_sem_alloc",
		 typeof(droid_lkm_ks.security_sem_alloc));
	DROID_LKM_THUNK(security_sem_free, "security_sem_free",
		 typeof(droid_lkm_ks.security_sem_free));
	DROID_LKM_THUNK(security_sem_associate, "security_sem_associate",
		 typeof(droid_lkm_ks.security_sem_associate));
	DROID_LKM_THUNK(security_sem_semctl, "security_sem_semctl",
		 typeof(droid_lkm_ks.security_sem_semctl));
	DROID_LKM_THUNK(security_sem_semop, "security_sem_semop",
		 typeof(droid_lkm_ks.security_sem_semop));
	DROID_LKM_THUNK(__audit_ipc_obj, "__audit_ipc_obj",
		 typeof(droid_lkm_ks.__audit_ipc_obj));
	DROID_LKM_THUNK(__audit_ipc_set_perm, "__audit_ipc_set_perm",
		 typeof(droid_lkm_ks.__audit_ipc_set_perm));
	DROID_LKM_THUNK(__audit_inode, "__audit_inode",
		 typeof(droid_lkm_ks.__audit_inode));
	DROID_LKM_THUNK(__audit_file, "__audit_file",
		 typeof(droid_lkm_ks.__audit_file));
	DROID_LKM_THUNK(__audit_mq_open, "__audit_mq_open",
		 typeof(droid_lkm_ks.__audit_mq_open));
	DROID_LKM_THUNK(__audit_mq_sendrecv, "__audit_mq_sendrecv",
		 typeof(droid_lkm_ks.__audit_mq_sendrecv));
	DROID_LKM_THUNK(__audit_mq_notify, "__audit_mq_notify",
		 typeof(droid_lkm_ks.__audit_mq_notify));
	DROID_LKM_THUNK(__audit_mq_getsetattr, "__audit_mq_getsetattr",
		 typeof(droid_lkm_ks.__audit_mq_getsetattr));

	/*
	 * missing on 5.10 only, where the shim drops the mqueue rlimit
	 * accounting instead of the queues, so the warning is the signal
	 */
	DROID_LKM_THUNK(ucounts_get, "get_ucounts",
		 typeof(droid_lkm_ks.ucounts_get));
	DROID_LKM_THUNK(ucounts_put, "put_ucounts",
		 typeof(droid_lkm_ks.ucounts_put));
	DROID_LKM_THUNK(ucounts_inc_rlimit, "inc_rlimit_ucounts",
		 typeof(droid_lkm_ks.ucounts_inc_rlimit));
	DROID_LKM_THUNK(ucounts_dec_rlimit, "dec_rlimit_ucounts",
		 typeof(droid_lkm_ks.ucounts_dec_rlimit));

	droid_lkm_ks_sysctl_overcommit_memory =
		(int *)droid_lkm_resolve("sysctl_overcommit_memory");
	if (!droid_lkm_ks_sysctl_overcommit_memory)
		droid_lkm_warn("thunk missing: sysctl_overcommit_memory\n");

	/*
	 * unexported kernel helpers. a stock image trims some of them, so the
	 * shims refuse the feature that needs one instead of failing the load,
	 * and the count is reported once rather than one line per symbol.
	 */
	missing = 0;
#define DROID_LKM_THUNK_MAYBE(_field, _name)                                   \
	do {                                                                   \
		DROID_LKM_THUNK_OPT(_field, _name,                             \
				    typeof(droid_lkm_ks._field));              \
		if (!droid_lkm_ks._field)                                      \
			missing++;                                             \
	} while (0)

	DROID_LKM_THUNK_MAYBE(dentry_open, "dentry_open");
	DROID_LKM_THUNK_MAYBE(do_send_sig_info, "do_send_sig_info");
	DROID_LKM_THUNK_MAYBE(fc_mount, "fc_mount");
	DROID_LKM_THUNK_MAYBE(mntget, "mntget");
	DROID_LKM_THUNK_MAYBE(put_fs_context, "put_fs_context");
	DROID_LKM_THUNK_MAYBE(free_ipcs, "free_ipcs");
	DROID_LKM_THUNK_MAYBE(put_ipc_ns, "put_ipc_ns");
	DROID_LKM_THUNK_MAYBE(pid_nr_ns, "pid_nr_ns");
	DROID_LKM_THUNK_MAYBE(pid_vnr, "pid_vnr");
	DROID_LKM_THUNK_MAYBE(proc_dointvec_minmax, "proc_dointvec_minmax");
	DROID_LKM_THUNK_MAYBE(proc_doulongvec_minmax, "proc_doulongvec_minmax");
	DROID_LKM_THUNK_MAYBE(register_kprobe, "register_kprobe");
	DROID_LKM_THUNK_MAYBE(unregister_kprobe, "unregister_kprobe");
	DROID_LKM_THUNK_MAYBE(kern_path, "kern_path");
	DROID_LKM_THUNK_MAYBE(path_put, "path_put");
	DROID_LKM_THUNK_MAYBE(d_set_d_op, "d_set_d_op");
	DROID_LKM_THUNK_MAYBE(down_write_killable, "down_write_killable");
	DROID_LKM_THUNK_MAYBE(radix_tree_tagged, "radix_tree_tagged");
	DROID_LKM_THUNK_MAYBE(vfs_unlink, "vfs_unlink");
	DROID_LKM_THUNK_MAYBE(register_sysctl, "register_sysctl");
	DROID_LKM_THUNK_MAYBE(unregister_sysctl_table, "unregister_sysctl_table");
	DROID_LKM_THUNK_MAYBE(copy_ipcs, "copy_ipcs");
#undef DROID_LKM_THUNK_MAYBE

	droid_lkm_ks_tasklist_lock = (rwlock_t *)droid_lkm_resolve("tasklist_lock");
	if (!droid_lkm_ks_tasklist_lock)
		missing++;
	droid_lkm_ks_ipcns_operations =
		(const struct proc_ns_operations *)droid_lkm_resolve("ipcns_operations");
	if (!droid_lkm_ks_ipcns_operations)
		missing++;

	if (missing)
		droid_lkm_warn("%d unexported kernel helper(s) unavailable, the features that need them refuse cleanly\n",
			       missing);

	if (!droid_lkm_ks.proc_alloc_inum || !droid_lkm_ks.proc_free_inum ||
	    !droid_lkm_ks.disable_pid_allocation || !droid_lkm_ks.group_send_sig_info ||
	    !droid_lkm_ks.kernel_wait4) {
		droid_lkm_err("required thunks unresolved\n");
		return -ENODATA;
	}

	return 0;
}
