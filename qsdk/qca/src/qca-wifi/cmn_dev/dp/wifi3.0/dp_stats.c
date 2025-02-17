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
#include "qdf_types.h"
#include "dp_peer.h"
#include "dp_types.h"
#include "dp_internal.h"
#include "htt_stats.h"
#include "htt_ppdu_stats.h"
#include "dp_htt.h"

#define DP_MAX_STRING_LEN 500

#define DP_HTT_HW_INTR_NAME_LEN  HTT_STATS_MAX_HW_INTR_NAME_LEN
#define DP_HTT_HW_MODULE_NAME_LEN  HTT_STATS_MAX_HW_MODULE_NAME_LEN
#define DP_HTT_COUNTER_NAME_LEN  HTT_MAX_COUNTER_NAME
#define DP_HTT_LOW_WM_HIT_COUNT_LEN  HTT_STATS_LOW_WM_BINS
#define DP_HTT_HIGH_WM_HIT_COUNT_LEN  HTT_STATS_HIGH_WM_BINS
#define DP_HTT_TX_MCS_LEN  HTT_TX_PDEV_STATS_NUM_MCS_COUNTERS
#define DP_HTT_TX_SU_MCS_LEN  HTT_TX_PDEV_STATS_NUM_MCS_COUNTERS
#define DP_HTT_TX_MU_MCS_LEN  HTT_TX_PDEV_STATS_NUM_MCS_COUNTERS
#define DP_HTT_TX_NSS_LEN  HTT_TX_PDEV_STATS_NUM_SPATIAL_STREAMS
#define DP_HTT_TX_BW_LEN  HTT_TX_PDEV_STATS_NUM_BW_COUNTERS
#define DP_HTT_TX_PREAM_LEN  HTT_TX_PDEV_STATS_NUM_PREAMBLE_TYPES
#define DP_HTT_TX_PDEV_GI_LEN  HTT_TX_PDEV_STATS_NUM_GI_COUNTERS
#define DP_HTT_TX_DCM_LEN  HTT_TX_PDEV_STATS_NUM_DCM_COUNTERS
#define DP_HTT_RX_MCS_LEN  HTT_RX_PDEV_STATS_NUM_MCS_COUNTERS
#define DP_HTT_RX_NSS_LEN  HTT_RX_PDEV_STATS_NUM_SPATIAL_STREAMS
#define DP_HTT_RX_DCM_LEN  HTT_RX_PDEV_STATS_NUM_DCM_COUNTERS
#define DP_HTT_RX_BW_LEN  HTT_RX_PDEV_STATS_NUM_BW_COUNTERS
#define DP_HTT_RX_PREAM_LEN  HTT_RX_PDEV_STATS_NUM_PREAMBLE_TYPES
#define DP_HTT_RSSI_CHAIN_LEN  HTT_RX_PDEV_STATS_NUM_SPATIAL_STREAMS
#define DP_HTT_RX_GI_LEN  HTT_RX_PDEV_STATS_NUM_GI_COUNTERS
#define DP_HTT_FW_RING_MGMT_SUBTYPE_LEN  HTT_STATS_SUBTYPE_MAX
#define DP_HTT_FW_RING_CTRL_SUBTYPE_LEN  HTT_STATS_SUBTYPE_MAX
#define DP_HTT_FW_RING_MPDU_ERR_LEN  HTT_RX_STATS_RXDMA_MAX_ERR
#define DP_HTT_TID_NAME_LEN  MAX_HTT_TID_NAME
#define DP_HTT_PEER_NUM_SS HTT_RX_PEER_STATS_NUM_SPATIAL_STREAMS
#define DP_HTT_PDEV_TX_GI_LEN HTT_TX_PDEV_STATS_NUM_GI_COUNTERS

#ifdef CONFIG_WIN
const char *fw_to_hw_delay_bucket[CDP_DELAY_BUCKET_MAX + 1] = {
	"0 to 10 ms", "11 to 20 ms",
	"21 to 30 ms", "31 to 40 ms",
	"41 to 50 ms", "51 to 60 ms",
	"61 to 70 ms", "71 to 80 ms",
	"81 to 90 ms", "91 to 100 ms",
	"101 to 250 ms", "251 to 500 ms", "500+ ms"
};

const char *sw_enq_delay_bucket[CDP_DELAY_BUCKET_MAX + 1] = {
	"0 to 1 ms", "1 to 2 ms",
	"2 to 3 ms", "3 to 4 ms",
	"4 to 5 ms", "5 to 6 ms",
	"6 to 7 ms", "7 to 8 ms",
	"8 to 9 ms", "9 to 10 ms",
	"10 to 11 ms", "11 to 12 ms", "12+ ms"
};

const char *intfrm_delay_bucket[CDP_DELAY_BUCKET_MAX + 1] = {
	"0 to 5 ms", "6 to 10 ms",
	"11 to 15 ms", "16 to 20 ms",
	"21 to 25 ms", "26 to 30 ms",
	"31 to 35 ms", "36 to 40 ms",
	"41 to 45 ms", "46 to 50 ms",
	"51 to 55 ms", "56 to 60 ms", "60+ ms"
};
#endif

#ifdef WLAN_TX_PKT_CAPTURE_ENH
#include "dp_tx_capture.h"
#endif

/*
 * dp_print_stats_string_tlv: display htt_stats_string_tlv
 * @tag_buf: buffer containing the tlv htt_stats_string_tlv
 *
 * return:void
 */
static inline void dp_print_stats_string_tlv(uint32_t *tag_buf)
{
	htt_stats_string_tlv *dp_stats_buf =
		(htt_stats_string_tlv *)tag_buf;
	uint8_t i;
	uint16_t index = 0;
	uint32_t tag_len = (HTT_STATS_TLV_LENGTH_GET(*tag_buf) >> 2);
	char *data = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!data) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	DP_TRACE_STATS(FATAL, "HTT_STATS_STRING_TLV:");
	for (i = 0; i <  tag_len; i++) {
		index += qdf_snprint(&data[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->data[i]);
	}
	DP_TRACE_STATS(FATAL, "data = %s\n", data);
	qdf_mem_free(data);
}

/*
 * dp_print_tx_pdev_stats_cmn_tlv: display htt_tx_pdev_stats_cmn_tlv
 * @tag_buf: buffer containing the tlv htt_tx_pdev_stats_cmn_tlv
 *
 * return:void
 */
static inline void dp_print_tx_pdev_stats_cmn_tlv(uint32_t *tag_buf)
{
	htt_tx_pdev_stats_cmn_tlv *dp_stats_buf =
		(htt_tx_pdev_stats_cmn_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_TX_PDEV_STATS_CMN_TLV:");
	DP_TRACE_STATS(FATAL, "mac_id__word = %u",
			dp_stats_buf->mac_id__word);
	DP_TRACE_STATS(FATAL, "hw_queued = %u",
			dp_stats_buf->hw_queued);
	DP_TRACE_STATS(FATAL, "hw_reaped = %u",
			dp_stats_buf->hw_reaped);
	DP_TRACE_STATS(FATAL, "underrun = %u",
			dp_stats_buf->underrun);
	DP_TRACE_STATS(FATAL, "hw_paused = %u",
			dp_stats_buf->hw_paused);
	DP_TRACE_STATS(FATAL, "hw_flush = %u",
			dp_stats_buf->hw_flush);
	DP_TRACE_STATS(FATAL, "hw_filt = %u",
			dp_stats_buf->hw_filt);
	DP_TRACE_STATS(FATAL, "tx_abort = %u",
			dp_stats_buf->tx_abort);
	DP_TRACE_STATS(FATAL, "mpdu_requeued = %u",
			dp_stats_buf->mpdu_requed);
	DP_TRACE_STATS(FATAL, "tx_xretry = %u",
			dp_stats_buf->tx_xretry);
	DP_TRACE_STATS(FATAL, "data_rc = %u",
			dp_stats_buf->data_rc);
	DP_TRACE_STATS(FATAL, "mpdu_dropped_xretry = %u",
			dp_stats_buf->mpdu_dropped_xretry);
	DP_TRACE_STATS(FATAL, "illegal_rate_phy_err = %u",
			dp_stats_buf->illgl_rate_phy_err);
	DP_TRACE_STATS(FATAL, "cont_xretry = %u",
			dp_stats_buf->cont_xretry);
	DP_TRACE_STATS(FATAL, "tx_timeout = %u",
			dp_stats_buf->tx_timeout);
	DP_TRACE_STATS(FATAL, "pdev_resets = %u",
			dp_stats_buf->pdev_resets);
	DP_TRACE_STATS(FATAL, "phy_underrun = %u",
			dp_stats_buf->phy_underrun);
	DP_TRACE_STATS(FATAL, "txop_ovf = %u",
			dp_stats_buf->txop_ovf);
	DP_TRACE_STATS(FATAL, "seq_posted = %u",
			dp_stats_buf->seq_posted);
	DP_TRACE_STATS(FATAL, "seq_failed_queueing = %u",
			dp_stats_buf->seq_failed_queueing);
	DP_TRACE_STATS(FATAL, "seq_completed = %u",
			dp_stats_buf->seq_completed);
	DP_TRACE_STATS(FATAL, "seq_restarted = %u",
			dp_stats_buf->seq_restarted);
	DP_TRACE_STATS(FATAL, "mu_seq_posted = %u",
			dp_stats_buf->mu_seq_posted);
	DP_TRACE_STATS(FATAL, "seq_switch_hw_paused = %u",
			dp_stats_buf->seq_switch_hw_paused);
	DP_TRACE_STATS(FATAL, "next_seq_posted_dsr = %u",
			dp_stats_buf->next_seq_posted_dsr);
	DP_TRACE_STATS(FATAL, "seq_posted_isr = %u",
			dp_stats_buf->seq_posted_isr);
	DP_TRACE_STATS(FATAL, "seq_ctrl_cached = %u",
			dp_stats_buf->seq_ctrl_cached);
	DP_TRACE_STATS(FATAL, "mpdu_count_tqm = %u",
			dp_stats_buf->mpdu_count_tqm);
	DP_TRACE_STATS(FATAL, "msdu_count_tqm = %u",
			dp_stats_buf->msdu_count_tqm);
	DP_TRACE_STATS(FATAL, "mpdu_removed_tqm = %u",
			dp_stats_buf->mpdu_removed_tqm);
	DP_TRACE_STATS(FATAL, "msdu_removed_tqm = %u",
			dp_stats_buf->msdu_removed_tqm);
	DP_TRACE_STATS(FATAL, "mpdus_sw_flush = %u",
			dp_stats_buf->mpdus_sw_flush);
	DP_TRACE_STATS(FATAL, "mpdus_hw_filter = %u",
			dp_stats_buf->mpdus_hw_filter);
	DP_TRACE_STATS(FATAL, "mpdus_truncated = %u",
			dp_stats_buf->mpdus_truncated);
	DP_TRACE_STATS(FATAL, "mpdus_ack_failed = %u",
			dp_stats_buf->mpdus_ack_failed);
	DP_TRACE_STATS(FATAL, "mpdus_expired = %u",
			dp_stats_buf->mpdus_expired);
	DP_TRACE_STATS(FATAL, "mpdus_seq_hw_retry = %u",
			dp_stats_buf->mpdus_seq_hw_retry);
	DP_TRACE_STATS(FATAL, "ack_tlv_proc = %u",
			dp_stats_buf->ack_tlv_proc);
	DP_TRACE_STATS(FATAL, "coex_abort_mpdu_cnt_valid = %u",
			dp_stats_buf->coex_abort_mpdu_cnt_valid);
	DP_TRACE_STATS(FATAL, "coex_abort_mpdu_cnt = %u\n",
			dp_stats_buf->coex_abort_mpdu_cnt);
}

/*
 * dp_print_tx_pdev_stats_urrn_tlv_v: display htt_tx_pdev_stats_urrn_tlv_v
 * @tag_buf: buffer containing the tlv htt_tx_pdev_stats_urrn_tlv_v
 *
 * return:void
 */
static inline void dp_print_tx_pdev_stats_urrn_tlv_v(uint32_t *tag_buf)
{
	htt_tx_pdev_stats_urrn_tlv_v *dp_stats_buf =
		(htt_tx_pdev_stats_urrn_tlv_v *)tag_buf;
	uint8_t i;
	uint16_t index = 0;
	uint32_t tag_len = (HTT_STATS_TLV_LENGTH_GET(*tag_buf) >> 2);
	char *urrn_stats = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!urrn_stats) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	tag_len = qdf_min(tag_len, (uint32_t)HTT_TX_PDEV_MAX_URRN_STATS);
	DP_TRACE_STATS(FATAL, "HTT_TX_PDEV_STATS_URRN_TLV_V:");
	for (i = 0; i <  tag_len; i++) {
		index += qdf_snprint(&urrn_stats[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->urrn_stats[i]);
	}
	DP_TRACE_STATS(FATAL, "urrn_stats = %s\n", urrn_stats);
	qdf_mem_free(urrn_stats);
}

/*
 * dp_print_tx_pdev_stats_flush_tlv_v: display htt_tx_pdev_stats_flush_tlv_v
 * @tag_buf: buffer containing the tlv htt_tx_pdev_stats_flush_tlv_v
 *
 * return:void
 */
static inline void dp_print_tx_pdev_stats_flush_tlv_v(uint32_t *tag_buf)
{
	htt_tx_pdev_stats_flush_tlv_v *dp_stats_buf =
		(htt_tx_pdev_stats_flush_tlv_v *)tag_buf;
	uint8_t i;
	uint16_t index = 0;
	uint32_t tag_len = (HTT_STATS_TLV_LENGTH_GET(*tag_buf) >> 2);
	char *flush_errs = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!flush_errs) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	tag_len = qdf_min(tag_len,
			(uint32_t)HTT_TX_PDEV_MAX_FLUSH_REASON_STATS);

	DP_TRACE_STATS(FATAL, "HTT_TX_PDEV_STATS_FLUSH_TLV_V:");
	for (i = 0; i <  tag_len; i++) {
		index += qdf_snprint(&flush_errs[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->flush_errs[i]);
	}
	DP_TRACE_STATS(FATAL, "flush_errs = %s\n", flush_errs);
	qdf_mem_free(flush_errs);
}

/*
 * dp_print_tx_pdev_stats_sifs_tlv_v: display htt_tx_pdev_stats_sifs_tlv_v
 * @tag_buf: buffer containing the tlv htt_tx_pdev_stats_sifs_tlv_v
 *
 * return:void
 */
static inline void dp_print_tx_pdev_stats_sifs_tlv_v(uint32_t *tag_buf)
{
	htt_tx_pdev_stats_sifs_tlv_v *dp_stats_buf =
		(htt_tx_pdev_stats_sifs_tlv_v *)tag_buf;
	uint8_t i;
	uint16_t index = 0;
	uint32_t tag_len = (HTT_STATS_TLV_LENGTH_GET(*tag_buf) >> 2);
	char *sifs_status = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!sifs_status) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	tag_len = qdf_min(tag_len, (uint32_t)HTT_TX_PDEV_MAX_SIFS_BURST_STATS);

	DP_TRACE_STATS(FATAL, "HTT_TX_PDEV_STATS_SIFS_TLV_V:");
	for (i = 0; i <  tag_len; i++) {
		index += qdf_snprint(&sifs_status[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->sifs_status[i]);
	}
	DP_TRACE_STATS(FATAL, "sifs_status = %s\n", sifs_status);
	qdf_mem_free(sifs_status);
}

/*
 * dp_print_tx_pdev_stats_phy_err_tlv_v: display htt_tx_pdev_stats_phy_err_tlv_v
 * @tag_buf: buffer containing the tlv htt_tx_pdev_stats_phy_err_tlv_v
 *
 * return:void
 */
static inline void dp_print_tx_pdev_stats_phy_err_tlv_v(uint32_t *tag_buf)
{
	htt_tx_pdev_stats_phy_err_tlv_v *dp_stats_buf =
		(htt_tx_pdev_stats_phy_err_tlv_v *)tag_buf;
	uint8_t i;
	uint16_t index = 0;
	uint32_t tag_len = (HTT_STATS_TLV_LENGTH_GET(*tag_buf) >> 2);
	char *phy_errs = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!phy_errs) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	tag_len = qdf_min(tag_len, (uint32_t)HTT_TX_PDEV_MAX_PHY_ERR_STATS);

	DP_TRACE_STATS(FATAL, "HTT_TX_PDEV_STATS_PHY_ERR_TLV_V:");
	for (i = 0; i <  tag_len; i++) {
		index += qdf_snprint(&phy_errs[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->phy_errs[i]);
	}
	DP_TRACE_STATS(FATAL, "phy_errs = %s\n", phy_errs);
	qdf_mem_free(phy_errs);
}

/*
 * dp_print_hw_stats_intr_misc_tlv: display htt_hw_stats_intr_misc_tlv
 * @tag_buf: buffer containing the tlv htt_hw_stats_intr_misc_tlv
 *
 * return:void
 */
static inline void dp_print_hw_stats_intr_misc_tlv(uint32_t *tag_buf)
{
	htt_hw_stats_intr_misc_tlv *dp_stats_buf =
		(htt_hw_stats_intr_misc_tlv *)tag_buf;
	uint8_t i;
	uint16_t index = 0;
	char *hw_intr_name = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!hw_intr_name) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	DP_TRACE_STATS(FATAL, "HTT_HW_STATS_INTR_MISC_TLV:");
	for (i = 0; i <  DP_HTT_HW_INTR_NAME_LEN; i++) {
		index += qdf_snprint(&hw_intr_name[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->hw_intr_name[i]);
	}
	DP_TRACE_STATS(FATAL, "hw_intr_name = %s ", hw_intr_name);
	DP_TRACE_STATS(FATAL, "mask = %u",
			dp_stats_buf->mask);
	DP_TRACE_STATS(FATAL, "count = %u\n",
			dp_stats_buf->count);
	qdf_mem_free(hw_intr_name);
}

/*
 * dp_print_hw_stats_wd_timeout_tlv: display htt_hw_stats_wd_timeout_tlv
 * @tag_buf: buffer containing the tlv htt_hw_stats_wd_timeout_tlv
 *
 * return:void
 */
static inline void dp_print_hw_stats_wd_timeout_tlv(uint32_t *tag_buf)
{
	htt_hw_stats_wd_timeout_tlv *dp_stats_buf =
		(htt_hw_stats_wd_timeout_tlv *)tag_buf;
	uint8_t i;
	uint16_t index = 0;
	char *hw_module_name = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!hw_module_name) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	DP_TRACE_STATS(FATAL, "HTT_HW_STATS_WD_TIMEOUT_TLV:");
	for (i = 0; i <  DP_HTT_HW_MODULE_NAME_LEN; i++) {
		index += qdf_snprint(&hw_module_name[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->hw_module_name[i]);
	}
	DP_TRACE_STATS(FATAL, "hw_module_name = %s ", hw_module_name);
	DP_TRACE_STATS(FATAL, "count = %u",
			dp_stats_buf->count);
	qdf_mem_free(hw_module_name);
}

/*
 * dp_print_hw_stats_pdev_errs_tlv: display htt_hw_stats_pdev_errs_tlv
 * @tag_buf: buffer containing the tlv htt_hw_stats_pdev_errs_tlv
 *
 * return:void
 */
static inline void dp_print_hw_stats_pdev_errs_tlv(uint32_t *tag_buf)
{
	htt_hw_stats_pdev_errs_tlv *dp_stats_buf =
		(htt_hw_stats_pdev_errs_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_HW_STATS_PDEV_ERRS_TLV:");
	DP_TRACE_STATS(FATAL, "mac_id__word = %u",
			dp_stats_buf->mac_id__word);
	DP_TRACE_STATS(FATAL, "tx_abort = %u",
			dp_stats_buf->tx_abort);
	DP_TRACE_STATS(FATAL, "tx_abort_fail_count = %u",
			dp_stats_buf->tx_abort_fail_count);
	DP_TRACE_STATS(FATAL, "rx_abort = %u",
			dp_stats_buf->rx_abort);
	DP_TRACE_STATS(FATAL, "rx_abort_fail_count = %u",
			dp_stats_buf->rx_abort_fail_count);
	DP_TRACE_STATS(FATAL, "warm_reset = %u",
			dp_stats_buf->warm_reset);
	DP_TRACE_STATS(FATAL, "cold_reset = %u",
			dp_stats_buf->cold_reset);
	DP_TRACE_STATS(FATAL, "tx_flush = %u",
			dp_stats_buf->tx_flush);
	DP_TRACE_STATS(FATAL, "tx_glb_reset = %u",
			dp_stats_buf->tx_glb_reset);
	DP_TRACE_STATS(FATAL, "tx_txq_reset = %u",
			dp_stats_buf->tx_txq_reset);
	DP_TRACE_STATS(FATAL, "rx_timeout_reset = %u\n",
			dp_stats_buf->rx_timeout_reset);
}

/*
 * dp_print_msdu_flow_stats_tlv: display htt_msdu_flow_stats_tlv
 * @tag_buf: buffer containing the tlv htt_msdu_flow_stats_tlv
 *
 * return:void
 */
static inline void dp_print_msdu_flow_stats_tlv(uint32_t *tag_buf)
{
	htt_msdu_flow_stats_tlv *dp_stats_buf =
		(htt_msdu_flow_stats_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_MSDU_FLOW_STATS_TLV:");
	DP_TRACE_STATS(FATAL, "last_update_timestamp = %u",
			dp_stats_buf->last_update_timestamp);
	DP_TRACE_STATS(FATAL, "last_add_timestamp = %u",
			dp_stats_buf->last_add_timestamp);
	DP_TRACE_STATS(FATAL, "last_remove_timestamp = %u",
			dp_stats_buf->last_remove_timestamp);
	DP_TRACE_STATS(FATAL, "total_processed_msdu_count = %u",
			dp_stats_buf->total_processed_msdu_count);
	DP_TRACE_STATS(FATAL, "cur_msdu_count_in_flowq = %u",
			dp_stats_buf->cur_msdu_count_in_flowq);
	DP_TRACE_STATS(FATAL, "sw_peer_id = %u",
			dp_stats_buf->sw_peer_id);
	DP_TRACE_STATS(FATAL, "tx_flow_no__tid_num__drop_rule = %u\n",
			dp_stats_buf->tx_flow_no__tid_num__drop_rule);
}

/*
 * dp_print_tx_tid_stats_tlv: display htt_tx_tid_stats_tlv
 * @tag_buf: buffer containing the tlv htt_tx_tid_stats_tlv
 *
 * return:void
 */
static inline void dp_print_tx_tid_stats_tlv(uint32_t *tag_buf)
{
	htt_tx_tid_stats_tlv *dp_stats_buf =
		(htt_tx_tid_stats_tlv *)tag_buf;
	uint8_t i;
	uint16_t index = 0;
	char *tid_name = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!tid_name) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	DP_TRACE_STATS(FATAL, "HTT_TX_TID_STATS_TLV:");
	for (i = 0; i <  DP_HTT_TID_NAME_LEN; i++) {
		index += qdf_snprint(&tid_name[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->tid_name[i]);
	}
	DP_TRACE_STATS(FATAL, "tid_name = %s ", tid_name);
	DP_TRACE_STATS(FATAL, "sw_peer_id__tid_num = %u",
			dp_stats_buf->sw_peer_id__tid_num);
	DP_TRACE_STATS(FATAL, "num_sched_pending__num_ppdu_in_hwq = %u",
			dp_stats_buf->num_sched_pending__num_ppdu_in_hwq);
	DP_TRACE_STATS(FATAL, "tid_flags = %u",
			dp_stats_buf->tid_flags);
	DP_TRACE_STATS(FATAL, "hw_queued = %u",
			dp_stats_buf->hw_queued);
	DP_TRACE_STATS(FATAL, "hw_reaped = %u",
			dp_stats_buf->hw_reaped);
	DP_TRACE_STATS(FATAL, "mpdus_hw_filter = %u",
			dp_stats_buf->mpdus_hw_filter);
	DP_TRACE_STATS(FATAL, "qdepth_bytes = %u",
			dp_stats_buf->qdepth_bytes);
	DP_TRACE_STATS(FATAL, "qdepth_num_msdu = %u",
			dp_stats_buf->qdepth_num_msdu);
	DP_TRACE_STATS(FATAL, "qdepth_num_mpdu = %u",
			dp_stats_buf->qdepth_num_mpdu);
	DP_TRACE_STATS(FATAL, "last_scheduled_tsmp = %u",
			dp_stats_buf->last_scheduled_tsmp);
	DP_TRACE_STATS(FATAL, "pause_module_id = %u",
			dp_stats_buf->pause_module_id);
	DP_TRACE_STATS(FATAL, "block_module_id = %u\n",
			dp_stats_buf->block_module_id);
	DP_TRACE_STATS(FATAL, "tid_tx_airtime = %u\n",
			dp_stats_buf->tid_tx_airtime);
	qdf_mem_free(tid_name);
}

#ifdef CONFIG_WIN
/*
 * dp_print_tx_tid_stats_v1_tlv: display htt_tx_tid_stats_v1_tlv
 * @tag_buf: buffer containing the tlv htt_tx_tid_stats_v1_tlv
 *
 * return:void
 */
static inline void dp_print_tx_tid_stats_v1_tlv(uint32_t *tag_buf)
{
	htt_tx_tid_stats_v1_tlv *dp_stats_buf =
		(htt_tx_tid_stats_v1_tlv *)tag_buf;
	uint8_t i;
	uint16_t index = 0;
	char *tid_name = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!tid_name) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	DP_TRACE_STATS(FATAL, "HTT_TX_TID_STATS_V1_TLV:");
	for (i = 0; i <  DP_HTT_TID_NAME_LEN; i++) {
		index += qdf_snprint(&tid_name[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->tid_name[i]);
	}
	DP_TRACE_STATS(FATAL, "tid_name = %s ", tid_name);
	DP_TRACE_STATS(FATAL, "sw_peer_id__tid_num = %u",
			dp_stats_buf->sw_peer_id__tid_num);
	DP_TRACE_STATS(FATAL, "num_sched_pending__num_ppdu_in_hwq = %u",
			dp_stats_buf->num_sched_pending__num_ppdu_in_hwq);
	DP_TRACE_STATS(FATAL, "tid_flags = %u",
			dp_stats_buf->tid_flags);
	DP_TRACE_STATS(FATAL, "max_qdepth_bytes = %u",
			dp_stats_buf->max_qdepth_bytes);
	DP_TRACE_STATS(FATAL, "max_qdepth_n_msdus = %u",
			dp_stats_buf->max_qdepth_n_msdus);
	DP_TRACE_STATS(FATAL, "rsvd = %u",
			dp_stats_buf->rsvd);
	DP_TRACE_STATS(FATAL, "qdepth_bytes = %u",
			dp_stats_buf->qdepth_bytes);
	DP_TRACE_STATS(FATAL, "qdepth_num_msdu = %u",
			dp_stats_buf->qdepth_num_msdu);
	DP_TRACE_STATS(FATAL, "qdepth_num_mpdu = %u",
			dp_stats_buf->qdepth_num_mpdu);
	DP_TRACE_STATS(FATAL, "last_scheduled_tsmp = %u",
			dp_stats_buf->last_scheduled_tsmp);
	DP_TRACE_STATS(FATAL, "pause_module_id = %u",
			dp_stats_buf->pause_module_id);
	DP_TRACE_STATS(FATAL, "block_module_id = %u\n",
			dp_stats_buf->block_module_id);
	DP_TRACE_STATS(FATAL, "tid_tx_airtime = %u\n",
			dp_stats_buf->tid_tx_airtime);
	qdf_mem_free(tid_name);
}
#endif

/*
 * dp_print_rx_tid_stats_tlv: display htt_rx_tid_stats_tlv
 * @tag_buf: buffer containing the tlv htt_rx_tid_stats_tlv
 *
 * return:void
 */
static inline void dp_print_rx_tid_stats_tlv(uint32_t *tag_buf)
{
	htt_rx_tid_stats_tlv *dp_stats_buf =
		(htt_rx_tid_stats_tlv *)tag_buf;
	uint8_t i;
	uint16_t index = 0;
	char *tid_name = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!tid_name) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	DP_TRACE_STATS(FATAL, "HTT_RX_TID_STATS_TLV:");
	DP_TRACE_STATS(FATAL, "sw_peer_id__tid_num = %u",
			dp_stats_buf->sw_peer_id__tid_num);
	for (i = 0; i <  DP_HTT_TID_NAME_LEN; i++) {
		index += qdf_snprint(&tid_name[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->tid_name[i]);
	}
	DP_TRACE_STATS(FATAL, "tid_name = %s ", tid_name);
	DP_TRACE_STATS(FATAL, "dup_in_reorder = %u",
			dp_stats_buf->dup_in_reorder);
	DP_TRACE_STATS(FATAL, "dup_past_outside_window = %u",
			dp_stats_buf->dup_past_outside_window);
	DP_TRACE_STATS(FATAL, "dup_past_within_window = %u",
			dp_stats_buf->dup_past_within_window);
	DP_TRACE_STATS(FATAL, "rxdesc_err_decrypt = %u\n",
			dp_stats_buf->rxdesc_err_decrypt);
	qdf_mem_free(tid_name);
}

/*
 * dp_print_counter_tlv: display htt_counter_tlv
 * @tag_buf: buffer containing the tlv htt_counter_tlv
 *
 * return:void
 */
static inline void dp_print_counter_tlv(uint32_t *tag_buf)
{
	htt_counter_tlv *dp_stats_buf =
		(htt_counter_tlv *)tag_buf;
	uint8_t i;
	uint16_t index = 0;
	char *counter_name = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!counter_name) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	DP_TRACE_STATS(FATAL, "HTT_COUNTER_TLV:");
	for (i = 0; i <  DP_HTT_COUNTER_NAME_LEN; i++) {
		index += qdf_snprint(&counter_name[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->counter_name[i]);
	}
	DP_TRACE_STATS(FATAL, "counter_name = %s ", counter_name);
	DP_TRACE_STATS(FATAL, "count = %u\n",
			dp_stats_buf->count);
	qdf_mem_free(counter_name);
}

/*
 * dp_print_peer_stats_cmn_tlv: display htt_peer_stats_cmn_tlv
 * @tag_buf: buffer containing the tlv htt_peer_stats_cmn_tlv
 *
 * return:void
 */
static inline void dp_print_peer_stats_cmn_tlv(uint32_t *tag_buf)
{
	htt_peer_stats_cmn_tlv *dp_stats_buf =
		(htt_peer_stats_cmn_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_PEER_STATS_CMN_TLV:");
	DP_TRACE_STATS(FATAL, "ppdu_cnt = %u",
			dp_stats_buf->ppdu_cnt);
	DP_TRACE_STATS(FATAL, "mpdu_cnt = %u",
			dp_stats_buf->mpdu_cnt);
	DP_TRACE_STATS(FATAL, "msdu_cnt = %u",
			dp_stats_buf->msdu_cnt);
	DP_TRACE_STATS(FATAL, "pause_bitmap = %u",
			dp_stats_buf->pause_bitmap);
	DP_TRACE_STATS(FATAL, "block_bitmap = %u",
			dp_stats_buf->block_bitmap);
	DP_TRACE_STATS(FATAL, "current_timestamp = %u\n",
			dp_stats_buf->current_timestamp);
	DP_TRACE_STATS(FATAL, "inactive_time = %u",
		       dp_stats_buf->inactive_time);
}

/*
 * dp_print_peer_details_tlv: display htt_peer_details_tlv
 * @tag_buf: buffer containing the tlv htt_peer_details_tlv
 *
 * return:void
 */
static inline void dp_print_peer_details_tlv(uint32_t *tag_buf)
{
	htt_peer_details_tlv *dp_stats_buf =
		(htt_peer_details_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_PEER_DETAILS_TLV:");
	DP_TRACE_STATS(FATAL, "peer_type = %u",
			dp_stats_buf->peer_type);
	DP_TRACE_STATS(FATAL, "sw_peer_id = %u",
			dp_stats_buf->sw_peer_id);
	DP_TRACE_STATS(FATAL, "vdev_pdev_ast_idx = %u",
			dp_stats_buf->vdev_pdev_ast_idx);
	DP_TRACE_STATS(FATAL, "mac_addr(upper 4 bytes) = %u",
			dp_stats_buf->mac_addr.mac_addr31to0);
	DP_TRACE_STATS(FATAL, "mac_addr(lower 2 bytes) = %u",
			dp_stats_buf->mac_addr.mac_addr47to32);
	DP_TRACE_STATS(FATAL, "peer_flags = %u",
			dp_stats_buf->peer_flags);
	DP_TRACE_STATS(FATAL, "qpeer_flags = %u\n",
			dp_stats_buf->qpeer_flags);
}

/*
 * dp_print_tx_peer_rate_stats_tlv: display htt_tx_peer_rate_stats_tlv
 * @tag_buf: buffer containing the tlv htt_tx_peer_rate_stats_tlv
 *
 * return:void
 */
static inline void dp_print_tx_peer_rate_stats_tlv(uint32_t *tag_buf)
{
	htt_tx_peer_rate_stats_tlv *dp_stats_buf =
		(htt_tx_peer_rate_stats_tlv *)tag_buf;
	uint8_t i, j;
	uint16_t index = 0;
	char *tx_gi[HTT_TX_PEER_STATS_NUM_GI_COUNTERS];
	char *str_buf = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!str_buf) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	for (i = 0; i < HTT_TX_PEER_STATS_NUM_GI_COUNTERS; i++)
		tx_gi[i] = (char *)qdf_mem_malloc(DP_MAX_STRING_LEN);

	DP_TRACE_STATS(FATAL, "HTT_TX_PEER_RATE_STATS_TLV:");
	DP_TRACE_STATS(FATAL, "tx_ldpc = %u",
			dp_stats_buf->tx_ldpc);
	DP_TRACE_STATS(FATAL, "rts_cnt = %u",
			dp_stats_buf->rts_cnt);
	DP_TRACE_STATS(FATAL, "ack_rssi = %u",
			dp_stats_buf->ack_rssi);

	index = 0;
	qdf_mem_zero(str_buf, DP_MAX_STRING_LEN);
	for (i = 0; i <  DP_HTT_TX_MCS_LEN; i++) {
		index += qdf_snprint(&str_buf[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->tx_mcs[i]);
	}
	DP_TRACE_STATS(FATAL, "tx_mcs = %s ", str_buf);

	index = 0;
	qdf_mem_zero(str_buf, DP_MAX_STRING_LEN);
	for (i = 0; i <  DP_HTT_TX_SU_MCS_LEN; i++) {
		index += qdf_snprint(&str_buf[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->tx_su_mcs[i]);
	}
	DP_TRACE_STATS(FATAL, "tx_su_mcs = %s ", str_buf);
	index = 0;
	qdf_mem_zero(str_buf, DP_MAX_STRING_LEN);
	for (i = 0; i <  DP_HTT_TX_MU_MCS_LEN; i++) {
		index += qdf_snprint(&str_buf[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->tx_mu_mcs[i]);
	}
	DP_TRACE_STATS(FATAL, "tx_mu_mcs = %s ", str_buf);
	index = 0;
	qdf_mem_zero(str_buf, DP_MAX_STRING_LEN);
	for (i = 0; i <  DP_HTT_TX_NSS_LEN; i++) {
		/* 0 stands for NSS 1, 1 stands for NSS 2, etc. */
		index += qdf_snprint(&str_buf[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", (i + 1),
				dp_stats_buf->tx_nss[i]);
	}
	DP_TRACE_STATS(FATAL, "tx_nss = %s ", str_buf);
	index = 0;
	qdf_mem_zero(str_buf, DP_MAX_STRING_LEN);
	for (i = 0; i <  DP_HTT_TX_BW_LEN; i++) {
		index += qdf_snprint(&str_buf[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->tx_bw[i]);
	}
	DP_TRACE_STATS(FATAL, "tx_bw = %s ", str_buf);
	index = 0;
	qdf_mem_zero(str_buf, DP_MAX_STRING_LEN);
	for (i = 0; i <  HTT_TX_PDEV_STATS_NUM_MCS_COUNTERS; i++) {
		index += qdf_snprint(&str_buf[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->tx_stbc[i]);
	}
	DP_TRACE_STATS(FATAL, "tx_stbc = %s ", str_buf);
	index = 0;
	qdf_mem_zero(str_buf, DP_MAX_STRING_LEN);

	for (i = 0; i <  DP_HTT_TX_PREAM_LEN; i++) {
		index += qdf_snprint(&str_buf[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->tx_pream[i]);
	}
	DP_TRACE_STATS(FATAL, "tx_pream = %s ", str_buf);

	for (j = 0; j < HTT_TX_PEER_STATS_NUM_GI_COUNTERS; j++) {
		index = 0;
		for (i = 0; i <  HTT_TX_PEER_STATS_NUM_MCS_COUNTERS; i++) {
			index += qdf_snprint(&tx_gi[j][index],
					DP_MAX_STRING_LEN - index,
					" %u:%u,", i,
					dp_stats_buf->tx_gi[j][i]);
		}
		DP_TRACE_STATS(FATAL, "tx_gi[%u] = %s ", j, tx_gi[j]);
	}

	index = 0;
	qdf_mem_zero(str_buf, DP_MAX_STRING_LEN);
	for (i = 0; i <  DP_HTT_TX_DCM_LEN; i++) {
		index += qdf_snprint(&str_buf[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->tx_dcm[i]);
	}
	DP_TRACE_STATS(FATAL, "tx_dcm = %s\n", str_buf);
	for (i = 0; i < HTT_TX_PEER_STATS_NUM_GI_COUNTERS; i++)
		qdf_mem_free(tx_gi[i]);

	qdf_mem_free(str_buf);
}

/*
 * dp_print_rx_peer_rate_stats_tlv: display htt_rx_peer_rate_stats_tlv
 * @tag_buf: buffer containing the tlv htt_rx_peer_rate_stats_tlv
 *
 * return:void
 */
static inline void dp_print_rx_peer_rate_stats_tlv(uint32_t *tag_buf)
{
	htt_rx_peer_rate_stats_tlv *dp_stats_buf =
		(htt_rx_peer_rate_stats_tlv *)tag_buf;
	uint8_t i, j;
	uint16_t index = 0;
	char *rssi_chain[DP_HTT_PEER_NUM_SS];
	char *rx_gi[HTT_RX_PEER_STATS_NUM_GI_COUNTERS];
	char *str_buf = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!str_buf) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	for (i = 0; i < DP_HTT_PEER_NUM_SS; i++)
		rssi_chain[i] = qdf_mem_malloc(DP_MAX_STRING_LEN);
	for (i = 0; i < HTT_RX_PEER_STATS_NUM_GI_COUNTERS; i++)
		rx_gi[i] = qdf_mem_malloc(DP_MAX_STRING_LEN);

	DP_TRACE_STATS(FATAL, "HTT_RX_PEER_RATE_STATS_TLV:");
	DP_TRACE_STATS(FATAL, "nsts = %u",
			dp_stats_buf->nsts);
	DP_TRACE_STATS(FATAL, "rx_ldpc = %u",
			dp_stats_buf->rx_ldpc);
	DP_TRACE_STATS(FATAL, "rts_cnt = %u",
			dp_stats_buf->rts_cnt);
	DP_TRACE_STATS(FATAL, "rssi_mgmt = %u",
			dp_stats_buf->rssi_mgmt);
	DP_TRACE_STATS(FATAL, "rssi_data = %u",
			dp_stats_buf->rssi_data);
	DP_TRACE_STATS(FATAL, "rssi_comb = %u",
			dp_stats_buf->rssi_comb);

	qdf_mem_zero(str_buf, DP_MAX_STRING_LEN);
	for (i = 0; i <  DP_HTT_RX_MCS_LEN; i++) {
		index += qdf_snprint(&str_buf[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->rx_mcs[i]);
	}
	DP_TRACE_STATS(FATAL, "rx_mcs = %s ", str_buf);

	index = 0;
	qdf_mem_zero(str_buf, DP_MAX_STRING_LEN);
	for (i = 0; i <  DP_HTT_RX_NSS_LEN; i++) {
		/* 0 stands for NSS 1, 1 stands for NSS 2, etc. */
		index += qdf_snprint(&str_buf[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", (i + 1),
				dp_stats_buf->rx_nss[i]);
	}
	DP_TRACE_STATS(FATAL, "rx_nss = %s ", str_buf);

	index = 0;
	qdf_mem_zero(str_buf, DP_MAX_STRING_LEN);
	for (i = 0; i <  DP_HTT_RX_DCM_LEN; i++) {
		index += qdf_snprint(&str_buf[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->rx_dcm[i]);
	}
	DP_TRACE_STATS(FATAL, "rx_dcm = %s ", str_buf);

	index = 0;
	qdf_mem_zero(str_buf, DP_MAX_STRING_LEN);
	for (i = 0; i <  HTT_RX_PDEV_STATS_NUM_MCS_COUNTERS; i++) {
		index += qdf_snprint(&str_buf[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->rx_stbc[i]);
	}
	DP_TRACE_STATS(FATAL, "rx_stbc = %s ", str_buf);

	index = 0;
	qdf_mem_zero(str_buf, DP_MAX_STRING_LEN);
	for (i = 0; i <  DP_HTT_RX_BW_LEN; i++) {
		index += qdf_snprint(&str_buf[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->rx_bw[i]);
	}
	DP_TRACE_STATS(FATAL, "rx_bw = %s ", str_buf);

	for (j = 0; j < DP_HTT_PEER_NUM_SS; j++) {
		qdf_mem_zero(str_buf, DP_MAX_STRING_LEN);
		index = 0;
		for (i = 0; i <  HTT_RX_PEER_STATS_NUM_BW_COUNTERS; i++) {
			index += qdf_snprint(&rssi_chain[j][index],
					DP_MAX_STRING_LEN - index,
					" %u:%u,", i,
					dp_stats_buf->rssi_chain[j][i]);
		}
		DP_TRACE_STATS(FATAL, "rssi_chain[%u] = %s ", j, rssi_chain[j]);
	}

	for (j = 0; j < HTT_RX_PEER_STATS_NUM_GI_COUNTERS; j++) {
		index = 0;
		for (i = 0; i <  HTT_RX_PDEV_STATS_NUM_MCS_COUNTERS; i++) {
			index += qdf_snprint(&rx_gi[j][index],
					DP_MAX_STRING_LEN - index,
					" %u:%u,", i,
					dp_stats_buf->rx_gi[j][i]);
		}
		DP_TRACE_STATS(FATAL, "rx_gi[%u] = %s ", j, rx_gi[j]);
	}
	index = 0;
	qdf_mem_zero(str_buf, DP_MAX_STRING_LEN);
	for (i = 0; i <  DP_HTT_RX_PREAM_LEN; i++) {
		index += qdf_snprint(&str_buf[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->rx_pream[i]);
	}
	DP_TRACE_STATS(FATAL, "rx_pream = %s\n", str_buf);

	for (i = 0; i < DP_HTT_PEER_NUM_SS; i++)
		qdf_mem_free(rssi_chain[i]);
	for (i = 0; i < HTT_RX_PEER_STATS_NUM_GI_COUNTERS; i++)
		qdf_mem_free(rx_gi[i]);

	qdf_mem_free(str_buf);
}

/*
 * dp_print_tx_hwq_mu_mimo_sch_stats_tlv: display htt_tx_hwq_mu_mimo_sch_stats
 * @tag_buf: buffer containing the tlv htt_tx_hwq_mu_mimo_sch_stats_tlv
 *
 * return:void
 */
static inline void dp_print_tx_hwq_mu_mimo_sch_stats_tlv(uint32_t *tag_buf)
{
	htt_tx_hwq_mu_mimo_sch_stats_tlv *dp_stats_buf =
		(htt_tx_hwq_mu_mimo_sch_stats_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_TX_HWQ_MU_MIMO_SCH_STATS_TLV:");
	DP_TRACE_STATS(FATAL, "mu_mimo_sch_posted = %u",
			dp_stats_buf->mu_mimo_sch_posted);
	DP_TRACE_STATS(FATAL, "mu_mimo_sch_failed = %u",
			dp_stats_buf->mu_mimo_sch_failed);
	DP_TRACE_STATS(FATAL, "mu_mimo_ppdu_posted = %u\n",
			dp_stats_buf->mu_mimo_ppdu_posted);
}

/*
 * dp_print_tx_hwq_mu_mimo_mpdu_stats_tlv: display htt_tx_hwq_mu_mimo_mpdu_stats
 * @tag_buf: buffer containing the tlv htt_tx_hwq_mu_mimo_mpdu_stats_tlv
 *
 * return:void
 */
static inline void dp_print_tx_hwq_mu_mimo_mpdu_stats_tlv(uint32_t *tag_buf)
{
	htt_tx_hwq_mu_mimo_mpdu_stats_tlv *dp_stats_buf =
		(htt_tx_hwq_mu_mimo_mpdu_stats_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_TX_HWQ_MU_MIMO_MPDU_STATS_TLV:");
	DP_TRACE_STATS(FATAL, "mu_mimo_mpdus_queued_usr = %u",
			dp_stats_buf->mu_mimo_mpdus_queued_usr);
	DP_TRACE_STATS(FATAL, "mu_mimo_mpdus_tried_usr = %u",
			dp_stats_buf->mu_mimo_mpdus_tried_usr);
	DP_TRACE_STATS(FATAL, "mu_mimo_mpdus_failed_usr = %u",
			dp_stats_buf->mu_mimo_mpdus_failed_usr);
	DP_TRACE_STATS(FATAL, "mu_mimo_mpdus_requeued_usr = %u",
			dp_stats_buf->mu_mimo_mpdus_requeued_usr);
	DP_TRACE_STATS(FATAL, "mu_mimo_err_no_ba_usr = %u",
			dp_stats_buf->mu_mimo_err_no_ba_usr);
	DP_TRACE_STATS(FATAL, "mu_mimo_mpdu_underrun_usr = %u",
			dp_stats_buf->mu_mimo_mpdu_underrun_usr);
	DP_TRACE_STATS(FATAL, "mu_mimo_ampdu_underrun_usr = %u\n",
			dp_stats_buf->mu_mimo_ampdu_underrun_usr);
}

/*
 * dp_print_tx_hwq_mu_mimo_cmn_stats_tlv: display htt_tx_hwq_mu_mimo_cmn_stats
 * @tag_buf: buffer containing the tlv htt_tx_hwq_mu_mimo_cmn_stats_tlv
 *
 * return:void
 */
static inline void dp_print_tx_hwq_mu_mimo_cmn_stats_tlv(uint32_t *tag_buf)
{
	htt_tx_hwq_mu_mimo_cmn_stats_tlv *dp_stats_buf =
		(htt_tx_hwq_mu_mimo_cmn_stats_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_TX_HWQ_MU_MIMO_CMN_STATS_TLV:");
	DP_TRACE_STATS(FATAL, "mac_id__hwq_id__word = %u\n",
			dp_stats_buf->mac_id__hwq_id__word);
}

/*
 * dp_print_tx_hwq_stats_cmn_tlv: display htt_tx_hwq_stats_cmn_tlv
 * @tag_buf: buffer containing the tlv htt_tx_hwq_stats_cmn_tlv
 *
 * return:void
 */
static inline void dp_print_tx_hwq_stats_cmn_tlv(uint32_t *tag_buf)
{
	htt_tx_hwq_stats_cmn_tlv *dp_stats_buf =
		(htt_tx_hwq_stats_cmn_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_TX_HWQ_STATS_CMN_TLV:");
	DP_TRACE_STATS(FATAL, "mac_id__hwq_id__word = %u",
			dp_stats_buf->mac_id__hwq_id__word);
	DP_TRACE_STATS(FATAL, "xretry = %u",
			dp_stats_buf->xretry);
	DP_TRACE_STATS(FATAL, "underrun_cnt = %u",
			dp_stats_buf->underrun_cnt);
	DP_TRACE_STATS(FATAL, "flush_cnt = %u",
			dp_stats_buf->flush_cnt);
	DP_TRACE_STATS(FATAL, "filt_cnt = %u",
			dp_stats_buf->filt_cnt);
	DP_TRACE_STATS(FATAL, "null_mpdu_bmap = %u",
			dp_stats_buf->null_mpdu_bmap);
	DP_TRACE_STATS(FATAL, "user_ack_failure = %u",
			dp_stats_buf->user_ack_failure);
	DP_TRACE_STATS(FATAL, "ack_tlv_proc = %u",
			dp_stats_buf->ack_tlv_proc);
	DP_TRACE_STATS(FATAL, "sched_id_proc = %u",
			dp_stats_buf->sched_id_proc);
	DP_TRACE_STATS(FATAL, "null_mpdu_tx_count = %u",
			dp_stats_buf->null_mpdu_tx_count);
	DP_TRACE_STATS(FATAL, "mpdu_bmap_not_recvd = %u",
			dp_stats_buf->mpdu_bmap_not_recvd);
	DP_TRACE_STATS(FATAL, "num_bar = %u",
			dp_stats_buf->num_bar);
	DP_TRACE_STATS(FATAL, "rts = %u",
			dp_stats_buf->rts);
	DP_TRACE_STATS(FATAL, "cts2self = %u",
			dp_stats_buf->cts2self);
	DP_TRACE_STATS(FATAL, "qos_null = %u",
			dp_stats_buf->qos_null);
	DP_TRACE_STATS(FATAL, "mpdu_tried_cnt = %u",
			dp_stats_buf->mpdu_tried_cnt);
	DP_TRACE_STATS(FATAL, "mpdu_queued_cnt = %u",
			dp_stats_buf->mpdu_queued_cnt);
	DP_TRACE_STATS(FATAL, "mpdu_ack_fail_cnt = %u",
			dp_stats_buf->mpdu_ack_fail_cnt);
	DP_TRACE_STATS(FATAL, "mpdu_filt_cnt = %u",
			dp_stats_buf->mpdu_filt_cnt);
	DP_TRACE_STATS(FATAL, "false_mpdu_ack_count = %u\n",
			dp_stats_buf->false_mpdu_ack_count);
}

/*
 * dp_print_tx_hwq_difs_latency_stats_tlv_v: display
 *					htt_tx_hwq_difs_latency_stats_tlv_v
 * @tag_buf: buffer containing the tlv htt_tx_hwq_difs_latency_stats_tlv_v
 *
 *return:void
 */
static inline void dp_print_tx_hwq_difs_latency_stats_tlv_v(uint32_t *tag_buf)
{
	htt_tx_hwq_difs_latency_stats_tlv_v *dp_stats_buf =
		(htt_tx_hwq_difs_latency_stats_tlv_v *)tag_buf;
	uint8_t i;
	uint16_t index = 0;
	uint32_t tag_len = (HTT_STATS_TLV_LENGTH_GET(*tag_buf) >> 2);
	char *difs_latency_hist = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!difs_latency_hist) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	tag_len = qdf_min(tag_len,
			(uint32_t)HTT_TX_HWQ_MAX_DIFS_LATENCY_BINS);

	DP_TRACE_STATS(FATAL, "HTT_TX_HWQ_DIFS_LATENCY_STATS_TLV_V:");
	DP_TRACE_STATS(FATAL, "hist_intvl = %u",
			dp_stats_buf->hist_intvl);

	for (i = 0; i <  tag_len; i++) {
		index += qdf_snprint(&difs_latency_hist[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i,
				dp_stats_buf->difs_latency_hist[i]);
	}
	DP_TRACE_STATS(FATAL, "difs_latency_hist = %s\n", difs_latency_hist);
	qdf_mem_free(difs_latency_hist);
}

/*
 * dp_print_tx_hwq_cmd_result_stats_tlv_v: display htt_tx_hwq_cmd_result_stats
 * @tag_buf: buffer containing the tlv htt_tx_hwq_cmd_result_stats_tlv_v
 *
 * return:void
 */
static inline void dp_print_tx_hwq_cmd_result_stats_tlv_v(uint32_t *tag_buf)
{
	htt_tx_hwq_cmd_result_stats_tlv_v *dp_stats_buf =
		(htt_tx_hwq_cmd_result_stats_tlv_v *)tag_buf;
	uint8_t i;
	uint16_t index = 0;
	uint32_t tag_len = (HTT_STATS_TLV_LENGTH_GET(*tag_buf) >> 2);
	char *cmd_result = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!cmd_result) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	tag_len = qdf_min(tag_len, (uint32_t)HTT_TX_HWQ_MAX_CMD_RESULT_STATS);

	DP_TRACE_STATS(FATAL, "HTT_TX_HWQ_CMD_RESULT_STATS_TLV_V:");
	for (i = 0; i <  tag_len; i++) {
		index += qdf_snprint(&cmd_result[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->cmd_result[i]);
	}
	DP_TRACE_STATS(FATAL, "cmd_result = %s ", cmd_result);
	qdf_mem_free(cmd_result);
}

/*
 * dp_print_tx_hwq_cmd_stall_stats_tlv_v: display htt_tx_hwq_cmd_stall_stats_tlv
 * @tag_buf: buffer containing the tlv htt_tx_hwq_cmd_stall_stats_tlv_v
 *
 * return:void
 */
static inline void dp_print_tx_hwq_cmd_stall_stats_tlv_v(uint32_t *tag_buf)
{
	htt_tx_hwq_cmd_stall_stats_tlv_v *dp_stats_buf =
		(htt_tx_hwq_cmd_stall_stats_tlv_v *)tag_buf;
	uint8_t i;
	uint16_t index = 0;
	uint32_t tag_len = (HTT_STATS_TLV_LENGTH_GET(*tag_buf) >> 2);
	char *cmd_stall_status = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!cmd_stall_status) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	tag_len = qdf_min(tag_len, (uint32_t)HTT_TX_HWQ_MAX_CMD_STALL_STATS);

	DP_TRACE_STATS(FATAL, "HTT_TX_HWQ_CMD_STALL_STATS_TLV_V:");
	for (i = 0; i <  tag_len; i++) {
		index += qdf_snprint(&cmd_stall_status[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i,
				dp_stats_buf->cmd_stall_status[i]);
	}
	DP_TRACE_STATS(FATAL, "cmd_stall_status = %s\n", cmd_stall_status);
	qdf_mem_free(cmd_stall_status);
}

/*
 * dp_print_tx_hwq_fes_result_stats_tlv_v: display htt_tx_hwq_fes_result_stats
 * @tag_buf: buffer containing the tlv htt_tx_hwq_fes_result_stats_tlv_v
 *
 * return:void
 */
static inline void dp_print_tx_hwq_fes_result_stats_tlv_v(uint32_t *tag_buf)
{
	htt_tx_hwq_fes_result_stats_tlv_v *dp_stats_buf =
		(htt_tx_hwq_fes_result_stats_tlv_v *)tag_buf;
	uint8_t i;
	uint16_t index = 0;
	uint32_t tag_len = (HTT_STATS_TLV_LENGTH_GET(*tag_buf) >> 2);
	char *fes_result = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!fes_result) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	tag_len = qdf_min(tag_len, (uint32_t)HTT_TX_HWQ_MAX_FES_RESULT_STATS);

	DP_TRACE_STATS(FATAL, "HTT_TX_HWQ_FES_RESULT_STATS_TLV_V:");
	for (i = 0; i <  tag_len; i++) {
		index += qdf_snprint(&fes_result[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->fes_result[i]);
	}
	DP_TRACE_STATS(FATAL, "fes_result = %s ", fes_result);
	qdf_mem_free(fes_result);
}

/*
 * dp_print_tx_selfgen_cmn_stats_tlv: display htt_tx_selfgen_cmn_stats_tlv
 * @tag_buf: buffer containing the tlv htt_tx_selfgen_cmn_stats_tlv
 *
 * return:void
 */
static inline void dp_print_tx_selfgen_cmn_stats_tlv(uint32_t *tag_buf)
{
	htt_tx_selfgen_cmn_stats_tlv *dp_stats_buf =
		(htt_tx_selfgen_cmn_stats_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_TX_SELFGEN_CMN_STATS_TLV:");
	DP_TRACE_STATS(FATAL, "mac_id__word = %u",
			dp_stats_buf->mac_id__word);
	DP_TRACE_STATS(FATAL, "su_bar = %u",
			dp_stats_buf->su_bar);
	DP_TRACE_STATS(FATAL, "rts = %u",
			dp_stats_buf->rts);
	DP_TRACE_STATS(FATAL, "cts2self = %u",
			dp_stats_buf->cts2self);
	DP_TRACE_STATS(FATAL, "qos_null = %u",
			dp_stats_buf->qos_null);
	DP_TRACE_STATS(FATAL, "delayed_bar_1 = %u",
			dp_stats_buf->delayed_bar_1);
	DP_TRACE_STATS(FATAL, "delayed_bar_2 = %u",
			dp_stats_buf->delayed_bar_2);
	DP_TRACE_STATS(FATAL, "delayed_bar_3 = %u",
			dp_stats_buf->delayed_bar_3);
	DP_TRACE_STATS(FATAL, "delayed_bar_4 = %u",
			dp_stats_buf->delayed_bar_4);
	DP_TRACE_STATS(FATAL, "delayed_bar_5 = %u",
			dp_stats_buf->delayed_bar_5);
	DP_TRACE_STATS(FATAL, "delayed_bar_6 = %u",
			dp_stats_buf->delayed_bar_6);
	DP_TRACE_STATS(FATAL, "delayed_bar_7 = %u\n",
			dp_stats_buf->delayed_bar_7);
}

/*
 * dp_print_tx_selfgen_ac_stats_tlv: display htt_tx_selfgen_ac_stats_tlv
 * @tag_buf: buffer containing the tlv htt_tx_selfgen_ac_stats_tlv
 *
 * return:void
 */
static inline void dp_print_tx_selfgen_ac_stats_tlv(uint32_t *tag_buf)
{
	htt_tx_selfgen_ac_stats_tlv *dp_stats_buf =
		(htt_tx_selfgen_ac_stats_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_TX_SELFGEN_AC_STATS_TLV:");
	DP_TRACE_STATS(FATAL, "ac_su_ndpa = %u",
			dp_stats_buf->ac_su_ndpa);
	DP_TRACE_STATS(FATAL, "ac_su_ndp = %u",
			dp_stats_buf->ac_su_ndp);
	DP_TRACE_STATS(FATAL, "ac_mu_mimo_ndpa = %u",
			dp_stats_buf->ac_mu_mimo_ndpa);
	DP_TRACE_STATS(FATAL, "ac_mu_mimo_ndp = %u",
			dp_stats_buf->ac_mu_mimo_ndp);
	DP_TRACE_STATS(FATAL, "ac_mu_mimo_brpoll_1 = %u",
			dp_stats_buf->ac_mu_mimo_brpoll_1);
	DP_TRACE_STATS(FATAL, "ac_mu_mimo_brpoll_2 = %u",
			dp_stats_buf->ac_mu_mimo_brpoll_2);
	DP_TRACE_STATS(FATAL, "ac_mu_mimo_brpoll_3 = %u\n",
			dp_stats_buf->ac_mu_mimo_brpoll_3);
}

/*
 * dp_print_tx_selfgen_ax_stats_tlv: display htt_tx_selfgen_ax_stats_tlv
 * @tag_buf: buffer containing the tlv htt_tx_selfgen_ax_stats_tlv
 *
 * return:void
 */
static inline void dp_print_tx_selfgen_ax_stats_tlv(uint32_t *tag_buf)
{
	htt_tx_selfgen_ax_stats_tlv *dp_stats_buf =
		(htt_tx_selfgen_ax_stats_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_TX_SELFGEN_AX_STATS_TLV:");
	DP_TRACE_STATS(FATAL, "ax_su_ndpa = %u",
			dp_stats_buf->ax_su_ndpa);
	DP_TRACE_STATS(FATAL, "ax_su_ndp = %u",
			dp_stats_buf->ax_su_ndp);
	DP_TRACE_STATS(FATAL, "ax_mu_mimo_ndpa = %u",
			dp_stats_buf->ax_mu_mimo_ndpa);
	DP_TRACE_STATS(FATAL, "ax_mu_mimo_ndp = %u",
			dp_stats_buf->ax_mu_mimo_ndp);
	DP_TRACE_STATS(FATAL, "ax_mu_mimo_brpoll_1 = %u",
			dp_stats_buf->ax_mu_mimo_brpoll_1);
	DP_TRACE_STATS(FATAL, "ax_mu_mimo_brpoll_2 = %u",
			dp_stats_buf->ax_mu_mimo_brpoll_2);
	DP_TRACE_STATS(FATAL, "ax_mu_mimo_brpoll_3 = %u",
			dp_stats_buf->ax_mu_mimo_brpoll_3);
	DP_TRACE_STATS(FATAL, "ax_mu_mimo_brpoll_4 = %u",
			dp_stats_buf->ax_mu_mimo_brpoll_4);
	DP_TRACE_STATS(FATAL, "ax_mu_mimo_brpoll_5 = %u",
			dp_stats_buf->ax_mu_mimo_brpoll_5);
	DP_TRACE_STATS(FATAL, "ax_mu_mimo_brpoll_6 = %u",
			dp_stats_buf->ax_mu_mimo_brpoll_6);
	DP_TRACE_STATS(FATAL, "ax_mu_mimo_brpoll_7 = %u",
			dp_stats_buf->ax_mu_mimo_brpoll_7);
	DP_TRACE_STATS(FATAL, "ax_basic_trigger = %u",
			dp_stats_buf->ax_basic_trigger);
	DP_TRACE_STATS(FATAL, "ax_bsr_trigger = %u",
			dp_stats_buf->ax_bsr_trigger);
	DP_TRACE_STATS(FATAL, "ax_mu_bar_trigger = %u",
			dp_stats_buf->ax_mu_bar_trigger);
	DP_TRACE_STATS(FATAL, "ax_mu_rts_trigger = %u\n",
			dp_stats_buf->ax_mu_rts_trigger);
}

/*
 * dp_print_tx_selfgen_ac_err_stats_tlv: display htt_tx_selfgen_ac_err_stats_tlv
 * @tag_buf: buffer containing the tlv htt_tx_selfgen_ac_err_stats_tlv
 *
 * return:void
 */
static inline void dp_print_tx_selfgen_ac_err_stats_tlv(uint32_t *tag_buf)
{
	htt_tx_selfgen_ac_err_stats_tlv *dp_stats_buf =
		(htt_tx_selfgen_ac_err_stats_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_TX_SELFGEN_AC_ERR_STATS_TLV:");
	DP_TRACE_STATS(FATAL, "ac_su_ndp_err = %u",
			dp_stats_buf->ac_su_ndp_err);
	DP_TRACE_STATS(FATAL, "ac_su_ndpa_err = %u",
			dp_stats_buf->ac_su_ndpa_err);
	DP_TRACE_STATS(FATAL, "ac_mu_mimo_ndpa_err = %u",
			dp_stats_buf->ac_mu_mimo_ndpa_err);
	DP_TRACE_STATS(FATAL, "ac_mu_mimo_ndp_err = %u",
			dp_stats_buf->ac_mu_mimo_ndp_err);
	DP_TRACE_STATS(FATAL, "ac_mu_mimo_brp1_err = %u",
			dp_stats_buf->ac_mu_mimo_brp1_err);
	DP_TRACE_STATS(FATAL, "ac_mu_mimo_brp2_err = %u",
			dp_stats_buf->ac_mu_mimo_brp2_err);
	DP_TRACE_STATS(FATAL, "ac_mu_mimo_brp3_err = %u\n",
			dp_stats_buf->ac_mu_mimo_brp3_err);
}

/*
 * dp_print_tx_selfgen_ax_err_stats_tlv: display htt_tx_selfgen_ax_err_stats_tlv
 * @tag_buf: buffer containing the tlv htt_tx_selfgen_ax_err_stats_tlv
 *
 * return:void
 */
static inline void dp_print_tx_selfgen_ax_err_stats_tlv(uint32_t *tag_buf)
{
	htt_tx_selfgen_ax_err_stats_tlv *dp_stats_buf =
		(htt_tx_selfgen_ax_err_stats_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_TX_SELFGEN_AX_ERR_STATS_TLV:");
	DP_TRACE_STATS(FATAL, "ax_su_ndp_err = %u",
			dp_stats_buf->ax_su_ndp_err);
	DP_TRACE_STATS(FATAL, "ax_su_ndpa_err = %u",
			dp_stats_buf->ax_su_ndpa_err);
	DP_TRACE_STATS(FATAL, "ax_mu_mimo_ndpa_err = %u",
			dp_stats_buf->ax_mu_mimo_ndpa_err);
	DP_TRACE_STATS(FATAL, "ax_mu_mimo_ndp_err = %u",
			dp_stats_buf->ax_mu_mimo_ndp_err);
	DP_TRACE_STATS(FATAL, "ax_mu_mimo_brp1_err = %u",
			dp_stats_buf->ax_mu_mimo_brp1_err);
	DP_TRACE_STATS(FATAL, "ax_mu_mimo_brp2_err = %u",
			dp_stats_buf->ax_mu_mimo_brp2_err);
	DP_TRACE_STATS(FATAL, "ax_mu_mimo_brp3_err = %u",
			dp_stats_buf->ax_mu_mimo_brp3_err);
	DP_TRACE_STATS(FATAL, "ax_mu_mimo_brp4_err = %u",
			dp_stats_buf->ax_mu_mimo_brp4_err);
	DP_TRACE_STATS(FATAL, "ax_mu_mimo_brp5_err = %u",
			dp_stats_buf->ax_mu_mimo_brp5_err);
	DP_TRACE_STATS(FATAL, "ax_mu_mimo_brp6_err = %u",
			dp_stats_buf->ax_mu_mimo_brp6_err);
	DP_TRACE_STATS(FATAL, "ax_mu_mimo_brp7_err = %u",
			dp_stats_buf->ax_mu_mimo_brp7_err);
	DP_TRACE_STATS(FATAL, "ax_basic_trigger_err = %u",
			dp_stats_buf->ax_basic_trigger_err);
	DP_TRACE_STATS(FATAL, "ax_bsr_trigger_err = %u",
			dp_stats_buf->ax_bsr_trigger_err);
	DP_TRACE_STATS(FATAL, "ax_mu_bar_trigger_err = %u",
			dp_stats_buf->ax_mu_bar_trigger_err);
	DP_TRACE_STATS(FATAL, "ax_mu_rts_trigger_err = %u\n",
			dp_stats_buf->ax_mu_rts_trigger_err);
}

/*
 * dp_print_tx_pdev_mu_mimo_sch_stats_tlv: display htt_tx_pdev_mu_mimo_sch_stats
 * @tag_buf: buffer containing the tlv htt_tx_pdev_mu_mimo_sch_stats_tlv
 *
 * return:void
 */
static inline void dp_print_tx_pdev_mu_mimo_sch_stats_tlv(uint32_t *tag_buf)
{
	uint8_t i;
	htt_tx_pdev_mu_mimo_sch_stats_tlv *dp_stats_buf =
		(htt_tx_pdev_mu_mimo_sch_stats_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_TX_PDEV_MU_MIMO_SCH_STATS_TLV:");
	DP_TRACE_STATS(FATAL, "mu_mimo_sch_posted = %u",
			dp_stats_buf->mu_mimo_sch_posted);
	DP_TRACE_STATS(FATAL, "mu_mimo_sch_failed = %u",
			dp_stats_buf->mu_mimo_sch_failed);
	DP_TRACE_STATS(FATAL, "mu_mimo_ppdu_posted = %u\n",
			dp_stats_buf->mu_mimo_ppdu_posted);

	DP_TRACE_STATS(FATAL, "11ac MU_MIMO SCH STATS:");

	for (i = 0; i < HTT_TX_PDEV_STATS_NUM_AC_MUMIMO_USER_STATS; i++) {
		DP_TRACE_STATS(FATAL, "ac_mu_mimo_sch_nusers_%u = %u", i,
			       dp_stats_buf->ac_mu_mimo_sch_nusers[i]);
	}

	DP_TRACE_STATS(FATAL, "\n11ax MU_MIMO SCH STATS:");

	for (i = 0; i < HTT_TX_PDEV_STATS_NUM_AX_MUMIMO_USER_STATS; i++) {
		DP_TRACE_STATS(FATAL, "ax_mu_mimo_sch_nusers_%u = %u", i,
			       dp_stats_buf->ax_mu_mimo_sch_nusers[i]);
	}

	DP_TRACE_STATS(FATAL, "\n11ax OFDMA SCH STATS:");

	for (i = 0; i < HTT_TX_PDEV_STATS_NUM_AX_MUMIMO_USER_STATS; i++) {
		DP_TRACE_STATS(FATAL, "ax_ofdma_sch_nusers_%u = %u", i,
			       dp_stats_buf->ax_ofdma_sch_nusers[i]);
	}
}

/*
 * dp_print_tx_pdev_mu_mimo_mpdu_stats_tlv: display
 *				htt_tx_pdev_mu_mimo_mpdu_stats_tlv
 * @tag_buf: buffer containing the tlv htt_tx_pdev_mu_mimo_mpdu_stats_tlv
 *
 * return:void
 */
static inline void dp_print_tx_pdev_mu_mimo_mpdu_stats_tlv(uint32_t *tag_buf)
{
	htt_tx_pdev_mpdu_stats_tlv *dp_stats_buf =
		(htt_tx_pdev_mpdu_stats_tlv *)tag_buf;

	if (dp_stats_buf->tx_sched_mode ==
			HTT_STATS_TX_SCHED_MODE_MU_MIMO_AC) {
		if (!dp_stats_buf->user_index)
			DP_TRACE_STATS(FATAL,
				       "HTT_TX_PDEV_MU_MIMO_AC_MPDU_STATS:\n");

		if (dp_stats_buf->user_index <
			HTT_TX_PDEV_STATS_NUM_AC_MUMIMO_USER_STATS) {
			DP_TRACE_STATS(FATAL,
				       "ac_mu_mimo_mpdus_queued_usr_%u = %u",
				       dp_stats_buf->user_index,
				       dp_stats_buf->mpdus_queued_usr);
			DP_TRACE_STATS(FATAL,
				       "ac_mu_mimo_mpdus_tried_usr_%u = %u",
				       dp_stats_buf->user_index,
				       dp_stats_buf->mpdus_tried_usr);
			DP_TRACE_STATS(FATAL,
				       "ac_mu_mimo_mpdus_failed_usr_%u = %u",
				       dp_stats_buf->user_index,
				       dp_stats_buf->mpdus_failed_usr);
			DP_TRACE_STATS(FATAL,
				       "ac_mu_mimo_mpdus_requeued_usr_%u = %u",
				       dp_stats_buf->user_index,
				       dp_stats_buf->mpdus_requeued_usr);
			DP_TRACE_STATS(FATAL,
				       "ac_mu_mimo_err_no_ba_usr_%u = %u",
				       dp_stats_buf->user_index,
				       dp_stats_buf->err_no_ba_usr);
			DP_TRACE_STATS(FATAL,
				       "ac_mu_mimo_mpdu_underrun_usr_%u = %u",
				       dp_stats_buf->user_index,
				       dp_stats_buf->mpdu_underrun_usr);
			DP_TRACE_STATS(FATAL,
				       "ac_mu_mimo_ampdu_underrun_usr_%u = %u\n",
				       dp_stats_buf->user_index,
				       dp_stats_buf->ampdu_underrun_usr);
		}
	}

	if (dp_stats_buf->tx_sched_mode == HTT_STATS_TX_SCHED_MODE_MU_MIMO_AX) {
		if (!dp_stats_buf->user_index)
			DP_TRACE_STATS(FATAL,
				       "HTT_TX_PDEV_MU_MIMO_AX_MPDU_STATS:\n");

		if (dp_stats_buf->user_index <
				HTT_TX_PDEV_STATS_NUM_AX_MUMIMO_USER_STATS) {
			DP_TRACE_STATS(FATAL,
				       "ax_mu_mimo_mpdus_queued_usr_%u = %u",
				       dp_stats_buf->user_index,
				       dp_stats_buf->mpdus_queued_usr);
			DP_TRACE_STATS(FATAL,
				       "ax_mu_mimo_mpdus_tried_usr_%u = %u",
				       dp_stats_buf->user_index,
				       dp_stats_buf->mpdus_tried_usr);
			DP_TRACE_STATS(FATAL,
				       "ax_mu_mimo_mpdus_failed_usr_%u = %u",
				       dp_stats_buf->user_index,
				       dp_stats_buf->mpdus_failed_usr);
			DP_TRACE_STATS(FATAL,
				       "ax_mu_mimo_mpdus_requeued_usr_%u = %u",
				       dp_stats_buf->user_index,
				       dp_stats_buf->mpdus_requeued_usr);
			DP_TRACE_STATS(FATAL,
				       "ax_mu_mimo_err_no_ba_usr_%u = %u",
				       dp_stats_buf->user_index,
				       dp_stats_buf->err_no_ba_usr);
			DP_TRACE_STATS(FATAL,
				       "ax_mu_mimo_mpdu_underrun_usr_%u = %u",
				       dp_stats_buf->user_index,
				       dp_stats_buf->mpdu_underrun_usr);
			DP_TRACE_STATS(FATAL,
				       "ax_mu_mimo_ampdu_underrun_usr_%u = %u\n",
				       dp_stats_buf->user_index,
				       dp_stats_buf->ampdu_underrun_usr);
		}
	}

	if (dp_stats_buf->tx_sched_mode ==
			HTT_STATS_TX_SCHED_MODE_MU_OFDMA_AX) {
		if (!dp_stats_buf->user_index)
			DP_TRACE_STATS(FATAL,
				       "HTT_TX_PDEV_AX_MU_OFDMA_MPDU_STATS:\n");

		if (dp_stats_buf->user_index <
				HTT_TX_PDEV_STATS_NUM_OFDMA_USER_STATS) {
			DP_TRACE_STATS(FATAL,
				       "ax_mu_ofdma_mpdus_queued_usr_%u = %u",
				       dp_stats_buf->user_index,
				       dp_stats_buf->mpdus_queued_usr);
			DP_TRACE_STATS(FATAL,
				       "ax_mu_ofdma_mpdus_tried_usr_%u = %u",
				       dp_stats_buf->user_index,
				       dp_stats_buf->mpdus_tried_usr);
			DP_TRACE_STATS(FATAL,
				       "ax_mu_ofdma_mpdus_failed_usr_%u = %u",
				       dp_stats_buf->user_index,
				       dp_stats_buf->mpdus_failed_usr);
			DP_TRACE_STATS(FATAL,
				       "ax_mu_ofdma_mpdus_requeued_usr_%u = %u",
				       dp_stats_buf->user_index,
				       dp_stats_buf->mpdus_requeued_usr);
			DP_TRACE_STATS(FATAL,
				       "ax_mu_ofdma_err_no_ba_usr_%u = %u",
				       dp_stats_buf->user_index,
				       dp_stats_buf->err_no_ba_usr);
			DP_TRACE_STATS(FATAL,
				       "ax_mu_ofdma_mpdu_underrun_usr_%u = %u",
				       dp_stats_buf->user_index,
				       dp_stats_buf->mpdu_underrun_usr);
			DP_TRACE_STATS(FATAL,
				       "ax_mu_ofdma_ampdu_underrun_usr_%u = %u\n",
				       dp_stats_buf->user_index,
				       dp_stats_buf->ampdu_underrun_usr);
		}
	}
}

/*
 * dp_print_sched_txq_cmd_posted_tlv_v: display htt_sched_txq_cmd_posted_tlv_v
 * @tag_buf: buffer containing the tlv htt_sched_txq_cmd_posted_tlv_v
 *
 * return:void
 */
static inline void dp_print_sched_txq_cmd_posted_tlv_v(uint32_t *tag_buf)
{
	htt_sched_txq_cmd_posted_tlv_v *dp_stats_buf =
		(htt_sched_txq_cmd_posted_tlv_v *)tag_buf;
	uint8_t i;
	uint16_t index = 0;
	uint32_t tag_len = (HTT_STATS_TLV_LENGTH_GET(*tag_buf) >> 2);
	char *sched_cmd_posted = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!sched_cmd_posted) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	tag_len = qdf_min(tag_len, (uint32_t)HTT_TX_PDEV_SCHED_TX_MODE_MAX);

	DP_TRACE_STATS(FATAL, "HTT_SCHED_TXQ_CMD_POSTED_TLV_V:");
	for (i = 0; i <  tag_len; i++) {
		index += qdf_snprint(&sched_cmd_posted[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i,
				dp_stats_buf->sched_cmd_posted[i]);
	}
	DP_TRACE_STATS(FATAL, "sched_cmd_posted = %s\n", sched_cmd_posted);
	qdf_mem_free(sched_cmd_posted);
}

/*
 * dp_print_sched_txq_cmd_reaped_tlv_v: display htt_sched_txq_cmd_reaped_tlv_v
 * @tag_buf: buffer containing the tlv htt_sched_txq_cmd_reaped_tlv_v
 *
 * return:void
 */
static inline void dp_print_sched_txq_cmd_reaped_tlv_v(uint32_t *tag_buf)
{
	htt_sched_txq_cmd_reaped_tlv_v *dp_stats_buf =
		(htt_sched_txq_cmd_reaped_tlv_v *)tag_buf;
	uint8_t i;
	uint16_t index = 0;
	uint32_t tag_len = (HTT_STATS_TLV_LENGTH_GET(*tag_buf) >> 2);
	char *sched_cmd_reaped = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!sched_cmd_reaped) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	tag_len = qdf_min(tag_len, (uint32_t)HTT_TX_PDEV_SCHED_TX_MODE_MAX);

	DP_TRACE_STATS(FATAL, "HTT_SCHED_TXQ_CMD_REAPED_TLV_V:");
	for (i = 0; i <  tag_len; i++) {
		index += qdf_snprint(&sched_cmd_reaped[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i,
				dp_stats_buf->sched_cmd_reaped[i]);
	}
	DP_TRACE_STATS(FATAL, "sched_cmd_reaped = %s\n", sched_cmd_reaped);
	qdf_mem_free(sched_cmd_reaped);
}

/*
 * dp_print_tx_pdev_stats_sched_per_txq_tlv: display
 *				htt_tx_pdev_stats_sched_per_txq_tlv
 * @tag_buf: buffer containing the tlv htt_tx_pdev_stats_sched_per_txq_tlv
 *
 * return:void
 */
static inline void dp_print_tx_pdev_stats_sched_per_txq_tlv(uint32_t *tag_buf)
{
	htt_tx_pdev_stats_sched_per_txq_tlv *dp_stats_buf =
		(htt_tx_pdev_stats_sched_per_txq_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_TX_PDEV_STATS_SCHED_PER_TXQ_TLV:");
	DP_TRACE_STATS(FATAL, "mac_id__txq_id__word = %u",
			dp_stats_buf->mac_id__txq_id__word);
	DP_TRACE_STATS(FATAL, "sched_policy = %u",
			dp_stats_buf->sched_policy);
	DP_TRACE_STATS(FATAL, "last_sched_cmd_posted_timestamp = %u",
			dp_stats_buf->last_sched_cmd_posted_timestamp);
	DP_TRACE_STATS(FATAL, "last_sched_cmd_compl_timestamp = %u",
			dp_stats_buf->last_sched_cmd_compl_timestamp);
	DP_TRACE_STATS(FATAL, "sched_2_tac_lwm_count = %u",
			dp_stats_buf->sched_2_tac_lwm_count);
	DP_TRACE_STATS(FATAL, "sched_2_tac_ring_full = %u",
			dp_stats_buf->sched_2_tac_ring_full);
	DP_TRACE_STATS(FATAL, "sched_cmd_post_failure = %u",
			dp_stats_buf->sched_cmd_post_failure);
	DP_TRACE_STATS(FATAL, "num_active_tids = %u",
			dp_stats_buf->num_active_tids);
	DP_TRACE_STATS(FATAL, "num_ps_schedules = %u",
			dp_stats_buf->num_ps_schedules);
	DP_TRACE_STATS(FATAL, "sched_cmds_pending = %u",
			dp_stats_buf->sched_cmds_pending);
	DP_TRACE_STATS(FATAL, "num_tid_register = %u",
			dp_stats_buf->num_tid_register);
	DP_TRACE_STATS(FATAL, "num_tid_unregister = %u",
			dp_stats_buf->num_tid_unregister);
	DP_TRACE_STATS(FATAL, "num_qstats_queried = %u",
			dp_stats_buf->num_qstats_queried);
	DP_TRACE_STATS(FATAL, "qstats_update_pending = %u",
			dp_stats_buf->qstats_update_pending);
	DP_TRACE_STATS(FATAL, "last_qstats_query_timestamp = %u",
			dp_stats_buf->last_qstats_query_timestamp);
	DP_TRACE_STATS(FATAL, "num_tqm_cmdq_full = %u",
			dp_stats_buf->num_tqm_cmdq_full);
	DP_TRACE_STATS(FATAL, "num_de_sched_algo_trigger = %u",
			dp_stats_buf->num_de_sched_algo_trigger);
	DP_TRACE_STATS(FATAL, "num_rt_sched_algo_trigger = %u",
			dp_stats_buf->num_rt_sched_algo_trigger);
	DP_TRACE_STATS(FATAL, "num_tqm_sched_algo_trigger = %u",
			dp_stats_buf->num_tqm_sched_algo_trigger);
	DP_TRACE_STATS(FATAL, "notify_sched = %u\n",
			dp_stats_buf->notify_sched);
}

/*
 * dp_print_stats_tx_sched_cmn_tlv: display htt_stats_tx_sched_cmn_tlv
 * @tag_buf: buffer containing the tlv htt_stats_tx_sched_cmn_tlv
 *
 * return:void
 */
static inline void dp_print_stats_tx_sched_cmn_tlv(uint32_t *tag_buf)
{
	htt_stats_tx_sched_cmn_tlv *dp_stats_buf =
		(htt_stats_tx_sched_cmn_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_STATS_TX_SCHED_CMN_TLV:");
	DP_TRACE_STATS(FATAL, "mac_id__word = %u",
			dp_stats_buf->mac_id__word);
	DP_TRACE_STATS(FATAL, "current_timestamp = %u\n",
			dp_stats_buf->current_timestamp);
}

/*
 * dp_print_tx_tqm_gen_mpdu_stats_tlv_v: display htt_tx_tqm_gen_mpdu_stats_tlv_v
 * @tag_buf: buffer containing the tlv htt_tx_tqm_gen_mpdu_stats_tlv_v
 *
 * return:void
 */
static inline void dp_print_tx_tqm_gen_mpdu_stats_tlv_v(uint32_t *tag_buf)
{
	htt_tx_tqm_gen_mpdu_stats_tlv_v *dp_stats_buf =
		(htt_tx_tqm_gen_mpdu_stats_tlv_v *)tag_buf;
	uint8_t i;
	uint16_t index = 0;
	uint32_t tag_len = (HTT_STATS_TLV_LENGTH_GET(*tag_buf) >> 2);
	char *gen_mpdu_end_reason = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!gen_mpdu_end_reason) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	tag_len = qdf_min(tag_len,
			(uint32_t)HTT_TX_TQM_MAX_GEN_MPDU_END_REASON);

	DP_TRACE_STATS(FATAL, "HTT_TX_TQM_GEN_MPDU_STATS_TLV_V:");
	for (i = 0; i <  tag_len; i++) {
		index += qdf_snprint(&gen_mpdu_end_reason[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i,
				dp_stats_buf->gen_mpdu_end_reason[i]);
	}
	DP_TRACE_STATS(FATAL, "gen_mpdu_end_reason = %s\n", gen_mpdu_end_reason);
	qdf_mem_free(gen_mpdu_end_reason);
}

/*
 * dp_print_tx_tqm_list_mpdu_stats_tlv_v: display htt_tx_tqm_list_mpdu_stats_tlv
 * @tag_buf: buffer containing the tlv htt_tx_tqm_list_mpdu_stats_tlv_v
 *
 * return:void
 */
static inline void dp_print_tx_tqm_list_mpdu_stats_tlv_v(uint32_t *tag_buf)
{
	htt_tx_tqm_list_mpdu_stats_tlv_v *dp_stats_buf =
		(htt_tx_tqm_list_mpdu_stats_tlv_v *)tag_buf;
	uint8_t i;
	uint16_t index = 0;
	uint32_t tag_len = (HTT_STATS_TLV_LENGTH_GET(*tag_buf) >> 2);
	char *list_mpdu_end_reason = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!list_mpdu_end_reason) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	tag_len = qdf_min(tag_len,
			(uint32_t)HTT_TX_TQM_MAX_LIST_MPDU_END_REASON);

	DP_TRACE_STATS(FATAL, "HTT_TX_TQM_LIST_MPDU_STATS_TLV_V:");
	for (i = 0; i <  tag_len; i++) {
		index += qdf_snprint(&list_mpdu_end_reason[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i,
				dp_stats_buf->list_mpdu_end_reason[i]);
	}
	DP_TRACE_STATS(FATAL, "list_mpdu_end_reason = %s\n",
			list_mpdu_end_reason);
	qdf_mem_free(list_mpdu_end_reason);
}

/*
 * dp_print_tx_tqm_list_mpdu_cnt_tlv_v: display htt_tx_tqm_list_mpdu_cnt_tlv_v
 * @tag_buf: buffer containing the tlv htt_tx_tqm_list_mpdu_cnt_tlv_v
 *
 * return:void
 */
static inline void dp_print_tx_tqm_list_mpdu_cnt_tlv_v(uint32_t *tag_buf)
{
	htt_tx_tqm_list_mpdu_cnt_tlv_v *dp_stats_buf =
		(htt_tx_tqm_list_mpdu_cnt_tlv_v *)tag_buf;
	uint8_t i;
	uint16_t index = 0;
	uint32_t tag_len = (HTT_STATS_TLV_LENGTH_GET(*tag_buf) >> 2);
	char *list_mpdu_cnt_hist = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!list_mpdu_cnt_hist) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	tag_len = qdf_min(tag_len,
			(uint32_t)HTT_TX_TQM_MAX_LIST_MPDU_CNT_HISTOGRAM_BINS);

	DP_TRACE_STATS(FATAL, "HTT_TX_TQM_LIST_MPDU_CNT_TLV_V:");
	for (i = 0; i <  tag_len; i++) {
		index += qdf_snprint(&list_mpdu_cnt_hist[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i,
				dp_stats_buf->list_mpdu_cnt_hist[i]);
	}
	DP_TRACE_STATS(FATAL, "list_mpdu_cnt_hist = %s\n", list_mpdu_cnt_hist);
	qdf_mem_free(list_mpdu_cnt_hist);
}

/*
 * dp_print_tx_tqm_pdev_stats_tlv_v: display htt_tx_tqm_pdev_stats_tlv_v
 * @tag_buf: buffer containing the tlv htt_tx_tqm_pdev_stats_tlv_v
 *
 * return:void
 */
static inline void dp_print_tx_tqm_pdev_stats_tlv_v(uint32_t *tag_buf)
{
	htt_tx_tqm_pdev_stats_tlv_v *dp_stats_buf =
		(htt_tx_tqm_pdev_stats_tlv_v *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_TX_TQM_PDEV_STATS_TLV_V:");
	DP_TRACE_STATS(FATAL, "msdu_count = %u",
			dp_stats_buf->msdu_count);
	DP_TRACE_STATS(FATAL, "mpdu_count = %u",
			dp_stats_buf->mpdu_count);
	DP_TRACE_STATS(FATAL, "remove_msdu = %u",
			dp_stats_buf->remove_msdu);
	DP_TRACE_STATS(FATAL, "remove_mpdu = %u",
			dp_stats_buf->remove_mpdu);
	DP_TRACE_STATS(FATAL, "remove_msdu_ttl = %u",
			dp_stats_buf->remove_msdu_ttl);
	DP_TRACE_STATS(FATAL, "send_bar = %u",
			dp_stats_buf->send_bar);
	DP_TRACE_STATS(FATAL, "bar_sync = %u",
			dp_stats_buf->bar_sync);
	DP_TRACE_STATS(FATAL, "notify_mpdu = %u",
			dp_stats_buf->notify_mpdu);
	DP_TRACE_STATS(FATAL, "sync_cmd = %u",
			dp_stats_buf->sync_cmd);
	DP_TRACE_STATS(FATAL, "write_cmd = %u",
			dp_stats_buf->write_cmd);
	DP_TRACE_STATS(FATAL, "hwsch_trigger = %u",
			dp_stats_buf->hwsch_trigger);
	DP_TRACE_STATS(FATAL, "ack_tlv_proc = %u",
			dp_stats_buf->ack_tlv_proc);
	DP_TRACE_STATS(FATAL, "gen_mpdu_cmd = %u",
			dp_stats_buf->gen_mpdu_cmd);
	DP_TRACE_STATS(FATAL, "gen_list_cmd = %u",
			dp_stats_buf->gen_list_cmd);
	DP_TRACE_STATS(FATAL, "remove_mpdu_cmd = %u",
			dp_stats_buf->remove_mpdu_cmd);
	DP_TRACE_STATS(FATAL, "remove_mpdu_tried_cmd = %u",
			dp_stats_buf->remove_mpdu_tried_cmd);
	DP_TRACE_STATS(FATAL, "mpdu_queue_stats_cmd = %u",
			dp_stats_buf->mpdu_queue_stats_cmd);
	DP_TRACE_STATS(FATAL, "mpdu_head_info_cmd = %u",
			dp_stats_buf->mpdu_head_info_cmd);
	DP_TRACE_STATS(FATAL, "msdu_flow_stats_cmd = %u",
			dp_stats_buf->msdu_flow_stats_cmd);
	DP_TRACE_STATS(FATAL, "remove_msdu_cmd = %u",
			dp_stats_buf->remove_msdu_cmd);
	DP_TRACE_STATS(FATAL, "remove_msdu_ttl_cmd = %u",
			dp_stats_buf->remove_msdu_ttl_cmd);
	DP_TRACE_STATS(FATAL, "flush_cache_cmd = %u",
			dp_stats_buf->flush_cache_cmd);
	DP_TRACE_STATS(FATAL, "update_mpduq_cmd = %u",
			dp_stats_buf->update_mpduq_cmd);
	DP_TRACE_STATS(FATAL, "enqueue = %u",
			dp_stats_buf->enqueue);
	DP_TRACE_STATS(FATAL, "enqueue_notify = %u",
			dp_stats_buf->enqueue_notify);
	DP_TRACE_STATS(FATAL, "notify_mpdu_at_head = %u",
			dp_stats_buf->notify_mpdu_at_head);
	DP_TRACE_STATS(FATAL, "notify_mpdu_state_valid = %u\n",
			dp_stats_buf->notify_mpdu_state_valid);
}

/*
 * dp_print_tx_tqm_cmn_stats_tlv: display htt_tx_tqm_cmn_stats_tlv
 * @tag_buf: buffer containing the tlv htt_tx_tqm_cmn_stats_tlv
 *
 * return:void
 */
static inline void dp_print_tx_tqm_cmn_stats_tlv(uint32_t *tag_buf)
{
	htt_tx_tqm_cmn_stats_tlv *dp_stats_buf =
		(htt_tx_tqm_cmn_stats_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_TX_TQM_CMN_STATS_TLV:");
	DP_TRACE_STATS(FATAL, "mac_id__word = %u",
			dp_stats_buf->mac_id__word);
	DP_TRACE_STATS(FATAL, "max_cmdq_id = %u",
			dp_stats_buf->max_cmdq_id);
	DP_TRACE_STATS(FATAL, "list_mpdu_cnt_hist_intvl = %u",
			dp_stats_buf->list_mpdu_cnt_hist_intvl);
	DP_TRACE_STATS(FATAL, "add_msdu = %u",
			dp_stats_buf->add_msdu);
	DP_TRACE_STATS(FATAL, "q_empty = %u",
			dp_stats_buf->q_empty);
	DP_TRACE_STATS(FATAL, "q_not_empty = %u",
			dp_stats_buf->q_not_empty);
	DP_TRACE_STATS(FATAL, "drop_notification = %u",
			dp_stats_buf->drop_notification);
	DP_TRACE_STATS(FATAL, "desc_threshold = %u\n",
			dp_stats_buf->desc_threshold);
}

/*
 * dp_print_tx_tqm_error_stats_tlv: display htt_tx_tqm_error_stats_tlv
 * @tag_buf: buffer containing the tlv htt_tx_tqm_error_stats_tlv
 *
 * return:void
 */
static inline void dp_print_tx_tqm_error_stats_tlv(uint32_t *tag_buf)
{
	htt_tx_tqm_error_stats_tlv *dp_stats_buf =
		(htt_tx_tqm_error_stats_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_TX_TQM_ERROR_STATS_TLV:");
	DP_TRACE_STATS(FATAL, "q_empty_failure = %u",
			dp_stats_buf->q_empty_failure);
	DP_TRACE_STATS(FATAL, "q_not_empty_failure = %u",
			dp_stats_buf->q_not_empty_failure);
	DP_TRACE_STATS(FATAL, "add_msdu_failure = %u\n",
			dp_stats_buf->add_msdu_failure);
}

/*
 * dp_print_tx_tqm_cmdq_status_tlv: display htt_tx_tqm_cmdq_status_tlv
 * @tag_buf: buffer containing the tlv htt_tx_tqm_cmdq_status_tlv
 *
 * return:void
 */
static inline void dp_print_tx_tqm_cmdq_status_tlv(uint32_t *tag_buf)
{
	htt_tx_tqm_cmdq_status_tlv *dp_stats_buf =
		(htt_tx_tqm_cmdq_status_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_TX_TQM_CMDQ_STATUS_TLV:");
	DP_TRACE_STATS(FATAL, "mac_id__cmdq_id__word = %u",
			dp_stats_buf->mac_id__cmdq_id__word);
	DP_TRACE_STATS(FATAL, "sync_cmd = %u",
			dp_stats_buf->sync_cmd);
	DP_TRACE_STATS(FATAL, "write_cmd = %u",
			dp_stats_buf->write_cmd);
	DP_TRACE_STATS(FATAL, "gen_mpdu_cmd = %u",
			dp_stats_buf->gen_mpdu_cmd);
	DP_TRACE_STATS(FATAL, "mpdu_queue_stats_cmd = %u",
			dp_stats_buf->mpdu_queue_stats_cmd);
	DP_TRACE_STATS(FATAL, "mpdu_head_info_cmd = %u",
			dp_stats_buf->mpdu_head_info_cmd);
	DP_TRACE_STATS(FATAL, "msdu_flow_stats_cmd = %u",
			dp_stats_buf->msdu_flow_stats_cmd);
	DP_TRACE_STATS(FATAL, "remove_mpdu_cmd = %u",
			dp_stats_buf->remove_mpdu_cmd);
	DP_TRACE_STATS(FATAL, "remove_msdu_cmd = %u",
			dp_stats_buf->remove_msdu_cmd);
	DP_TRACE_STATS(FATAL, "flush_cache_cmd = %u",
			dp_stats_buf->flush_cache_cmd);
	DP_TRACE_STATS(FATAL, "update_mpduq_cmd = %u",
			dp_stats_buf->update_mpduq_cmd);
	DP_TRACE_STATS(FATAL, "update_msduq_cmd = %u\n",
			dp_stats_buf->update_msduq_cmd);
}

/*
 * dp_print_tx_de_eapol_packets_stats_tlv: display htt_tx_de_eapol_packets_stats
 * @tag_buf: buffer containing the tlv htt_tx_de_eapol_packets_stats_tlv
 *
 * return:void
 */
static inline void dp_print_tx_de_eapol_packets_stats_tlv(uint32_t *tag_buf)
{
	htt_tx_de_eapol_packets_stats_tlv *dp_stats_buf =
		(htt_tx_de_eapol_packets_stats_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_TX_DE_EAPOL_PACKETS_STATS_TLV:");
	DP_TRACE_STATS(FATAL, "m1_packets = %u",
			dp_stats_buf->m1_packets);
	DP_TRACE_STATS(FATAL, "m2_packets = %u",
			dp_stats_buf->m2_packets);
	DP_TRACE_STATS(FATAL, "m3_packets = %u",
			dp_stats_buf->m3_packets);
	DP_TRACE_STATS(FATAL, "m4_packets = %u",
			dp_stats_buf->m4_packets);
	DP_TRACE_STATS(FATAL, "g1_packets = %u",
			dp_stats_buf->g1_packets);
	DP_TRACE_STATS(FATAL, "g2_packets = %u\n",
			dp_stats_buf->g2_packets);
}

/*
 * dp_print_tx_de_classify_failed_stats_tlv: display
 *				htt_tx_de_classify_failed_stats_tlv
 * @tag_buf: buffer containing the tlv htt_tx_de_classify_failed_stats_tlv
 *
 * return:void
 */
static inline void dp_print_tx_de_classify_failed_stats_tlv(uint32_t *tag_buf)
{
	htt_tx_de_classify_failed_stats_tlv *dp_stats_buf =
		(htt_tx_de_classify_failed_stats_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_TX_DE_CLASSIFY_FAILED_STATS_TLV:");
	DP_TRACE_STATS(FATAL, "ap_bss_peer_not_found = %u",
			dp_stats_buf->ap_bss_peer_not_found);
	DP_TRACE_STATS(FATAL, "ap_bcast_mcast_no_peer = %u",
			dp_stats_buf->ap_bcast_mcast_no_peer);
	DP_TRACE_STATS(FATAL, "sta_delete_in_progress = %u",
			dp_stats_buf->sta_delete_in_progress);
	DP_TRACE_STATS(FATAL, "ibss_no_bss_peer = %u",
			dp_stats_buf->ibss_no_bss_peer);
	DP_TRACE_STATS(FATAL, "invaild_vdev_type = %u",
			dp_stats_buf->invaild_vdev_type);
	DP_TRACE_STATS(FATAL, "invalid_ast_peer_entry = %u",
			dp_stats_buf->invalid_ast_peer_entry);
	DP_TRACE_STATS(FATAL, "peer_entry_invalid = %u",
			dp_stats_buf->peer_entry_invalid);
	DP_TRACE_STATS(FATAL, "ethertype_not_ip = %u",
			dp_stats_buf->ethertype_not_ip);
	DP_TRACE_STATS(FATAL, "eapol_lookup_failed = %u",
			dp_stats_buf->eapol_lookup_failed);
	DP_TRACE_STATS(FATAL, "qpeer_not_allow_data = %u",
			dp_stats_buf->qpeer_not_allow_data);
	DP_TRACE_STATS(FATAL, "fse_tid_override = %u\n",
			dp_stats_buf->fse_tid_override);
}

/*
 * dp_print_tx_de_classify_stats_tlv: display htt_tx_de_classify_stats_tlv
 * @tag_buf: buffer containing the tlv htt_tx_de_classify_stats_tlv
 *
 * return:void
 */
static inline void dp_print_tx_de_classify_stats_tlv(uint32_t *tag_buf)
{
	htt_tx_de_classify_stats_tlv *dp_stats_buf =
		(htt_tx_de_classify_stats_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_TX_DE_CLASSIFY_STATS_TLV:");
	DP_TRACE_STATS(FATAL, "arp_packets = %u",
			dp_stats_buf->arp_packets);
	DP_TRACE_STATS(FATAL, "igmp_packets = %u",
			dp_stats_buf->igmp_packets);
	DP_TRACE_STATS(FATAL, "dhcp_packets = %u",
			dp_stats_buf->dhcp_packets);
	DP_TRACE_STATS(FATAL, "host_inspected = %u",
			dp_stats_buf->host_inspected);
	DP_TRACE_STATS(FATAL, "htt_included = %u",
			dp_stats_buf->htt_included);
	DP_TRACE_STATS(FATAL, "htt_valid_mcs = %u",
			dp_stats_buf->htt_valid_mcs);
	DP_TRACE_STATS(FATAL, "htt_valid_nss = %u",
			dp_stats_buf->htt_valid_nss);
	DP_TRACE_STATS(FATAL, "htt_valid_preamble_type = %u",
			dp_stats_buf->htt_valid_preamble_type);
	DP_TRACE_STATS(FATAL, "htt_valid_chainmask = %u",
			dp_stats_buf->htt_valid_chainmask);
	DP_TRACE_STATS(FATAL, "htt_valid_guard_interval = %u",
			dp_stats_buf->htt_valid_guard_interval);
	DP_TRACE_STATS(FATAL, "htt_valid_retries = %u",
			dp_stats_buf->htt_valid_retries);
	DP_TRACE_STATS(FATAL, "htt_valid_bw_info = %u",
			dp_stats_buf->htt_valid_bw_info);
	DP_TRACE_STATS(FATAL, "htt_valid_power = %u",
			dp_stats_buf->htt_valid_power);
	DP_TRACE_STATS(FATAL, "htt_valid_key_flags = %u",
			dp_stats_buf->htt_valid_key_flags);
	DP_TRACE_STATS(FATAL, "htt_valid_no_encryption = %u",
			dp_stats_buf->htt_valid_no_encryption);
	DP_TRACE_STATS(FATAL, "fse_entry_count = %u",
			dp_stats_buf->fse_entry_count);
	DP_TRACE_STATS(FATAL, "fse_priority_be = %u",
			dp_stats_buf->fse_priority_be);
	DP_TRACE_STATS(FATAL, "fse_priority_high = %u",
			dp_stats_buf->fse_priority_high);
	DP_TRACE_STATS(FATAL, "fse_priority_low = %u",
			dp_stats_buf->fse_priority_low);
	DP_TRACE_STATS(FATAL, "fse_traffic_ptrn_be = %u",
			dp_stats_buf->fse_traffic_ptrn_be);
	DP_TRACE_STATS(FATAL, "fse_traffic_ptrn_over_sub = %u",
			dp_stats_buf->fse_traffic_ptrn_over_sub);
	DP_TRACE_STATS(FATAL, "fse_traffic_ptrn_bursty = %u",
			dp_stats_buf->fse_traffic_ptrn_bursty);
	DP_TRACE_STATS(FATAL, "fse_traffic_ptrn_interactive = %u",
			dp_stats_buf->fse_traffic_ptrn_interactive);
	DP_TRACE_STATS(FATAL, "fse_traffic_ptrn_periodic = %u",
			dp_stats_buf->fse_traffic_ptrn_periodic);
	DP_TRACE_STATS(FATAL, "fse_hwqueue_alloc = %u",
			dp_stats_buf->fse_hwqueue_alloc);
	DP_TRACE_STATS(FATAL, "fse_hwqueue_created = %u",
			dp_stats_buf->fse_hwqueue_created);
	DP_TRACE_STATS(FATAL, "fse_hwqueue_send_to_host = %u",
			dp_stats_buf->fse_hwqueue_send_to_host);
	DP_TRACE_STATS(FATAL, "mcast_entry = %u",
			dp_stats_buf->mcast_entry);
	DP_TRACE_STATS(FATAL, "bcast_entry = %u\n",
			dp_stats_buf->bcast_entry);
}

/*
 * dp_print_tx_de_classify_status_stats_tlv: display
 *				htt_tx_de_classify_status_stats_tlv
 * @tag_buf: buffer containing the tlv htt_tx_de_classify_status_stats_tlv
 *
 * return:void
 */
static inline void dp_print_tx_de_classify_status_stats_tlv(uint32_t *tag_buf)
{
	htt_tx_de_classify_status_stats_tlv *dp_stats_buf =
		(htt_tx_de_classify_status_stats_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_TX_DE_CLASSIFY_STATUS_STATS_TLV:");
	DP_TRACE_STATS(FATAL, "eok = %u",
			dp_stats_buf->eok);
	DP_TRACE_STATS(FATAL, "classify_done = %u",
			dp_stats_buf->classify_done);
	DP_TRACE_STATS(FATAL, "lookup_failed = %u",
			dp_stats_buf->lookup_failed);
	DP_TRACE_STATS(FATAL, "send_host_dhcp = %u",
			dp_stats_buf->send_host_dhcp);
	DP_TRACE_STATS(FATAL, "send_host_mcast = %u",
			dp_stats_buf->send_host_mcast);
	DP_TRACE_STATS(FATAL, "send_host_unknown_dest = %u",
			dp_stats_buf->send_host_unknown_dest);
	DP_TRACE_STATS(FATAL, "send_host = %u",
			dp_stats_buf->send_host);
	DP_TRACE_STATS(FATAL, "status_invalid = %u\n",
			dp_stats_buf->status_invalid);
}

/*
 * dp_print_tx_de_enqueue_packets_stats_tlv: display
 *				htt_tx_de_enqueue_packets_stats_tlv
 * @tag_buf: buffer containing the tlv htt_tx_de_enqueue_packets_stats_tlv
 *
 * return:void
 */
static inline void dp_print_tx_de_enqueue_packets_stats_tlv(uint32_t *tag_buf)
{
	htt_tx_de_enqueue_packets_stats_tlv *dp_stats_buf =
		(htt_tx_de_enqueue_packets_stats_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_TX_DE_ENQUEUE_PACKETS_STATS_TLV:");
	DP_TRACE_STATS(FATAL, "enqueued_pkts = %u",
			dp_stats_buf->enqueued_pkts);
	DP_TRACE_STATS(FATAL, "to_tqm = %u",
			dp_stats_buf->to_tqm);
	DP_TRACE_STATS(FATAL, "to_tqm_bypass = %u\n",
			dp_stats_buf->to_tqm_bypass);
}

/*
 * dp_print_tx_de_enqueue_discard_stats_tlv: display
 *					htt_tx_de_enqueue_discard_stats_tlv
 * @tag_buf: buffer containing the tlv htt_tx_de_enqueue_discard_stats_tlv
 *
 * return:void
 */
static inline void dp_print_tx_de_enqueue_discard_stats_tlv(uint32_t *tag_buf)
{
	htt_tx_de_enqueue_discard_stats_tlv *dp_stats_buf =
		(htt_tx_de_enqueue_discard_stats_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_TX_DE_ENQUEUE_DISCARD_STATS_TLV:");
	DP_TRACE_STATS(FATAL, "discarded_pkts = %u",
			dp_stats_buf->discarded_pkts);
	DP_TRACE_STATS(FATAL, "local_frames = %u",
			dp_stats_buf->local_frames);
	DP_TRACE_STATS(FATAL, "is_ext_msdu = %u\n",
			dp_stats_buf->is_ext_msdu);
}

/*
 * dp_print_tx_de_compl_stats_tlv: display htt_tx_de_compl_stats_tlv
 * @tag_buf: buffer containing the tlv htt_tx_de_compl_stats_tlv
 *
 * return:void
 */
static inline void dp_print_tx_de_compl_stats_tlv(uint32_t *tag_buf)
{
	htt_tx_de_compl_stats_tlv *dp_stats_buf =
		(htt_tx_de_compl_stats_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_TX_DE_COMPL_STATS_TLV:");
	DP_TRACE_STATS(FATAL, "tcl_dummy_frame = %u",
			dp_stats_buf->tcl_dummy_frame);
	DP_TRACE_STATS(FATAL, "tqm_dummy_frame = %u",
			dp_stats_buf->tqm_dummy_frame);
	DP_TRACE_STATS(FATAL, "tqm_notify_frame = %u",
			dp_stats_buf->tqm_notify_frame);
	DP_TRACE_STATS(FATAL, "fw2wbm_enq = %u",
			dp_stats_buf->fw2wbm_enq);
	DP_TRACE_STATS(FATAL, "tqm_bypass_frame = %u\n",
			dp_stats_buf->tqm_bypass_frame);
}

/*
 * dp_print_tx_de_cmn_stats_tlv: display htt_tx_de_cmn_stats_tlv
 * @tag_buf: buffer containing the tlv htt_tx_de_cmn_stats_tlv
 *
 * return:void
 */
static inline void dp_print_tx_de_cmn_stats_tlv(uint32_t *tag_buf)
{
	htt_tx_de_cmn_stats_tlv *dp_stats_buf =
		(htt_tx_de_cmn_stats_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_TX_DE_CMN_STATS_TLV:");
	DP_TRACE_STATS(FATAL, "mac_id__word = %u",
			dp_stats_buf->mac_id__word);
	DP_TRACE_STATS(FATAL, "tcl2fw_entry_count = %u",
			dp_stats_buf->tcl2fw_entry_count);
	DP_TRACE_STATS(FATAL, "not_to_fw = %u",
			dp_stats_buf->not_to_fw);
	DP_TRACE_STATS(FATAL, "invalid_pdev_vdev_peer = %u",
			dp_stats_buf->invalid_pdev_vdev_peer);
	DP_TRACE_STATS(FATAL, "tcl_res_invalid_addrx = %u",
			dp_stats_buf->tcl_res_invalid_addrx);
	DP_TRACE_STATS(FATAL, "wbm2fw_entry_count = %u",
			dp_stats_buf->wbm2fw_entry_count);
	DP_TRACE_STATS(FATAL, "invalid_pdev = %u\n",
			dp_stats_buf->invalid_pdev);
}

/*
 * dp_print_ring_if_stats_tlv: display htt_ring_if_stats_tlv
 * @tag_buf: buffer containing the tlv htt_ring_if_stats_tlv
 *
 * return:void
 */
static inline void dp_print_ring_if_stats_tlv(uint32_t *tag_buf)
{
	htt_ring_if_stats_tlv *dp_stats_buf =
		(htt_ring_if_stats_tlv *)tag_buf;
	uint8_t i;
	uint16_t index = 0;
	char *wm_hit_count = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!wm_hit_count) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	DP_TRACE_STATS(FATAL, "HTT_RING_IF_STATS_TLV:");
	DP_TRACE_STATS(FATAL, "base_addr = %u",
			dp_stats_buf->base_addr);
	DP_TRACE_STATS(FATAL, "elem_size = %u",
			dp_stats_buf->elem_size);
	DP_TRACE_STATS(FATAL, "num_elems__prefetch_tail_idx = %u",
			dp_stats_buf->num_elems__prefetch_tail_idx);
	DP_TRACE_STATS(FATAL, "head_idx__tail_idx = %u",
			dp_stats_buf->head_idx__tail_idx);
	DP_TRACE_STATS(FATAL, "shadow_head_idx__shadow_tail_idx = %u",
			dp_stats_buf->shadow_head_idx__shadow_tail_idx);
	DP_TRACE_STATS(FATAL, "num_tail_incr = %u",
			dp_stats_buf->num_tail_incr);
	DP_TRACE_STATS(FATAL, "lwm_thresh__hwm_thresh = %u",
			dp_stats_buf->lwm_thresh__hwm_thresh);
	DP_TRACE_STATS(FATAL, "overrun_hit_count = %u",
			dp_stats_buf->overrun_hit_count);
	DP_TRACE_STATS(FATAL, "underrun_hit_count = %u",
			dp_stats_buf->underrun_hit_count);
	DP_TRACE_STATS(FATAL, "prod_blockwait_count = %u",
			dp_stats_buf->prod_blockwait_count);
	DP_TRACE_STATS(FATAL, "cons_blockwait_count = %u",
			dp_stats_buf->cons_blockwait_count);

	for (i = 0; i <  DP_HTT_LOW_WM_HIT_COUNT_LEN; i++) {
		index += qdf_snprint(&wm_hit_count[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i,
				dp_stats_buf->low_wm_hit_count[i]);
	}
	DP_TRACE_STATS(FATAL, "low_wm_hit_count = %s ", wm_hit_count);

	qdf_mem_zero(wm_hit_count, DP_MAX_STRING_LEN);

	index = 0;
	for (i = 0; i <  DP_HTT_HIGH_WM_HIT_COUNT_LEN; i++) {
		index += qdf_snprint(&wm_hit_count[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i,
				dp_stats_buf->high_wm_hit_count[i]);
	}
	DP_TRACE_STATS(FATAL, "high_wm_hit_count = %s\n", wm_hit_count);
}

/*
 * dp_print_ring_if_cmn_tlv: display htt_ring_if_cmn_tlv
 * @tag_buf: buffer containing the tlv htt_ring_if_cmn_tlv
 *
 * return:void
 */
static inline void dp_print_ring_if_cmn_tlv(uint32_t *tag_buf)
{
	htt_ring_if_cmn_tlv *dp_stats_buf =
		(htt_ring_if_cmn_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_RING_IF_CMN_TLV:");
	DP_TRACE_STATS(FATAL, "mac_id__word = %u",
			dp_stats_buf->mac_id__word);
	DP_TRACE_STATS(FATAL, "num_records = %u\n",
			dp_stats_buf->num_records);
}

/*
 * dp_print_sfm_client_user_tlv_v: display htt_sfm_client_user_tlv_v
 * @tag_buf: buffer containing the tlv htt_sfm_client_user_tlv_v
 *
 * return:void
 */
static inline void dp_print_sfm_client_user_tlv_v(uint32_t *tag_buf)
{
	htt_sfm_client_user_tlv_v *dp_stats_buf =
		(htt_sfm_client_user_tlv_v *)tag_buf;
	uint8_t i;
	uint16_t index = 0;
	uint32_t tag_len = (HTT_STATS_TLV_LENGTH_GET(*tag_buf) >> 2);
	char *dwords_used_by_user_n = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!dwords_used_by_user_n) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	DP_TRACE_STATS(FATAL, "HTT_SFM_CLIENT_USER_TLV_V:");
	for (i = 0; i <  tag_len; i++) {
		index += qdf_snprint(&dwords_used_by_user_n[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i,
				dp_stats_buf->dwords_used_by_user_n[i]);
	}
	DP_TRACE_STATS(FATAL, "dwords_used_by_user_n = %s\n",
			dwords_used_by_user_n);
	qdf_mem_free(dwords_used_by_user_n);
}

/*
 * dp_print_sfm_client_tlv: display htt_sfm_client_tlv
 * @tag_buf: buffer containing the tlv htt_sfm_client_tlv
 *
 * return:void
 */
static inline void dp_print_sfm_client_tlv(uint32_t *tag_buf)
{
	htt_sfm_client_tlv *dp_stats_buf =
		(htt_sfm_client_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_SFM_CLIENT_TLV:");
	DP_TRACE_STATS(FATAL, "client_id = %u",
			dp_stats_buf->client_id);
	DP_TRACE_STATS(FATAL, "buf_min = %u",
			dp_stats_buf->buf_min);
	DP_TRACE_STATS(FATAL, "buf_max = %u",
			dp_stats_buf->buf_max);
	DP_TRACE_STATS(FATAL, "buf_busy = %u",
			dp_stats_buf->buf_busy);
	DP_TRACE_STATS(FATAL, "buf_alloc = %u",
			dp_stats_buf->buf_alloc);
	DP_TRACE_STATS(FATAL, "buf_avail = %u",
			dp_stats_buf->buf_avail);
	DP_TRACE_STATS(FATAL, "num_users = %u\n",
			dp_stats_buf->num_users);
}

/*
 * dp_print_sfm_cmn_tlv: display htt_sfm_cmn_tlv
 * @tag_buf: buffer containing the tlv htt_sfm_cmn_tlv
 *
 * return:void
 */
static inline void dp_print_sfm_cmn_tlv(uint32_t *tag_buf)
{
	htt_sfm_cmn_tlv *dp_stats_buf =
		(htt_sfm_cmn_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_SFM_CMN_TLV:");
	DP_TRACE_STATS(FATAL, "mac_id__word = %u",
			dp_stats_buf->mac_id__word);
	DP_TRACE_STATS(FATAL, "buf_total = %u",
			dp_stats_buf->buf_total);
	DP_TRACE_STATS(FATAL, "mem_empty = %u",
			dp_stats_buf->mem_empty);
	DP_TRACE_STATS(FATAL, "deallocate_bufs = %u",
			dp_stats_buf->deallocate_bufs);
	DP_TRACE_STATS(FATAL, "num_records = %u\n",
			dp_stats_buf->num_records);
}

/*
 * dp_print_sring_stats_tlv: display htt_sring_stats_tlv
 * @tag_buf: buffer containing the tlv htt_sring_stats_tlv
 *
 * return:void
 */
static inline void dp_print_sring_stats_tlv(uint32_t *tag_buf)
{
	htt_sring_stats_tlv *dp_stats_buf =
		(htt_sring_stats_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_SRING_STATS_TLV:");
	DP_TRACE_STATS(FATAL, "mac_id__ring_id__arena__ep = %u",
			dp_stats_buf->mac_id__ring_id__arena__ep);
	DP_TRACE_STATS(FATAL, "base_addr_lsb = %u",
			dp_stats_buf->base_addr_lsb);
	DP_TRACE_STATS(FATAL, "base_addr_msb = %u",
			dp_stats_buf->base_addr_msb);
	DP_TRACE_STATS(FATAL, "ring_size = %u",
			dp_stats_buf->ring_size);
	DP_TRACE_STATS(FATAL, "elem_size = %u",
			dp_stats_buf->elem_size);
	DP_TRACE_STATS(FATAL, "num_avail_words__num_valid_words = %u",
			dp_stats_buf->num_avail_words__num_valid_words);
	DP_TRACE_STATS(FATAL, "head_ptr__tail_ptr = %u",
			dp_stats_buf->head_ptr__tail_ptr);
	DP_TRACE_STATS(FATAL, "consumer_empty__producer_full = %u",
			dp_stats_buf->consumer_empty__producer_full);
	DP_TRACE_STATS(FATAL, "prefetch_count__internal_tail_ptr = %u\n",
			dp_stats_buf->prefetch_count__internal_tail_ptr);
}

/*
 * dp_print_sring_cmn_tlv: display htt_sring_cmn_tlv
 * @tag_buf: buffer containing the tlv htt_sring_cmn_tlv
 *
 * return:void
 */
static inline void dp_print_sring_cmn_tlv(uint32_t *tag_buf)
{
	htt_sring_cmn_tlv *dp_stats_buf =
		(htt_sring_cmn_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_SRING_CMN_TLV:");
	DP_TRACE_STATS(FATAL, "num_records = %u\n",
			dp_stats_buf->num_records);
}

/*
 * dp_print_tx_pdev_rate_stats_tlv: display htt_tx_pdev_rate_stats_tlv
 * @tag_buf: buffer containing the tlv htt_tx_pdev_rate_stats_tlv
 *
 * return:void
 */
static inline void dp_print_tx_pdev_rate_stats_tlv(uint32_t *tag_buf)
{
	htt_tx_pdev_rate_stats_tlv *dp_stats_buf =
		(htt_tx_pdev_rate_stats_tlv *)tag_buf;
	uint8_t i, j;
	uint16_t index = 0;
	char *tx_gi[HTT_TX_PEER_STATS_NUM_GI_COUNTERS];
	char *str_buf = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!str_buf) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	for (i = 0; i < HTT_TX_PEER_STATS_NUM_GI_COUNTERS; i++) {
		tx_gi[i] = (char *)qdf_mem_malloc(DP_MAX_STRING_LEN);
	}

	DP_PRINT_STATS("HTT_TX_PDEV_RATE_STATS_TLV:");
	DP_PRINT_STATS("mac_id__word = %u",
		       dp_stats_buf->mac_id__word);
	DP_PRINT_STATS("tx_ldpc = %u",
		       dp_stats_buf->tx_ldpc);
	DP_PRINT_STATS("rts_cnt = %u",
		       dp_stats_buf->rts_cnt);
	DP_PRINT_STATS("rts_success = %u",
		       dp_stats_buf->rts_success);

	DP_PRINT_STATS("ack_rssi = %u",
		       dp_stats_buf->ack_rssi);

	qdf_mem_zero(str_buf, DP_MAX_STRING_LEN);
	for (i = 0; i <  DP_HTT_TX_MCS_LEN; i++) {
		index += qdf_snprint(&str_buf[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->tx_mcs[i]);
	}
	DP_PRINT_STATS("tx_mcs = %s ", str_buf);

	index = 0;
	qdf_mem_zero(str_buf, DP_MAX_STRING_LEN);
	for (i = 0; i <  DP_HTT_TX_SU_MCS_LEN; i++) {
		index += qdf_snprint(&str_buf[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->tx_su_mcs[i]);
	}
	DP_PRINT_STATS("tx_su_mcs = %s ", str_buf);

	index = 0;
	qdf_mem_zero(str_buf, DP_MAX_STRING_LEN);
	for (i = 0; i <  DP_HTT_TX_MU_MCS_LEN; i++) {
		index += qdf_snprint(&str_buf[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->tx_mu_mcs[i]);
	}
	DP_PRINT_STATS("tx_mu_mcs = %s ", str_buf);

	index = 0;
	qdf_mem_zero(str_buf, DP_MAX_STRING_LEN);
	for (i = 0; i <  DP_HTT_TX_NSS_LEN; i++) {
		/* 0 stands for NSS 1, 1 stands for NSS 2, etc. */
		index += qdf_snprint(&str_buf[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", (i + 1),
				dp_stats_buf->tx_nss[i]);
	}
	DP_PRINT_STATS("tx_nss = %s ", str_buf);

	index = 0;
	qdf_mem_zero(str_buf, DP_MAX_STRING_LEN);
	for (i = 0; i <  DP_HTT_TX_BW_LEN; i++) {
		index += qdf_snprint(&str_buf[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->tx_bw[i]);
	}
	DP_PRINT_STATS("tx_bw = %s ", str_buf);

	index = 0;
	qdf_mem_zero(str_buf, DP_MAX_STRING_LEN);
	for (i = 0; i <  HTT_TX_PDEV_STATS_NUM_MCS_COUNTERS; i++) {
		index += qdf_snprint(&str_buf[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->tx_stbc[i]);
	}
	DP_PRINT_STATS("tx_stbc = %s ", str_buf);

	index = 0;
	qdf_mem_zero(str_buf, DP_MAX_STRING_LEN);
	for (i = 0; i <  DP_HTT_TX_PREAM_LEN; i++) {
		index += qdf_snprint(&str_buf[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->tx_pream[i]);
	}
	DP_PRINT_STATS("tx_pream = %s ", str_buf);

	for (j = 0; j < DP_HTT_PDEV_TX_GI_LEN; j++) {
		index = 0;
		for (i = 0; i <  HTT_TX_PDEV_STATS_NUM_MCS_COUNTERS; i++) {
			index += qdf_snprint(&tx_gi[j][index],
					DP_MAX_STRING_LEN - index,
					" %u:%u,", i,
					dp_stats_buf->tx_gi[j][i]);
		}
		DP_PRINT_STATS("tx_gi[%u] = %s ", j, tx_gi[j]);
	}

	index = 0;
	qdf_mem_zero(str_buf, DP_MAX_STRING_LEN);
	for (i = 0; i <  DP_HTT_TX_DCM_LEN; i++) {
		index += qdf_snprint(&str_buf[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->tx_dcm[i]);
	}
	DP_PRINT_STATS("tx_dcm = %s\n", str_buf);

	for (i = 0; i < HTT_TX_PEER_STATS_NUM_GI_COUNTERS; i++)
		qdf_mem_free(tx_gi[i]);

	qdf_mem_free(str_buf);
}

/*
 * dp_print_rx_pdev_rate_stats_tlv: display htt_rx_pdev_rate_stats_tlv
 * @tag_buf: buffer containing the tlv htt_rx_pdev_rate_stats_tlv
 *
 * return:void
 */
static inline void dp_print_rx_pdev_rate_stats_tlv(uint32_t *tag_buf)
{
	htt_rx_pdev_rate_stats_tlv *dp_stats_buf =
		(htt_rx_pdev_rate_stats_tlv *)tag_buf;
	uint8_t i, j;
	uint16_t index = 0;
	char *rssi_chain[DP_HTT_RSSI_CHAIN_LEN];
	char *rx_gi[HTT_RX_PDEV_STATS_NUM_GI_COUNTERS];
	char *str_buf = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!str_buf) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	for (i = 0; i < DP_HTT_RSSI_CHAIN_LEN; i++)
		rssi_chain[i] = qdf_mem_malloc(DP_MAX_STRING_LEN);
	for (i = 0; i < HTT_RX_PDEV_STATS_NUM_GI_COUNTERS; i++)
		rx_gi[i] = qdf_mem_malloc(DP_MAX_STRING_LEN);

	DP_TRACE_STATS(FATAL, "HTT_RX_PDEV_RATE_STATS_TLV:");
	DP_TRACE_STATS(FATAL, "mac_id__word = %u",
			dp_stats_buf->mac_id__word);
	DP_TRACE_STATS(FATAL, "nsts = %u",
			dp_stats_buf->nsts);
	DP_TRACE_STATS(FATAL, "rx_ldpc = %u",
			dp_stats_buf->rx_ldpc);
	DP_TRACE_STATS(FATAL, "rts_cnt = %u",
			dp_stats_buf->rts_cnt);
	DP_TRACE_STATS(FATAL, "rssi_mgmt = %u",
			dp_stats_buf->rssi_mgmt);
	DP_TRACE_STATS(FATAL, "rssi_data = %u",
			dp_stats_buf->rssi_data);
	DP_TRACE_STATS(FATAL, "rssi_comb = %u",
			dp_stats_buf->rssi_comb);

	qdf_mem_zero(str_buf, DP_MAX_STRING_LEN);
	for (i = 0; i <  DP_HTT_RX_MCS_LEN; i++) {
		index += qdf_snprint(&str_buf[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->rx_mcs[i]);
	}
	DP_TRACE_STATS(FATAL, "rx_mcs = %s ", str_buf);

	index = 0;
	qdf_mem_zero(str_buf, DP_MAX_STRING_LEN);
	for (i = 0; i <  DP_HTT_RX_NSS_LEN; i++) {
		/* 0 stands for NSS 1, 1 stands for NSS 2, etc. */
		index += qdf_snprint(&str_buf[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", (i + 1),
				dp_stats_buf->rx_nss[i]);
	}
	DP_TRACE_STATS(FATAL, "rx_nss = %s ", str_buf);

	index = 0;
	qdf_mem_zero(str_buf, DP_MAX_STRING_LEN);
	for (i = 0; i <  DP_HTT_RX_DCM_LEN; i++) {
		index += qdf_snprint(&str_buf[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->rx_dcm[i]);
	}
	DP_TRACE_STATS(FATAL, "rx_dcm = %s ", str_buf);

	index = 0;
	qdf_mem_zero(str_buf, DP_MAX_STRING_LEN);
	for (i = 0; i <  HTT_RX_PDEV_STATS_NUM_MCS_COUNTERS; i++) {
		index += qdf_snprint(&str_buf[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->rx_stbc[i]);
	}
	DP_TRACE_STATS(FATAL, "rx_stbc = %s ", str_buf);

	index = 0;
	qdf_mem_zero(str_buf, DP_MAX_STRING_LEN);
	for (i = 0; i <  DP_HTT_RX_BW_LEN; i++) {
		index += qdf_snprint(&str_buf[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->rx_bw[i]);
	}
	DP_TRACE_STATS(FATAL, "rx_bw = %s ", str_buf);

	for (j = 0; j < DP_HTT_RSSI_CHAIN_LEN; j++) {
		index = 0;
		for (i = 0; i <  HTT_RX_PDEV_STATS_NUM_BW_COUNTERS; i++) {
			index += qdf_snprint(&rssi_chain[j][index],
					DP_MAX_STRING_LEN - index,
					" %u:%u,", i,
					dp_stats_buf->rssi_chain[j][i]);
		}
		DP_TRACE_STATS(FATAL, "rssi_chain[%u] = %s ", j, rssi_chain[j]);
	}

	for (j = 0; j < DP_HTT_RX_GI_LEN; j++) {
		index = 0;
		for (i = 0; i <  HTT_RX_PDEV_STATS_NUM_MCS_COUNTERS; i++) {
			index += qdf_snprint(&rx_gi[j][index],
					DP_MAX_STRING_LEN - index,
					" %u:%u,", i,
					dp_stats_buf->rx_gi[j][i]);
		}
		DP_TRACE_STATS(FATAL, "rx_gi[%u] = %s ", j, rx_gi[j]);
	}

	index = 0;
	qdf_mem_zero(str_buf, DP_MAX_STRING_LEN);
	for (i = 0; i <  DP_HTT_RX_PREAM_LEN; i++) {
		index += qdf_snprint(&str_buf[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->rx_pream[i]);
	}
	DP_TRACE_STATS(FATAL, "rx_pream = %s\n", str_buf);
	for (i = 0; i < DP_HTT_RSSI_CHAIN_LEN; i++)
		qdf_mem_free(rssi_chain[i]);
	for (i = 0; i < HTT_RX_PDEV_STATS_NUM_GI_COUNTERS; i++)
		qdf_mem_free(rx_gi[i]);

	qdf_mem_free(str_buf);
}

/*
 * dp_print_rx_soc_fw_stats_tlv: display htt_rx_soc_fw_stats_tlv
 * @tag_buf: buffer containing the tlv htt_rx_soc_fw_stats_tlv
 *
 * return:void
 */
static inline void dp_print_rx_soc_fw_stats_tlv(uint32_t *tag_buf)
{
	htt_rx_soc_fw_stats_tlv *dp_stats_buf =
		(htt_rx_soc_fw_stats_tlv *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_RX_SOC_FW_STATS_TLV:");
	DP_TRACE_STATS(FATAL, "fw_reo_ring_data_msdu = %u",
		      dp_stats_buf->fw_reo_ring_data_msdu);
	DP_TRACE_STATS(FATAL, "fw_to_host_data_msdu_bcmc = %u",
		      dp_stats_buf->fw_to_host_data_msdu_bcmc);
	DP_TRACE_STATS(FATAL, "fw_to_host_data_msdu_uc = %u",
		      dp_stats_buf->fw_to_host_data_msdu_uc);
	DP_TRACE_STATS(FATAL, "ofld_remote_data_buf_recycle_cnt = %u",
		      dp_stats_buf->ofld_remote_data_buf_recycle_cnt);
	DP_TRACE_STATS(FATAL, "ofld_remote_free_buf_indication_cnt = %u",
		      dp_stats_buf->ofld_remote_free_buf_indication_cnt);
	DP_TRACE_STATS(FATAL, "ofld_buf_to_host_data_msdu_uc = %u ",
		      dp_stats_buf->ofld_buf_to_host_data_msdu_uc);
	DP_TRACE_STATS(FATAL, "reo_fw_ring_to_host_data_msdu_uc = %u ",
		      dp_stats_buf->reo_fw_ring_to_host_data_msdu_uc);
	DP_TRACE_STATS(FATAL, "wbm_sw_ring_reap = %u ",
		      dp_stats_buf->wbm_sw_ring_reap);
	DP_TRACE_STATS(FATAL, "wbm_forward_to_host_cnt = %u ",
		      dp_stats_buf->wbm_forward_to_host_cnt);
	DP_TRACE_STATS(FATAL, "wbm_target_recycle_cnt = %u ",
		      dp_stats_buf->wbm_target_recycle_cnt);
	DP_TRACE_STATS(FATAL, "target_refill_ring_recycle_cnt = %u",
		      dp_stats_buf->target_refill_ring_recycle_cnt);

}

/*
 * dp_print_rx_soc_fw_refill_ring_empty_tlv_v: display
 *					htt_rx_soc_fw_refill_ring_empty_tlv_v
 * @tag_buf: buffer containing the tlv htt_rx_soc_fw_refill_ring_empty_tlv_v
 *
 * return:void
 */
static inline void dp_print_rx_soc_fw_refill_ring_empty_tlv_v(uint32_t *tag_buf)
{
	htt_rx_soc_fw_refill_ring_empty_tlv_v *dp_stats_buf =
		(htt_rx_soc_fw_refill_ring_empty_tlv_v *)tag_buf;
	uint8_t i;
	uint16_t index = 0;
	uint32_t tag_len = (HTT_STATS_TLV_LENGTH_GET(*tag_buf) >> 2);
	char *refill_ring_empty_cnt = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!refill_ring_empty_cnt) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	tag_len = qdf_min(tag_len, (uint32_t)HTT_RX_STATS_REFILL_MAX_RING);

	DP_TRACE_STATS(FATAL, "HTT_RX_SOC_FW_REFILL_RING_EMPTY_TLV_V:");
	for (i = 0; i <  tag_len; i++) {
		index += qdf_snprint(&refill_ring_empty_cnt[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i,
				dp_stats_buf->refill_ring_empty_cnt[i]);
	}
	DP_TRACE_STATS(FATAL, "refill_ring_empty_cnt = %s\n",
			refill_ring_empty_cnt);
	qdf_mem_free(refill_ring_empty_cnt);
}

/*
 * dp_print_rx_soc_fw_refill_ring_num_refill_tlv_v: display
 *				htt_rx_soc_fw_refill_ring_num_refill_tlv_v
 * @tag_buf: buffer containing the tlv htt_rx_soc_fw_refill_ring_num_refill_tlv
 *
 * return:void
 */
static inline void dp_print_rx_soc_fw_refill_ring_num_refill_tlv_v(
		uint32_t *tag_buf)
{
	htt_rx_soc_fw_refill_ring_num_refill_tlv_v *dp_stats_buf =
		(htt_rx_soc_fw_refill_ring_num_refill_tlv_v *)tag_buf;
	uint8_t i;
	uint16_t index = 0;
	uint32_t tag_len = (HTT_STATS_TLV_LENGTH_GET(*tag_buf) >> 2);
	char *refill_ring_num_refill = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!refill_ring_num_refill) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	tag_len = qdf_min(tag_len, (uint32_t)HTT_TX_PDEV_MAX_URRN_STATS);

	DP_TRACE_STATS(FATAL, "HTT_RX_SOC_FW_REFILL_RING_NUM_REFILL_TLV_V:");
	for (i = 0; i <  tag_len; i++) {
		index += qdf_snprint(&refill_ring_num_refill[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i,
				dp_stats_buf->refill_ring_num_refill[i]);
	}
	DP_TRACE_STATS(FATAL, "refill_ring_num_refill = %s\n",
			refill_ring_num_refill);
	qdf_mem_free(refill_ring_num_refill);
}

/*
 * dp_print_rx_pdev_fw_stats_tlv: display htt_rx_pdev_fw_stats_tlv
 * @tag_buf: buffer containing the tlv htt_rx_pdev_fw_stats_tlv
 *
 * return:void
 */
static inline void dp_print_rx_pdev_fw_stats_tlv(uint32_t *tag_buf)
{
	htt_rx_pdev_fw_stats_tlv *dp_stats_buf =
		(htt_rx_pdev_fw_stats_tlv *)tag_buf;
	uint8_t i;
	uint16_t index = 0;
	char fw_ring_mgmt_subtype[DP_MAX_STRING_LEN];
	char fw_ring_ctrl_subtype[DP_MAX_STRING_LEN];

	DP_TRACE_STATS(FATAL, "HTT_RX_PDEV_FW_STATS_TLV:");
	DP_TRACE_STATS(FATAL, "mac_id__word = %u",
			dp_stats_buf->mac_id__word);
	DP_TRACE_STATS(FATAL, "ppdu_recvd = %u",
			dp_stats_buf->ppdu_recvd);
	DP_TRACE_STATS(FATAL, "mpdu_cnt_fcs_ok = %u",
			dp_stats_buf->mpdu_cnt_fcs_ok);
	DP_TRACE_STATS(FATAL, "mpdu_cnt_fcs_err = %u",
			dp_stats_buf->mpdu_cnt_fcs_err);
	DP_TRACE_STATS(FATAL, "tcp_msdu_cnt = %u",
			dp_stats_buf->tcp_msdu_cnt);
	DP_TRACE_STATS(FATAL, "tcp_ack_msdu_cnt = %u",
			dp_stats_buf->tcp_ack_msdu_cnt);
	DP_TRACE_STATS(FATAL, "udp_msdu_cnt = %u",
			dp_stats_buf->udp_msdu_cnt);
	DP_TRACE_STATS(FATAL, "other_msdu_cnt = %u",
			dp_stats_buf->other_msdu_cnt);
	DP_TRACE_STATS(FATAL, "fw_ring_mpdu_ind = %u",
			dp_stats_buf->fw_ring_mpdu_ind);

	for (i = 0; i <  DP_HTT_FW_RING_MGMT_SUBTYPE_LEN; i++) {
		index += qdf_snprint(&fw_ring_mgmt_subtype[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i,
				dp_stats_buf->fw_ring_mgmt_subtype[i]);
	}
	DP_TRACE_STATS(FATAL, "fw_ring_mgmt_subtype = %s ", fw_ring_mgmt_subtype);

	index = 0;
	for (i = 0; i <  DP_HTT_FW_RING_CTRL_SUBTYPE_LEN; i++) {
		index += qdf_snprint(&fw_ring_ctrl_subtype[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i,
				dp_stats_buf->fw_ring_ctrl_subtype[i]);
	}
	DP_TRACE_STATS(FATAL, "fw_ring_ctrl_subtype = %s ", fw_ring_ctrl_subtype);
	DP_TRACE_STATS(FATAL, "fw_ring_mcast_data_msdu = %u",
			dp_stats_buf->fw_ring_mcast_data_msdu);
	DP_TRACE_STATS(FATAL, "fw_ring_bcast_data_msdu = %u",
			dp_stats_buf->fw_ring_bcast_data_msdu);
	DP_TRACE_STATS(FATAL, "fw_ring_ucast_data_msdu = %u",
			dp_stats_buf->fw_ring_ucast_data_msdu);
	DP_TRACE_STATS(FATAL, "fw_ring_null_data_msdu = %u",
			dp_stats_buf->fw_ring_null_data_msdu);
	DP_TRACE_STATS(FATAL, "fw_ring_mpdu_drop = %u",
			dp_stats_buf->fw_ring_mpdu_drop);
	DP_TRACE_STATS(FATAL, "ofld_local_data_ind_cnt = %u",
			dp_stats_buf->ofld_local_data_ind_cnt);
	DP_TRACE_STATS(FATAL, "ofld_local_data_buf_recycle_cnt = %u",
			dp_stats_buf->ofld_local_data_buf_recycle_cnt);
	DP_TRACE_STATS(FATAL, "drx_local_data_ind_cnt = %u",
			dp_stats_buf->drx_local_data_ind_cnt);
	DP_TRACE_STATS(FATAL, "drx_local_data_buf_recycle_cnt = %u",
			dp_stats_buf->drx_local_data_buf_recycle_cnt);
	DP_TRACE_STATS(FATAL, "local_nondata_ind_cnt = %u",
			dp_stats_buf->local_nondata_ind_cnt);
	DP_TRACE_STATS(FATAL, "local_nondata_buf_recycle_cnt = %u",
			dp_stats_buf->local_nondata_buf_recycle_cnt);
	DP_TRACE_STATS(FATAL, "fw_status_buf_ring_refill_cnt = %u",
			dp_stats_buf->fw_status_buf_ring_refill_cnt);
	DP_TRACE_STATS(FATAL, "fw_status_buf_ring_empty_cnt = %u",
			dp_stats_buf->fw_status_buf_ring_empty_cnt);
	DP_TRACE_STATS(FATAL, "fw_pkt_buf_ring_refill_cnt = %u",
			dp_stats_buf->fw_pkt_buf_ring_refill_cnt);
	DP_TRACE_STATS(FATAL, "fw_pkt_buf_ring_empty_cnt = %u",
			dp_stats_buf->fw_pkt_buf_ring_empty_cnt);
	DP_TRACE_STATS(FATAL, "fw_link_buf_ring_refill_cnt = %u",
			dp_stats_buf->fw_link_buf_ring_refill_cnt);
	DP_TRACE_STATS(FATAL, "fw_link_buf_ring_empty_cnt = %u",
			dp_stats_buf->fw_link_buf_ring_empty_cnt);
	DP_TRACE_STATS(FATAL, "host_pkt_buf_ring_refill_cnt = %u",
			dp_stats_buf->host_pkt_buf_ring_refill_cnt);
	DP_TRACE_STATS(FATAL, "host_pkt_buf_ring_empty_cnt = %u",
			dp_stats_buf->host_pkt_buf_ring_empty_cnt);
	DP_TRACE_STATS(FATAL, "mon_pkt_buf_ring_refill_cnt = %u",
			dp_stats_buf->mon_pkt_buf_ring_refill_cnt);
	DP_TRACE_STATS(FATAL, "mon_pkt_buf_ring_empty_cnt = %u",
			dp_stats_buf->mon_pkt_buf_ring_empty_cnt);
	DP_TRACE_STATS(FATAL, "mon_status_buf_ring_refill_cnt = %u",
			dp_stats_buf->mon_status_buf_ring_refill_cnt);
	DP_TRACE_STATS(FATAL, "mon_status_buf_ring_empty_cnt = %u",
			dp_stats_buf->mon_status_buf_ring_empty_cnt);
	DP_TRACE_STATS(FATAL, "mon_desc_buf_ring_refill_cnt = %u",
			dp_stats_buf->mon_desc_buf_ring_refill_cnt);
	DP_TRACE_STATS(FATAL, "mon_desc_buf_ring_empty_cnt = %u",
			dp_stats_buf->mon_desc_buf_ring_empty_cnt);
	DP_TRACE_STATS(FATAL, "mon_dest_ring_update_cnt = %u",
			dp_stats_buf->mon_dest_ring_update_cnt);
	DP_TRACE_STATS(FATAL, "mon_dest_ring_full_cnt = %u",
			dp_stats_buf->mon_dest_ring_full_cnt);
	DP_TRACE_STATS(FATAL, "rx_suspend_cnt = %u",
			dp_stats_buf->rx_suspend_cnt);
	DP_TRACE_STATS(FATAL, "rx_suspend_fail_cnt = %u",
			dp_stats_buf->rx_suspend_fail_cnt);
	DP_TRACE_STATS(FATAL, "rx_resume_cnt = %u",
			dp_stats_buf->rx_resume_cnt);
	DP_TRACE_STATS(FATAL, "rx_resume_fail_cnt = %u",
			dp_stats_buf->rx_resume_fail_cnt);
	DP_TRACE_STATS(FATAL, "rx_ring_switch_cnt = %u",
			dp_stats_buf->rx_ring_switch_cnt);
	DP_TRACE_STATS(FATAL, "rx_ring_restore_cnt = %u",
			dp_stats_buf->rx_ring_restore_cnt);
	DP_TRACE_STATS(FATAL, "rx_flush_cnt = %u\n",
			dp_stats_buf->rx_flush_cnt);
}

/*
 * dp_print_rx_pdev_fw_ring_mpdu_err_tlv_v: display
 *				htt_rx_pdev_fw_ring_mpdu_err_tlv_v
 * @tag_buf: buffer containing the tlv htt_rx_pdev_fw_ring_mpdu_err_tlv_v
 *
 * return:void
 */
static inline void dp_print_rx_pdev_fw_ring_mpdu_err_tlv_v(uint32_t *tag_buf)
{
	htt_rx_pdev_fw_ring_mpdu_err_tlv_v *dp_stats_buf =
		(htt_rx_pdev_fw_ring_mpdu_err_tlv_v *)tag_buf;
	uint8_t i;
	uint16_t index = 0;
	char *fw_ring_mpdu_err = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!fw_ring_mpdu_err) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	DP_TRACE_STATS(FATAL, "HTT_RX_PDEV_FW_RING_MPDU_ERR_TLV_V:");
	for (i = 0; i <  DP_HTT_FW_RING_MPDU_ERR_LEN; i++) {
		index += qdf_snprint(&fw_ring_mpdu_err[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i,
				dp_stats_buf->fw_ring_mpdu_err[i]);
	}
	DP_TRACE_STATS(FATAL, "fw_ring_mpdu_err = %s\n", fw_ring_mpdu_err);
	qdf_mem_free(fw_ring_mpdu_err);
}

/*
 * dp_print_rx_pdev_fw_mpdu_drop_tlv_v: display htt_rx_pdev_fw_mpdu_drop_tlv_v
 * @tag_buf: buffer containing the tlv htt_rx_pdev_fw_mpdu_drop_tlv_v
 *
 * return:void
 */
static inline void dp_print_rx_pdev_fw_mpdu_drop_tlv_v(uint32_t *tag_buf)
{
	htt_rx_pdev_fw_mpdu_drop_tlv_v *dp_stats_buf =
		(htt_rx_pdev_fw_mpdu_drop_tlv_v *)tag_buf;
	uint8_t i;
	uint16_t index = 0;
	uint32_t tag_len = (HTT_STATS_TLV_LENGTH_GET(*tag_buf) >> 2);
	char *fw_mpdu_drop = qdf_mem_malloc(DP_MAX_STRING_LEN);

	if (!fw_mpdu_drop) {
		QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
			  FL("Output buffer not allocated"));
		return;
	}

	tag_len = qdf_min(tag_len, (uint32_t)HTT_RX_STATS_FW_DROP_REASON_MAX);

	DP_TRACE_STATS(FATAL, "HTT_RX_PDEV_FW_MPDU_DROP_TLV_V:");
	for (i = 0; i <  tag_len; i++) {
		index += qdf_snprint(&fw_mpdu_drop[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->fw_mpdu_drop[i]);
	}
	DP_TRACE_STATS(FATAL, "fw_mpdu_drop = %s\n", fw_mpdu_drop);
	qdf_mem_free(fw_mpdu_drop);
}

/*
 * dp_print_rx_soc_fw_refill_ring_num_rxdma_err_tlv() - Accounts for rxdma error
 * packets
 *
 * tag_buf - Buffer
 * Return - NULL
 */
static inline void dp_print_rx_soc_fw_refill_ring_num_rxdma_err_tlv(uint32_t *tag_buf)
{
	htt_rx_soc_fw_refill_ring_num_rxdma_err_tlv_v *dp_stats_buf =
		(htt_rx_soc_fw_refill_ring_num_rxdma_err_tlv_v *)tag_buf;

	uint8_t i;
	uint16_t index = 0;
	char rxdma_err_cnt[DP_MAX_STRING_LEN];
	uint32_t tag_len = (HTT_STATS_TLV_LENGTH_GET(*tag_buf) >> 2);

	tag_len = qdf_min(tag_len, (uint32_t)HTT_RX_RXDMA_MAX_ERR_CODE);

	DP_TRACE_STATS(FATAL, "HTT_RX_SOC_FW_REFILL_RING_NUM_RXDMA_ERR_TLV_V");

	for (i = 0; i <  tag_len; i++) {
		index += snprintf(&rxdma_err_cnt[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i,
				dp_stats_buf->rxdma_err[i]);
	}

	DP_TRACE_STATS(FATAL, "rxdma_err = %s\n", rxdma_err_cnt);
}

/*
 * dp_print_rx_soc_fw_refill_ring_num_reo_err_tlv() - Accounts for reo error
 * packets
 *
 * tag_buf - Buffer
 * Return - NULL
 */
static inline void dp_print_rx_soc_fw_refill_ring_num_reo_err_tlv(uint32_t *tag_buf)
{
	htt_rx_soc_fw_refill_ring_num_reo_err_tlv_v *dp_stats_buf =
		(htt_rx_soc_fw_refill_ring_num_reo_err_tlv_v *)tag_buf;

	uint8_t i;
	uint16_t index = 0;
	char reo_err_cnt[DP_MAX_STRING_LEN];
	uint32_t tag_len = (HTT_STATS_TLV_LENGTH_GET(*tag_buf) >> 2);

	tag_len = qdf_min(tag_len, (uint32_t)HTT_RX_REO_MAX_ERR_CODE);

	DP_TRACE_STATS(FATAL, "HTT_RX_SOC_FW_REFILL_RING_NUM_REO_ERR_TLV_V");

	for (i = 0; i <  tag_len; i++) {
		index += snprintf(&reo_err_cnt[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i,
				dp_stats_buf->reo_err[i]);
	}

	DP_TRACE_STATS(FATAL, "reo_err = %s\n", reo_err_cnt);
}

/*
 * dp_print_rx_reo_debug_stats_tlv() - REO Statistics
 *
 * tag_buf - Buffer
 * Return - NULL
 */
static inline void dp_print_rx_reo_debug_stats_tlv(uint32_t *tag_buf)
{
	htt_rx_reo_resource_stats_tlv_v *dp_stats_buf =
			(htt_rx_reo_resource_stats_tlv_v *)tag_buf;

	DP_TRACE_STATS(FATAL, "HTT_RX_REO_RESOURCE_STATS_TLV");

	DP_TRACE_STATS(FATAL, "sample_id: %u ",
		      dp_stats_buf->sample_id);
	DP_TRACE_STATS(FATAL, "total_max: %u ",
		      dp_stats_buf->total_max);
	DP_TRACE_STATS(FATAL, "total_avg: %u ",
		      dp_stats_buf->total_avg);
	DP_TRACE_STATS(FATAL, "total_sample: %u ",
		      dp_stats_buf->total_sample);
	DP_TRACE_STATS(FATAL, "non_zeros_avg: %u ",
		      dp_stats_buf->non_zeros_avg);
	DP_TRACE_STATS(FATAL, "non_zeros_sample: %u ",
		      dp_stats_buf->non_zeros_sample);
	DP_TRACE_STATS(FATAL, "last_non_zeros_max: %u ",
		      dp_stats_buf->last_non_zeros_max);
	DP_TRACE_STATS(FATAL, "last_non_zeros_min: %u ",
		      dp_stats_buf->last_non_zeros_min);
	DP_TRACE_STATS(FATAL, "last_non_zeros_avg: %u ",
		      dp_stats_buf->last_non_zeros_avg);
	DP_TRACE_STATS(FATAL, "last_non_zeros_sample: %u\n ",
		      dp_stats_buf->last_non_zeros_sample);
}

/*
 * dp_print_rx_pdev_fw_stats_phy_err_tlv() - Accounts for phy errors
 *
 * tag_buf - Buffer
 * Return - NULL
 */
static inline void dp_print_rx_pdev_fw_stats_phy_err_tlv(uint32_t *tag_buf)
{
	htt_rx_pdev_fw_stats_phy_err_tlv *dp_stats_buf =
		(htt_rx_pdev_fw_stats_phy_err_tlv *)tag_buf;

	uint8_t i = 0;
	uint16_t index = 0;
	char phy_errs[DP_MAX_STRING_LEN];

	DP_TRACE_STATS(FATAL, "HTT_RX_PDEV_FW_STATS_PHY_ERR_TLV");

	DP_TRACE_STATS(FATAL, "mac_id_word: %u",
		      dp_stats_buf->mac_id__word);
	DP_TRACE_STATS(FATAL, "total_phy_err_cnt: %u",
		      dp_stats_buf->total_phy_err_cnt);

	for (i = 0; i < HTT_STATS_PHY_ERR_MAX; i++) {
		index += snprintf(&phy_errs[index],
				DP_MAX_STRING_LEN - index,
				" %u:%u,", i, dp_stats_buf->phy_err[i]);
	}

	DP_TRACE_STATS(FATAL, "phy_errs: %s\n",  phy_errs);
}

/*
 * dp_htt_stats_print_tag: function to select the tag type and
 * print the corresponding tag structure
 * @tag_type: tag type that is to be printed
 * @tag_buf: pointer to the tag structure
 *
 * return: void
 */
void dp_htt_stats_print_tag(uint8_t tag_type, uint32_t *tag_buf)
{
	switch (tag_type) {
	case HTT_STATS_TX_PDEV_CMN_TAG:
		dp_print_tx_pdev_stats_cmn_tlv(tag_buf);
		break;
	case HTT_STATS_TX_PDEV_UNDERRUN_TAG:
		dp_print_tx_pdev_stats_urrn_tlv_v(tag_buf);
		break;
	case HTT_STATS_TX_PDEV_SIFS_TAG:
		dp_print_tx_pdev_stats_sifs_tlv_v(tag_buf);
		break;
	case HTT_STATS_TX_PDEV_FLUSH_TAG:
		dp_print_tx_pdev_stats_flush_tlv_v(tag_buf);
		break;

	case HTT_STATS_TX_PDEV_PHY_ERR_TAG:
		dp_print_tx_pdev_stats_phy_err_tlv_v(tag_buf);
		break;

	case HTT_STATS_STRING_TAG:
		dp_print_stats_string_tlv(tag_buf);
		break;

	case HTT_STATS_TX_HWQ_CMN_TAG:
		dp_print_tx_hwq_stats_cmn_tlv(tag_buf);
		break;

	case HTT_STATS_TX_HWQ_DIFS_LATENCY_TAG:
		dp_print_tx_hwq_difs_latency_stats_tlv_v(tag_buf);
		break;

	case HTT_STATS_TX_HWQ_CMD_RESULT_TAG:
		dp_print_tx_hwq_cmd_result_stats_tlv_v(tag_buf);
		break;

	case HTT_STATS_TX_HWQ_CMD_STALL_TAG:
		dp_print_tx_hwq_cmd_stall_stats_tlv_v(tag_buf);
		break;

	case HTT_STATS_TX_HWQ_FES_STATUS_TAG:
		dp_print_tx_hwq_fes_result_stats_tlv_v(tag_buf);
		break;

	case HTT_STATS_TX_TQM_GEN_MPDU_TAG:
		dp_print_tx_tqm_gen_mpdu_stats_tlv_v(tag_buf);
		break;

	case HTT_STATS_TX_TQM_LIST_MPDU_TAG:
		dp_print_tx_tqm_list_mpdu_stats_tlv_v(tag_buf);
		break;

	case HTT_STATS_TX_TQM_LIST_MPDU_CNT_TAG:
		dp_print_tx_tqm_list_mpdu_cnt_tlv_v(tag_buf);
		break;

	case HTT_STATS_TX_TQM_CMN_TAG:
		dp_print_tx_tqm_cmn_stats_tlv(tag_buf);
		break;

	case HTT_STATS_TX_TQM_PDEV_TAG:
		dp_print_tx_tqm_pdev_stats_tlv_v(tag_buf);
		break;

	case HTT_STATS_TX_TQM_CMDQ_STATUS_TAG:
		dp_print_tx_tqm_cmdq_status_tlv(tag_buf);
		break;

	case HTT_STATS_TX_DE_EAPOL_PACKETS_TAG:
		dp_print_tx_de_eapol_packets_stats_tlv(tag_buf);
		break;

	case HTT_STATS_TX_DE_CLASSIFY_FAILED_TAG:
		dp_print_tx_de_classify_failed_stats_tlv(tag_buf);
		break;

	case HTT_STATS_TX_DE_CLASSIFY_STATS_TAG:
		dp_print_tx_de_classify_stats_tlv(tag_buf);
		break;

	case HTT_STATS_TX_DE_CLASSIFY_STATUS_TAG:
		dp_print_tx_de_classify_status_stats_tlv(tag_buf);
		break;

	case HTT_STATS_TX_DE_ENQUEUE_PACKETS_TAG:
		dp_print_tx_de_enqueue_packets_stats_tlv(tag_buf);
		break;

	case HTT_STATS_TX_DE_ENQUEUE_DISCARD_TAG:
		dp_print_tx_de_enqueue_discard_stats_tlv(tag_buf);
		break;

	case HTT_STATS_TX_DE_CMN_TAG:
		dp_print_tx_de_cmn_stats_tlv(tag_buf);
		break;

	case HTT_STATS_RING_IF_TAG:
		dp_print_ring_if_stats_tlv(tag_buf);
		break;

	case HTT_STATS_TX_PDEV_MU_MIMO_STATS_TAG:
		dp_print_tx_pdev_mu_mimo_sch_stats_tlv(tag_buf);
		break;

	case HTT_STATS_SFM_CMN_TAG:
		dp_print_sfm_cmn_tlv(tag_buf);
		break;

	case HTT_STATS_SRING_STATS_TAG:
		dp_print_sring_stats_tlv(tag_buf);
		break;

	case HTT_STATS_RX_PDEV_FW_STATS_TAG:
		dp_print_rx_pdev_fw_stats_tlv(tag_buf);
		break;

	case HTT_STATS_RX_PDEV_FW_RING_MPDU_ERR_TAG:
		dp_print_rx_pdev_fw_ring_mpdu_err_tlv_v(tag_buf);
		break;

	case HTT_STATS_RX_PDEV_FW_MPDU_DROP_TAG:
		dp_print_rx_pdev_fw_mpdu_drop_tlv_v(tag_buf);
		break;

	case HTT_STATS_RX_SOC_FW_STATS_TAG:
		dp_print_rx_soc_fw_stats_tlv(tag_buf);
		break;

	case HTT_STATS_RX_SOC_FW_REFILL_RING_EMPTY_TAG:
		dp_print_rx_soc_fw_refill_ring_empty_tlv_v(tag_buf);
		break;

	case HTT_STATS_RX_SOC_FW_REFILL_RING_NUM_REFILL_TAG:
		dp_print_rx_soc_fw_refill_ring_num_refill_tlv_v(
				tag_buf);
		break;

	case HTT_STATS_TX_PDEV_RATE_STATS_TAG:
		dp_print_tx_pdev_rate_stats_tlv(tag_buf);
		break;

	case HTT_STATS_RX_PDEV_RATE_STATS_TAG:
		dp_print_rx_pdev_rate_stats_tlv(tag_buf);
		break;

	case HTT_STATS_TX_PDEV_SCHEDULER_TXQ_STATS_TAG:
		dp_print_tx_pdev_stats_sched_per_txq_tlv(tag_buf);
		break;

	case HTT_STATS_TX_SCHED_CMN_TAG:
		dp_print_stats_tx_sched_cmn_tlv(tag_buf);
		break;

	case HTT_STATS_TX_PDEV_MPDU_STATS_TAG:
		dp_print_tx_pdev_mu_mimo_mpdu_stats_tlv(tag_buf);
		break;

	case HTT_STATS_SCHED_TXQ_CMD_POSTED_TAG:
		dp_print_sched_txq_cmd_posted_tlv_v(tag_buf);
		break;

	case HTT_STATS_RING_IF_CMN_TAG:
		dp_print_ring_if_cmn_tlv(tag_buf);
		break;

	case HTT_STATS_SFM_CLIENT_USER_TAG:
		dp_print_sfm_client_user_tlv_v(tag_buf);
		break;

	case HTT_STATS_SFM_CLIENT_TAG:
		dp_print_sfm_client_tlv(tag_buf);
		break;

	case HTT_STATS_TX_TQM_ERROR_STATS_TAG:
		dp_print_tx_tqm_error_stats_tlv(tag_buf);
		break;

	case HTT_STATS_SCHED_TXQ_CMD_REAPED_TAG:
		dp_print_sched_txq_cmd_reaped_tlv_v(tag_buf);
		break;

	case HTT_STATS_SRING_CMN_TAG:
		dp_print_sring_cmn_tlv(tag_buf);
		break;

	case HTT_STATS_TX_SELFGEN_AC_ERR_STATS_TAG:
		dp_print_tx_selfgen_ac_err_stats_tlv(tag_buf);
		break;

	case HTT_STATS_TX_SELFGEN_CMN_STATS_TAG:
		dp_print_tx_selfgen_cmn_stats_tlv(tag_buf);
		break;

	case HTT_STATS_TX_SELFGEN_AC_STATS_TAG:
		dp_print_tx_selfgen_ac_stats_tlv(tag_buf);
		break;

	case HTT_STATS_TX_SELFGEN_AX_STATS_TAG:
		dp_print_tx_selfgen_ax_stats_tlv(tag_buf);
		break;

	case HTT_STATS_TX_SELFGEN_AX_ERR_STATS_TAG:
		dp_print_tx_selfgen_ax_err_stats_tlv(tag_buf);
		break;

	case HTT_STATS_TX_HWQ_MUMIMO_SCH_STATS_TAG:
		dp_print_tx_hwq_mu_mimo_sch_stats_tlv(tag_buf);
		break;

	case HTT_STATS_TX_HWQ_MUMIMO_MPDU_STATS_TAG:
		dp_print_tx_hwq_mu_mimo_mpdu_stats_tlv(tag_buf);
		break;

	case HTT_STATS_TX_HWQ_MUMIMO_CMN_STATS_TAG:
		dp_print_tx_hwq_mu_mimo_cmn_stats_tlv(tag_buf);
		break;

	case HTT_STATS_HW_INTR_MISC_TAG:
		dp_print_hw_stats_intr_misc_tlv(tag_buf);
		break;

	case HTT_STATS_HW_WD_TIMEOUT_TAG:
		dp_print_hw_stats_wd_timeout_tlv(tag_buf);
		break;

	case HTT_STATS_HW_PDEV_ERRS_TAG:
		dp_print_hw_stats_pdev_errs_tlv(tag_buf);
		break;

	case HTT_STATS_COUNTER_NAME_TAG:
		dp_print_counter_tlv(tag_buf);
		break;

	case HTT_STATS_TX_TID_DETAILS_TAG:
		dp_print_tx_tid_stats_tlv(tag_buf);
		break;

#ifdef CONFIG_WIN
	case HTT_STATS_TX_TID_DETAILS_V1_TAG:
		dp_print_tx_tid_stats_v1_tlv(tag_buf);
		break;
#endif

	case HTT_STATS_RX_TID_DETAILS_TAG:
		dp_print_rx_tid_stats_tlv(tag_buf);
		break;

	case HTT_STATS_PEER_STATS_CMN_TAG:
		dp_print_peer_stats_cmn_tlv(tag_buf);
		break;

	case HTT_STATS_PEER_DETAILS_TAG:
		dp_print_peer_details_tlv(tag_buf);
		break;

	case HTT_STATS_PEER_MSDU_FLOWQ_TAG:
		dp_print_msdu_flow_stats_tlv(tag_buf);
		break;

	case HTT_STATS_PEER_TX_RATE_STATS_TAG:
		dp_print_tx_peer_rate_stats_tlv(tag_buf);
		break;

	case HTT_STATS_PEER_RX_RATE_STATS_TAG:
		dp_print_rx_peer_rate_stats_tlv(tag_buf);
		break;

	case HTT_STATS_TX_DE_COMPL_STATS_TAG:
		dp_print_tx_de_compl_stats_tlv(tag_buf);
		break;

	case HTT_STATS_RX_REFILL_RXDMA_ERR_TAG:
		dp_print_rx_soc_fw_refill_ring_num_rxdma_err_tlv(tag_buf);
		break;

	case HTT_STATS_RX_REFILL_REO_ERR_TAG:
		dp_print_rx_soc_fw_refill_ring_num_reo_err_tlv(tag_buf);
		break;

	case HTT_STATS_RX_REO_RESOURCE_STATS_TAG:
		dp_print_rx_reo_debug_stats_tlv(tag_buf);
		break;

	case HTT_STATS_RX_PDEV_FW_STATS_PHY_ERR_TAG:
		dp_print_rx_pdev_fw_stats_phy_err_tlv(tag_buf);
		break;

	default:
		break;
	}
}

/*
 * dp_htt_stats_copy_tag: function to select the tag type and
 * copy the corresponding tag structure
 * @pdev: DP_PDEV handle
 * @tag_type: tag type that is to be printed
 * @tag_buf: pointer to the tag structure
 *
 * return: void
 */
void dp_htt_stats_copy_tag(struct dp_pdev *pdev, uint8_t tag_type, uint32_t *tag_buf)
{
	void *dest_ptr = NULL;
	uint32_t size = 0;

	switch (tag_type) {
	case HTT_STATS_TX_PDEV_CMN_TAG:
		dest_ptr = &pdev->stats.htt_tx_pdev_stats.cmn_tlv;
		size = sizeof(htt_tx_pdev_stats_cmn_tlv);
		break;
	case HTT_STATS_TX_PDEV_UNDERRUN_TAG:
		dest_ptr = &pdev->stats.htt_tx_pdev_stats.underrun_tlv;
		size = sizeof(htt_tx_pdev_stats_urrn_tlv_v);
		break;
	case HTT_STATS_TX_PDEV_SIFS_TAG:
		dest_ptr = &pdev->stats.htt_tx_pdev_stats.sifs_tlv;
		size = sizeof(htt_tx_pdev_stats_sifs_tlv_v);
		break;
	case HTT_STATS_TX_PDEV_FLUSH_TAG:
		dest_ptr = &pdev->stats.htt_tx_pdev_stats.flush_tlv;
		size = sizeof(htt_tx_pdev_stats_flush_tlv_v);
		break;
	case HTT_STATS_TX_PDEV_PHY_ERR_TAG:
		dest_ptr = &pdev->stats.htt_tx_pdev_stats.phy_err_tlv;
		size = sizeof(htt_tx_pdev_stats_phy_err_tlv_v);
		break;
	case HTT_STATS_RX_PDEV_FW_STATS_TAG:
		dest_ptr = &pdev->stats.htt_rx_pdev_stats.fw_stats_tlv;
		size = sizeof(htt_rx_pdev_fw_stats_tlv);
		break;
	case HTT_STATS_RX_SOC_FW_STATS_TAG:
		dest_ptr = &pdev->stats.htt_rx_pdev_stats.soc_stats.fw_tlv;
		size = sizeof(htt_rx_soc_fw_stats_tlv);
		break;
	case HTT_STATS_RX_SOC_FW_REFILL_RING_EMPTY_TAG:
		dest_ptr = &pdev->stats.htt_rx_pdev_stats.soc_stats.fw_refill_ring_empty_tlv;
		size = sizeof(htt_rx_soc_fw_refill_ring_empty_tlv_v);
		break;
	case HTT_STATS_RX_SOC_FW_REFILL_RING_NUM_REFILL_TAG:
		dest_ptr = &pdev->stats.htt_rx_pdev_stats.soc_stats.fw_refill_ring_num_refill_tlv;
		size = sizeof(htt_rx_soc_fw_refill_ring_num_refill_tlv_v);
		break;
	case HTT_STATS_RX_PDEV_FW_RING_MPDU_ERR_TAG:
		dest_ptr = &pdev->stats.htt_rx_pdev_stats.fw_ring_mpdu_err_tlv;
		size = sizeof(htt_rx_pdev_fw_ring_mpdu_err_tlv_v);
		break;
	case HTT_STATS_RX_PDEV_FW_MPDU_DROP_TAG:
		dest_ptr = &pdev->stats.htt_rx_pdev_stats.fw_ring_mpdu_drop;
		size = sizeof(htt_rx_pdev_fw_mpdu_drop_tlv_v);
		break;
	default:
		break;
	}

	if (dest_ptr)
		qdf_mem_copy(dest_ptr, tag_buf, size);
}

#if defined(CONFIG_WIN) && WDI_EVENT_ENABLE
QDF_STATUS dp_peer_stats_notify(struct dp_peer *peer)
{
	struct dp_pdev *dp_pdev;
	struct cdp_interface_peer_stats peer_stats_intf;
	struct cdp_peer_stats *peer_stats = &peer->stats;

	if (!peer->vdev)
		return QDF_STATUS_E_FAULT;

	dp_pdev = peer->vdev->pdev;
	qdf_mem_zero(&peer_stats_intf, sizeof(peer_stats_intf));
	if (peer_stats->rx.last_rssi != peer_stats->rx.rssi)
		peer_stats_intf.rssi_changed = true;

	if ((peer_stats->rx.rssi && peer_stats_intf.rssi_changed) ||
	    (peer_stats->tx.tx_rate &&
	     peer_stats->tx.tx_rate != peer_stats->tx.last_tx_rate)) {
		peer_stats_intf.peer_hdl = peer->ctrl_peer;
		peer_stats_intf.last_peer_tx_rate = peer_stats->tx.last_tx_rate;
		peer_stats_intf.peer_tx_rate = peer_stats->tx.tx_rate;
		peer_stats_intf.peer_rssi = peer_stats->rx.rssi;
		peer_stats_intf.tx_packet_count = peer_stats->tx.ucast.num;
		peer_stats_intf.rx_packet_count = peer_stats->rx.to_stack.num;
		peer_stats_intf.tx_byte_count = peer_stats->tx.tx_success.bytes;
		peer_stats_intf.rx_byte_count = peer_stats->rx.to_stack.bytes;
		peer_stats_intf.per = peer_stats->tx.last_per;
		peer_stats_intf.ack_rssi = peer_stats->tx.last_ack_rssi;
		dp_wdi_event_handler(WDI_EVENT_PEER_STATS, dp_pdev->soc,
				     (void *)&peer_stats_intf, 0,
				     WDI_NO_VAL, dp_pdev->pdev_id);
	}

	return QDF_STATUS_SUCCESS;
}
#endif

#ifdef CONFIG_WIN
/**
 * dp_vow_str_fw_to_hw_delay() - Return string for a delay
 * @index: Index of delay
 *
 * Return: char const pointer
 */
static inline const char *dp_vow_str_fw_to_hw_delay(uint8_t index)
{
	if (index > CDP_DELAY_BUCKET_MAX) {
		return "Invalid index";
	}
	return fw_to_hw_delay_bucket[index];
}

/**
 * dp_vow_str_sw_enq_delay() - Return string for a delay
 * @index: Index of delay
 *
 * Return: char const pointer
 */
static inline const char *dp_vow_str_sw_enq_delay(uint8_t index)
{
	if (index > CDP_DELAY_BUCKET_MAX) {
		return "Invalid index";
	}
	return sw_enq_delay_bucket[index];
}

/**
 * dp_vow_str_intfrm_delay() - Return string for a delay
 * @index: Index of delay
 *
 * Return: char const pointer
 */
static inline const char *dp_vow_str_intfrm_delay(uint8_t index)
{
	if (index > CDP_DELAY_BUCKET_MAX) {
		return "Invalid index";
	}
	return intfrm_delay_bucket[index];
}

void dp_pdev_print_tid_stats(struct dp_pdev *pdev)
{
	struct cdp_tid_stats *tid_stats;
	struct cdp_tid_tx_stats *txstats;
	struct cdp_tid_rx_stats *rxstats;
	struct dp_soc *soc = pdev->soc;
	uint8_t tid;

	if (!soc)
		return;
	tid = 0;
	rxstats = NULL;
	txstats = NULL;
	tid_stats = &pdev->stats.tid_stats;

	DP_PRINT_STATS("Packets received in hardstart: %llu ",
		       tid_stats->ingress_stack);
	DP_PRINT_STATS("Packets dropped in osif layer: %llu ",
		       tid_stats->osif_drop);
	DP_PRINT_STATS("Per TID Video Stats:\n");

	for (tid = 0; tid < CDP_MAX_DATA_TIDS; tid++) {
		txstats = &tid_stats->tid_tx_stats[tid];
		rxstats = &tid_stats->tid_rx_stats[tid];
		DP_PRINT_STATS("----TID: %d----", tid);
		DP_PRINT_STATS("Tx Success Count: %llu", txstats->success_cnt);
		DP_PRINT_STATS("Tx Complete Count: %llu",
			       txstats->complete_cnt);
		DP_PRINT_STATS("Tx Firmware Drop Count: %llu",
			       txstats->comp_fail_cnt);
		DP_PRINT_STATS("Tx Hardware Drop Count: %llu",
			       txstats->swdrop_cnt[TX_HW_ENQUEUE]);
		DP_PRINT_STATS("Tx Software Drop Count: %llu",
			       txstats->swdrop_cnt[TX_SW_ENQUEUE]);
		DP_PRINT_STATS("Tx Descriptor Error Count: %llu",
			       txstats->swdrop_cnt[TX_DESC_ERR]);
		DP_PRINT_STATS("Tx HAL Ring Error Count: %llu",
			       txstats->swdrop_cnt[TX_HAL_RING_ACCESS_ERR]);
		DP_PRINT_STATS("Tx Dma Map Error Count: %llu",
			       txstats->swdrop_cnt[TX_DMA_MAP_ERR]);
		DP_PRINT_STATS("Rx Delievered Count: %llu",
			       rxstats->delivered_to_stack);
		DP_PRINT_STATS("Rx Software Enqueue Drop Count: %llu",
			       rxstats->fail_cnt[ENQUEUE_DROP]);
		DP_PRINT_STATS("Rx Intrabss Drop Count: %llu",
			       rxstats->fail_cnt[INTRABSS_DROP]);
		DP_PRINT_STATS("Rx Msdu Done Failure Count: %llu",
			       rxstats->fail_cnt[MSDU_DONE_FAILURE]);
		DP_PRINT_STATS("Rx Invalid Peer Count: %llu",
			       rxstats->fail_cnt[INVALID_PEER_VDEV]);
		DP_PRINT_STATS("Rx Policy Check Drop Count: %llu",
			       rxstats->fail_cnt[POLICY_CHECK_DROP]);
		DP_PRINT_STATS("Rx Mec Drop Count: %llu",
			       rxstats->fail_cnt[MEC_DROP]);
		DP_PRINT_STATS("Rx Nawds Mcast Drop Count: %llu",
			       rxstats->fail_cnt[NAWDS_MCAST_DROP]);
		DP_PRINT_STATS("Rx Mesh Filter Drop Count: %llu",
			       rxstats->fail_cnt[MESH_FILTER_DROP]);
		DP_PRINT_STATS("Rx Intra Bss Deliver Count: %llu",
			       rxstats->intrabss_cnt);
		DP_PRINT_STATS("Rx MSDU Count: %llu", rxstats->msdu_cnt);
		DP_PRINT_STATS("Rx Multicast MSDU Count: %llu",
			       rxstats->mcast_msdu_cnt);
		DP_PRINT_STATS("Rx Broadcast MSDU Count: %llu\n",
			       rxstats->bcast_msdu_cnt);
	}
}

void dp_pdev_print_delay_stats(struct dp_pdev *pdev)
{
	struct dp_soc *soc = pdev->soc;
	struct cdp_tid_tx_stats *txstats = NULL;
	struct cdp_tid_rx_stats *rxstats;
	uint8_t tid, index;
	uint64_t count = 0;

	if (!soc)
		return;

	tid = 0;
	index = 0;
	rxstats = NULL;

	DP_PRINT_STATS("Per TID Delay Non-Zero Stats:\n");
	for (tid = 0; tid < CDP_MAX_DATA_TIDS; tid++) {
		DP_PRINT_STATS("----TID: %d----", tid);
		txstats = &pdev->stats.tid_stats.tid_tx_stats[tid];
		rxstats = &pdev->stats.tid_stats.tid_rx_stats[tid];

		DP_PRINT_STATS("Software Enqueue Delay:");
		for (index = 0; index < CDP_DELAY_BUCKET_MAX; index++) {
			count = txstats->swq_delay.delay_bucket[index];
			if (count) {
				DP_PRINT_STATS("%s:  Packets = %llu",
					       dp_vow_str_sw_enq_delay(index),
					       count);
			}
		}

		DP_PRINT_STATS("Min = %u", txstats->swq_delay.min_delay);
		DP_PRINT_STATS("Max = %u", txstats->swq_delay.max_delay);
		DP_PRINT_STATS("Avg = %u\n", txstats->swq_delay.avg_delay);

		DP_PRINT_STATS("Hardware Transmission Delay:");
		for (index = 0; index < CDP_DELAY_BUCKET_MAX; index++) {
			count = txstats->hwtx_delay.delay_bucket[index];
			if (count) {
				DP_PRINT_STATS("%s:  Packets = %llu",
					       dp_vow_str_fw_to_hw_delay(index),
					       count);
			}
		}
		DP_PRINT_STATS("Min = %u", txstats->hwtx_delay.min_delay);
		DP_PRINT_STATS("Max = %u", txstats->hwtx_delay.max_delay);
		DP_PRINT_STATS("Avg = %u\n", txstats->hwtx_delay.avg_delay);

		DP_PRINT_STATS("Tx Interframe Delay:");
		for (index = 0; index < CDP_DELAY_BUCKET_MAX; index++) {
			count = txstats->intfrm_delay.delay_bucket[index];
			if (count) {
				DP_PRINT_STATS("%s:  Packets = %llu",
					       dp_vow_str_intfrm_delay(index),
					       count);
			}
		}
		DP_PRINT_STATS("Min = %u", txstats->intfrm_delay.min_delay);
		DP_PRINT_STATS("Max = %u", txstats->intfrm_delay.max_delay);
		DP_PRINT_STATS("Avg = %u\n", txstats->intfrm_delay.avg_delay);

		DP_PRINT_STATS("Rx Interframe Delay:");
		for (index = 0; index < CDP_DELAY_BUCKET_MAX; index++) {
			count = rxstats->intfrm_delay.delay_bucket[index];
			if (count) {
				DP_PRINT_STATS("%s:  Packets = %llu",
					       dp_vow_str_intfrm_delay(index),
					       count);
			}
		}
		DP_PRINT_STATS("Min = %u", rxstats->intfrm_delay.min_delay);
		DP_PRINT_STATS("Max = %u", rxstats->intfrm_delay.max_delay);
		DP_PRINT_STATS("Avg = %u\n", rxstats->intfrm_delay.avg_delay);

		DP_PRINT_STATS("Rx Reap to Stack Delay:");
		for (index = 0; index < CDP_DELAY_BUCKET_MAX; index++) {
			count = rxstats->to_stack_delay.delay_bucket[index];
			if (count) {
				DP_PRINT_STATS("%s:  Packets = %llu",
					       dp_vow_str_intfrm_delay(index),
					       count);
			}
		}

		DP_PRINT_STATS("Min = %u", rxstats->to_stack_delay.min_delay);
		DP_PRINT_STATS("Max = %u", rxstats->to_stack_delay.max_delay);
		DP_PRINT_STATS("Avg = %u\n", rxstats->to_stack_delay.avg_delay);
	}
}
#endif
