// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2004-2012 Sage Weil <sage@newdream.net>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software
 * Foundation.  See file COPYING.
 *
 */

#ifndef CEPH_LIBRADOS_H
#define CEPH_LIBRADOS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @typedef rados_t
 *
 * A handle for interacting with a RADOS cluster. It encapsulates all
 * RADOS client configuration, including username, key for
 * authentication, logging, and debugging. Talking different clusters
 * -- or to the same cluster with different users -- requires
 * different cluster handles.
 */
typedef void *rados_t;

/**
 * @tyepdef rados_config_t
 *
 * A handle for the ceph configuration context for the rados_t cluster
 * instance.  This can be used to share configuration context/state
 * (e.g., logging configuration) between librados instance.
 *
 * @warning The config context does not have independent reference
 * counting.  As such, a rados_config_t handle retrieved from a given
 * rados_t is only valid as long as that rados_t.
 */
typedef void *rados_config_t;

/**
 * @typedef rados_ioctx_t
 *
 * An io context encapsulates a few settings for all I/O operations
 * done on it:
 * - pool - set when the io context is created (see rados_ioctx_create())
 * - snapshot context for writes (see
 *   rados_ioctx_selfmanaged_snap_set_write_ctx())
 * - snapshot id to read from (see rados_ioctx_snap_set_read())
 * - object locator for all single-object operations (see
 *   rados_ioctx_locator_set_key())
 *
 * @warning changing any of these settings is not thread-safe -
 * librados users must synchronize any of these changes on their own,
 * or use separate io contexts for each thread
 */
typedef void *rados_ioctx_t;

/**
 * @typedef rados_list_ctx_t
 *
 * An iterator for listing the objects in a pool.
 * Used with rados_objects_list_open(),
 * rados_objects_list_next(), and
 * rados_objects_list_close().
 */
typedef void *rados_list_ctx_t;

/**
 * @typedef rados_snap_t
 * The id of a snapshot.
 */
typedef uint64_t rados_snap_t;

/**
 * @typedef rados_xattrs_iter_t
 * An iterator for listing extended attrbutes on an object.
 * Used with rados_getxattrs(), rados_getxattrs_next(), and
 * rados_getxattrs_end().
 */
typedef void *rados_xattrs_iter_t;

/**
 * @typedef rados_omap_iter_t
 * An iterator for listing omap key/value pairs on an object.
 * Used with rados_read_op_omap_get_keys(), rados_read_op_omap_get_vals(),
 * rados_read_op_omap_get_vals_by_keys(), rados_omap_get_next(), and
 * rados_omap_get_end().
 */
typedef void *rados_omap_iter_t;

/**
 * @struct rados_pool_stat_t
 * Usage information for a pool.
 */
struct rados_pool_stat_t {
  /// space used in bytes
  uint64_t num_bytes;
  /// space used in KB
  uint64_t num_kb;
  /// number of objects in the pool
  uint64_t num_objects;
  /// number of clones of objects
  uint64_t num_object_clones;
  /// num_objects * num_replicas
  uint64_t num_object_copies;
  uint64_t num_objects_missing_on_primary;
  /// number of objects found on no OSDs
  uint64_t num_objects_unfound;
  /// number of objects replicated fewer times than they should be
  /// (but found on at least one OSD)
  uint64_t num_objects_degraded;
  uint64_t num_rd;
  uint64_t num_rd_kb;
  uint64_t num_wr;
  uint64_t num_wr_kb;
};

/**
 * @struct rados_cluster_stat_t
 * Cluster-wide usage information
 */
struct rados_cluster_stat_t {
  uint64_t kb, kb_used, kb_avail;
  uint64_t num_objects;
};

/**
 * @typedef rados_write_op_t
 *
 * An object write operation stores a number of operations which can be
 * executed atomically. For usage, see:
 * - Creation and deletion: rados_create_write_op() rados_release_write_op()
 * - Extended attribute manipulation: rados_write_op_cmpxattr()
 *   rados_write_op_cmpxattr(), rados_write_op_setxattr(),
 *   rados_write_op_rmxattr()
 * - Object map key/value pairs: rados_write_op_omap_set(),
 *   rados_write_op_omap_rm_keys(), rados_write_op_omap_clear(),
 *   rados_write_op_omap_cmp()
 * - Creating objects: rados_write_op_create()
 * - IO on objects: rados_write_op_append(), rados_write_op_write(), rados_write_op_zero
 *   rados_write_op_write_full(), rados_write_op_remove, rados_write_op_truncate(),
 *   rados_write_op_zero()
 * - Hints: rados_write_op_set_alloc_hint()
 * - Performing the operation: rados_write_op_operate(), rados_aio_write_op_operate()
 */
typedef void *rados_write_op_t;

/**
 * @typedef rados_read_op_t
 *
 * An object read operation stores a number of operations which can be
 * executed atomically. For usage, see:
 * - Creation and deletion: rados_create_read_op() rados_release_read_op()
 * - Extended attribute manipulation: rados_read_op_cmpxattr(),
 *   rados_read_op_getxattr(), rados_read_op_getxattrs()
 * - Object map key/value pairs: rados_read_op_omap_get_vals(),
 *   rados_read_op_omap_get_keys(), rados_read_op_omap_get_vals_by_keys(),
 *   rados_read_op_omap_cmp()
 * - Object properties: rados_read_op_stat(), rados_read_op_assert_exists()
 * - IO on objects: rados_read_op_read()
 * - Custom operations: rados_read_op_exec(), rados_read_op_exec_user_buf()
 * - Request properties: rados_read_op_set_flags()
 * - Performing the operation: rados_read_op_operate(),
 *   rados_aio_read_op_operate()
 */
typedef void *rados_read_op_t;

/** @} Synchronous I/O */

/**
 * @defgroup librados_h_asynch_io Asynchronous I/O
 * Read and write to objects without blocking.
 *
 * @{
 */

/**
 * @typedef rados_completion_t
 * Represents the state of an asynchronous operation - it contains the
 * return value once the operation completes, and can be used to block
 * until the operation is complete or safe.
 */
typedef void *rados_completion_t;

/**
 * @typedef rados_callback_t
 * Callbacks for asynchrous operations take two parameters:
 * - cb the completion that has finished
 * - arg application defined data made available to the callback function
 */
typedef void (*rados_callback_t)(rados_completion_t cb, void *arg);

/*
 * This is not a doxygen comment leadin, because doxygen breaks on
 * a typedef with function params and returns, and I can't figure out
 * how to fix it.
 *
 * Monitor cluster log
 *
 * Monitor events logged to the cluster log.  The callback get each
 * log entry both as a single formatted line and with each field in a
 * separate arg.
 *
 * Calling with a cb argument of NULL will deregister any previously
 * registered callback.
 *
 * @param cluster cluster handle
 * @param level minimum log level (debug, info, warn|warning, err|error)
 * @param cb callback to run for each log message
 * @param arg void argument to pass to cb
 *
 * @returns 0 on success, negative code on error
 */
typedef void (*rados_log_callback_t)(void *arg,
                                     const char *line,
                                     const char *who,
                                     uint64_t sec, uint64_t nsec,
                                     uint64_t seq, const char *level,
                                     const char *msg);

/** @} Mon/OSD/PG commands */

#ifdef __cplusplus
}
#endif

#endif
