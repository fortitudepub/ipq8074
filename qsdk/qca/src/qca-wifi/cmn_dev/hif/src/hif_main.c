/*
 * Copyright (c) 2015-2019 The Linux Foundation. All rights reserved.
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

#include "targcfg.h"
#include "qdf_lock.h"
#include "qdf_status.h"
#include "qdf_status.h"
#include <qdf_atomic.h>         /* qdf_atomic_read */
#include <targaddrs.h>
#include "hif_io32.h"
#include <hif.h>
#include <target_type.h>
#include "regtable.h"
#define ATH_MODULE_NAME hif
#include <a_debug.h>
#include "hif_main.h"
#include "hif_hw_version.h"
#if defined(HIF_PCI) || defined(HIF_SNOC) || defined(HIF_AHB)
#include "ce_tasklet.h"
#include "ce_api.h"
#endif
#include "qdf_trace.h"
#include "qdf_status.h"
#include "hif_debug.h"
#include "mp_dev.h"
#ifdef QCA_WIFI_QCA8074
#include "hal_api.h"
#endif
#include "hif_napi.h"
#include "hif_unit_test_suspend_i.h"
#include "qdf_module.h"

void hif_dump(struct hif_opaque_softc *hif_ctx, uint8_t cmd_id, bool start)
{
	hif_trigger_dump(hif_ctx, cmd_id, start);
}

/**
 * hif_get_target_id(): hif_get_target_id
 *
 * Return the virtual memory base address to the caller
 *
 * @scn: hif_softc
 *
 * Return: A_target_id_t
 */
A_target_id_t hif_get_target_id(struct hif_softc *scn)
{
	return scn->mem;
}

/**
 * hif_get_targetdef(): hif_get_targetdef
 * @scn: scn
 *
 * Return: void *
 */
void *hif_get_targetdef(struct hif_opaque_softc *hif_ctx)
{
	struct hif_softc *scn = HIF_GET_SOFTC(hif_ctx);

	return scn->targetdef;
}

/**
 * hif_vote_link_down(): unvote for link up
 *
 * Call hif_vote_link_down to release a previous request made using
 * hif_vote_link_up. A hif_vote_link_down call should only be made
 * after a corresponding hif_vote_link_up, otherwise you could be
 * negating a vote from another source. When no votes are present
 * hif will not guarantee the linkstate after hif_bus_suspend.
 *
 * SYNCHRONIZE WITH hif_vote_link_up by only calling in MC thread
 * and initialization deinitialization sequencences.
 *
 * Return: n/a
 */
void hif_vote_link_down(struct hif_opaque_softc *hif_ctx)
{
	struct hif_softc *scn = HIF_GET_SOFTC(hif_ctx);

	QDF_BUG(scn);
	scn->linkstate_vote--;
	if (scn->linkstate_vote == 0)
		hif_bus_prevent_linkdown(scn, false);
}

/**
 * hif_vote_link_up(): vote to prevent bus from suspending
 *
 * Makes hif guarantee that fw can message the host normally
 * durring suspend.
 *
 * SYNCHRONIZE WITH hif_vote_link_up by only calling in MC thread
 * and initialization deinitialization sequencences.
 *
 * Return: n/a
 */
void hif_vote_link_up(struct hif_opaque_softc *hif_ctx)
{
	struct hif_softc *scn = HIF_GET_SOFTC(hif_ctx);

	QDF_BUG(scn);
	scn->linkstate_vote++;
	if (scn->linkstate_vote == 1)
		hif_bus_prevent_linkdown(scn, true);
}

/**
 * hif_can_suspend_link(): query if hif is permitted to suspend the link
 *
 * Hif will ensure that the link won't be suspended if the upperlayers
 * don't want it to.
 *
 * SYNCHRONIZATION: MC thread is stopped before bus suspend thus
 * we don't need extra locking to ensure votes dont change while
 * we are in the process of suspending or resuming.
 *
 * Return: false if hif will guarantee link up durring suspend.
 */
bool hif_can_suspend_link(struct hif_opaque_softc *hif_ctx)
{
	struct hif_softc *scn = HIF_GET_SOFTC(hif_ctx);

	QDF_BUG(scn);
	return scn->linkstate_vote == 0;
}

/**
 * hif_hia_item_address(): hif_hia_item_address
 * @target_type: target_type
 * @item_offset: item_offset
 *
 * Return: n/a
 */
uint32_t hif_hia_item_address(uint32_t target_type, uint32_t item_offset)
{
	switch (target_type) {
	case TARGET_TYPE_AR6002:
		return AR6002_HOST_INTEREST_ADDRESS + item_offset;
	case TARGET_TYPE_AR6003:
		return AR6003_HOST_INTEREST_ADDRESS + item_offset;
	case TARGET_TYPE_AR6004:
		return AR6004_HOST_INTEREST_ADDRESS + item_offset;
	case TARGET_TYPE_AR6006:
		return AR6006_HOST_INTEREST_ADDRESS + item_offset;
	case TARGET_TYPE_AR9888:
		return AR9888_HOST_INTEREST_ADDRESS + item_offset;
	case TARGET_TYPE_AR6320:
	case TARGET_TYPE_AR6320V2:
		return AR6320_HOST_INTEREST_ADDRESS + item_offset;
	case TARGET_TYPE_ADRASTEA:
		/* ADRASTEA doesn't have a host interest address */
		ASSERT(0);
		return 0;
	case TARGET_TYPE_AR900B:
		return AR900B_HOST_INTEREST_ADDRESS + item_offset;
	case TARGET_TYPE_QCA9984:
		return QCA9984_HOST_INTEREST_ADDRESS + item_offset;
	case TARGET_TYPE_QCA9888:
		return QCA9888_HOST_INTEREST_ADDRESS + item_offset;
	case TARGET_TYPE_IPQ4019:
		return IPQ4019_HOST_INTEREST_ADDRESS + item_offset;

	default:
		ASSERT(0);
		return 0;
	}
}

/**
 * hif_max_num_receives_reached() - check max receive is reached
 * @scn: HIF Context
 * @count: unsigned int.
 *
 * Output check status as bool
 *
 * Return: bool
 */
bool hif_max_num_receives_reached(struct hif_softc *scn, unsigned int count)
{
	if (QDF_IS_EPPING_ENABLED(hif_get_conparam(scn)))
		return count > 120;
	else
		return count > MAX_NUM_OF_RECEIVES;
}

/**
 * init_buffer_count() - initial buffer count
 * @maxSize: qdf_size_t
 *
 * routine to modify the initial buffer count to be allocated on an os
 * platform basis. Platform owner will need to modify this as needed
 *
 * Return: qdf_size_t
 */
qdf_size_t init_buffer_count(qdf_size_t maxSize)
{
	return maxSize;
}

/**
 * hif_save_htc_htt_config_endpoint() - save htt_tx_endpoint
 * @hif_ctx: hif context
 * @htc_htt_tx_endpoint: htt_tx_endpoint
 *
 * Return: void
 */
void hif_save_htc_htt_config_endpoint(struct hif_opaque_softc *hif_ctx,
							int htc_htt_tx_endpoint)
{
	struct hif_softc *scn = HIF_GET_SOFTC(hif_ctx);

	if (!scn) {
		HIF_ERROR("%s: error: scn or scn->hif_sc is NULL!",
		       __func__);
		return;
	}

	scn->htc_htt_tx_endpoint = htc_htt_tx_endpoint;
}
qdf_export_symbol(hif_save_htc_htt_config_endpoint);

static const struct qwlan_hw qwlan_hw_list[] = {
	{
		.id = AR6320_REV1_VERSION,
		.subid = 0,
		.name = "QCA6174_REV1",
	},
	{
		.id = AR6320_REV1_1_VERSION,
		.subid = 0x1,
		.name = "QCA6174_REV1_1",
	},
	{
		.id = AR6320_REV1_3_VERSION,
		.subid = 0x2,
		.name = "QCA6174_REV1_3",
	},
	{
		.id = AR6320_REV2_1_VERSION,
		.subid = 0x4,
		.name = "QCA6174_REV2_1",
	},
	{
		.id = AR6320_REV2_1_VERSION,
		.subid = 0x5,
		.name = "QCA6174_REV2_2",
	},
	{
		.id = AR6320_REV3_VERSION,
		.subid = 0x6,
		.name = "QCA6174_REV2.3",
	},
	{
		.id = AR6320_REV3_VERSION,
		.subid = 0x8,
		.name = "QCA6174_REV3",
	},
	{
		.id = AR6320_REV3_VERSION,
		.subid = 0x9,
		.name = "QCA6174_REV3_1",
	},
	{
		.id = AR6320_REV3_2_VERSION,
		.subid = 0xA,
		.name = "AR6320_REV3_2_VERSION",
	},
	{
		.id = WCN3990_v1,
		.subid = 0x0,
		.name = "WCN3990_V1",
	},
	{
		.id = WCN3990_v2,
		.subid = 0x0,
		.name = "WCN3990_V2",
	},
	{
		.id = WCN3990_v2_1,
		.subid = 0x0,
		.name = "WCN3990_V2.1",
	},
	{
		.id = WCN3998,
		.subid = 0x0,
		.name = "WCN3998",
	},
	{
		.id = QCA9379_REV1_VERSION,
		.subid = 0xC,
		.name = "QCA9379_REV1",
	},
	{
		.id = QCA9379_REV1_VERSION,
		.subid = 0xD,
		.name = "QCA9379_REV1_1",
	}
};

/**
 * hif_get_hw_name(): get a human readable name for the hardware
 * @info: Target Info
 *
 * Return: human readable name for the underlying wifi hardware.
 */
static const char *hif_get_hw_name(struct hif_target_info *info)
{
	int i;

	if (info->hw_name)
		return info->hw_name;

	for (i = 0; i < ARRAY_SIZE(qwlan_hw_list); i++) {
		if (info->target_version == qwlan_hw_list[i].id &&
		    info->target_revision == qwlan_hw_list[i].subid) {
			return qwlan_hw_list[i].name;
		}
	}

	info->hw_name = qdf_mem_malloc(64);
	if (!info->hw_name)
		return "Unknown Device (nomem)";

	i = qdf_snprint(info->hw_name, 64, "HW_VERSION=%x.",
			info->target_version);
	if (i < 0)
		return "Unknown Device (snprintf failure)";
	else
		return info->hw_name;
}

/**
 * hif_get_hw_info(): hif_get_hw_info
 * @scn: scn
 * @version: version
 * @revision: revision
 *
 * Return: n/a
 */
void hif_get_hw_info(struct hif_opaque_softc *scn, u32 *version, u32 *revision,
			const char **target_name)
{
	struct hif_target_info *info = hif_get_target_info_handle(scn);
	struct hif_softc *sc = HIF_GET_SOFTC(scn);

	if (sc->bus_type == QDF_BUS_TYPE_USB)
		hif_usb_get_hw_info(sc);

	*version = info->target_version;
	*revision = info->target_revision;
	*target_name = hif_get_hw_name(info);
}

/**
 * hif_get_dev_ba(): API to get device base address.
 * @scn: scn
 * @version: version
 * @revision: revision
 *
 * Return: n/a
 */
void *hif_get_dev_ba(struct hif_opaque_softc *hif_handle)
{
	struct hif_softc *scn = (struct hif_softc *)hif_handle;

	return scn->mem;
}
qdf_export_symbol(hif_get_dev_ba);
/**
 * hif_open(): hif_open
 * @qdf_ctx: QDF Context
 * @mode: Driver Mode
 * @bus_type: Bus Type
 * @cbk: CDS Callbacks
 *
 * API to open HIF Context
 *
 * Return: HIF Opaque Pointer
 */
struct hif_opaque_softc *hif_open(qdf_device_t qdf_ctx, uint32_t mode,
				  enum qdf_bus_type bus_type,
				  struct hif_driver_state_callbacks *cbk)
{
	struct hif_softc *scn;
	QDF_STATUS status = QDF_STATUS_SUCCESS;
	int bus_context_size = hif_bus_get_context_size(bus_type);

	if (bus_context_size == 0) {
		HIF_ERROR("%s: context size 0 not allowed", __func__);
		return NULL;
	}

	scn = (struct hif_softc *)qdf_mem_malloc(bus_context_size);
	if (!scn) {
		HIF_ERROR("%s: cannot alloc memory for HIF context of size:%d",
						__func__, bus_context_size);
		return GET_HIF_OPAQUE_HDL(scn);
	}

	scn->qdf_dev = qdf_ctx;
	scn->hif_con_param = mode;
	qdf_atomic_init(&scn->active_tasklet_cnt);
	qdf_atomic_init(&scn->active_grp_tasklet_cnt);
	qdf_atomic_init(&scn->link_suspended);
	qdf_atomic_init(&scn->tasklet_from_intr);
	qdf_mem_copy(&scn->callbacks, cbk,
		     sizeof(struct hif_driver_state_callbacks));
	scn->bus_type  = bus_type;
	status = hif_bus_open(scn, bus_type);
	if (status != QDF_STATUS_SUCCESS) {
		HIF_ERROR("%s: hif_bus_open error = %d, bus_type = %d",
				  __func__, status, bus_type);
		qdf_mem_free(scn);
		scn = NULL;
	}

	return GET_HIF_OPAQUE_HDL(scn);
}

#ifdef ADRASTEA_RRI_ON_DDR
/**
 * hif_uninit_rri_on_ddr(): free consistent memory allocated for rri
 * @scn: hif context
 *
 * Return: none
 */
void hif_uninit_rri_on_ddr(struct hif_softc *scn)
{
	if (scn->vaddr_rri_on_ddr)
		qdf_mem_free_consistent(scn->qdf_dev, scn->qdf_dev->dev,
					(CE_COUNT * sizeof(uint32_t)),
					scn->vaddr_rri_on_ddr,
					scn->paddr_rri_on_ddr, 0);
	scn->vaddr_rri_on_ddr = NULL;
}
#endif

/**
 * hif_close(): hif_close
 * @hif_ctx: hif_ctx
 *
 * Return: n/a
 */
void hif_close(struct hif_opaque_softc *hif_ctx)
{
	struct hif_softc *scn = HIF_GET_SOFTC(hif_ctx);

	if (!scn) {
		HIF_ERROR("%s: hif_opaque_softc is NULL", __func__);
		return;
	}

	if (scn->athdiag_procfs_inited) {
		athdiag_procfs_remove();
		scn->athdiag_procfs_inited = false;
	}

	if (scn->target_info.hw_name) {
		char *hw_name = scn->target_info.hw_name;

		scn->target_info.hw_name = "ErrUnloading";
		qdf_mem_free(hw_name);
	}

	hif_uninit_rri_on_ddr(scn);

	hif_bus_close(scn);
	qdf_mem_free(scn);
}

#if defined(QCA_WIFI_QCA8074) || \
	defined(QCA_WIFI_QCA6290) || defined(QCA_WIFI_QCA6390)
static QDF_STATUS hif_hal_attach(struct hif_softc *scn)
{
	if (ce_srng_based(scn)) {
		scn->hal_soc = hal_attach(scn, scn->qdf_dev);
		if (!scn->hal_soc)
			return QDF_STATUS_E_FAILURE;
	}

	return QDF_STATUS_SUCCESS;
}

static QDF_STATUS hif_hal_detach(struct hif_softc *scn)
{
	if (ce_srng_based(scn)) {
		hal_detach(scn->hal_soc);
		scn->hal_soc = NULL;
	}

	return QDF_STATUS_SUCCESS;
}
#else
static QDF_STATUS hif_hal_attach(struct hif_softc *scn)
{
	return QDF_STATUS_SUCCESS;
}

static QDF_STATUS hif_hal_detach(struct hif_softc *scn)
{
	return QDF_STATUS_SUCCESS;
}
#endif

/**
 * hif_enable(): hif_enable
 * @hif_ctx: hif_ctx
 * @dev: dev
 * @bdev: bus dev
 * @bid: bus ID
 * @bus_type: bus type
 * @type: enable type
 *
 * Return: QDF_STATUS
 */
QDF_STATUS hif_enable(struct hif_opaque_softc *hif_ctx, struct device *dev,
					  void *bdev,
					  const struct hif_bus_id *bid,
					  enum qdf_bus_type bus_type,
					  enum hif_enable_type type)
{
	QDF_STATUS status;
	struct hif_softc *scn = HIF_GET_SOFTC(hif_ctx);

	if (!scn) {
		HIF_ERROR("%s: hif_ctx = NULL", __func__);
		return QDF_STATUS_E_NULL_VALUE;
	}

	status = hif_enable_bus(scn, dev, bdev, bid, type);
	if (status != QDF_STATUS_SUCCESS) {
		HIF_ERROR("%s: hif_enable_bus error = %d",
				  __func__, status);
		return status;
	}

	status = hif_hal_attach(scn);
	if (status != QDF_STATUS_SUCCESS) {
		HIF_ERROR("%s: hal attach failed", __func__);
		goto disable_bus;
	}

	if (hif_bus_configure(scn)) {
		HIF_ERROR("%s: Target probe failed.", __func__);
		status = QDF_STATUS_E_FAILURE;
		goto hal_detach;
	}

	hif_ut_suspend_init(scn);

	/*
	 * Flag to avoid potential unallocated memory access from MSI
	 * interrupt handler which could get scheduled as soon as MSI
	 * is enabled, i.e to take care of the race due to the order
	 * in where MSI is enabled before the memory, that will be
	 * in interrupt handlers, is allocated.
	 */

	scn->hif_init_done = true;

	HIF_DBG("%s: OK", __func__);

	return QDF_STATUS_SUCCESS;

hal_detach:
	hif_hal_detach(scn);
disable_bus:
	hif_disable_bus(scn);
	return status;
}

void hif_disable(struct hif_opaque_softc *hif_ctx, enum hif_disable_type type)
{
	struct hif_softc *scn = HIF_GET_SOFTC(hif_ctx);

	if (!scn)
		return;

	hif_nointrs(scn);
	if (scn->hif_init_done == false)
		hif_shutdown_device(hif_ctx);
	else
		hif_stop(hif_ctx);

	hif_hal_detach(scn);

	hif_disable_bus(scn);

	hif_wlan_disable(scn);

	scn->notice_send = false;

	HIF_DBG("%s: X", __func__);
}

void hif_display_stats(struct hif_opaque_softc *hif_ctx)
{
	hif_display_bus_stats(hif_ctx);
}

void hif_clear_stats(struct hif_opaque_softc *hif_ctx)
{
	hif_clear_bus_stats(hif_ctx);
}

/**
 * hif_crash_shutdown_dump_bus_register() - dump bus registers
 * @hif_ctx: hif_ctx
 *
 * Return: n/a
 */
#if defined(TARGET_RAMDUMP_AFTER_KERNEL_PANIC) \
&& defined(DEBUG)

static void hif_crash_shutdown_dump_bus_register(void *hif_ctx)
{
	struct hif_opaque_softc *scn = hif_ctx;

	if (hif_check_soc_status(scn))
		return;

	if (hif_dump_registers(scn))
		HIF_ERROR("Failed to dump bus registers!");
}

/**
 * hif_crash_shutdown(): hif_crash_shutdown
 *
 * This function is called by the platform driver to dump CE registers
 *
 * @hif_ctx: hif_ctx
 *
 * Return: n/a
 */
void hif_crash_shutdown(struct hif_opaque_softc *hif_ctx)
{
	struct hif_softc *scn = HIF_GET_SOFTC(hif_ctx);

	if (!hif_ctx)
		return;

	if (scn->bus_type == QDF_BUS_TYPE_SNOC) {
		HIF_INFO_MED("%s: RAM dump disabled for bustype %d",
				__func__, scn->bus_type);
		return;
	}

	if (TARGET_STATUS_RESET == scn->target_status) {
		HIF_INFO_MED("%s: Target is already asserted, ignore!",
			    __func__);
		return;
	}

	if (hif_is_load_or_unload_in_progress(scn)) {
		HIF_ERROR("%s: Load/unload is in progress, ignore!", __func__);
		return;
	}

	hif_crash_shutdown_dump_bus_register(hif_ctx);

	if (ol_copy_ramdump(hif_ctx))
		goto out;

	HIF_INFO_MED("%s: RAM dump collecting completed!", __func__);

out:
	return;
}
#else
void hif_crash_shutdown(struct hif_opaque_softc *hif_ctx)
{
	HIF_INFO_MED("%s: Collecting target RAM dump disabled",
		__func__);
}
#endif /* TARGET_RAMDUMP_AFTER_KERNEL_PANIC */

#ifdef QCA_WIFI_3_0
/**
 * hif_check_fw_reg(): hif_check_fw_reg
 * @scn: scn
 * @state:
 *
 * Return: int
 */
int hif_check_fw_reg(struct hif_opaque_softc *scn)
{
	return 0;
}
#endif

/**
 * hif_read_phy_mem_base(): hif_read_phy_mem_base
 * @scn: scn
 * @phy_mem_base: physical mem base
 *
 * Return: n/a
 */
void hif_read_phy_mem_base(struct hif_softc *scn, qdf_dma_addr_t *phy_mem_base)
{
	*phy_mem_base = scn->mem_pa;
}
qdf_export_symbol(hif_read_phy_mem_base);

/**
 * hif_get_device_type(): hif_get_device_type
 * @device_id: device_id
 * @revision_id: revision_id
 * @hif_type: returned hif_type
 * @target_type: returned target_type
 *
 * Return: int
 */
int hif_get_device_type(uint32_t device_id,
			uint32_t revision_id,
			uint32_t *hif_type, uint32_t *target_type)
{
	int ret = 0;

	switch (device_id) {
	case ADRASTEA_DEVICE_ID_P2_E12:

		*hif_type = HIF_TYPE_ADRASTEA;
		*target_type = TARGET_TYPE_ADRASTEA;
		break;

	case AR9888_DEVICE_ID:
		*hif_type = HIF_TYPE_AR9888;
		*target_type = TARGET_TYPE_AR9888;
		break;

	case AR6320_DEVICE_ID:
		switch (revision_id) {
		case AR6320_FW_1_1:
		case AR6320_FW_1_3:
			*hif_type = HIF_TYPE_AR6320;
			*target_type = TARGET_TYPE_AR6320;
			break;

		case AR6320_FW_2_0:
		case AR6320_FW_3_0:
		case AR6320_FW_3_2:
			*hif_type = HIF_TYPE_AR6320V2;
			*target_type = TARGET_TYPE_AR6320V2;
			break;

		default:
			HIF_ERROR("%s: error - dev_id = 0x%x, rev_id = 0x%x",
				   __func__, device_id, revision_id);
			ret = -ENODEV;
			goto end;
		}
		break;

	case AR9887_DEVICE_ID:
		*hif_type = HIF_TYPE_AR9888;
		*target_type = TARGET_TYPE_AR9888;
		HIF_INFO(" *********** AR9887 **************");
		break;

	case QCA9984_DEVICE_ID:
		*hif_type = HIF_TYPE_QCA9984;
		*target_type = TARGET_TYPE_QCA9984;
		HIF_INFO(" *********** QCA9984 *************");
		break;

	case QCA9888_DEVICE_ID:
		*hif_type = HIF_TYPE_QCA9888;
		*target_type = TARGET_TYPE_QCA9888;
		HIF_INFO(" *********** QCA9888 *************");
		break;

	case AR900B_DEVICE_ID:
		*hif_type = HIF_TYPE_AR900B;
		*target_type = TARGET_TYPE_AR900B;
		HIF_INFO(" *********** AR900B *************");
		break;

	case IPQ4019_DEVICE_ID:
		*hif_type = HIF_TYPE_IPQ4019;
		*target_type = TARGET_TYPE_IPQ4019;
		HIF_INFO(" *********** IPQ4019  *************");
		break;

	case QCA8074_DEVICE_ID:
		*hif_type = HIF_TYPE_QCA8074;
		*target_type = TARGET_TYPE_QCA8074;
		HIF_INFO(" *********** QCA8074  *************\n");
		break;

	case QCA6290_EMULATION_DEVICE_ID:
	case QCA6290_DEVICE_ID:
		*hif_type = HIF_TYPE_QCA6290;
		*target_type = TARGET_TYPE_QCA6290;
		HIF_INFO(" *********** QCA6290EMU *************\n");
		break;

	case QCN7605_DEVICE_ID:
	case QCN7605_COMPOSITE:
	case QCN7605_STANDALONE:
		*hif_type = HIF_TYPE_QCN7605;
		*target_type = TARGET_TYPE_QCN7605;
		HIF_INFO(" *********** QCN7605 *************\n");
		break;

	case QCA6390_DEVICE_ID:
	case QCA6390_EMULATION_DEVICE_ID:
		*hif_type = HIF_TYPE_QCA6390;
		*target_type = TARGET_TYPE_QCA6390;
		HIF_INFO(" *********** QCA6390 *************\n");
		break;

	case QCA8074V2_DEVICE_ID:
		*hif_type = HIF_TYPE_QCA8074V2;
		*target_type = TARGET_TYPE_QCA8074V2;
		HIF_INFO(" *********** QCA8074V2 *************\n");
		break;

	case QCA6018_DEVICE_ID:
	case RUMIM2M_DEVICE_ID_NODE0:
	case RUMIM2M_DEVICE_ID_NODE1:
	case RUMIM2M_DEVICE_ID_NODE2:
	case RUMIM2M_DEVICE_ID_NODE3:
	case RUMIM2M_DEVICE_ID_NODE4:
	case RUMIM2M_DEVICE_ID_NODE5:
		*hif_type = HIF_TYPE_QCA6018;
		*target_type = TARGET_TYPE_QCA6018;
		HIF_INFO(" *********** QCA6018 *************\n");
		break;

	default:
		HIF_ERROR("%s: Unsupported device ID = 0x%x!",
			  __func__, device_id);
		ret = -ENODEV;
		break;
	}

	if (*target_type == TARGET_TYPE_UNKNOWN) {
		HIF_ERROR("%s: Unsupported target_type!", __func__);
		ret = -ENODEV;
	}
end:
	return ret;
}

/**
 * hif_get_bus_type() - return the bus type
 *
 * Return: enum qdf_bus_type
 */
enum qdf_bus_type hif_get_bus_type(struct hif_opaque_softc *hif_hdl)
{
	struct hif_softc *scn = HIF_GET_SOFTC(hif_hdl);

	return scn->bus_type;
}

/**
 * Target info and ini parameters are global to the driver
 * Hence these structures are exposed to all the modules in
 * the driver and they don't need to maintains multiple copies
 * of the same info, instead get the handle from hif and
 * modify them in hif
 */

/**
 * hif_get_ini_handle() - API to get hif_config_param handle
 * @hif_ctx: HIF Context
 *
 * Return: pointer to hif_config_info
 */
struct hif_config_info *hif_get_ini_handle(struct hif_opaque_softc *hif_ctx)
{
	struct hif_softc *sc = HIF_GET_SOFTC(hif_ctx);

	return &sc->hif_config;
}

/**
 * hif_get_target_info_handle() - API to get hif_target_info handle
 * @hif_ctx: HIF context
 *
 * Return: Pointer to hif_target_info
 */
struct hif_target_info *hif_get_target_info_handle(
					struct hif_opaque_softc *hif_ctx)
{
	struct hif_softc *sc = HIF_GET_SOFTC(hif_ctx);

	return &sc->target_info;

}
qdf_export_symbol(hif_get_target_info_handle);

#ifdef RECEIVE_OFFLOAD
void hif_offld_flush_cb_register(struct hif_opaque_softc *scn,
				 void (offld_flush_handler)(void *))
{
	if (hif_napi_enabled(scn, -1))
		hif_napi_rx_offld_flush_cb_register(scn, offld_flush_handler);
	else
		HIF_ERROR("NAPI not enabled\n");
}
qdf_export_symbol(hif_offld_flush_cb_register);

void hif_offld_flush_cb_deregister(struct hif_opaque_softc *scn)
{
	if (hif_napi_enabled(scn, -1))
		hif_napi_rx_offld_flush_cb_deregister(scn);
	else
		HIF_ERROR("NAPI not enabled\n");
}
qdf_export_symbol(hif_offld_flush_cb_deregister);

int hif_get_rx_ctx_id(int ctx_id, struct hif_opaque_softc *hif_hdl)
{
	if (hif_napi_enabled(hif_hdl, -1))
		return NAPI_PIPE2ID(ctx_id);
	else
		return ctx_id;
}
#else /* RECEIVE_OFFLOAD */
int hif_get_rx_ctx_id(int ctx_id, struct hif_opaque_softc *hif_hdl)
{
	return 0;
}
qdf_export_symbol(hif_get_rx_ctx_id);
#endif /* RECEIVE_OFFLOAD */

#if defined(FEATURE_LRO)

/**
 * hif_get_lro_info - Returns LRO instance for instance ID
 * @ctx_id: LRO instance ID
 * @hif_hdl: HIF Context
 *
 * Return: Pointer to LRO instance.
 */
void *hif_get_lro_info(int ctx_id, struct hif_opaque_softc *hif_hdl)
{
	void *data;

	if (hif_napi_enabled(hif_hdl, -1))
		data = hif_napi_get_lro_info(hif_hdl, ctx_id);
	else
		data = hif_ce_get_lro_ctx(hif_hdl, ctx_id);

	return data;
}
#endif

/**
 * hif_get_target_status - API to get target status
 * @hif_ctx: HIF Context
 *
 * Return: enum hif_target_status
 */
enum hif_target_status hif_get_target_status(struct hif_opaque_softc *hif_ctx)
{
	struct hif_softc *scn = HIF_GET_SOFTC(hif_ctx);

	return scn->target_status;
}
qdf_export_symbol(hif_get_target_status);

/**
 * hif_set_target_status() - API to set target status
 * @hif_ctx: HIF Context
 * @status: Target Status
 *
 * Return: void
 */
void hif_set_target_status(struct hif_opaque_softc *hif_ctx, enum
			   hif_target_status status)
{
	struct hif_softc *scn = HIF_GET_SOFTC(hif_ctx);

	scn->target_status = status;
}

/**
 * hif_init_ini_config() - API to initialize HIF configuration parameters
 * @hif_ctx: HIF Context
 * @cfg: HIF Configuration
 *
 * Return: void
 */
void hif_init_ini_config(struct hif_opaque_softc *hif_ctx,
			 struct hif_config_info *cfg)
{
	struct hif_softc *scn = HIF_GET_SOFTC(hif_ctx);

	qdf_mem_copy(&scn->hif_config, cfg, sizeof(struct hif_config_info));
}

/**
 * hif_get_conparam() - API to get driver mode in HIF
 * @scn: HIF Context
 *
 * Return: driver mode of operation
 */
uint32_t hif_get_conparam(struct hif_softc *scn)
{
	if (!scn)
		return 0;

	return scn->hif_con_param;
}

/**
 * hif_get_callbacks_handle() - API to get callbacks Handle
 * @scn: HIF Context
 *
 * Return: pointer to HIF Callbacks
 */
struct hif_driver_state_callbacks *hif_get_callbacks_handle(
							struct hif_softc *scn)
{
	return &scn->callbacks;
}

/**
 * hif_is_driver_unloading() - API to query upper layers if driver is unloading
 * @scn: HIF Context
 *
 * Return: True/False
 */
bool hif_is_driver_unloading(struct hif_softc *scn)
{
	struct hif_driver_state_callbacks *cbk = hif_get_callbacks_handle(scn);

	if (cbk && cbk->is_driver_unloading)
		return cbk->is_driver_unloading(cbk->context);

	return false;
}

/**
 * hif_is_load_or_unload_in_progress() - API to query upper layers if
 * load/unload in progress
 * @scn: HIF Context
 *
 * Return: True/False
 */
bool hif_is_load_or_unload_in_progress(struct hif_softc *scn)
{
	struct hif_driver_state_callbacks *cbk = hif_get_callbacks_handle(scn);

	if (cbk && cbk->is_load_unload_in_progress)
		return cbk->is_load_unload_in_progress(cbk->context);

	return false;
}

/**
 * hif_is_recovery_in_progress() - API to query upper layers if recovery in
 * progress
 * @scn: HIF Context
 *
 * Return: True/False
 */
bool hif_is_recovery_in_progress(struct hif_softc *scn)
{
	struct hif_driver_state_callbacks *cbk = hif_get_callbacks_handle(scn);

	if (cbk && cbk->is_recovery_in_progress)
		return cbk->is_recovery_in_progress(cbk->context);

	return false;
}

#if defined(HIF_PCI) || defined(HIF_SNOC) || defined(HIF_AHB)

/**
 * hif_update_pipe_callback() - API to register pipe specific callbacks
 * @osc: Opaque softc
 * @pipeid: pipe id
 * @callbacks: callbacks to register
 *
 * Return: void
 */

void hif_update_pipe_callback(struct hif_opaque_softc *osc,
					u_int8_t pipeid,
					struct hif_msg_callbacks *callbacks)
{
	struct hif_softc *scn = HIF_GET_SOFTC(osc);
	struct HIF_CE_state *hif_state = HIF_GET_CE_STATE(scn);
	struct HIF_CE_pipe_info *pipe_info;

	QDF_BUG(pipeid < CE_COUNT_MAX);

	HIF_INFO_LO("+%s pipeid %d\n", __func__, pipeid);

	pipe_info = &hif_state->pipe_info[pipeid];

	qdf_mem_copy(&pipe_info->pipe_callbacks,
			callbacks, sizeof(pipe_info->pipe_callbacks));

	HIF_INFO_LO("-%s\n", __func__);
}
qdf_export_symbol(hif_update_pipe_callback);

/**
 * hif_is_target_ready() - API to query if target is in ready state
 * progress
 * @scn: HIF Context
 *
 * Return: True/False
 */
bool hif_is_target_ready(struct hif_softc *scn)
{
	struct hif_driver_state_callbacks *cbk = hif_get_callbacks_handle(scn);

	if (cbk && cbk->is_target_ready)
		return cbk->is_target_ready(cbk->context);
	/*
	 * if callback is not registered then there is no way to determine
	 * if target is ready. In-such case return true to indicate that
	 * target is ready.
	 */
	return true;
}
qdf_export_symbol(hif_is_target_ready);

/**
 * hif_batch_send() - API to access hif specific function
 * ce_batch_send.
 * @osc: HIF Context
 * @msdu : list of msdus to be sent
 * @transfer_id : transfer id
 * @len : donwloaded length
 *
 * Return: list of msds not sent
 */
qdf_nbuf_t hif_batch_send(struct hif_opaque_softc *osc, qdf_nbuf_t msdu,
		uint32_t transfer_id, u_int32_t len, uint32_t sendhead)
{
	void *ce_tx_hdl = hif_get_ce_handle(osc, CE_HTT_TX_CE);

	return ce_batch_send((struct CE_handle *)ce_tx_hdl, msdu, transfer_id,
			len, sendhead);
}
qdf_export_symbol(hif_batch_send);

/**
 * hif_update_tx_ring() - API to access hif specific function
 * ce_update_tx_ring.
 * @osc: HIF Context
 * @num_htt_cmpls : number of htt compl received.
 *
 * Return: void
 */
void hif_update_tx_ring(struct hif_opaque_softc *osc, u_int32_t num_htt_cmpls)
{
	void *ce_tx_hdl = hif_get_ce_handle(osc, CE_HTT_TX_CE);

	ce_update_tx_ring(ce_tx_hdl, num_htt_cmpls);
}
qdf_export_symbol(hif_update_tx_ring);


/**
 * hif_send_single() - API to access hif specific function
 * ce_send_single.
 * @osc: HIF Context
 * @msdu : msdu to be sent
 * @transfer_id: transfer id
 * @len : downloaded length
 *
 * Return: msdu sent status
 */
int hif_send_single(struct hif_opaque_softc *osc, qdf_nbuf_t msdu, uint32_t
		transfer_id, u_int32_t len)
{
	void *ce_tx_hdl = hif_get_ce_handle(osc, CE_HTT_TX_CE);

	return ce_send_single((struct CE_handle *)ce_tx_hdl, msdu, transfer_id,
			len);
}
qdf_export_symbol(hif_send_single);
#endif

/**
 * hif_reg_write() - API to access hif specific function
 * hif_write32_mb.
 * @hif_ctx : HIF Context
 * @offset : offset on which value has to be written
 * @value : value to be written
 *
 * Return: None
 */
void hif_reg_write(struct hif_opaque_softc *hif_ctx, uint32_t offset,
		uint32_t value)
{
	struct hif_softc *scn = HIF_GET_SOFTC(hif_ctx);

	hif_write32_mb(scn, scn->mem + offset, value);

}
qdf_export_symbol(hif_reg_write);

/**
 * hif_reg_read() - API to access hif specific function
 * hif_read32_mb.
 * @hif_ctx : HIF Context
 * @offset : offset from which value has to be read
 *
 * Return: Read value
 */
uint32_t hif_reg_read(struct hif_opaque_softc *hif_ctx, uint32_t offset)
{

	struct hif_softc *scn = HIF_GET_SOFTC(hif_ctx);

	return hif_read32_mb(scn, scn->mem + offset);
}
qdf_export_symbol(hif_reg_read);

/**
 * hif_ramdump_handler(): generic ramdump handler
 * @scn: struct hif_opaque_softc
 *
 * Return: None
 */
void hif_ramdump_handler(struct hif_opaque_softc *scn)
{
	if (hif_get_bus_type(scn) == QDF_BUS_TYPE_USB)
		hif_usb_ramdump_handler(scn);
}

#ifdef WLAN_SUSPEND_RESUME_TEST
irqreturn_t hif_wake_interrupt_handler(int irq, void *context)
{
	struct hif_softc *scn = context;

	HIF_INFO("wake interrupt received on irq %d", irq);

	if (scn->initial_wakeup_cb)
		scn->initial_wakeup_cb(scn->initial_wakeup_priv);

	if (hif_is_ut_suspended(scn))
		hif_ut_fw_resume(scn);

	return IRQ_HANDLED;
}
#else /* WLAN_SUSPEND_RESUME_TEST */
irqreturn_t hif_wake_interrupt_handler(int irq, void *context)
{
	struct hif_softc *scn = context;

	HIF_INFO("wake interrupt received on irq %d", irq);

	if (scn->initial_wakeup_cb)
		scn->initial_wakeup_cb(scn->initial_wakeup_priv);

	return IRQ_HANDLED;
}
#endif /* WLAN_SUSPEND_RESUME_TEST */

void hif_set_initial_wakeup_cb(struct hif_opaque_softc *hif_ctx,
			       void (*callback)(void *),
			       void *priv)
{
	struct hif_softc *scn = HIF_GET_SOFTC(hif_ctx);

	scn->initial_wakeup_cb = callback;
	scn->initial_wakeup_priv = priv;
}

void hif_set_ce_service_max_yield_time(struct hif_opaque_softc *hif,
				       uint32_t ce_service_max_yield_time)
{
	struct hif_softc *hif_ctx = HIF_GET_SOFTC(hif);

	hif_ctx->ce_service_max_yield_time =
		ce_service_max_yield_time * 1000;
}

unsigned long long
hif_get_ce_service_max_yield_time(struct hif_opaque_softc *hif)
{
	struct hif_softc *hif_ctx = HIF_GET_SOFTC(hif);

	return hif_ctx->ce_service_max_yield_time;
}

void hif_set_ce_service_max_rx_ind_flush(struct hif_opaque_softc *hif,
				       uint8_t ce_service_max_rx_ind_flush)
{
	struct hif_softc *hif_ctx = HIF_GET_SOFTC(hif);

	if (ce_service_max_rx_ind_flush == 0 ||
	    ce_service_max_rx_ind_flush > MSG_FLUSH_NUM)
		hif_ctx->ce_service_max_rx_ind_flush = MSG_FLUSH_NUM;
	else
		hif_ctx->ce_service_max_rx_ind_flush =
						ce_service_max_rx_ind_flush;
}
