/*
 * Copyright (c) 2017-2019 The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * DOC: contains scan api
 */

#ifndef _WLAN_SCAN_API_H_
#define _WLAN_SCAN_API_H_

#include <wlan_objmgr_psoc_obj.h>
#include <wlan_objmgr_pdev_obj.h>
#include <wlan_objmgr_vdev_obj.h>
#include "../../core/src/wlan_scan_main.h"

/**
 * wlan_scan_cfg_set_active_dwelltime() - API to set scan active dwelltime
 * @psoc: pointer to psoc object
 * @dwell_time: scan active dwell time
 *
 * Return: none
 */
void wlan_scan_cfg_set_active_dwelltime(struct wlan_objmgr_psoc *psoc,
					uint32_t dwell_time);
/**
 * wlan_scan_cfg_get_active_dwelltime() - API to get active dwelltime
 * @psoc: pointer to psoc object
 * @dwell_time: scan active dwelltime
 *
 * Return: scan active dwell time
 */
void wlan_scan_cfg_get_active_dwelltime(struct wlan_objmgr_psoc *psoc,
					uint32_t *dwell_time);

/**
 * wlan_scan_cfg_set_passive_dwelltime() - API to set scan passive dwelltime
 * @psoc: pointer to psoc object
 * @dwell_time: scan passive dwell time
 *
 * Return: none
 */
void wlan_scan_cfg_set_passive_dwelltime(struct wlan_objmgr_psoc *psoc,
					 uint32_t dwell_time);
/**
 * wlan_scan_cfg_get_passive_dwelltime() - API to get passive dwelltime
 * @psoc: pointer to psoc object
 * @dwell_time: scan passive dwelltime
 *
 * Return: scan passive dwell time
 */
void wlan_scan_cfg_get_passive_dwelltime(struct wlan_objmgr_psoc *psoc,
					 uint32_t *dwell_time);

/**
 * wlan_scan_cfg_get_conc_active_dwelltime() - Get concurrent active dwelltime
 * @psoc: pointer to psoc object
 * @dwell_time: scan active dwelltime
 *
 * Return: scan concurrent active dwell time
 */
void wlan_scan_cfg_get_conc_active_dwelltime(struct wlan_objmgr_psoc *psoc,
					     uint32_t *dwell_time);

/**
 * wlan_scan_cfg_set_conc_active_dwelltime() - Set concurrent active dwelltime
 * @psoc: pointer to psoc object
 * @dwell_time: scan active dwelltime
 *
 * Return: scan concurrent active dwell time
 */
void wlan_scan_cfg_set_conc_active_dwelltime(struct wlan_objmgr_psoc *psoc,
					     uint32_t dwell_time);

/**
 * wlan_scan_cfg_get_conc_passive_dwelltime() - Get passive concurrent dwelltime
 * @psoc: pointer to psoc object
 * @dwell_time: scan passive dwelltime
 *
 * Return: scan concurrent passive dwell time
 */
void wlan_scan_cfg_get_conc_passive_dwelltime(struct wlan_objmgr_psoc *psoc,
					      uint32_t *dwell_time);

/**
 * wlan_scan_cfg_set_conc_passive_dwelltime() - Set passive concurrent dwelltime
 * @psoc: pointer to psoc object
 * @dwell_time: scan passive dwelltime
 *
 * Return: scan concurrent passive dwell time
 */
void wlan_scan_cfg_set_conc_passive_dwelltime(struct wlan_objmgr_psoc *psoc,
					      uint32_t dwell_time);

/**
 * wlan_scan_cfg_get_conc_max_resttime() - API to get max rest time
 * @psoc: pointer to psoc object
 * @rest_time: scan concurrent max resttime
 *
 * Return: scan concurrent max rest time
 */
void wlan_scan_cfg_get_conc_max_resttime(struct wlan_objmgr_psoc *psoc,
					 uint32_t *rest_time);

/**
 * wlan_scan_cfg_get_dfs_chan_scan_allowed() - API to get dfs scan enabled
 * @psoc: pointer to psoc object
 * @enable_dfs_scan: DFS scan enabled or not.
 *
 * Return: None
 */
void wlan_scan_cfg_get_dfs_chan_scan_allowed(struct wlan_objmgr_psoc *psoc,
					     bool *enable_dfs_scan);

/**
 * wlan_scan_cfg_set_dfs_chan_scan_allowed() - API to set dfs scan enabled.
 * @psoc: pointer to psoc object
 * @enable_dfs_scan: Set dfs scan enabled or not.
 *
 * Return: None
 */
void wlan_scan_cfg_set_dfs_chan_scan_allowed(struct wlan_objmgr_psoc *psoc,
					     bool enable_dfs_scan);

/**
 * wlan_scan_cfg_get_conc_min_resttime() - API to get concurrent min rest time
 * @psoc: pointer to psoc object
 * @rest_time: scan concurrent min rest time
 *
 * Return: scan concurrent min rest time
 */
void wlan_scan_cfg_get_conc_min_resttime(struct wlan_objmgr_psoc *psoc,
					 uint32_t *rest_time);

/**
 * wlan_scan_is_snr_monitor_enabled() - API to get SNR monitoring enabled or not
 * @psoc: pointer to psoc object
 *
 * Return: enable/disable snr monitor mode.
 */
bool wlan_scan_is_snr_monitor_enabled(struct wlan_objmgr_psoc *psoc);

#endif
