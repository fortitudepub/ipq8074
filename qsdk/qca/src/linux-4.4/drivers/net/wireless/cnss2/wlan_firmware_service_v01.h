/* Copyright (c) 2015-2019, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef WLAN_FIRMWARE_SERVICE_V01_H
#define WLAN_FIRMWARE_SERVICE_V01_H

#include <linux/qmi_encdec.h>
#include <soc/qcom/msm_qmi_interface.h>

#define WLFW_SERVICE_ID_V01_NPR 0x45
#define WLFW_SERVICE_ID_V01_HK 0x45
#define WLFW_SERVICE_ID_V01 (plat_priv->service_id)
#define WLFW_SERVICE_VERS_V01 0x01

#define QMI_WLFW_BDF_DOWNLOAD_REQ_V01 0x0025
#define QMI_WLFW_FW_MEM_READY_IND_V01 0x0037
#define QMI_WLFW_QDSS_TRACE_CONFIG_DOWNLOAD_REQ_V01 0x0044
#define QMI_WLFW_INITIATE_CAL_UPDATE_IND_V01 0x002A
#define QMI_WLFW_CAL_DONE_IND_V01 0x003E
#define QMI_WLFW_HOST_CAP_REQ_V01 0x0034
#define QMI_WLFW_DYNAMIC_FEATURE_MASK_RESP_V01 0x003B
#define QMI_WLFW_M3_INFO_REQ_V01 0x003C
#define QMI_WLFW_CAP_REQ_V01 0x0024
#define QMI_WLFW_FW_INIT_DONE_IND_V01 0x0038
#define QMI_WLFW_CAL_REPORT_REQ_V01 0x0026
#define QMI_WLFW_M3_INFO_RESP_V01 0x003C
#define QMI_WLFW_CAL_UPDATE_RESP_V01 0x0029
#define QMI_WLFW_CAL_DOWNLOAD_RESP_V01 0x0027
#define QMI_WLFW_XO_CAL_IND_V01 0x003D
#define QMI_WLFW_INI_RESP_V01 0x002F
#define QMI_WLFW_CAL_REPORT_RESP_V01 0x0026
#define QMI_WLFW_QDSS_TRACE_MEM_INFO_REQ_V01 0x0040
#define QMI_WLFW_QDSS_TRACE_REQ_MEM_IND_V01 0x003F
#define QMI_WLFW_SHUTDOWN_RESP_V01 0x0043
#define QMI_WLFW_MAC_ADDR_RESP_V01 0x0033
#define QMI_WLFW_INITIATE_CAL_DOWNLOAD_IND_V01 0x0028
#define QMI_WLFW_HOST_CAP_RESP_V01 0x0034
#define QMI_WLFW_MSA_READY_IND_V01 0x002B
#define QMI_WLFW_ATHDIAG_WRITE_RESP_V01 0x0031
#define QMI_WLFW_WLAN_MODE_REQ_V01 0x0022
#define QMI_WLFW_IND_REGISTER_REQ_V01 0x0020
#define QMI_WLFW_WLAN_CFG_RESP_V01 0x0023
#define QMI_WLFW_QDSS_TRACE_MODE_REQ_V01 0x0045
#define QMI_WLFW_REQUEST_MEM_IND_V01 0x0035
#define QMI_WLFW_QDSS_TRACE_CONFIG_DOWNLOAD_RESP_V01 0x0044
#define QMI_WLFW_REJUVENATE_IND_V01 0x0039
#define QMI_WLFW_DYNAMIC_FEATURE_MASK_REQ_V01 0x003B
#define QMI_WLFW_ATHDIAG_WRITE_REQ_V01 0x0031
#define QMI_WLFW_WLAN_MODE_RESP_V01 0x0022
#define QMI_WLFW_RESPOND_MEM_REQ_V01 0x0036
#define QMI_WLFW_QDSS_TRACE_MODE_RESP_V01 0x0045
#define QMI_WLFW_PIN_CONNECT_RESULT_IND_V01 0x002C
#define QMI_WLFW_FW_READY_IND_V01 0x0021
#define QMI_WLFW_QDSS_TRACE_SAVE_IND_V01 0x0041
#define QMI_WLFW_QDSS_TRACE_MEM_INFO_RESP_V01 0x0040
#define QMI_WLFW_MSA_READY_RESP_V01 0x002E
#define QMI_WLFW_QDSS_TRACE_DATA_REQ_V01 0x0042
#define QMI_WLFW_CAL_UPDATE_REQ_V01 0x0029
#define QMI_WLFW_INI_REQ_V01 0x002F
#define QMI_WLFW_BDF_DOWNLOAD_RESP_V01 0x0025
#define QMI_WLFW_REJUVENATE_ACK_RESP_V01 0x003A
#define QMI_WLFW_MSA_INFO_RESP_V01 0x002D
#define QMI_WLFW_MSA_READY_REQ_V01 0x002E
#define QMI_WLFW_QDSS_TRACE_DATA_RESP_V01 0x0042
#define QMI_WLFW_CAP_RESP_V01 0x0024
#define QMI_WLFW_REJUVENATE_ACK_REQ_V01 0x003A
#define QMI_WLFW_ATHDIAG_READ_RESP_V01 0x0030
#define QMI_WLFW_SHUTDOWN_REQ_V01 0x0043
#define QMI_WLFW_VBATT_REQ_V01 0x0032
#define QMI_WLFW_MAC_ADDR_REQ_V01 0x0033
#define QMI_WLFW_RESPOND_MEM_RESP_V01 0x0036
#define QMI_WLFW_VBATT_RESP_V01 0x0032
#define QMI_WLFW_MSA_INFO_REQ_V01 0x002D
#define QMI_WLFW_QDSS_TRACE_FREE_IND_V01 0x0046
#define QMI_WLFW_CAL_DOWNLOAD_REQ_V01 0x0027
#define QMI_WLFW_ATHDIAG_READ_REQ_V01 0x0030
#define QMI_WLFW_WLAN_CFG_REQ_V01 0x0023
#define QMI_WLFW_IND_REGISTER_RESP_V01 0x0020

#define QMI_WLFW_MAX_NUM_MEMORY_REGIONS_V01 2
#define QMI_WLFW_MAX_NUM_MEM_SEG_V01 32
#define QMI_WLFW_MAX_NUM_CAL_V01 5
#define QMI_WLFW_MAX_DATA_SIZE_V01 6144
#define QMI_WLFW_FUNCTION_NAME_LEN_V01 128
#define QMI_WLFW_MAX_NUM_CE_V01 12
#define QMI_WLFW_MAX_TIMESTAMP_LEN_V01 32
#define QMI_WLFW_MAX_ATHDIAG_DATA_SIZE_V01 6144
#define QMI_WLFW_MAX_NUM_GPIO_V01 32
#define QMI_WLFW_MAX_BUILD_ID_LEN_V01 128
#define QMI_WLFW_MAX_NUM_MEM_CFG_V01 2
#define QMI_WLFW_MAX_STR_LEN_V01 16
#define QMI_WLFW_MAX_NUM_SHADOW_REG_V01 24
#define QMI_WLFW_MAC_ADDR_SIZE_V01 6
#define QMI_WLFW_MAX_NUM_SHADOW_REG_V2_V01 36
#define QMI_WLFW_MAX_NUM_SVC_V01 24

enum wlfw_driver_mode_enum_v01 {
	WLFW_DRIVER_MODE_ENUM_MIN_VAL_V01 = INT_MIN,
	QMI_WLFW_MISSION_V01 = 0,
	QMI_WLFW_FTM_V01 = 1,
	QMI_WLFW_EPPING_V01 = 2,
	QMI_WLFW_WALTEST_V01 = 3,
	QMI_WLFW_OFF_V01 = 4,
	QMI_WLFW_CCPM_V01 = 5,
	QMI_WLFW_QVIT_V01 = 6,
	QMI_WLFW_CALIBRATION_V01 = 7,
	WLFW_DRIVER_MODE_ENUM_MAX_VAL_V01 = INT_MAX,
};

enum wlfw_cal_temp_id_enum_v01 {
	WLFW_CAL_TEMP_ID_ENUM_MIN_VAL_V01 = INT_MIN,
	QMI_WLFW_CAL_TEMP_IDX_0_V01 = 0,
	QMI_WLFW_CAL_TEMP_IDX_1_V01 = 1,
	QMI_WLFW_CAL_TEMP_IDX_2_V01 = 2,
	QMI_WLFW_CAL_TEMP_IDX_3_V01 = 3,
	QMI_WLFW_CAL_TEMP_IDX_4_V01 = 4,
	WLFW_CAL_TEMP_ID_ENUM_MAX_VAL_V01 = INT_MAX,
};

enum wlfw_pipedir_enum_v01 {
	WLFW_PIPEDIR_ENUM_MIN_VAL_V01 = INT_MIN,
	QMI_WLFW_PIPEDIR_NONE_V01 = 0,
	QMI_WLFW_PIPEDIR_IN_V01 = 1,
	QMI_WLFW_PIPEDIR_OUT_V01 = 2,
	QMI_WLFW_PIPEDIR_INOUT_V01 = 3,
	WLFW_PIPEDIR_ENUM_MAX_VAL_V01 = INT_MAX,
};

enum wlfw_mem_type_enum_v01 {
	WLFW_MEM_TYPE_ENUM_MIN_VAL_V01 = INT_MIN,
	QMI_WLFW_MEM_TYPE_MSA_V01 = 0,
	QMI_WLFW_MEM_TYPE_DDR_V01 = 1,
	QMI_WLFW_MEM_BDF_V01 = 2,
	QMI_WLFW_MEM_M3_V01 = 3,
	QMI_WLFW_MEM_CAL_V01 = 4,
	QMI_WLFW_MEM_DPD_V01 = 5,
	QMI_WLFW_MEM_QDSS_V01 = 6,
	WLFW_MEM_TYPE_ENUM_MAX_VAL_V01 = INT_MAX,
};

enum wlfw_qdss_trace_mode_enum_v01 {
	WLFW_QDSS_TRACE_MODE_ENUM_MIN_VAL_V01 = INT_MIN,
	QMI_WLFW_QDSS_TRACE_OFF_V01 = 0,
	QMI_WLFW_QDSS_TRACE_ON_V01 = 1,
	WLFW_QDSS_TRACE_MODE_ENUM_MAX_VAL_V01 = INT_MAX,
};

#define QMI_WLFW_CE_ATTR_FLAGS_V01 ((u32)0x00)
#define QMI_WLFW_CE_ATTR_NO_SNOOP_V01 ((u32)0x01)
#define QMI_WLFW_CE_ATTR_BYTE_SWAP_DATA_V01 ((u32)0x02)
#define QMI_WLFW_CE_ATTR_SWIZZLE_DESCRIPTORS_V01 ((u32)0x04)
#define QMI_WLFW_CE_ATTR_DISABLE_INTR_V01 ((u32)0x08)
#define QMI_WLFW_CE_ATTR_ENABLE_POLL_V01 ((u32)0x10)

#define QMI_WLFW_ALREADY_REGISTERED_V01 ((u64)0x01ULL)
#define QMI_WLFW_FW_READY_V01 ((u64)0x02ULL)
#define QMI_WLFW_MSA_READY_V01 ((u64)0x04ULL)
#define QMI_WLFW_FW_MEM_READY_V01 ((u64)0x08ULL)
#define QMI_WLFW_FW_INIT_DONE_V01 ((u64)0x10ULL)

#define QMI_WLFW_FW_REJUVENATE_V01 ((u64)0x01ULL)

struct wlfw_ce_tgt_pipe_cfg_s_v01 {
	u32 pipe_num;
	enum wlfw_pipedir_enum_v01 pipe_dir;
	u32 nentries;
	u32 nbytes_max;
	u32 flags;
};

struct wlfw_ce_svc_pipe_cfg_s_v01 {
	u32 service_id;
	enum wlfw_pipedir_enum_v01 pipe_dir;
	u32 pipe_num;
};

struct wlfw_shadow_reg_cfg_s_v01 {
	u16 id;
	u16 offset;
};

struct wlfw_shadow_reg_v2_cfg_s_v01 {
	u32 addr;
};

struct wlfw_rri_over_ddr_cfg_s_v01 {
	u32 base_addr_low;
	u32 base_addr_high;
};

struct wlfw_msi_cfg_s_v01 {
	u16 ce_id;
	u16 msi_vector;
};

struct wlfw_memory_region_info_s_v01 {
	u64 region_addr;
	u32 size;
	u8 secure_flag;
};

struct wlfw_mem_cfg_s_v01 {
	u64 offset;
	u32 size;
	u8 secure_flag;
};

struct wlfw_mem_seg_s_v01 {
	u32 size;
	enum wlfw_mem_type_enum_v01 type;
	u32 mem_cfg_len;
	struct wlfw_mem_cfg_s_v01 mem_cfg[QMI_WLFW_MAX_NUM_MEM_CFG_V01];
};

struct wlfw_mem_seg_resp_s_v01 {
	u64 addr;
	u32 size;
	enum wlfw_mem_type_enum_v01 type;
	u8 restore;
};

struct wlfw_rf_chip_info_s_v01 {
	u32 chip_id;
	u32 chip_family;
};

struct wlfw_rf_board_info_s_v01 {
	u32 board_id;
};

struct wlfw_soc_info_s_v01 {
	u32 soc_id;
};

struct wlfw_fw_version_info_s_v01 {
	u32 fw_version;
	char fw_build_timestamp[QMI_WLFW_MAX_TIMESTAMP_LEN_V01 + 1];
};

struct wlfw_ind_register_req_msg_v01 {
	u8 fw_ready_enable_valid;
	u8 fw_ready_enable;
	u8 initiate_cal_download_enable_valid;
	u8 initiate_cal_download_enable;
	u8 initiate_cal_update_enable_valid;
	u8 initiate_cal_update_enable;
	u8 msa_ready_enable_valid;
	u8 msa_ready_enable;
	u8 pin_connect_result_enable_valid;
	u8 pin_connect_result_enable;
	u8 client_id_valid;
	u32 client_id;
	u8 request_mem_enable_valid;
	u8 request_mem_enable;
	u8 fw_mem_ready_enable_valid;
	u8 fw_mem_ready_enable;
	u8 fw_init_done_enable_valid;
	u8 fw_init_done_enable;
	u8 rejuvenate_enable_valid;
	u32 rejuvenate_enable;
	u8 xo_cal_enable_valid;
	u8 xo_cal_enable;
	u8 cal_done_enable_valid;
	u8 cal_done_enable;
	u8 qdss_trace_req_mem_enable_valid;
	u8 qdss_trace_req_mem_enable;
	u8 qdss_trace_save_enable_valid;
	u8 qdss_trace_save_enable;
	u8 qdss_trace_free_enable_valid;
	u8 qdss_trace_free_enable;
};

#define WLFW_IND_REGISTER_REQ_MSG_V01_MAX_MSG_LEN 66
extern struct elem_info wlfw_ind_register_req_msg_v01_ei[];

struct wlfw_ind_register_resp_msg_v01 {
	struct qmi_response_type_v01 resp;
	u8 fw_status_valid;
	u64 fw_status;
};

#define WLFW_IND_REGISTER_RESP_MSG_V01_MAX_MSG_LEN 18
extern struct elem_info wlfw_ind_register_resp_msg_v01_ei[];

struct wlfw_fw_ready_ind_msg_v01 {
	char placeholder;
};

#define WLFW_FW_READY_IND_MSG_V01_MAX_MSG_LEN 0
extern struct elem_info wlfw_fw_ready_ind_msg_v01_ei[];

struct wlfw_msa_ready_ind_msg_v01 {
	char placeholder;
};

#define WLFW_MSA_READY_IND_MSG_V01_MAX_MSG_LEN 0
extern struct elem_info wlfw_msa_ready_ind_msg_v01_ei[];

struct wlfw_pin_connect_result_ind_msg_v01 {
	u8 pwr_pin_result_valid;
	u32 pwr_pin_result;
	u8 phy_io_pin_result_valid;
	u32 phy_io_pin_result;
	u8 rf_pin_result_valid;
	u32 rf_pin_result;
};

#define WLFW_PIN_CONNECT_RESULT_IND_MSG_V01_MAX_MSG_LEN 21
extern struct elem_info wlfw_pin_connect_result_ind_msg_v01_ei[];

struct wlfw_wlan_mode_req_msg_v01 {
	enum wlfw_driver_mode_enum_v01 mode;
	u8 hw_debug_valid;
	u8 hw_debug;
};

#define WLFW_WLAN_MODE_REQ_MSG_V01_MAX_MSG_LEN 11
extern struct elem_info wlfw_wlan_mode_req_msg_v01_ei[];

struct wlfw_wlan_mode_resp_msg_v01 {
	struct qmi_response_type_v01 resp;
};

#define WLFW_WLAN_MODE_RESP_MSG_V01_MAX_MSG_LEN 7
extern struct elem_info wlfw_wlan_mode_resp_msg_v01_ei[];

struct wlfw_wlan_cfg_req_msg_v01 {
	u8 host_version_valid;
	char host_version[QMI_WLFW_MAX_STR_LEN_V01 + 1];
	u8 tgt_cfg_valid;
	u32 tgt_cfg_len;
	struct wlfw_ce_tgt_pipe_cfg_s_v01 tgt_cfg[QMI_WLFW_MAX_NUM_CE_V01];
	u8 svc_cfg_valid;
	u32 svc_cfg_len;
	struct wlfw_ce_svc_pipe_cfg_s_v01 svc_cfg[QMI_WLFW_MAX_NUM_SVC_V01];
	u8 shadow_reg_valid;
	u32 shadow_reg_len;
	struct wlfw_shadow_reg_cfg_s_v01
		shadow_reg[QMI_WLFW_MAX_NUM_SHADOW_REG_V01];
	u8 shadow_reg_v2_valid;
	u32 shadow_reg_v2_len;
	struct wlfw_shadow_reg_v2_cfg_s_v01
		shadow_reg_v2[QMI_WLFW_MAX_NUM_SHADOW_REG_V2_V01];
	u8 rri_over_ddr_cfg_valid;
	struct wlfw_rri_over_ddr_cfg_s_v01 rri_over_ddr_cfg;
	u8 msi_cfg_valid;
	u32 msi_cfg_len;
	struct wlfw_msi_cfg_s_v01 msi_cfg[QMI_WLFW_MAX_NUM_CE_V01];
};

#define WLFW_WLAN_CFG_REQ_MSG_V01_MAX_MSG_LEN 866
extern struct elem_info wlfw_wlan_cfg_req_msg_v01_ei[];

struct wlfw_wlan_cfg_resp_msg_v01 {
	struct qmi_response_type_v01 resp;
};

#define WLFW_WLAN_CFG_RESP_MSG_V01_MAX_MSG_LEN 7
extern struct elem_info wlfw_wlan_cfg_resp_msg_v01_ei[];

struct wlfw_cap_req_msg_v01 {
	char placeholder;
};

#define WLFW_CAP_REQ_MSG_V01_MAX_MSG_LEN 0
extern struct elem_info wlfw_cap_req_msg_v01_ei[];

struct wlfw_cap_resp_msg_v01 {
	struct qmi_response_type_v01 resp;
	u8 chip_info_valid;
	struct wlfw_rf_chip_info_s_v01 chip_info;
	u8 board_info_valid;
	struct wlfw_rf_board_info_s_v01 board_info;
	u8 soc_info_valid;
	struct wlfw_soc_info_s_v01 soc_info;
	u8 fw_version_info_valid;
	struct wlfw_fw_version_info_s_v01 fw_version_info;
	u8 fw_build_id_valid;
	char fw_build_id[QMI_WLFW_MAX_BUILD_ID_LEN_V01 + 1];
	u8 num_macs_valid;
	u8 num_macs;
};

#define WLFW_CAP_RESP_MSG_V01_MAX_MSG_LEN 207
extern struct elem_info wlfw_cap_resp_msg_v01_ei[];

struct wlfw_bdf_download_req_msg_v01 {
	u8 valid;
	u8 file_id_valid;
	enum wlfw_cal_temp_id_enum_v01 file_id;
	u8 total_size_valid;
	u32 total_size;
	u8 seg_id_valid;
	u32 seg_id;
	u8 data_valid;
	u32 data_len;
	u8 data[QMI_WLFW_MAX_DATA_SIZE_V01];
	u8 end_valid;
	u8 end;
	u8 bdf_type_valid;
	u8 bdf_type;
};

#define WLFW_BDF_DOWNLOAD_REQ_MSG_V01_MAX_MSG_LEN 6182
extern struct elem_info wlfw_bdf_download_req_msg_v01_ei[];

struct wlfw_bdf_download_resp_msg_v01 {
	struct qmi_response_type_v01 resp;
};

#define WLFW_BDF_DOWNLOAD_RESP_MSG_V01_MAX_MSG_LEN 7
extern struct elem_info wlfw_bdf_download_resp_msg_v01_ei[];

struct wlfw_cal_report_req_msg_v01 {
	u32 meta_data_len;
	enum wlfw_cal_temp_id_enum_v01 meta_data[QMI_WLFW_MAX_NUM_CAL_V01];
	u8 xo_cal_data_valid;
	u8 xo_cal_data;
};

#define WLFW_CAL_REPORT_REQ_MSG_V01_MAX_MSG_LEN 28
extern struct elem_info wlfw_cal_report_req_msg_v01_ei[];

struct wlfw_cal_report_resp_msg_v01 {
	struct qmi_response_type_v01 resp;
};

#define WLFW_CAL_REPORT_RESP_MSG_V01_MAX_MSG_LEN 7
extern struct elem_info wlfw_cal_report_resp_msg_v01_ei[];

struct wlfw_initiate_cal_download_ind_msg_v01 {
	enum wlfw_cal_temp_id_enum_v01 cal_id;
	u8 total_size_valid;
	u32 total_size;
	u8 cal_data_location_valid;
	u32 cal_data_location;
};

#define WLFW_INITIATE_CAL_DOWNLOAD_IND_MSG_V01_MAX_MSG_LEN 21
extern struct elem_info wlfw_initiate_cal_download_ind_msg_v01_ei[];

struct wlfw_cal_download_req_msg_v01 {
	u8 valid;
	u8 file_id_valid;
	enum wlfw_cal_temp_id_enum_v01 file_id;
	u8 total_size_valid;
	u32 total_size;
	u8 seg_id_valid;
	u32 seg_id;
	u8 data_valid;
	u32 data_len;
	u8 data[QMI_WLFW_MAX_DATA_SIZE_V01];
	u8 end_valid;
	u8 end;
	u8 cal_data_location_valid;
	u32 cal_data_location;
};

#define WLFW_CAL_DOWNLOAD_REQ_MSG_V01_MAX_MSG_LEN 6185
extern struct elem_info wlfw_cal_download_req_msg_v01_ei[];

struct wlfw_cal_download_resp_msg_v01 {
	struct qmi_response_type_v01 resp;
};

#define WLFW_CAL_DOWNLOAD_RESP_MSG_V01_MAX_MSG_LEN 7
extern struct elem_info wlfw_cal_download_resp_msg_v01_ei[];

struct wlfw_initiate_cal_update_ind_msg_v01 {
	enum wlfw_cal_temp_id_enum_v01 cal_id;
	u32 total_size;
	u8 cal_data_location_valid;
	u32 cal_data_location;
};

#define WLFW_INITIATE_CAL_UPDATE_IND_MSG_V01_MAX_MSG_LEN 21
extern struct elem_info wlfw_initiate_cal_update_ind_msg_v01_ei[];

struct wlfw_cal_update_req_msg_v01 {
	enum wlfw_cal_temp_id_enum_v01 cal_id;
	u32 seg_id;
};

#define WLFW_CAL_UPDATE_REQ_MSG_V01_MAX_MSG_LEN 14
extern struct elem_info wlfw_cal_update_req_msg_v01_ei[];

struct wlfw_cal_update_resp_msg_v01 {
	struct qmi_response_type_v01 resp;
	u8 file_id_valid;
	enum wlfw_cal_temp_id_enum_v01 file_id;
	u8 total_size_valid;
	u32 total_size;
	u8 seg_id_valid;
	u32 seg_id;
	u8 data_valid;
	u32 data_len;
	u8 data[QMI_WLFW_MAX_DATA_SIZE_V01];
	u8 end_valid;
	u8 end;
	u8 cal_data_location_valid;
	u32 cal_data_location;
};

#define WLFW_CAL_UPDATE_RESP_MSG_V01_MAX_MSG_LEN 6188
extern struct elem_info wlfw_cal_update_resp_msg_v01_ei[];

struct wlfw_msa_info_req_msg_v01 {
	u64 msa_addr;
	u32 size;
};

#define WLFW_MSA_INFO_REQ_MSG_V01_MAX_MSG_LEN 18
extern struct elem_info wlfw_msa_info_req_msg_v01_ei[];

struct wlfw_msa_info_resp_msg_v01 {
	struct qmi_response_type_v01 resp;
	u32 mem_region_info_len;
	struct wlfw_memory_region_info_s_v01
		mem_region_info[QMI_WLFW_MAX_NUM_MEMORY_REGIONS_V01];
};

#define WLFW_MSA_INFO_RESP_MSG_V01_MAX_MSG_LEN 37
extern struct elem_info wlfw_msa_info_resp_msg_v01_ei[];

struct wlfw_msa_ready_req_msg_v01 {
	char placeholder;
};

#define WLFW_MSA_READY_REQ_MSG_V01_MAX_MSG_LEN 0
extern struct elem_info wlfw_msa_ready_req_msg_v01_ei[];

struct wlfw_msa_ready_resp_msg_v01 {
	struct qmi_response_type_v01 resp;
};

#define WLFW_MSA_READY_RESP_MSG_V01_MAX_MSG_LEN 7
extern struct elem_info wlfw_msa_ready_resp_msg_v01_ei[];

struct wlfw_ini_req_msg_v01 {
	u8 enablefwlog_valid;
	u8 enablefwlog;
};

#define WLFW_INI_REQ_MSG_V01_MAX_MSG_LEN 4
extern struct elem_info wlfw_ini_req_msg_v01_ei[];

struct wlfw_ini_resp_msg_v01 {
	struct qmi_response_type_v01 resp;
};

#define WLFW_INI_RESP_MSG_V01_MAX_MSG_LEN 7
extern struct elem_info wlfw_ini_resp_msg_v01_ei[];

struct wlfw_athdiag_read_req_msg_v01 {
	u32 offset;
	u32 mem_type;
	u32 data_len;
};

#define WLFW_ATHDIAG_READ_REQ_MSG_V01_MAX_MSG_LEN 21
extern struct elem_info wlfw_athdiag_read_req_msg_v01_ei[];

struct wlfw_athdiag_read_resp_msg_v01 {
	struct qmi_response_type_v01 resp;
	u8 data_valid;
	u32 data_len;
	u8 data[QMI_WLFW_MAX_ATHDIAG_DATA_SIZE_V01];
};

#define WLFW_ATHDIAG_READ_RESP_MSG_V01_MAX_MSG_LEN 6156
extern struct elem_info wlfw_athdiag_read_resp_msg_v01_ei[];

struct wlfw_athdiag_write_req_msg_v01 {
	u32 offset;
	u32 mem_type;
	u32 data_len;
	u8 data[QMI_WLFW_MAX_ATHDIAG_DATA_SIZE_V01];
};

#define WLFW_ATHDIAG_WRITE_REQ_MSG_V01_MAX_MSG_LEN 6163
extern struct elem_info wlfw_athdiag_write_req_msg_v01_ei[];

struct wlfw_athdiag_write_resp_msg_v01 {
	struct qmi_response_type_v01 resp;
};

#define WLFW_ATHDIAG_WRITE_RESP_MSG_V01_MAX_MSG_LEN 7
extern struct elem_info wlfw_athdiag_write_resp_msg_v01_ei[];

struct wlfw_vbatt_req_msg_v01 {
	u64 voltage_uv;
};

#define WLFW_VBATT_REQ_MSG_V01_MAX_MSG_LEN 11
extern struct elem_info wlfw_vbatt_req_msg_v01_ei[];

struct wlfw_vbatt_resp_msg_v01 {
	struct qmi_response_type_v01 resp;
};

#define WLFW_VBATT_RESP_MSG_V01_MAX_MSG_LEN 7
extern struct elem_info wlfw_vbatt_resp_msg_v01_ei[];

struct wlfw_mac_addr_req_msg_v01 {
	u8 mac_addr_valid;
	u8 mac_addr[QMI_WLFW_MAC_ADDR_SIZE_V01];
};

#define WLFW_MAC_ADDR_REQ_MSG_V01_MAX_MSG_LEN 9
extern struct elem_info wlfw_mac_addr_req_msg_v01_ei[];

struct wlfw_mac_addr_resp_msg_v01 {
	struct qmi_response_type_v01 resp;
};

#define WLFW_MAC_ADDR_RESP_MSG_V01_MAX_MSG_LEN 7
extern struct elem_info wlfw_mac_addr_resp_msg_v01_ei[];

struct wlfw_host_cap_req_msg_v01 {
	u8 num_clients_valid;
	u32 num_clients;
	u8 wake_msi_valid;
	u32 wake_msi;
	u8 gpios_valid;
	u32 gpios_len;
	u32 gpios[QMI_WLFW_MAX_NUM_GPIO_V01];
	u8 nm_modem_valid;
	u8 nm_modem;
	u8 bdf_support_valid;
	u8 bdf_support;
	u8 bdf_cache_support_valid;
	u8 bdf_cache_support;
	u8 m3_support_valid;
	u8 m3_support;
	u8 m3_cache_support_valid;
	u8 m3_cache_support;
	u8 cal_filesys_support_valid;
	u8 cal_filesys_support;
	u8 cal_cache_support_valid;
	u8 cal_cache_support;
	u8 cal_done_valid;
	u8 cal_done;
	u8 mem_bucket_valid;
	u32 mem_bucket;
	u8 mem_cfg_mode_valid;
	u8 mem_cfg_mode;
	u8 cal_duration_valid;
	u16 cal_duration;
};

#define WLFW_HOST_CAP_REQ_MSG_V01_MAX_MSG_LEN 194
extern struct elem_info wlfw_host_cap_req_msg_v01_ei[];

struct wlfw_host_cap_resp_msg_v01 {
	struct qmi_response_type_v01 resp;
};

#define WLFW_HOST_CAP_RESP_MSG_V01_MAX_MSG_LEN 7
extern struct elem_info wlfw_host_cap_resp_msg_v01_ei[];

struct wlfw_request_mem_ind_msg_v01 {
	u32 mem_seg_len;
	struct wlfw_mem_seg_s_v01 mem_seg[QMI_WLFW_MAX_NUM_MEM_SEG_V01];
};

#define WLFW_REQUEST_MEM_IND_MSG_V01_MAX_MSG_LEN 1124
extern struct elem_info wlfw_request_mem_ind_msg_v01_ei[];

struct wlfw_respond_mem_req_msg_v01 {
	u32 mem_seg_len;
	struct wlfw_mem_seg_resp_s_v01 mem_seg[QMI_WLFW_MAX_NUM_MEM_SEG_V01];
};

#define WLFW_RESPOND_MEM_REQ_MSG_V01_MAX_MSG_LEN 548
extern struct elem_info wlfw_respond_mem_req_msg_v01_ei[];

struct wlfw_respond_mem_resp_msg_v01 {
	struct qmi_response_type_v01 resp;
};

#define WLFW_RESPOND_MEM_RESP_MSG_V01_MAX_MSG_LEN 7
extern struct elem_info wlfw_respond_mem_resp_msg_v01_ei[];

struct wlfw_fw_mem_ready_ind_msg_v01 {
	char placeholder;
};

#define WLFW_FW_MEM_READY_IND_MSG_V01_MAX_MSG_LEN 0
extern struct elem_info wlfw_fw_mem_ready_ind_msg_v01_ei[];

struct wlfw_fw_init_done_ind_msg_v01 {
	char placeholder;
};

#define WLFW_FW_INIT_DONE_IND_MSG_V01_MAX_MSG_LEN 0
extern struct elem_info wlfw_fw_init_done_ind_msg_v01_ei[];

struct wlfw_rejuvenate_ind_msg_v01 {
	u8 cause_for_rejuvenation_valid;
	u8 cause_for_rejuvenation;
	u8 requesting_sub_system_valid;
	u8 requesting_sub_system;
	u8 line_number_valid;
	u16 line_number;
	u8 function_name_valid;
	char function_name[QMI_WLFW_FUNCTION_NAME_LEN_V01 + 1];
};

#define WLFW_REJUVENATE_IND_MSG_V01_MAX_MSG_LEN 144
extern struct elem_info wlfw_rejuvenate_ind_msg_v01_ei[];

struct wlfw_rejuvenate_ack_req_msg_v01 {
	char placeholder;
};

#define WLFW_REJUVENATE_ACK_REQ_MSG_V01_MAX_MSG_LEN 0
extern struct elem_info wlfw_rejuvenate_ack_req_msg_v01_ei[];

struct wlfw_rejuvenate_ack_resp_msg_v01 {
	struct qmi_response_type_v01 resp;
};

#define WLFW_REJUVENATE_ACK_RESP_MSG_V01_MAX_MSG_LEN 7
extern struct elem_info wlfw_rejuvenate_ack_resp_msg_v01_ei[];

struct wlfw_dynamic_feature_mask_req_msg_v01 {
	u8 mask_valid;
	u64 mask;
};

#define WLFW_DYNAMIC_FEATURE_MASK_REQ_MSG_V01_MAX_MSG_LEN 11
extern struct elem_info wlfw_dynamic_feature_mask_req_msg_v01_ei[];

struct wlfw_dynamic_feature_mask_resp_msg_v01 {
	struct qmi_response_type_v01 resp;
	u8 prev_mask_valid;
	u64 prev_mask;
	u8 curr_mask_valid;
	u64 curr_mask;
};

#define WLFW_DYNAMIC_FEATURE_MASK_RESP_MSG_V01_MAX_MSG_LEN 29
extern struct elem_info wlfw_dynamic_feature_mask_resp_msg_v01_ei[];

struct wlfw_m3_info_req_msg_v01 {
	u64 addr;
	u32 size;
};

#define WLFW_M3_INFO_REQ_MSG_V01_MAX_MSG_LEN 18
extern struct elem_info wlfw_m3_info_req_msg_v01_ei[];

struct wlfw_m3_info_resp_msg_v01 {
	struct qmi_response_type_v01 resp;
};

#define WLFW_M3_INFO_RESP_MSG_V01_MAX_MSG_LEN 7
extern struct elem_info wlfw_m3_info_resp_msg_v01_ei[];

struct wlfw_xo_cal_ind_msg_v01 {
	u8 xo_cal_data;
};

#define WLFW_XO_CAL_IND_MSG_V01_MAX_MSG_LEN 4
extern struct elem_info wlfw_xo_cal_ind_msg_v01_ei[];

struct wlfw_cal_done_ind_msg_v01 {
	char placeholder;
};

#define WLFW_CAL_DONE_IND_MSG_V01_MAX_MSG_LEN 0
extern struct elem_info wlfw_cal_done_ind_msg_v01_ei[];

struct wlfw_qdss_trace_req_mem_ind_msg_v01 {
	u32 mem_seg_len;
	struct wlfw_mem_seg_s_v01 mem_seg[QMI_WLFW_MAX_NUM_MEM_SEG_V01];
};

#define WLFW_QDSS_TRACE_REQ_MEM_IND_MSG_V01_MAX_MSG_LEN 1124
extern struct elem_info wlfw_qdss_trace_req_mem_ind_msg_v01_ei[];

struct wlfw_qdss_trace_mem_info_req_msg_v01 {
	u32 mem_seg_len;
	struct wlfw_mem_seg_resp_s_v01 mem_seg[QMI_WLFW_MAX_NUM_MEM_SEG_V01];
};

#define WLFW_QDSS_TRACE_MEM_INFO_REQ_MSG_V01_MAX_MSG_LEN 548
extern struct elem_info wlfw_qdss_trace_mem_info_req_msg_v01_ei[];

struct wlfw_qdss_trace_mem_info_resp_msg_v01 {
	struct qmi_response_type_v01 resp;
};

#define WLFW_QDSS_TRACE_MEM_INFO_RESP_MSG_V01_MAX_MSG_LEN 7
extern struct elem_info wlfw_qdss_trace_mem_info_resp_msg_v01_ei[];

struct wlfw_qdss_trace_save_ind_msg_v01 {
	u32 source;
	u32 total_size;
	u8 mem_seg_valid;
	u32 mem_seg_len;
	struct wlfw_mem_seg_resp_s_v01 mem_seg[QMI_WLFW_MAX_NUM_MEM_SEG_V01];
	u8 file_name_valid;
	char file_name[QMI_WLFW_MAX_STR_LEN_V01 + 1];
};

#define WLFW_QDSS_TRACE_SAVE_IND_MSG_V01_MAX_MSG_LEN 581
extern struct elem_info wlfw_qdss_trace_save_ind_msg_v01_ei[];

struct wlfw_qdss_trace_data_req_msg_v01 {
	u32 seg_id;
};

#define WLFW_QDSS_TRACE_DATA_REQ_MSG_V01_MAX_MSG_LEN 7
extern struct elem_info wlfw_qdss_trace_data_req_msg_v01_ei[];

struct wlfw_qdss_trace_data_resp_msg_v01 {
	struct qmi_response_type_v01 resp;
	u8 total_size_valid;
	u32 total_size;
	u8 seg_id_valid;
	u32 seg_id;
	u8 data_valid;
	u32 data_len;
	u8 data[QMI_WLFW_MAX_DATA_SIZE_V01];
	u8 end_valid;
	u8 end;
};

#define WLFW_QDSS_TRACE_DATA_RESP_MSG_V01_MAX_MSG_LEN 6174
extern struct elem_info wlfw_qdss_trace_data_resp_msg_v01_ei[];

struct wlfw_qdss_trace_config_download_req_msg_v01 {
	u8 total_size_valid;
	u32 total_size;
	u8 seg_id_valid;
	u32 seg_id;
	u8 data_valid;
	u32 data_len;
	u8 data[QMI_WLFW_MAX_DATA_SIZE_V01];
	u8 end_valid;
	u8 end;
};

#define WLFW_QDSS_TRACE_CONFIG_DOWNLOAD_REQ_MSG_V01_MAX_MSG_LEN 6167
extern struct elem_info wlfw_qdss_trace_config_download_req_msg_v01_ei[];

struct wlfw_qdss_trace_config_download_resp_msg_v01 {
	struct qmi_response_type_v01 resp;
};

#define WLFW_QDSS_TRACE_CONFIG_DOWNLOAD_RESP_MSG_V01_MAX_MSG_LEN 7
extern struct elem_info wlfw_qdss_trace_config_download_resp_msg_v01_ei[];

struct wlfw_qdss_trace_mode_req_msg_v01 {
	u8 mode_valid;
	enum wlfw_qdss_trace_mode_enum_v01 mode;
	u8 option_valid;
	u64 option;
};

#define WLFW_QDSS_TRACE_MODE_REQ_MSG_V01_MAX_MSG_LEN 18
extern struct elem_info wlfw_qdss_trace_mode_req_msg_v01_ei[];

struct wlfw_qdss_trace_mode_resp_msg_v01 {
	struct qmi_response_type_v01 resp;
};

#define WLFW_QDSS_TRACE_MODE_RESP_MSG_V01_MAX_MSG_LEN 7
extern struct elem_info wlfw_qdss_trace_mode_resp_msg_v01_ei[];

struct wlfw_qdss_trace_free_ind_msg_v01 {
	u8 mem_seg_valid;
	u32 mem_seg_len;
	struct wlfw_mem_seg_resp_s_v01 mem_seg[QMI_WLFW_MAX_NUM_MEM_SEG_V01];
};

#define WLFW_QDSS_TRACE_FREE_IND_MSG_V01_MAX_MSG_LEN 548
extern struct elem_info wlfw_qdss_trace_free_ind_msg_v01_ei[];

struct wlfw_shutdown_req_msg_v01 {
	u8 shutdown_valid;
	u8 shutdown;
};

#define WLFW_SHUTDOWN_REQ_MSG_V01_MAX_MSG_LEN 4
extern struct elem_info wlfw_shutdown_req_msg_v01_ei[];

struct wlfw_shutdown_resp_msg_v01 {
	struct qmi_response_type_v01 resp;
};

#define WLFW_SHUTDOWN_RESP_MSG_V01_MAX_MSG_LEN 7
extern struct elem_info wlfw_shutdown_resp_msg_v01_ei[];

#endif
