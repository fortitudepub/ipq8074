/*
 * Copyright (c) 2012-2019 The Linux Foundation. All rights reserved.
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

/**
 * DOC: This file contains centralized definitions of SCAN component
 */
#ifndef __CONFIG_SCAN_H
#define __CONFIG_SCAN_H

#include "cfg_define.h"

/*
 * <ini>
 * drop_bcn_on_chan_mismatch - drop the beacon for chan mismatch
 * @Min: 0
 * @Max: 1
 * @Default: 1
 *
 * This ini is used to decide drop the beacon or not if channel received
 * in metadata doesn't match the one in beacon.
 *
 * Related: None
 *
 * Usage: External
 *
 * </ini>
 */
#define CFG_DROP_BCN_ON_CHANNEL_MISMATCH CFG_INI_BOOL(\
		"drop_bcn_on_chan_mismatch",\
		true,\
		"drop bcn on channel mismatch")

/*
 * <ini>
 * gActiveMaxChannelTime - Set max channel time for active scan
 * @Min: 0
 * @Max: 10000
 * @Default: 40
 *
 * This ini is used to set maximum channel time in msecs spent in
 * active scan
 *
 * Related: None
 *
 * Usage: External
 *
 * </ini>
 */
#define CFG_ACTIVE_MAX_CHANNEL_TIME CFG_INI_UINT(\
		"gActiveMaxChannelTime",\
		0, 10000, MCL_OR_WIN_VALUE(40, 105),\
		CFG_VALUE_OR_DEFAULT, "active dwell time")

/*
 * <ini>
 * gEnableDFSChnlScan - Enable/Disable scan on DFS channels
 * @Min: 0
 * @Max: 1
 * @Default: 1
 *
 * This ini is used to enable/disable scan on DFS channels.
 *
 * Related: Scan
 *
 * Usage: External
 *
 * </ini>
 */
#define CFG_ENABLE_DFS_SCAN CFG_INI_BOOL( \
				"gEnableDFSChnlScan", \
				true, \
				"enable dfs scan")

/*
 * <ini>
 * gInitialScanNoDFSChnl - Exclude DFS channels in first scan
 * @Min: 0
 * @Max: 1
 * @Default: 0
 *
 * This ini is used to enable/disable scan on DFS channels, in first scan only
 *
 * Related: Scan
 *
 * Usage: External
 *
 * </ini>
 */
#define CFG_INITIAL_NO_DFS_SCAN CFG_INI_BOOL( \
				"gInitialScanNoDFSChnl", \
				false, \
				"disable initial dfs scan")

/*
 * <ini>
 * active_max_channel_time_2g - Set max time for active 2G channel scan
 * @Min: 0
 * @Max: 10000
 * @Default: 80
 *
 * This ini is used to set maximum time in msecs spent in active 2G channel scan
 * if it's not zero, in case of zero, CFG_ACTIVE_MAX_CHANNEL_TIME is used for 2G
 * channels also.
 *
 * Related: None
 *
 * Usage: External
 *
 * </ini>
 */
#define CFG_ACTIVE_MAX_2G_CHANNEL_TIME CFG_INI_UINT(\
		"active_max_channel_time_2g",\
		0, 10000, MCL_OR_WIN_VALUE(80, 0),\
		CFG_VALUE_OR_DEFAULT, "active dwell time for 2G channels")

/*
 * <ini>
 * gPassiveMaxChannelTime - Set max channel time for passive scan
 * @Min: 0
 * @Max: 10000
 * @Default: 110
 *
 * This ini is used to set maximum channel time in msecs spent in
 * passive scan
 *
 * Related: None
 *
 * Usage: External
 *
 * </ini>
 */
#define CFG_PASSIVE_MAX_CHANNEL_TIME CFG_INI_UINT(\
		"gPassiveMaxChannelTime",\
		0, 10000, MCL_OR_WIN_VALUE(110, 300),\
		CFG_VALUE_OR_DEFAULT, "passive dwell time")

/*
 * <ini>
 * gScanNumProbes - Set the number of probes on each channel for active scan
 * @Min: 0
 * @Max: 20
 * @Default: 0
 *
 * This ini is used to set number of probes on each channel for
 * active scan
 */
#define CFG_SCAN_NUM_PROBES CFG_INI_UINT(\
			"gScanNumProbes",\
			0, 20, MCL_OR_WIN_VALUE(0, 2),\
			CFG_VALUE_OR_DEFAULT,\
			"number of probes on each channel")

/*
 * <ini>
 * gScanProbeRepeatTime - Set the probe repeat time on each channel
 * @Min: 0
 * @Max: 30
 * @Default: 0
 *
 * This ini is used to set probe repeat time on each channel for
 * active scan
 *
 * Related: None
 *
 * Supported Feature: Scan
 *
 * Usage: External
 *
 * </ini>
 */
#define CFG_SCAN_PROBE_REPEAT_TIME CFG_INI_UINT(\
			"gScanProbeRepeatTime",\
			0, 30, MCL_OR_WIN_VALUE(20, 50),\
			CFG_VALUE_OR_DEFAULT,\
			"probe repeat time on each channel")

/*
 * <ini>
 * hostscan_adaptive_dwell_mode - Enable adaptive dwell mode
 * during host scan with conneciton
 * @Min: 0
 * @Max: 4
 * @Default: 2
 *
 * This ini will set the algo used in dwell time optimization
 * during host scan with connection.
 * See enum wmi_dwelltime_adaptive_mode.
 * Acceptable values for this:
 * 0: Default (Use firmware default mode)
 * 1: Conservative optimization
 * 2: Moderate optimization
 * 3: Aggressive optimization
 * 4: Static
 *
 * Related: None
 *
 * Supported Feature: Scan
 *
 * Usage: External
 *
 * </ini>
 */
#define CFG_ADAPTIVE_SCAN_DWELL_MODE CFG_INI_UINT(\
			"hostscan_adaptive_dwell_mode",\
			0, 4, MCL_OR_WIN_VALUE(2, 0),\
			CFG_VALUE_OR_DEFAULT,\
			"Enable adaptive dwell mode")

/*
 * <ini>
 * hostscan_adaptive_dwell_mode_no_conn - Enable adaptive dwell mode
 * during host scan without conneciton
 * @Min: 0
 * @Max: 4
 * @Default: 4
 *
 * This ini will set the algo used in dwell time optimization
 * during host scan with connection.
 * See enum wmi_dwelltime_adaptive_mode.
 * Acceptable values for this:
 * 0: Default (Use firmware default mode)
 * 1: Conservative optimization
 * 2: Moderate optimization
 * 3: Aggressive optimization
 * 4: Static
 *
 * Related: None
 *
 * Supported Feature: Scan
 *
 * Usage: External
 *
 * </ini>
 */
#define CFG_ADAPTIVE_SCAN_DWELL_MODE_NC CFG_INI_UINT(\
			"hostscan_adaptive_dwell_mode_no_conn",\
			0, 4, MCL_OR_WIN_VALUE(4, 0),\
			CFG_VALUE_OR_DEFAULT,\
			"Enable adaptive dwell mode without connection")
/*
 * <ini>
 * is_bssid_hint_priority - Set priority for connection with bssid_hint
 * BSSID.
 * @Min: 0
 * @Max: 1
 * @Default: 1
 *
 * This ini is used to give priority to BSS for connection which comes
 * as part of bssid_hint
 *
 * Related: None
 *
 * Supported Feature: STA
 *
 * Usage: External
 *
 * </ini>
 */
#define CFG_IS_BSSID_HINT_PRIORITY CFG_INI_UINT(\
			"is_bssid_hint_priority",\
			0, 1, 0,\
			CFG_VALUE_OR_DEFAULT, \
			"Set priority for connection with bssid_hint")

#ifdef FEATURE_WLAN_SCAN_PNO
/*
 * <ini>
 * g_max_sched_scan_plan_iterations - pno sched max scan plan iterations.
 * @Min: 1
 * @Max: 100
 * @Default: 10
 *
 * This ini is used to set max sched scan plan iterations for pno scan
 * (value in seconds).
 *
 * Related: gPNOScanSupport
 *
 * Supported Feature: PNO scan
 *
 * Usage: External
 *
 * </ini>
 */
#define CFG_MAX_SCHED_SCAN_PLAN_ITERATIONS CFG_INI_UINT( \
			"g_max_sched_scan_plan_iterations", \
			1, 100, 10, \
			CFG_VALUE_OR_DEFAULT, \
			"Max sched scan plan iterations")

/*
 * <ini>
 * g_max_sched_scan_plan_int - pno sched max scan plan interval.
 * @Min: 1
 * @Max: 7200
 * @Default: 3600
 *
 * This ini is used to set max sched scan plan interval for pno scan
 * (value in seconds).
 *
 * Related: gPNOScanSupport
 *
 * Supported Feature: PNO scan
 *
 * Usage: External
 *
 * </ini>
 */
#define CFG_MAX_SCHED_SCAN_PLAN_INTERVAL CFG_INI_UINT( \
			"g_max_sched_scan_plan_int", \
			1, 7200, 3600, \
			CFG_VALUE_OR_DEFAULT, \
			"Max sched scan plan interval")

/*
 * <ini>
 * gEnableDFSPnoChnlScan - enable dfs channels in PNO scan
 * @Min: 0
 * @Max: 1
 * @Default: 1
 *
 * This ini is used to enable/disable dfs channels in PNO scan request,
 * enabling this ini enables driver to include dfs channels in its
 * PNO scan request
 * Related: NA
 *
 * Supported Feature: DFS, PNO
 *
 * Usage: Internal/External
 *
 * </ini>
 */
#define CFG_ENABLE_DFS_PNO_CHNL_SCAN CFG_INI_BOOL( \
			"gEnableDFSPnoChnlScan", \
			true, \
			"Enable dfs channels in PNO Scan")

/*
 * <ini>
 * gPNOScanSupport - Enable or Disable PNO scan
 * @Min: 0
 * @Max: 1
 * @Default: 1
 *
 * This ini is used to Enable or Disable PNO scan
 *
 * Related: None
 *
 * Supported Feature: Scan
 *
 * Usage: External
 *
 * </ini>
 */
#define CFG_PNO_SCAN_SUPPORT CFG_INI_BOOL( \
			"gPNOScanSupport", \
			true, \
			"Enable/Disable PNO scan")

/*
 * <ini>
 * gPNOScanTimerRepeatValue - Set PNO scan timer repeat value
 * @Min: 0
 * @Max: 0xffffffff
 * @Default: 30
 *
 * This ini is used by firmware to set fast scan max cycles
 * equal to gPNOScanTimerRepeatValue. Taking power consumption
 * into account firmware after gPNOScanTimerRepeatValue times
 * fast_scan_period switches to slow_scan_period.
 *
 * Usage: External
 *
 * </ini>
 */
#define CFG_PNO_SCAN_TIMER_REPEAT_VALUE CFG_INI_UINT( \
			"gPNOScanTimerRepeatValue", \
			0, \
			0xffffffff, \
			30, \
			CFG_VALUE_OR_DEFAULT, \
			"PNO scan timer repeat value")

/*
 * <ini>
 * gPNOSlowScanMultiplier - Set PNO slow scan multiplier
 * @Min: 0
 * @Max: 30
 * @Default: 6
 *
 * This ini is used by firmware to set slow scan period
 * as gPNOSlowScanMultiplier times fast_scan_period.
 *
 * Related: None
 *
 * Supported Feature: Scan
 *
 * Usage: External
 *
 * </ini>
 */
#define CFG_PNO_SLOW_SCAN_MULTIPLIER CFG_INI_UINT( \
			"gPNOSlowScanMultiplier", \
			0, \
			30, \
			6, \
			CFG_VALUE_OR_DEFAULT, \
			"PNO slow scan multiplier")

/*
 * <ini>
 * gPNOChannelPrediction - Enable/disable the PNO channel
 * prediction feature.
 * @Min: 0
 * @Max: 1
 * @Default: 0
 *
 * In current PNO implementation, scan is always done until all configured
 * channels are scanned. If we can determine DUT is stationary based on
 * scanning a subset of channels, we may cancel the remaining channels.
 * Hence, we can save additional power consumption.
 *
 * Related: None
 *
 * Supported Feature: Scan
 *
 * Usage: External
 *
 * </ini>
 */
#define CFG_PNO_CHANNEL_PREDICTION CFG_INI_BOOL( \
			"gPNOChannelPrediction", \
			false, \
			"enable/disable PNO channel prediction feature")

/*
 * <ini>
 * gTopKNumOfChannels - top K number of channels are used for tanimoto distance
 * @Min: 1
 * @Max: 5
 * @Default: 3
 *
 * These are the top channels on which the probability of finding the AP's is
 * extremely high. This number is intended for tweaking the internal algorithm
 * for experiments. This should not be changed externally.
 *
 * Related: None
 *
 * Supported Feature: Scan
 *
 * Usage: External
 *
 * </ini>
 */
#define CFG_TOP_K_NUM_OF_CHANNELS CFG_INI_UINT( \
			"gTopKNumOfChannels", \
			1, \
			5, \
			3, \
			CFG_VALUE_OR_DEFAULT, \
			"Top K number of channels")
/*
 * <ini>
 * gStationaryThreshold - STA threshold value to determine if it is stationary
 * @Min: 0
 * @Max: 100
 * @Default: 10
 *
 * This is the threshold value to determine that the STA is
 * stationary. If the tanimoto distance is less than this
 * value, then the device is considered to be stationary.
 * This parameter is intended to tweak the internal algorithm
 * for experiments. This should not be changed externally.
 *
 *
 * Related: None
 *
 * Supported Feature: Scan
 *
 * Usage: External
 *
 * </ini>
 */
#define CFG_STATIONARY_THRESHOLD CFG_INI_UINT( \
			"gStationaryThreshold", \
			0, \
			100, \
			10, \
			CFG_VALUE_OR_DEFAULT, \
			"Threshold to determine if sta is stationary")

/*
 * <ini>
 * gChPredictionFullScanMs - Set periodic timer for channel prediction
 * @Min: 3000
 * @Max: 0x7fffffff
 * @Default: 60000
 *
 * This ini is used to set the periodic timer upon which
 * a full scan needs to be triggered when PNO channel
 * prediction feature is enabled. This parameter is intended
 * to tweak the internal algortihm for experiments.
 *
 * Related: None
 *
 * Supported Feature: Scan
 *
 * Usage: Internal
 *
 * </ini>
 */
#define CFG_CHANNEL_PREDICTION_SCAN_TIMER CFG_INI_UINT( \
			"gChPredictionFullScanMs", \
			3000, \
			0x7fffffff, \
			60000, \
			CFG_VALUE_OR_DEFAULT, \
			"Timer value for channel prediction")

/*
 * <ini>
 * pnoscan_adaptive_dwell_mode - Enable adaptive dwell mode
 * during pno scan
 * @Min: 0
 * @Max: 4
 * @Default: 1
 *
 * This ini will set the algo used in dwell time optimization
 * during pno scan. see enum scan_dwelltime_adaptive_mode.
 * Acceptable values for this:
 * 0: Default (Use firmware default mode)
 * 1: Conservative optimization
 * 2: Moderate optimization
 * 3: Aggressive optimization
 * 4: Static
 *
 * Related: None
 *
 * Supported Feature: Scan
 *
 * Usage: External
 *
 * </ini>
 */
#define CFG_ADAPTIVE_PNOSCAN_DWELL_MODE CFG_INI_UINT( \
			"pnoscan_adaptive_dwell_mode", \
			0, \
			4, \
			1, \
			CFG_VALUE_OR_DEFAULT, \
			"Algorithm used in dwell time optimization")

/*
 * <ini>
 * gScanBackoffMultiplier - For NLO/PNO, multiply fast scan period by this every
 *      max cycles
 * @Min: 0
 * @Max: 255
 * @Default: 0
 *
 * For Network Listen Offload and Perfered Network Offload, multiply the fast
 * scan period by this value after max cycles have occurred. Setting this to 0
 * disables the feature.
 *
 * @E.g.
 *      # Disable scan backoff multiplier
 *      gScanBackoffMultiplier=0
 *      # Effectively the same
 *      gScanBackoffMultiplier=1
 *      # Double the scan period after each max cycles have occurred
 *      gScanBackoffMultiplier=2
 *
 * Related: NLO, PNO
 *
 * Usage: Internal/External
 *
 * </ini>
 */
#define CFG_SCAN_BACKOFF_MULTIPLIER CFG_INI_UINT( \
			"gScanBackoffMultiplier", \
			0, \
			255, \
			0, \
			CFG_VALUE_OR_DEFAULT, \
			"Scan backoff multiplier")

/*
 * <ini>
 * mawc_nlo_enabled - For NLO/PNO, enable MAWC based scan
 * @Min: 0
 * @Max: 1
 * @Default: 1
 *
 * Enable/Disable the Motion Aided Wireless Connectivity
 * based NLO using this parameter
 *
 * Related: NLO, PNO
 *
 * Usage: Internal/External
 *
 * </ini>
 */
#define CFG_MAWC_NLO_ENABLED CFG_INI_BOOL( \
			"mawc_nlo_enabled", \
			1, \
			"Enable MAWC based scan")

/*
 * <ini>
 * mawc_nlo_exp_backoff_ratio - Exponential back off ratio
 * @Min: 0
 * @Max: 300
 * @Default: 3
 *
 * Configure the exponential back off ratio using this
 * parameter for MAWC based NLO
 * ratio of exponential backoff, next = current + current*ratio/100
 *
 * Related: NLO, PNO
 *
 * Usage: Internal/External
 *
 * </ini>
 */
#define CFG_MAWC_NLO_EXP_BACKOFF_RATIO CFG_INI_UINT( \
			"mawc_nlo_exp_backoff_ratio", \
			0, \
			300, \
			3, \
			CFG_VALUE_OR_DEFAULT, \
			"MWAC based NLO exponential ratio")

/*
 * <ini>
 * mawc_nlo_init_scan_interval - Initial Scan Interval
 * @Min: 1000
 * @Max: 0xFFFFFFFF
 * @Default: 10000
 *
 * Configure the initial scan interval  using this
 * parameter for MAWC based NLO (Units in Milliseconds)
 *
 * Related: NLO, PNO
 *
 * Usage: Internal/External
 *
 * </ini>
 */
#define CFG_MAWC_NLO_INIT_SCAN_INTERVAL CFG_INI_UINT( \
			"mawc_nlo_init_scan_interval", \
			1000, \
			0xFFFFFFFF, \
			10000, \
			CFG_VALUE_OR_DEFAULT, \
			"Initial Scan Interval")

/*
 * <ini>
 * mawc_nlo_max_scan_interval - Maximum Scan Interval
 * @Min: 1000
 * @Max: 0xFFFFFFFF
 * @Default: 60000
 *
 * Configure the maximum scan interval  using this
 * parameter for MAWC based NLO (Units in Milliseconds)
 *
 * Related: NLO, PNO
 *
 * Usage: Internal/External
 *
 * </ini>
 */
#define CFG_MAWC_NLO_MAX_SCAN_INTERVAL CFG_INI_UINT( \
			"mawc_nlo_max_scan_interval", \
			1000, \
			0xFFFFFFFF, \
			60000, \
			CFG_VALUE_OR_DEFAULT, \
			"Maximum Scan Interval")

#define CFG_SCAN_PNO \
	CFG(CFG_MAX_SCHED_SCAN_PLAN_ITERATIONS) \
	CFG(CFG_MAX_SCHED_SCAN_PLAN_INTERVAL) \
	CFG(CFG_PNO_SCAN_SUPPORT) \
	CFG(CFG_ENABLE_DFS_PNO_CHNL_SCAN) \
	CFG(CFG_PNO_SCAN_TIMER_REPEAT_VALUE) \
	CFG(CFG_PNO_SLOW_SCAN_MULTIPLIER) \
	CFG(CFG_PNO_CHANNEL_PREDICTION) \
	CFG(CFG_TOP_K_NUM_OF_CHANNELS) \
	CFG(CFG_STATIONARY_THRESHOLD) \
	CFG(CFG_CHANNEL_PREDICTION_SCAN_TIMER) \
	CFG(CFG_ADAPTIVE_PNOSCAN_DWELL_MODE) \
	CFG(CFG_SCAN_BACKOFF_MULTIPLIER) \
	CFG(CFG_MAWC_NLO_ENABLED) \
	CFG(CFG_MAWC_NLO_EXP_BACKOFF_RATIO) \
	CFG(CFG_MAWC_NLO_INIT_SCAN_INTERVAL) \
	CFG(CFG_MAWC_NLO_MAX_SCAN_INTERVAL)

#else
#define CFG_SCAN_PNO
#endif  /* FEATURE_WLAN_SCAN_PNO */

/*
 * <ini>
 * gActiveMaxChannelTimeConc - Maximum active scan time in milliseconds.
 * @Min: 0
 * @Max: 10000
 * @Default: 40
 *
 * This ini is used to set maximum active scan time in STA+SAP concurrent
 * mode.
 *
 * Related: None.
 *
 * Supported Feature: Concurrency
 *
 * Usage: Internal/External
 *
 * </ini>
 */
#define CFG_ACTIVE_MAX_CHANNEL_TIME_CONC CFG_INI_UINT(\
				"gActiveMaxChannelTimeConc",\
				0, 10000, MCL_OR_WIN_VALUE(40, 0),\
				CFG_VALUE_OR_DEFAULT, \
				"active scan time in STA+SAP concurrent")

/*
 * <ini>
 * gPassiveMaxChannelTimeConc - Maximum passive scan time in milliseconds.
 * @Min: 0
 * @Max: 10000
 * @Default: 110
 *
 * This ini is used to set maximum passive scan time in STA+SAP concurrent
 * mode.
 *
 * Related: None.
 *
 * Supported Feature: Concurrency
 *
 * Usage: Internal/External
 *
 * </ini>
 */
#define CFG_PASSIVE_MAX_CHANNEL_TIME_CONC CFG_INI_UINT(\
				"gPassiveMaxChannelTimeConc",\
				0, 10000, MCL_OR_WIN_VALUE(110, 0),\
				CFG_VALUE_OR_DEFAULT, \
				"Set priority for connection with bssid_hint")

/*
 * <ini>
 * gRestTimeConc - Rest time before moving to a new channel to scan.
 * @Min: 0
 * @Max: 10000
 * @Default: 100
 *
 * This ini is used to configure rest time.
 *
 * Related: None.
 *
 * Supported Feature: Concurrency
 *
 * Usage: Internal/External
 *
 * </ini>
 */
#define CFG_MAX_REST_TIME_CONC CFG_INI_UINT(\
				"nRestTimeConc",\
				0, 10000, MCL_OR_WIN_VALUE(100, 0),\
				CFG_VALUE_OR_DEFAULT, \
				"Rest time before moving to a new channel")

/*
 * <ini>
 * min_rest_time_conc - Mininum time spent on home channel before moving to a
 * new channel to scan.
 * @Min: 0
 * @Max: 50
 * @Default: 50
 *
 * This ini is used to configure minimum time spent on home channel before
 * moving to a new channel to scan.
 *
 * Related: None.
 *
 * Supported Feature: Concurrency
 *
 * Usage: Internal/External
 *
 * </ini>
 */
#define CFG_MIN_REST_TIME_CONC CFG_INI_UINT(\
				"min_rest_time_conc",\
				0, 50, MCL_OR_WIN_VALUE(50, 0),\
				CFG_VALUE_OR_DEFAULT, \
				"minimum time spent on home channel")

/*
 * <ini>
 * wake_lock_in_user_scan - use wake lock during user scan
 * @Min: 0
 * @Max: 1
 * @Default: 0
 *
 * This ini is used to define if wake lock is held used during user scan req
 *
 * Related: Scan
 *
 * Usage: Internal/External
 *
 * </ini>
 */
#define CFG_ENABLE_WAKE_LOCK_IN_SCAN CFG_INI_BOOL( \
				"wake_lock_in_user_scan", \
				false, \
				"use wake lock during scan")

/*
 * <ini>
 * gIdleTimeConc - Data inactivity time in msec.
 * @Min: 0
 * @Max: 25
 * @Default: 25
 *
 * This ini is used to configure data inactivity time in msec on bss channel
 * that will be used by scan engine in firmware.
 * For example if this value is 25ms then firmware will check for data
 * inactivity every 25ms till gRestTimeConc is reached.
 * If inactive then scan engine will move from home channel to scan the next
 * frequency.
 *
 * Related: None.
 *
 * Supported Feature: Concurrency
 *
 * Usage: Internal/External
 *
 * </ini>
 */
#define CFG_IDLE_TIME_CONC CFG_INI_UINT(\
				"gIdleTimeConc",\
				0, 25, MCL_OR_WIN_VALUE(25, 0),\
				CFG_VALUE_OR_DEFAULT, \
				"data inactivity time on bss channel")

/*
 * <ini>
 * gEnableMacAddrSpoof - Enable mac address randomization feature.
 * @Min: 0
 * @Max: 1
 * @Default: 1
 *
 * This ini is used to enable/disable mac address randomization for scan.
 *
 * Supported Feature: SCAN
 *
 *
 * Usage: Internal/External
 *
 * </ini>
 */
#define CFG_ENABLE_MAC_ADDR_SPOOFING CFG_INI_BOOL( \
		"gEnableMacAddrSpoof", \
		true, \
		"Enable mac spoofing")

/*
 * <ini>
 * gScanAgingTime - Set scan aging time
 * @Min: 0
 * @Max: 200
 * @Default: 30
 *
 * This ini is used to set scan aging timeout value
 * in secs. For example after 30 secs the bss results
 * greater than 30secs age will be flushed.
 *
 * Related: None
 *
 * Supported Feature: Scan
 *
 * Usage: External
 *
 * </ini>
 */
#ifdef QCA_WIFI_NAPIER_EMULATION
#define CFG_SCAN_AGING_TIME_DEFAULT (90)
#else
#define CFG_SCAN_AGING_TIME_DEFAULT (30)
#endif

#define CFG_SCAN_AGING_TIME CFG_INI_UINT( \
			"gScanAgingTime", \
			0, \
			200, \
			CFG_SCAN_AGING_TIME_DEFAULT, \
			CFG_VALUE_OR_DEFAULT, \
			"scan aging time")
/*
 * <ini>
 * extscan_adaptive_dwell_mode  Enable adaptive dwell mode
 * during ext scan
 * @Min: 0
 * @Max: 4
 * @Default: 1
 *
 * This ini will set the algo used in dwell time optimization
 * during ext scan. see enum scan_dwelltime_adaptive_mode.
 * Acceptable values for this:
 * 0: Default (Use firmware default mode)
 * 1: Conservative optimization
 * 2: Moderate optimization
 * 3: Aggressive optimization
 * 4: Static
 *
 * Related: None
 *
 * Supported Feature: Scan
 *
 * Usage: External
 *
 * </ini>
 */
#define CFG_ADAPTIVE_EXTSCAN_DWELL_MODE CFG_INI_UINT( \
			"extscan_adaptive_dwell_mode", \
			0, \
			4, \
			1, \
			CFG_VALUE_OR_DEFAULT, \
			"ext scan adaptive dwell mode")

/*
 * <ini>
 * sta_scan_burst_duration - Burst duration in case of split scan.
 * @Min: 0
 * @Max: 180
 * @Default: 0
 *
 * This ini is used to set burst duration of sta scan requests.
 *
 * Related: None.
 *
 * Supported Feature: Concurrency
 *
 * Usage: External
 *
 * </ini>
 */
#define CFG_STA_SCAN_BURST_DURATION CFG_INI_UINT( \
			"sta_scan_burst_duration", \
			0, \
			180, \
			0, \
			CFG_VALUE_OR_DEFAULT, \
			"sta scan burst duration")

/*
 * <ini>
 * p2p_scan_burst_duration - Burst duration in case of split scan for p2p scan.
 * @Min: 0
 * @Max: 180
 * @Default: 0
 *
 * This ini is used to set burst duration of scan for p2p scan requests.
 *
 * Related: None.
 *
 * Supported Feature: Concurrency
 *
 * Usage: External
 *
 * </ini>
 */

#define CFG_P2P_SCAN_BURST_DURATION CFG_INI_UINT( \
			"p2p_scan_burst_duration", \
			0, \
			180, \
			0, \
			CFG_VALUE_OR_DEFAULT, \
			"p2p scan burst duration")
/*
 * <ini>
 * go_scan_burst_duration - Burst duration in case of split scan when GO is
 * active.
 * @Min: 0
 * @Max: 180
 * @Default: 0
 *
 * This ini is used to set burst duration of scan when GO is active.
 *
 * Related: None.
 *
 * Supported Feature: Concurrency
 *
 * Usage: External
 *
 * </ini>
 */
#define CFG_GO_SCAN_BURST_DURATION CFG_INI_UINT( \
			"go_scan_burst_duration", \
			0, \
			180, \
			0, \
			CFG_VALUE_OR_DEFAULT, \
			"go scan burst duration")

/*
 * <ini>
 * ap_scan_burst_duration - Burst duration in case of split scan when ap
 * is active.
 * @Min: 0
 * @Max: 32
 * @Default: 0
 *
 * This ini is used to set burst duration of scan when SAP is active.
 *
 * Related: None.
 *
 * Supported Feature: Concurrency
 *
 * Usage: External
 *
 * </ini>
 */
#define CFG_AP_SCAN_BURST_DURATION CFG_INI_UINT( \
			"ap_scan_burst_duration", \
			0, \
			32, \
			0, \
			CFG_VALUE_OR_DEFAULT, \
			"ap scan burst duration")

/*
 * <ini>
 * gSkipDfsChannelInP2pSearch - Skip DFS Channel in case of P2P Search
 * @Min: 0
 * @Max: 1
 * @Default: 1
 *
 * This ini is used to disable(skip) dfs channel in p2p search.
 * Related: None.
 *
 * Supported Feature: DFS P2P
 *
 * Usage: External
 *
 * <ini>
 */
#define CFG_ENABLE_SKIP_DFS_IN_P2P_SEARCH CFG_INI_BOOL( \
	"gSkipDfsChannelInP2pSearch", \
	1, \
	"skip dfs channel in p2p search")

/*
 * <ini>
 * gEnableConnectedScan - Will enable or disable scan in connected state
 * This ini is used to enable or disable the scanning in
 * Connected state
 * @Min: 0
 * @Max: 1
 * @Default: 1
 *
 * Related: None
 *
 * Supported Feature: STA
 *
 * Usage: External
 *
 * <ini>
 */
#define CFG_ENABLE_CONNECTED_SCAN CFG_INI_BOOL( \
				"gEnableConnectedScan", \
				true, \
				"Enable/disable scan in connected state")

/*
 * <ini>
 * gEnableSNRMonitoring - Enables SNR Monitoring
 * @Min: 0
 * @Max: 1
 * @Default: 0
 *
 * This ini is used to set default snr monitor
 *
 * Related: None
 *
 * Supported Feature: STA
 *
 * Usage: Internal/External
 *
 * </ini>
 */
#define CFG_ENABLE_SNR_MONITORING CFG_INI_BOOL(\
			"gEnableSNRMonitoring",\
			false,\
			"Enable/Disable SNR Monitoring")

#define CFG_SCAN_ALL \
	CFG(CFG_DROP_BCN_ON_CHANNEL_MISMATCH) \
	CFG(CFG_ENABLE_WAKE_LOCK_IN_SCAN) \
	CFG(CFG_ACTIVE_MAX_CHANNEL_TIME) \
	CFG(CFG_ENABLE_DFS_SCAN) \
	CFG(CFG_ENABLE_CONNECTED_SCAN) \
	CFG(CFG_INITIAL_NO_DFS_SCAN) \
	CFG(CFG_ACTIVE_MAX_2G_CHANNEL_TIME) \
	CFG(CFG_PASSIVE_MAX_CHANNEL_TIME) \
	CFG(CFG_SCAN_NUM_PROBES) \
	CFG(CFG_SCAN_PROBE_REPEAT_TIME) \
	CFG(CFG_ADAPTIVE_SCAN_DWELL_MODE) \
	CFG(CFG_ADAPTIVE_SCAN_DWELL_MODE_NC) \
	CFG(CFG_IS_BSSID_HINT_PRIORITY) \
	CFG(CFG_PASSIVE_MAX_CHANNEL_TIME_CONC) \
	CFG(CFG_ACTIVE_MAX_CHANNEL_TIME_CONC) \
	CFG(CFG_MAX_REST_TIME_CONC) \
	CFG(CFG_MIN_REST_TIME_CONC) \
	CFG(CFG_IDLE_TIME_CONC) \
	CFG(CFG_ENABLE_MAC_ADDR_SPOOFING) \
	CFG(CFG_SCAN_AGING_TIME) \
	CFG(CFG_ADAPTIVE_EXTSCAN_DWELL_MODE) \
	CFG(CFG_STA_SCAN_BURST_DURATION) \
	CFG(CFG_P2P_SCAN_BURST_DURATION) \
	CFG(CFG_GO_SCAN_BURST_DURATION) \
	CFG(CFG_ENABLE_SNR_MONITORING) \
	CFG(CFG_AP_SCAN_BURST_DURATION) \
	CFG(CFG_ENABLE_SKIP_DFS_IN_P2P_SEARCH) \
	CFG_SCAN_PNO

#endif /* __CONFIG_SCAN_H */
