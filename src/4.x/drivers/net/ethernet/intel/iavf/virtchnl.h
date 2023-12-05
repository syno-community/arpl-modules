/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright (c) 2013, Intel Corporation. */

#ifndef _VIRTCHNL_H_
#define _VIRTCHNL_H_

/* Description:
 * This header file describes the Virtual Function (VF) - Physical Function
 * (PF) communication protocol used by the drivers for all devices starting
 * from our 40G product line
 *
 * Admin queue buffer usage:
 * desc->opcode is always aqc_opc_send_msg_to_pf
 * flags, retval, datalen, and data addr are all used normally.
 * The Firmware copies the cookie fields when sending messages between the
 * PF and VF, but uses all other fields internally. Due to this limitation,
 * we must send all messages as "indirect", i.e. using an external buffer.
 *
 * All the VSI indexes are relative to the VF. Each VF can have maximum of
 * three VSIs. All the queue indexes are relative to the VSI.  Each VF can
 * have a maximum of sixteen queues for all of its VSIs.
 *
 * The PF is required to return a status code in v_retval for all messages
 * except RESET_VF, which does not require any response. The returned value
 * is of virtchnl_status_code type, defined here.
 *
 * In general, VF driver initialization should roughly follow the order of
 * these opcodes. The VF driver must first validate the API version of the
 * PF driver, then request a reset, then get resources, then configure
 * queues and interrupts. After these operations are complete, the VF
 * driver may start its queues, optionally add MAC and VLAN filters, and
 * process traffic.
 */

/* START GENERIC DEFINES
 * Need to ensure the following enums and defines hold the same meaning and
 * value in current and future projects
 */

/* These macros are used to generate compilation errors if a structure/union
 * is not exactly the correct length. It gives a divide by zero error if the
 * structure/union is not of the correct size, otherwise it creates an enum
 * that is never used.
 */
#define VIRTCHNL_CHECK_STRUCT_LEN(n, X) enum virtchnl_static_assert_enum_##X \
	{ virtchnl_static_assert_##X = (n)/((sizeof(struct X) == (n)) ? 1 : 0) }
#define VIRTCHNL_CHECK_UNION_LEN(n, X) enum virtchnl_static_asset_enum_##X \
	{ virtchnl_static_assert_##X = (n)/((sizeof(union X) == (n)) ? 1 : 0) }

/* Error Codes
 * Note that many older versions of various iAVF drivers convert the reported
 * status code directly into an iavf_status enumeration. For this reason, it
 * is important that the values of these enumerations line up.
 */
enum virtchnl_status_code {
	VIRTCHNL_STATUS_SUCCESS				= 0,
	VIRTCHNL_STATUS_ERR_PARAM			= -5,
	VIRTCHNL_STATUS_ERR_NO_MEMORY			= -18,
	VIRTCHNL_STATUS_ERR_OPCODE_MISMATCH		= -38,
	VIRTCHNL_STATUS_ERR_CQP_COMPL_ERROR		= -39,
	VIRTCHNL_STATUS_ERR_INVALID_VF_ID		= -40,
	VIRTCHNL_STATUS_ERR_ADMIN_QUEUE_ERROR		= -53,
	VIRTCHNL_STATUS_ERR_NOT_SUPPORTED		= -64,
};

/* Backward compatibility */
#define VIRTCHNL_ERR_PARAM VIRTCHNL_STATUS_ERR_PARAM
#define VIRTCHNL_STATUS_NOT_SUPPORTED VIRTCHNL_STATUS_ERR_NOT_SUPPORTED

#define VIRTCHNL_LINK_SPEED_2_5GB_SHIFT		0x0
#define VIRTCHNL_LINK_SPEED_100MB_SHIFT		0x1
#define VIRTCHNL_LINK_SPEED_1000MB_SHIFT	0x2
#define VIRTCHNL_LINK_SPEED_10GB_SHIFT		0x3
#define VIRTCHNL_LINK_SPEED_40GB_SHIFT		0x4
#define VIRTCHNL_LINK_SPEED_20GB_SHIFT		0x5
#define VIRTCHNL_LINK_SPEED_25GB_SHIFT		0x6
#define VIRTCHNL_LINK_SPEED_5GB_SHIFT		0x7

enum virtchnl_link_speed {
	VIRTCHNL_LINK_SPEED_UNKNOWN	= 0,
	VIRTCHNL_LINK_SPEED_100MB	= BIT(VIRTCHNL_LINK_SPEED_100MB_SHIFT),
	VIRTCHNL_LINK_SPEED_1GB		= BIT(VIRTCHNL_LINK_SPEED_1000MB_SHIFT),
	VIRTCHNL_LINK_SPEED_10GB	= BIT(VIRTCHNL_LINK_SPEED_10GB_SHIFT),
	VIRTCHNL_LINK_SPEED_40GB	= BIT(VIRTCHNL_LINK_SPEED_40GB_SHIFT),
	VIRTCHNL_LINK_SPEED_20GB	= BIT(VIRTCHNL_LINK_SPEED_20GB_SHIFT),
	VIRTCHNL_LINK_SPEED_25GB	= BIT(VIRTCHNL_LINK_SPEED_25GB_SHIFT),
	VIRTCHNL_LINK_SPEED_2_5GB	= BIT(VIRTCHNL_LINK_SPEED_2_5GB_SHIFT),
	VIRTCHNL_LINK_SPEED_5GB		= BIT(VIRTCHNL_LINK_SPEED_5GB_SHIFT),
};

/* for hsplit_0 field of Rx HMC context */
/* deprecated with AVF 1.0 */
enum virtchnl_rx_hsplit {
	VIRTCHNL_RX_HSPLIT_NO_SPLIT      = 0,
	VIRTCHNL_RX_HSPLIT_SPLIT_L2      = 1,
	VIRTCHNL_RX_HSPLIT_SPLIT_IP      = 2,
	VIRTCHNL_RX_HSPLIT_SPLIT_TCP_UDP = 4,
	VIRTCHNL_RX_HSPLIT_SPLIT_SCTP    = 8,
};

enum virtchnl_bw_limit_type {
	VIRTCHNL_BW_SHAPER = 0,
};
/* END GENERIC DEFINES */

/* Opcodes for VF-PF communication. These are placed in the v_opcode field
 * of the virtchnl_msg structure.
 */
enum virtchnl_ops {
/* The PF sends status change events to VFs using
 * the VIRTCHNL_OP_EVENT opcode.
 * VFs send requests to the PF using the other ops.
 * Use of "advanced opcode" features must be negotiated as part of capabilities
 * exchange and are not considered part of base mode feature set.
 *
 */
	VIRTCHNL_OP_UNKNOWN = 0,
	VIRTCHNL_OP_VERSION = 1, /* must ALWAYS be 1 */
	VIRTCHNL_OP_RESET_VF = 2,
	VIRTCHNL_OP_GET_VF_RESOURCES = 3,
	VIRTCHNL_OP_CONFIG_TX_QUEUE = 4,
	VIRTCHNL_OP_CONFIG_RX_QUEUE = 5,
	VIRTCHNL_OP_CONFIG_VSI_QUEUES = 6,
	VIRTCHNL_OP_CONFIG_IRQ_MAP = 7,
	VIRTCHNL_OP_ENABLE_QUEUES = 8,
	VIRTCHNL_OP_DISABLE_QUEUES = 9,
	VIRTCHNL_OP_ADD_ETH_ADDR = 10,
	VIRTCHNL_OP_DEL_ETH_ADDR = 11,
	VIRTCHNL_OP_ADD_VLAN = 12,
	VIRTCHNL_OP_DEL_VLAN = 13,
	VIRTCHNL_OP_CONFIG_PROMISCUOUS_MODE = 14,
	VIRTCHNL_OP_GET_STATS = 15,
	VIRTCHNL_OP_RSVD = 16,
	VIRTCHNL_OP_EVENT = 17, /* must ALWAYS be 17 */
	/* opcode 19 is reserved */
	VIRTCHNL_OP_IWARP = 20, /* advanced opcode */
	VIRTCHNL_OP_RDMA = VIRTCHNL_OP_IWARP,
	VIRTCHNL_OP_CONFIG_IWARP_IRQ_MAP = 21, /* advanced opcode */
	VIRTCHNL_OP_CONFIG_RDMA_IRQ_MAP = VIRTCHNL_OP_CONFIG_IWARP_IRQ_MAP,
	VIRTCHNL_OP_RELEASE_IWARP_IRQ_MAP = 22, /* advanced opcode */
	VIRTCHNL_OP_RELEASE_RDMA_IRQ_MAP = VIRTCHNL_OP_RELEASE_IWARP_IRQ_MAP,
	VIRTCHNL_OP_CONFIG_RSS_KEY = 23,
	VIRTCHNL_OP_CONFIG_RSS_LUT = 24,
	VIRTCHNL_OP_GET_RSS_HENA_CAPS = 25,
	VIRTCHNL_OP_SET_RSS_HENA = 26,
	VIRTCHNL_OP_ENABLE_VLAN_STRIPPING = 27,
	VIRTCHNL_OP_DISABLE_VLAN_STRIPPING = 28,
	VIRTCHNL_OP_REQUEST_QUEUES = 29,
	VIRTCHNL_OP_ENABLE_CHANNELS = 30,
	VIRTCHNL_OP_DISABLE_CHANNELS = 31,
	VIRTCHNL_OP_ADD_CLOUD_FILTER = 32,
	VIRTCHNL_OP_DEL_CLOUD_FILTER = 33,
	/* opcode 34 is reserved */
	/* opcodes 38, 39, 40, 41, 42 and 43 are reserved */
	VIRTCHNL_OP_GET_SUPPORTED_RXDIDS = 44,
	VIRTCHNL_OP_ADD_RSS_CFG = 45,
	VIRTCHNL_OP_DEL_RSS_CFG = 46,
	VIRTCHNL_OP_ADD_FDIR_FILTER = 47,
	VIRTCHNL_OP_DEL_FDIR_FILTER = 48,
	VIRTCHNL_OP_GET_MAX_RSS_QREGION = 50,
	VIRTCHNL_OP_GET_OFFLOAD_VLAN_V2_CAPS = 51,
	VIRTCHNL_OP_ADD_VLAN_V2 = 52,
	VIRTCHNL_OP_DEL_VLAN_V2 = 53,
	VIRTCHNL_OP_ENABLE_VLAN_STRIPPING_V2 = 54,
	VIRTCHNL_OP_DISABLE_VLAN_STRIPPING_V2 = 55,
	VIRTCHNL_OP_ENABLE_VLAN_INSERTION_V2 = 56,
	VIRTCHNL_OP_DISABLE_VLAN_INSERTION_V2 = 57,
	VIRTCHNL_OP_ENABLE_VLAN_FILTERING_V2 = 58,
	VIRTCHNL_OP_DISABLE_VLAN_FILTERING_V2 = 59,
	VIRTCHNL_OP_1588_PTP_GET_CAPS = 60,
	VIRTCHNL_OP_1588_PTP_GET_TIME = 61,
	VIRTCHNL_OP_1588_PTP_SET_TIME = 62,
	VIRTCHNL_OP_1588_PTP_ADJ_TIME = 63,
	VIRTCHNL_OP_1588_PTP_ADJ_FREQ = 64,
	VIRTCHNL_OP_1588_PTP_TX_TIMESTAMP = 65,
	VIRTCHNL_OP_GET_QOS_CAPS = 66,
	VIRTCHNL_OP_CONFIG_QUEUE_TC_MAP = 67,
	VIRTCHNL_OP_1588_PTP_GET_PIN_CFGS = 68,
	VIRTCHNL_OP_1588_PTP_SET_PIN_CFG = 69,
	VIRTCHNL_OP_1588_PTP_EXT_TIMESTAMP = 70,
	VIRTCHNL_OP_ENABLE_QUEUES_V2 = 107,
	VIRTCHNL_OP_DISABLE_QUEUES_V2 = 108,
	VIRTCHNL_OP_MAP_QUEUE_VECTOR = 111,
	VIRTCHNL_OP_MAX,
};

static inline const char *virtchnl_op_str(enum virtchnl_ops v_opcode)
{
	switch (v_opcode) {
	case VIRTCHNL_OP_UNKNOWN:
		return "VIRTCHNL_OP_UNKNOWN";
	case VIRTCHNL_OP_VERSION:
		return "VIRTCHNL_OP_VERSION";
	case VIRTCHNL_OP_RESET_VF:
		return "VIRTCHNL_OP_RESET_VF";
	case VIRTCHNL_OP_GET_VF_RESOURCES:
		return "VIRTCHNL_OP_GET_VF_RESOURCES";
	case VIRTCHNL_OP_CONFIG_TX_QUEUE:
		return "VIRTCHNL_OP_CONFIG_TX_QUEUE";
	case VIRTCHNL_OP_CONFIG_RX_QUEUE:
		return "VIRTCHNL_OP_CONFIG_RX_QUEUE";
	case VIRTCHNL_OP_CONFIG_VSI_QUEUES:
		return "VIRTCHNL_OP_CONFIG_VSI_QUEUES";
	case VIRTCHNL_OP_CONFIG_IRQ_MAP:
		return "VIRTCHNL_OP_CONFIG_IRQ_MAP";
	case VIRTCHNL_OP_ENABLE_QUEUES:
		return "VIRTCHNL_OP_ENABLE_QUEUES";
	case VIRTCHNL_OP_DISABLE_QUEUES:
		return "VIRTCHNL_OP_DISABLE_QUEUES";
	case VIRTCHNL_OP_ADD_ETH_ADDR:
		return "VIRTCHNL_OP_ADD_ETH_ADDR";
	case VIRTCHNL_OP_DEL_ETH_ADDR:
		return "VIRTCHNL_OP_DEL_ETH_ADDR";
	case VIRTCHNL_OP_ADD_VLAN:
		return "VIRTCHNL_OP_ADD_VLAN";
	case VIRTCHNL_OP_DEL_VLAN:
		return "VIRTCHNL_OP_DEL_VLAN";
	case VIRTCHNL_OP_CONFIG_PROMISCUOUS_MODE:
		return "VIRTCHNL_OP_CONFIG_PROMISCUOUS_MODE";
	case VIRTCHNL_OP_GET_STATS:
		return "VIRTCHNL_OP_GET_STATS";
	case VIRTCHNL_OP_RSVD:
		return "VIRTCHNL_OP_RSVD";
	case VIRTCHNL_OP_EVENT:
		return "VIRTCHNL_OP_EVENT";
	case VIRTCHNL_OP_RDMA:
		return "VIRTCHNL_OP_RDMA";
	case VIRTCHNL_OP_CONFIG_RDMA_IRQ_MAP:
		return "VIRTCHNL_OP_CONFIG_RDMA_IRQ_MAP:";
	case VIRTCHNL_OP_RELEASE_RDMA_IRQ_MAP:
		return "VIRTCHNL_OP_RELEASE_RDMA_IRQ_MAP";
	case VIRTCHNL_OP_CONFIG_RSS_KEY:
		return "VIRTCHNL_OP_CONFIG_RSS_KEY";
	case VIRTCHNL_OP_CONFIG_RSS_LUT:
		return "VIRTCHNL_OP_CONFIG_RSS_LUT";
	case VIRTCHNL_OP_GET_RSS_HENA_CAPS:
		return "VIRTCHNL_OP_GET_RSS_HENA_CAPS";
	case VIRTCHNL_OP_SET_RSS_HENA:
		return "VIRTCHNL_OP_SET_RSS_HENA";
	case VIRTCHNL_OP_ENABLE_VLAN_STRIPPING:
		return "VIRTCHNL_OP_ENABLE_VLAN_STRIPPING";
	case VIRTCHNL_OP_DISABLE_VLAN_STRIPPING:
		return "VIRTCHNL_OP_DISABLE_VLAN_STRIPPING";
	case VIRTCHNL_OP_REQUEST_QUEUES:
		return "VIRTCHNL_OP_REQUEST_QUEUES";
	case VIRTCHNL_OP_ENABLE_CHANNELS:
		return "VIRTCHNL_OP_ENABLE_CHANNELS";
	case VIRTCHNL_OP_DISABLE_CHANNELS:
		return "VIRTCHNL_OP_DISABLE_CHANNELS";
	case VIRTCHNL_OP_ADD_CLOUD_FILTER:
		return "VIRTCHNL_OP_ADD_CLOUD_FILTER";
	case VIRTCHNL_OP_DEL_CLOUD_FILTER:
		return "VIRTCHNL_OP_DEL_CLOUD_FILTER";
	case VIRTCHNL_OP_GET_SUPPORTED_RXDIDS:
		return "VIRTCHNL_OP_GET_SUPPORTED_RXDIDS";
	case VIRTCHNL_OP_ADD_RSS_CFG:
		return "VIRTCHNL_OP_ADD_RSS_CFG";
	case VIRTCHNL_OP_DEL_RSS_CFG:
		return "VIRTCHNL_OP_DEL_RSS_CFG";
	case VIRTCHNL_OP_ADD_FDIR_FILTER:
		return "VIRTCHNL_OP_ADD_FDIR_FILTER";
	case VIRTCHNL_OP_DEL_FDIR_FILTER:
		return "VIRTCHNL_OP_DEL_FDIR_FILTER";
	case VIRTCHNL_OP_GET_MAX_RSS_QREGION:
		return "VIRTCHNL_OP_GET_MAX_RSS_QREGION";
	case VIRTCHNL_OP_GET_OFFLOAD_VLAN_V2_CAPS:
		return "VIRTCHNL_OP_GET_OFFLOAD_VLAN_V2_CAPS";
	case VIRTCHNL_OP_ADD_VLAN_V2:
		return "VIRTCHNL_OP_ADD_VLAN_V2";
	case VIRTCHNL_OP_DEL_VLAN_V2:
		return "VIRTCHNL_OP_DEL_VLAN_V2";
	case VIRTCHNL_OP_ENABLE_VLAN_STRIPPING_V2:
		return "VIRTCHNL_OP_ENABLE_VLAN_STRIPPING_V2";
	case VIRTCHNL_OP_DISABLE_VLAN_STRIPPING_V2:
		return "VIRTCHNL_OP_DISABLE_VLAN_STRIPPING_V2";
	case VIRTCHNL_OP_ENABLE_VLAN_INSERTION_V2:
		return "VIRTCHNL_OP_ENABLE_VLAN_INSERTION_V2";
	case VIRTCHNL_OP_DISABLE_VLAN_INSERTION_V2:
		return "VIRTCHNL_OP_DISABLE_VLAN_INSERTION_V2";
	case VIRTCHNL_OP_ENABLE_VLAN_FILTERING_V2:
		return "VIRTCHNL_OP_ENABLE_VLAN_FILTERING_V2";
	case VIRTCHNL_OP_DISABLE_VLAN_FILTERING_V2:
		return "VIRTCHNL_OP_DISABLE_VLAN_FILTERING_V2";
	case VIRTCHNL_OP_1588_PTP_GET_CAPS:
		return "VIRTCHNL_OP_1588_PTP_GET_CAPS";
	case VIRTCHNL_OP_1588_PTP_GET_TIME:
		return "VIRTCHNL_OP_1588_PTP_GET_TIME";
	case VIRTCHNL_OP_1588_PTP_SET_TIME:
		return "VIRTCHNL_OP_1588_PTP_SET_TIME";
	case VIRTCHNL_OP_1588_PTP_ADJ_TIME:
		return "VIRTCHNL_OP_1588_PTP_ADJ_TIME";
	case VIRTCHNL_OP_1588_PTP_ADJ_FREQ:
		return "VIRTCHNL_OP_1588_PTP_ADJ_FREQ";
	case VIRTCHNL_OP_1588_PTP_TX_TIMESTAMP:
		return "VIRTCHNL_OP_1588_PTP_TX_TIMESTAMP";
	case VIRTCHNL_OP_1588_PTP_GET_PIN_CFGS:
		return "VIRTCHNL_OP_1588_PTP_GET_PIN_CFGS";
	case VIRTCHNL_OP_1588_PTP_SET_PIN_CFG:
		return "VIRTCHNL_OP_1588_PTP_SET_PIN_CFG";
	case VIRTCHNL_OP_1588_PTP_EXT_TIMESTAMP:
		return "VIRTCHNL_OP_1588_PTP_EXT_TIMESTAMP";
	case VIRTCHNL_OP_ENABLE_QUEUES_V2:
		return "VIRTCHNL_OP_ENABLE_QUEUES_V2";
	case VIRTCHNL_OP_DISABLE_QUEUES_V2:
		return "VIRTCHNL_OP_DISABLE_QUEUES_V2";
	case VIRTCHNL_OP_MAP_QUEUE_VECTOR:
		return "VIRTCHNL_OP_MAP_QUEUE_VECTOR";
	case VIRTCHNL_OP_MAX:
		return "VIRTCHNL_OP_MAX";
	default:
		return "Unsupported (update virtchnl.h)";
	}
}

static inline const char *virtchnl_stat_str(enum virtchnl_status_code v_status)
{
	switch (v_status) {
	case VIRTCHNL_STATUS_SUCCESS:
		return "VIRTCHNL_STATUS_SUCCESS";
	case VIRTCHNL_STATUS_ERR_PARAM:
		return "VIRTCHNL_STATUS_ERR_PARAM";
	case VIRTCHNL_STATUS_ERR_NO_MEMORY:
		return "VIRTCHNL_STATUS_ERR_NO_MEMORY";
	case VIRTCHNL_STATUS_ERR_OPCODE_MISMATCH:
		return "VIRTCHNL_STATUS_ERR_OPCODE_MISMATCH";
	case VIRTCHNL_STATUS_ERR_CQP_COMPL_ERROR:
		return "VIRTCHNL_STATUS_ERR_CQP_COMPL_ERROR";
	case VIRTCHNL_STATUS_ERR_INVALID_VF_ID:
		return "VIRTCHNL_STATUS_ERR_INVALID_VF_ID";
	case VIRTCHNL_STATUS_ERR_ADMIN_QUEUE_ERROR:
		return "VIRTCHNL_STATUS_ERR_ADMIN_QUEUE_ERROR";
	case VIRTCHNL_STATUS_ERR_NOT_SUPPORTED:
		return "VIRTCHNL_STATUS_ERR_NOT_SUPPORTED";
	default:
		return "Unknown status code (update virtchnl.h)";
	}
}

/* Message descriptions and data structures. */

/* VIRTCHNL_OP_VERSION
 * VF posts its version number to the PF. PF responds with its version number
 * in the same format, along with a return code.
 * Reply from PF has its major/minor versions also in param0 and param1.
 * If there is a major version mismatch, then the VF cannot operate.
 * If there is a minor version mismatch, then the VF can operate but should
 * add a warning to the system log.
 *
 * This enum element MUST always be specified as == 1, regardless of other
 * changes in the API. The PF must always respond to this message without
 * error regardless of version mismatch.
 */
#define VIRTCHNL_VERSION_MAJOR		1
#define VIRTCHNL_VERSION_MINOR		1
#define VIRTCHNL_VERSION_MAJOR_2	2
#define VIRTCHNL_VERSION_MINOR_0	0
#define VIRTCHNL_VERSION_MINOR_NO_VF_CAPS	0

struct virtchnl_version_info {
	u32 major;
	u32 minor;
};

VIRTCHNL_CHECK_STRUCT_LEN(8, virtchnl_version_info);

#define VF_IS_V10(_ver) (((_ver)->major == 1) && ((_ver)->minor == 0))
#define VF_IS_V11(_ver) (((_ver)->major == 1) && ((_ver)->minor == 1))
#define VF_IS_V20(_ver) (((_ver)->major == 2) && ((_ver)->minor == 0))

/* VIRTCHNL_OP_RESET_VF
 * VF sends this request to PF with no parameters
 * PF does NOT respond! VF driver must delay then poll VFGEN_RSTAT register
 * until reset completion is indicated. The admin queue must be reinitialized
 * after this operation.
 *
 * When reset is complete, PF must ensure that all queues in all VSIs associated
 * with the VF are stopped, all queue configurations in the HMC are set to 0,
 * and all MAC and VLAN filters (except the default MAC address) on all VSIs
 * are cleared.
 */

/* VSI types that use VIRTCHNL interface for VF-PF communication. VSI_SRIOV
 * vsi_type should always be 6 for backward compatibility. Add other fields
 * as needed.
 */
enum virtchnl_vsi_type {
	VIRTCHNL_VSI_TYPE_INVALID = 0,
	VIRTCHNL_VSI_SRIOV = 6,
};

/* VIRTCHNL_OP_GET_VF_RESOURCES
 * Version 1.0 VF sends this request to PF with no parameters
 * Version 1.1 VF sends this request to PF with u32 bitmap of its capabilities
 * PF responds with an indirect message containing
 * virtchnl_vf_resource and one or more
 * virtchnl_vsi_resource structures.
 */

struct virtchnl_vsi_resource {
	u16 vsi_id;
	u16 num_queue_pairs;

	/* see enum virtchnl_vsi_type */
	s32 vsi_type;
	u16 qset_handle;
	u8 default_mac_addr[ETH_ALEN];
};

VIRTCHNL_CHECK_STRUCT_LEN(16, virtchnl_vsi_resource);

/* VF capability flags
 * VIRTCHNL_VF_OFFLOAD_L2 flag is inclusive of base mode L2 offloads including
 * TX/RX Checksum offloading and TSO for non-tunnelled packets.
 */
#define VIRTCHNL_VF_OFFLOAD_L2			BIT(0)
#define VIRTCHNL_VF_OFFLOAD_IWARP		BIT(1)
#define VIRTCHNL_VF_CAP_RDMA			VIRTCHNL_VF_OFFLOAD_IWARP
#define VIRTCHNL_VF_OFFLOAD_RSS_AQ		BIT(3)
#define VIRTCHNL_VF_OFFLOAD_RSS_REG		BIT(4)
#define VIRTCHNL_VF_OFFLOAD_WB_ON_ITR		BIT(5)
#define VIRTCHNL_VF_OFFLOAD_REQ_QUEUES		BIT(6)
/* used to negotiate communicating link speeds in Mbps */
#define VIRTCHNL_VF_CAP_ADV_LINK_SPEED		BIT(7)
	/* BIT(8) is reserved */
#define VIRTCHNL_VF_LARGE_NUM_QPAIRS		BIT(9)
#define VIRTCHNL_VF_OFFLOAD_CRC			BIT(10)
#define VIRTCHNL_VF_OFFLOAD_VLAN_V2		BIT(15)
#define VIRTCHNL_VF_OFFLOAD_VLAN		BIT(16)
#define VIRTCHNL_VF_OFFLOAD_RX_POLLING		BIT(17)
#define VIRTCHNL_VF_OFFLOAD_RSS_PCTYPE_V2	BIT(18)
#define VIRTCHNL_VF_OFFLOAD_RSS_PF		BIT(19)
#define VIRTCHNL_VF_OFFLOAD_ENCAP		BIT(20)
#define VIRTCHNL_VF_OFFLOAD_ENCAP_CSUM		BIT(21)
#define VIRTCHNL_VF_OFFLOAD_RX_ENCAP_CSUM	BIT(22)
#define VIRTCHNL_VF_OFFLOAD_ADQ			BIT(23)
#define VIRTCHNL_VF_OFFLOAD_ADQ_V2		BIT(24)
#define VIRTCHNL_VF_OFFLOAD_USO			BIT(25)
#define VIRTCHNL_VF_OFFLOAD_RX_FLEX_DESC	BIT(26)
#define VIRTCHNL_VF_OFFLOAD_ADV_RSS_PF		BIT(27)
#define VIRTCHNL_VF_OFFLOAD_FDIR_PF		BIT(28)
#define VIRTCHNL_VF_OFFLOAD_QOS			BIT(29)
	/* BIT(30) is reserved */
#define VIRTCHNL_VF_CAP_PTP			BIT(31)

#define VF_BASE_MODE_OFFLOADS (VIRTCHNL_VF_OFFLOAD_L2 | \
			       VIRTCHNL_VF_OFFLOAD_VLAN | \
			       VIRTCHNL_VF_OFFLOAD_RSS_PF)

struct virtchnl_vf_resource {
	u16 num_vsis;
	u16 num_queue_pairs;
	u16 max_vectors;
	u16 max_mtu;

	u32 vf_cap_flags;
	u32 rss_key_size;
	u32 rss_lut_size;

	struct virtchnl_vsi_resource vsi_res[1];
};

VIRTCHNL_CHECK_STRUCT_LEN(36, virtchnl_vf_resource);

/* VIRTCHNL_OP_CONFIG_TX_QUEUE
 * VF sends this message to set up parameters for one TX queue.
 * External data buffer contains one instance of virtchnl_txq_info.
 * PF configures requested queue and returns a status code.
 */

/* Tx queue config info */
struct virtchnl_txq_info {
	u16 vsi_id;
	u16 queue_id;
	u16 ring_len;		/* number of descriptors, multiple of 8 */
	u16 headwb_enabled; /* deprecated with AVF 1.0 */
	u64 dma_ring_addr;
	u64 dma_headwb_addr; /* deprecated with AVF 1.0 */
};

VIRTCHNL_CHECK_STRUCT_LEN(24, virtchnl_txq_info);

/* RX descriptor IDs (range from 0 to 63) */
enum virtchnl_rx_desc_ids {
	VIRTCHNL_RXDID_0_16B_BASE		= 0,
	/* 32B_BASE and FLEX_SPLITQ share desc ids as default descriptors
	 * because they can be differentiated based on queue model; e.g. single
	 * queue model can only use 32B_BASE and split queue model can only use
	 * FLEX_SPLITQ.  Having these as 1 allows them to be used as default
	 * descriptors without negotiation.
	 */
	VIRTCHNL_RXDID_1_32B_BASE		= 1,
	VIRTCHNL_RXDID_1_FLEX_SPLITQ		= 1,
	VIRTCHNL_RXDID_2_FLEX_SQ_NIC		= 2,
	VIRTCHNL_RXDID_3_FLEX_SQ_SW		= 3,
	VIRTCHNL_RXDID_4_FLEX_SQ_NIC_VEB	= 4,
	VIRTCHNL_RXDID_5_FLEX_SQ_NIC_ACL	= 5,
	VIRTCHNL_RXDID_6_FLEX_SQ_NIC_2		= 6,
	VIRTCHNL_RXDID_7_HW_RSVD		= 7,
	/* 9 through 15 are reserved */
	VIRTCHNL_RXDID_16_COMMS_GENERIC 	= 16,
	VIRTCHNL_RXDID_17_COMMS_AUX_VLAN 	= 17,
	VIRTCHNL_RXDID_18_COMMS_AUX_IPV4 	= 18,
	VIRTCHNL_RXDID_19_COMMS_AUX_IPV6 	= 19,
	VIRTCHNL_RXDID_20_COMMS_AUX_FLOW 	= 20,
	VIRTCHNL_RXDID_21_COMMS_AUX_TCP 	= 21,
	/* 22 through 63 are reserved */
};

/* RX descriptor ID bitmasks */
enum virtchnl_rx_desc_id_bitmasks {
	VIRTCHNL_RXDID_0_16B_BASE_M		= BIT(VIRTCHNL_RXDID_0_16B_BASE),
	VIRTCHNL_RXDID_1_32B_BASE_M		= BIT(VIRTCHNL_RXDID_1_32B_BASE),
	VIRTCHNL_RXDID_1_FLEX_SPLITQ_M		= BIT(VIRTCHNL_RXDID_1_FLEX_SPLITQ),
	VIRTCHNL_RXDID_2_FLEX_SQ_NIC_M		= BIT(VIRTCHNL_RXDID_2_FLEX_SQ_NIC),
	VIRTCHNL_RXDID_3_FLEX_SQ_SW_M		= BIT(VIRTCHNL_RXDID_3_FLEX_SQ_SW),
	VIRTCHNL_RXDID_4_FLEX_SQ_NIC_VEB_M	= BIT(VIRTCHNL_RXDID_4_FLEX_SQ_NIC_VEB),
	VIRTCHNL_RXDID_5_FLEX_SQ_NIC_ACL_M	= BIT(VIRTCHNL_RXDID_5_FLEX_SQ_NIC_ACL),
	VIRTCHNL_RXDID_6_FLEX_SQ_NIC_2_M	= BIT(VIRTCHNL_RXDID_6_FLEX_SQ_NIC_2),
	VIRTCHNL_RXDID_7_HW_RSVD_M		= BIT(VIRTCHNL_RXDID_7_HW_RSVD),
	/* 9 through 15 are reserved */
	VIRTCHNL_RXDID_16_COMMS_GENERIC_M	= BIT(VIRTCHNL_RXDID_16_COMMS_GENERIC),
	VIRTCHNL_RXDID_17_COMMS_AUX_VLAN_M	= BIT(VIRTCHNL_RXDID_17_COMMS_AUX_VLAN),
	VIRTCHNL_RXDID_18_COMMS_AUX_IPV4_M	= BIT(VIRTCHNL_RXDID_18_COMMS_AUX_IPV4),
	VIRTCHNL_RXDID_19_COMMS_AUX_IPV6_M	= BIT(VIRTCHNL_RXDID_19_COMMS_AUX_IPV6),
	VIRTCHNL_RXDID_20_COMMS_AUX_FLOW_M	= BIT(VIRTCHNL_RXDID_20_COMMS_AUX_FLOW),
	VIRTCHNL_RXDID_21_COMMS_AUX_TCP_M	= BIT(VIRTCHNL_RXDID_21_COMMS_AUX_TCP),
	/* 22 through 63 are reserved */
};

/* VIRTCHNL_OP_CONFIG_RX_QUEUE
 * VF sends this message to set up parameters for one RX queue.
 * External data buffer contains one instance of virtchnl_rxq_info.
 * PF configures requested queue and returns a status code. The
 * crc_disable flag disables CRC stripping on the VF. Setting
 * the crc_disable flag to 1 will disable CRC stripping for each
 * queue in the VF where the flag is set. The VIRTCHNL_VF_OFFLOAD_CRC
 * offload must have been set prior to sending this info or the PF
 * will ignore the request. This flag should be set the same for
 * all of the queues for a VF.
 */

/* Rx queue config info */
struct virtchnl_rxq_info {
	u16 vsi_id;
	u16 queue_id;
	u32 ring_len;		/* number of descriptors, multiple of 32 */
	u16 hdr_size;
	u16 splithdr_enabled; /* deprecated with AVF 1.0 */
	u32 databuffer_size;
	u32 max_pkt_size;
	u8 crc_disable;
	/* see enum virtchnl_rx_desc_ids;
	 * only used when VIRTCHNL_VF_OFFLOAD_RX_FLEX_DESC is supported. Note
	 * that when the offload is not supported, the descriptor format aligns
	 * with VIRTCHNL_RXDID_1_32B_BASE.
	 */
	u8 rxdid;
	u8 pad1[2];
	u64 dma_ring_addr;

	/* see enum virtchnl_rx_hsplit; deprecated with AVF 1.0 */
	s32 rx_split_pos;
	u32 pad2;
};

VIRTCHNL_CHECK_STRUCT_LEN(40, virtchnl_rxq_info);

/* VIRTCHNL_OP_CONFIG_VSI_QUEUES
 * VF sends this message to set parameters for active TX and RX queues
 * associated with the specified VSI.
 * PF configures queues and returns status.
 * If the number of queues specified is greater than the number of queues
 * associated with the VSI, an error is returned and no queues are configured.
 * NOTE: The VF is not required to configure all queues in a single request.
 * It may send multiple messages. PF drivers must correctly handle all VF
 * requests.
 */
struct virtchnl_queue_pair_info {
	/* NOTE: vsi_id and queue_id should be identical for both queues. */
	struct virtchnl_txq_info txq;
	struct virtchnl_rxq_info rxq;
};

VIRTCHNL_CHECK_STRUCT_LEN(64, virtchnl_queue_pair_info);

struct virtchnl_vsi_queue_config_info {
	u16 vsi_id;
	u16 num_queue_pairs;
	u32 pad;
	struct virtchnl_queue_pair_info qpair[1];
};

VIRTCHNL_CHECK_STRUCT_LEN(72, virtchnl_vsi_queue_config_info);

/* VIRTCHNL_OP_REQUEST_QUEUES
 * VF sends this message to request the PF to allocate additional queues to
 * this VF.  Each VF gets a guaranteed number of queues on init but asking for
 * additional queues must be negotiated.  This is a best effort request as it
 * is possible the PF does not have enough queues left to support the request.
 * If the PF cannot support the number requested it will respond with the
 * maximum number it is able to support.  If the request is successful, PF will
 * then reset the VF to institute required changes.
 */

/* VF resource request */
struct virtchnl_vf_res_request {
	u16 num_queue_pairs;
};

/* VIRTCHNL_OP_CONFIG_IRQ_MAP
 * VF uses this message to map vectors to queues.
 * The rxq_map and txq_map fields are bitmaps used to indicate which queues
 * are to be associated with the specified vector.
 * The "other" causes are always mapped to vector 0. The VF may not request
 * that vector 0 be used for traffic.
 * PF configures interrupt mapping and returns status.
 * NOTE: due to hardware requirements, all active queues (both TX and RX)
 * should be mapped to interrupts, even if the driver intends to operate
 * only in polling mode. In this case the interrupt may be disabled, but
 * the ITR timer will still run to trigger writebacks.
 */
struct virtchnl_vector_map {
	u16 vsi_id;
	u16 vector_id;
	u16 rxq_map;
	u16 txq_map;
	u16 rxitr_idx;
	u16 txitr_idx;
};

VIRTCHNL_CHECK_STRUCT_LEN(12, virtchnl_vector_map);

struct virtchnl_irq_map_info {
	u16 num_vectors;
	struct virtchnl_vector_map vecmap[1];
};

VIRTCHNL_CHECK_STRUCT_LEN(14, virtchnl_irq_map_info);

/* VIRTCHNL_OP_ENABLE_QUEUES
 * VIRTCHNL_OP_DISABLE_QUEUES
 * VF sends these message to enable or disable TX/RX queue pairs.
 * The queues fields are bitmaps indicating which queues to act upon.
 * (Currently, we only support 16 queues per VF, but we make the field
 * u32 to allow for expansion.)
 * PF performs requested action and returns status.
 * NOTE: The VF is not required to enable/disable all queues in a single
 * request. It may send multiple messages.
 * PF drivers must correctly handle all VF requests.
 */
struct virtchnl_queue_select {
	u16 vsi_id;
	u16 pad;
	u32 rx_queues;
	u32 tx_queues;
};

VIRTCHNL_CHECK_STRUCT_LEN(12, virtchnl_queue_select);

/* VIRTCHNL_OP_GET_MAX_RSS_QREGION
 *
 * if VIRTCHNL_VF_LARGE_NUM_QPAIRS was negotiated in VIRTCHNL_OP_GET_VF_RESOURCES
 * then this op must be supported.
 *
 * VF sends this message in order to query the max RSS queue region
 * size supported by PF, when VIRTCHNL_VF_LARGE_NUM_QPAIRS is enabled.
 * This information should be used when configuring the RSS LUT and/or
 * configuring queue region based filters.
 *
 * The maximum RSS queue region is 2^qregion_width. So, a qregion_width
 * of 6 would inform the VF that the PF supports a maximum RSS queue region
 * of 64.
 *
 * A queue region represents a range of queues that can be used to configure
 * a RSS LUT. For example, if a VF is given 64 queues, but only a max queue
 * region size of 16 (i.e. 2^qregion_width = 16) then it will only be able
 * to configure the RSS LUT with queue indices from 0 to 15. However, other
 * filters can be used to direct packets to queues >15 via specifying a queue
 * base/offset and queue region width.
 */
struct virtchnl_max_rss_qregion {
	u16 vport_id;
	u16 qregion_width;
	u8 pad[4];
};

VIRTCHNL_CHECK_STRUCT_LEN(8, virtchnl_max_rss_qregion);

/* VIRTCHNL_OP_ADD_ETH_ADDR
 * VF sends this message in order to add one or more unicast or multicast
 * address filters for the specified VSI.
 * PF adds the filters and returns status.
 */

/* VIRTCHNL_OP_DEL_ETH_ADDR
 * VF sends this message in order to remove one or more unicast or multicast
 * filters for the specified VSI.
 * PF removes the filters and returns status.
 */

/* VIRTCHNL_ETHER_ADDR_LEGACY
 * Prior to adding the @type member to virtchnl_ether_addr, there were 2 pad
 * bytes. Moving forward all VF drivers should not set type to
 * VIRTCHNL_ETHER_ADDR_LEGACY. This is only here to not break previous/legacy
 * behavior. The control plane function (i.e. PF) can use a best effort method
 * of tracking the primary/device unicast in this case, but there is no
 * guarantee and functionality depends on the implementation of the PF.
 */

/* VIRTCHNL_ETHER_ADDR_PRIMARY
 * All VF drivers should set @type to VIRTCHNL_ETHER_ADDR_PRIMARY for the
 * primary/device unicast MAC address filter for VIRTCHNL_OP_ADD_ETH_ADDR and
 * VIRTCHNL_OP_DEL_ETH_ADDR. This allows for the underlying control plane
 * function (i.e. PF) to accurately track and use this MAC address for
 * displaying on the host and for VM/function reset.
 */

/* VIRTCHNL_ETHER_ADDR_EXTRA
 * All VF drivers should set @type to VIRTCHNL_ETHER_ADDR_EXTRA for any extra
 * unicast and/or multicast filters that are being added/deleted via
 * VIRTCHNL_OP_DEL_ETH_ADDR/VIRTCHNL_OP_ADD_ETH_ADDR respectively.
 */
struct virtchnl_ether_addr {
	u8 addr[ETH_ALEN];
	u8 type;
#define VIRTCHNL_ETHER_ADDR_LEGACY	0
#define VIRTCHNL_ETHER_ADDR_PRIMARY	1
#define VIRTCHNL_ETHER_ADDR_EXTRA	2
#define VIRTCHNL_ETHER_ADDR_TYPE_MASK	3 /* first two bits of type are valid */
	u8 pad;
};

VIRTCHNL_CHECK_STRUCT_LEN(8, virtchnl_ether_addr);

struct virtchnl_ether_addr_list {
	u16 vsi_id;
	u16 num_elements;
	struct virtchnl_ether_addr list[1];
};

VIRTCHNL_CHECK_STRUCT_LEN(12, virtchnl_ether_addr_list);

/* VIRTCHNL_OP_ADD_VLAN
 * VF sends this message to add one or more VLAN tag filters for receives.
 * PF adds the filters and returns status.
 * If a port VLAN is configured by the PF, this operation will return an
 * error to the VF.
 */

/* VIRTCHNL_OP_DEL_VLAN
 * VF sends this message to remove one or more VLAN tag filters for receives.
 * PF removes the filters and returns status.
 * If a port VLAN is configured by the PF, this operation will return an
 * error to the VF.
 */

struct virtchnl_vlan_filter_list {
	u16 vsi_id;
	u16 num_elements;
	u16 vlan_id[1];
};

VIRTCHNL_CHECK_STRUCT_LEN(6, virtchnl_vlan_filter_list);

/* This enum is used for all of the VIRTCHNL_VF_OFFLOAD_VLAN_V2_CAPS related
 * structures and opcodes.
 *
 * VIRTCHNL_VLAN_UNSUPPORTED - This field is not supported and if a VF driver
 * populates it the PF should return VIRTCHNL_STATUS_ERR_NOT_SUPPORTED.
 *
 * VIRTCHNL_VLAN_ETHERTYPE_8100 - This field supports 0x8100 ethertype.
 * VIRTCHNL_VLAN_ETHERTYPE_88A8 - This field supports 0x88A8 ethertype.
 * VIRTCHNL_VLAN_ETHERTYPE_9100 - This field supports 0x9100 ethertype.
 *
 * VIRTCHNL_VLAN_ETHERTYPE_AND - Used when multiple ethertypes can be supported
 * by the PF concurrently. For example, if the PF can support
 * VIRTCHNL_VLAN_ETHERTYPE_8100 AND VIRTCHNL_VLAN_ETHERTYPE_88A8 filters it
 * would OR the following bits:
 *
 *	VIRTHCNL_VLAN_ETHERTYPE_8100 |
 *	VIRTCHNL_VLAN_ETHERTYPE_88A8 |
 *	VIRTCHNL_VLAN_ETHERTYPE_AND;
 *
 * The VF would interpret this as VLAN filtering can be supported on both 0x8100
 * and 0x88A8 VLAN ethertypes.
 *
 * VIRTCHNL_ETHERTYPE_XOR - Used when only a single ethertype can be supported
 * by the PF concurrently. For example if the PF can support
 * VIRTCHNL_VLAN_ETHERTYPE_8100 XOR VIRTCHNL_VLAN_ETHERTYPE_88A8 stripping
 * offload it would OR the following bits:
 *
 *	VIRTCHNL_VLAN_ETHERTYPE_8100 |
 *	VIRTCHNL_VLAN_ETHERTYPE_88A8 |
 *	VIRTCHNL_VLAN_ETHERTYPE_XOR;
 *
 * The VF would interpret this as VLAN stripping can be supported on either
 * 0x8100 or 0x88a8 VLAN ethertypes. So when requesting VLAN stripping via
 * VIRTCHNL_OP_ENABLE_VLAN_STRIPPING_V2 the specified ethertype will override
 * the previously set value.
 *
 * VIRTCHNL_VLAN_TAG_LOCATION_L2TAG1 - Used to tell the VF to insert and/or
 * strip the VLAN tag using the L2TAG1 field of the Tx/Rx descriptors.
 *
 * VIRTCHNL_VLAN_TAG_LOCATION_L2TAG2 - Used to tell the VF to insert hardware
 * offloaded VLAN tags using the L2TAG2 field of the Tx descriptor.
 *
 * VIRTCHNL_VLAN_TAG_LOCATION_L2TAG2 - Used to tell the VF to strip hardware
 * offloaded VLAN tags using the L2TAG2_2 field of the Rx descriptor.
 *
 * VIRTCHNL_VLAN_PRIO - This field supports VLAN priority bits. This is used for
 * VLAN filtering if the underlying PF supports it.
 *
 * VIRTCHNL_VLAN_TOGGLE_ALLOWED - This field is used to say whether a
 * certain VLAN capability can be toggled. For example if the underlying PF/CP
 * allows the VF to toggle VLAN filtering, stripping, and/or insertion it should
 * set this bit along with the supported ethertypes.
 */
enum virtchnl_vlan_support {
	VIRTCHNL_VLAN_UNSUPPORTED =		0,
	VIRTCHNL_VLAN_ETHERTYPE_8100 =		0x00000001,
	VIRTCHNL_VLAN_ETHERTYPE_88A8 =		0x00000002,
	VIRTCHNL_VLAN_ETHERTYPE_9100 =		0x00000004,
	VIRTCHNL_VLAN_TAG_LOCATION_L2TAG1 =	0x00000100,
	VIRTCHNL_VLAN_TAG_LOCATION_L2TAG2 =	0x00000200,
	VIRTCHNL_VLAN_TAG_LOCATION_L2TAG2_2 =	0x00000400,
	VIRTCHNL_VLAN_PRIO =			0x01000000,
	VIRTCHNL_VLAN_FILTER_MASK =		0x10000000,
	VIRTCHNL_VLAN_ETHERTYPE_AND =		0x20000000,
	VIRTCHNL_VLAN_ETHERTYPE_XOR =		0x40000000,
	VIRTCHNL_VLAN_TOGGLE =			0x80000000
};

/* This structure is used as part of the VIRTCHNL_OP_GET_OFFLOAD_VLAN_V2_CAPS
 * for filtering, insertion, and stripping capabilities.
 *
 * If only outer capabilities are supported (for filtering, insertion, and/or
 * stripping) then this refers to the outer most or single VLAN from the VF's
 * perspective.
 *
 * If only inner capabilities are supported (for filtering, insertion, and/or
 * stripping) then this refers to the outer most or single VLAN from the VF's
 * perspective. Functionally this is the same as if only outer capabilities are
 * supported. The VF driver is just forced to use the inner fields when
 * adding/deleting filters and enabling/disabling offloads (if supported).
 *
 * If both outer and inner capabilities are supported (for filtering, insertion,
 * and/or stripping) then outer refers to the outer most or single VLAN and
 * inner refers to the second VLAN, if it exists, in the packet.
 *
 * There is no support for tunneled VLAN offloads, so outer or inner are never
 * referring to a tunneled packet from the VF's perspective.
 */
struct virtchnl_vlan_supported_caps {
	u32 outer;
	u32 inner;
};

/* The PF populates these fields based on the supported VLAN filtering. If a
 * field is VIRTCHNL_VLAN_UNSUPPORTED then it's not supported and the PF will
 * reject any VIRTCHNL_OP_ADD_VLAN_V2 or VIRTCHNL_OP_DEL_VLAN_V2 messages using
 * the unsupported fields.
 *
 * Also, a VF is only allowed to toggle its VLAN filtering setting if the
 * VIRTCHNL_VLAN_TOGGLE bit is set.
 *
 * The ethertype(s) specified in the ethertype_init field are the ethertypes
 * enabled for VLAN filtering. VLAN filtering in this case refers to the outer
 * most VLAN from the VF's perspective. If both inner and outer filtering are
 * allowed then ethertype_init only refers to the outer most VLAN as only
 * VLAN ethertype supported for inner VLAN filtering is
 * VIRTCHNL_VLAN_ETHERTYPE_8100. By default, inner VLAN filtering is disabled
 * when both inner and outer filtering are allowed.
 *
 * The max_filters field tells the VF how many VLAN filters it's allowed to have
 * at any one time. If it exceeds this amount and tries to add another filter,
 * then the request will be rejected by the PF. To prevent failures, the VF
 * should keep track of how many VLAN filters it has added and not attempt to
 * add more than max_filters.
 */
struct virtchnl_vlan_filtering_caps {
	struct virtchnl_vlan_supported_caps filtering_support;
	u32 ethertype_init;
	u16 max_filters;
	u8 pad[2];
};

VIRTCHNL_CHECK_STRUCT_LEN(16, virtchnl_vlan_filtering_caps);

/* This enum is used for the virtchnl_vlan_offload_caps structure to specify
 * if the PF supports a different ethertype for stripping and insertion.
 *
 * VIRTCHNL_ETHERTYPE_STRIPPING_MATCHES_INSERTION - The ethertype(s) specified
 * for stripping affect the ethertype(s) specified for insertion and visa versa
 * as well. If the VF tries to configure VLAN stripping via
 * VIRTCHNL_OP_ENABLE_VLAN_STRIPPING_V2 with VIRTCHNL_VLAN_ETHERTYPE_8100 then
 * that will be the ethertype for both stripping and insertion.
 *
 * VIRTCHNL_ETHERTYPE_MATCH_NOT_REQUIRED - The ethertype(s) specified for
 * stripping do not affect the ethertype(s) specified for insertion and visa
 * versa.
 */
enum virtchnl_vlan_ethertype_match {
	VIRTCHNL_ETHERTYPE_STRIPPING_MATCHES_INSERTION = 0,
	VIRTCHNL_ETHERTYPE_MATCH_NOT_REQUIRED = 1,
};

/* The PF populates these fields based on the supported VLAN offloads. If a
 * field is VIRTCHNL_VLAN_UNSUPPORTED then it's not supported and the PF will
 * reject any VIRTCHNL_OP_ENABLE_VLAN_STRIPPING_V2 or
 * VIRTCHNL_OP_DISABLE_VLAN_STRIPPING_V2 messages using the unsupported fields.
 *
 * Also, a VF is only allowed to toggle its VLAN offload setting if the
 * VIRTCHNL_VLAN_TOGGLE_ALLOWED bit is set.
 *
 * The VF driver needs to be aware of how the tags are stripped by hardware and
 * inserted by the VF driver based on the level of offload support. The PF will
 * populate these fields based on where the VLAN tags are expected to be
 * offloaded via the VIRTHCNL_VLAN_TAG_LOCATION_* bits. The VF will need to
 * interpret these fields. See the definition of the
 * VIRTCHNL_VLAN_TAG_LOCATION_* bits above the virtchnl_vlan_support
 * enumeration.
 */
struct virtchnl_vlan_offload_caps {
	struct virtchnl_vlan_supported_caps stripping_support;
	struct virtchnl_vlan_supported_caps insertion_support;
	u32 ethertype_init;
	u8 ethertype_match;
	u8 pad[3];
};

VIRTCHNL_CHECK_STRUCT_LEN(24, virtchnl_vlan_offload_caps);

/* VIRTCHNL_OP_GET_OFFLOAD_VLAN_V2_CAPS
 * VF sends this message to determine its VLAN capabilities.
 *
 * PF will mark which capabilities it supports based on hardware support and
 * current configuration. For example, if a port VLAN is configured the PF will
 * not allow outer VLAN filtering, stripping, or insertion to be configured so
 * it will block these features from the VF.
 *
 * The VF will need to cross reference its capabilities with the PFs
 * capabilities in the response message from the PF to determine the VLAN
 * support.
 */
struct virtchnl_vlan_caps {
	struct virtchnl_vlan_filtering_caps filtering;
	struct virtchnl_vlan_offload_caps offloads;
};

VIRTCHNL_CHECK_STRUCT_LEN(40, virtchnl_vlan_caps);

struct virtchnl_vlan {
	u16 tci;	/* tci[15:13] = PCP and tci[11:0] = VID */
	u16 tci_mask;	/* only valid if VIRTCHNL_VLAN_FILTER_MASK set in
			 * filtering caps
			 */
	u16 tpid;	/* 0x8100, 0x88a8, etc. and only type(s) set in
			 * filtering caps. Note that tpid here does not refer to
			 * VIRTCHNL_VLAN_ETHERTYPE_*, but it refers to the
			 * actual 2-byte VLAN TPID
			 */
	u8 pad[2];
};

VIRTCHNL_CHECK_STRUCT_LEN(8, virtchnl_vlan);

struct virtchnl_vlan_filter {
	struct virtchnl_vlan inner;
	struct virtchnl_vlan outer;
	u8 pad[16];
};

VIRTCHNL_CHECK_STRUCT_LEN(32, virtchnl_vlan_filter);

/* VIRTCHNL_OP_ADD_VLAN_V2
 * VIRTCHNL_OP_DEL_VLAN_V2
 *
 * VF sends these messages to add/del one or more VLAN tag filters for Rx
 * traffic.
 *
 * The PF attempts to add the filters and returns status.
 *
 * The VF should only ever attempt to add/del virtchnl_vlan_filter(s) using the
 * supported fields negotiated via VIRTCHNL_OP_GET_OFFLOAD_VLAN_V2_CAPS.
 */
struct virtchnl_vlan_filter_list_v2 {
	u16 vport_id;
	u16 num_elements;
	u8 pad[4];
	struct virtchnl_vlan_filter filters[1];
};

VIRTCHNL_CHECK_STRUCT_LEN(40, virtchnl_vlan_filter_list_v2);

/* VIRTCHNL_OP_ENABLE_VLAN_STRIPPING_V2
 * VIRTCHNL_OP_DISABLE_VLAN_STRIPPING_V2
 * VIRTCHNL_OP_ENABLE_VLAN_INSERTION_V2
 * VIRTCHNL_OP_DISABLE_VLAN_INSERTION_V2
 *
 * VF sends this message to enable or disable VLAN stripping or insertion. It
 * also needs to specify an ethertype. The VF knows which VLAN ethertypes are
 * allowed and whether or not it's allowed to enable/disable the specific
 * offload via the VIRTCHNL_OP_GET_OFFLOAD_VLAN_V2_CAPS message. The VF needs to
 * parse the virtchnl_vlan_caps.offloads fields to determine which offload
 * messages are allowed.
 *
 * For example, if the PF populates the virtchnl_vlan_caps.offloads in the
 * following manner the VF will be allowed to enable and/or disable 0x8100 inner
 * VLAN insertion and/or stripping via the opcodes listed above. Inner in this
 * case means the outer most or single VLAN from the VF's perspective. This is
 * because no outer offloads are supported. See the comments above the
 * virtchnl_vlan_supported_caps structure for more details.
 *
 * virtchnl_vlan_caps.offloads.stripping_support.inner =
 *			VIRTCHNL_VLAN_TOGGLE |
 *			VIRTCHNL_VLAN_ETHERTYPE_8100;
 *
 * virtchnl_vlan_caps.offloads.insertion_support.inner =
 *			VIRTCHNL_VLAN_TOGGLE |
 *			VIRTCHNL_VLAN_ETHERTYPE_8100;
 *
 * In order to enable inner (again note that in this case inner is the outer
 * most or single VLAN from the VF's perspective) VLAN stripping for 0x8100
 * VLANs, the VF would populate the virtchnl_vlan_setting structure in the
 * following manner and send the VIRTCHNL_OP_ENABLE_VLAN_STRIPPING_V2 message.
 *
 * virtchnl_vlan_setting.inner_ethertype_setting =
 *			VIRTCHNL_VLAN_ETHERTYPE_8100;
 *
 * virtchnl_vlan_setting.vport_id = vport_id or vsi_id assigned to the VF on
 * initialization.
 *
 * The reason that VLAN TPID(s) are not being used for the
 * outer_ethertype_setting and inner_ethertype_setting fields is because it's
 * possible a device could support VLAN insertion and/or stripping offload on
 * multiple ethertypes concurrently, so this method allows a VF to request
 * multiple ethertypes in one message using the virtchnl_vlan_support
 * enumeration.
 *
 * For example, if the PF populates the virtchnl_vlan_caps.offloads in the
 * following manner the VF will be allowed to enable 0x8100 and 0x88a8 outer
 * VLAN insertion and stripping simultaneously. The
 * virtchnl_vlan_caps.offloads.ethertype_match field will also have to be
 * populated based on what the PF can support.
 *
 * virtchnl_vlan_caps.offloads.stripping_support.outer =
 *			VIRTCHNL_VLAN_TOGGLE |
 *			VIRTCHNL_VLAN_ETHERTYPE_8100 |
 *			VIRTCHNL_VLAN_ETHERTYPE_88A8 |
 *			VIRTCHNL_VLAN_ETHERTYPE_AND;
 *
 * virtchnl_vlan_caps.offloads.insertion_support.outer =
 *			VIRTCHNL_VLAN_TOGGLE |
 *			VIRTCHNL_VLAN_ETHERTYPE_8100 |
 *			VIRTCHNL_VLAN_ETHERTYPE_88A8 |
 *			VIRTCHNL_VLAN_ETHERTYPE_AND;
 *
 * In order to enable outer VLAN stripping for 0x8100 and 0x88a8 VLANs, the VF
 * would populate the virthcnl_vlan_offload_structure in the following manner
 * and send the VIRTCHNL_OP_ENABLE_VLAN_STRIPPING_V2 message.
 *
 * virtchnl_vlan_setting.outer_ethertype_setting =
 *			VIRTHCNL_VLAN_ETHERTYPE_8100 |
 *			VIRTHCNL_VLAN_ETHERTYPE_88A8;
 *
 * virtchnl_vlan_setting.vport_id = vport_id or vsi_id assigned to the VF on
 * initialization.
 *
 * There is also the case where a PF and the underlying hardware can support
 * VLAN offloads on multiple ethertypes, but not concurrently. For example, if
 * the PF populates the virtchnl_vlan_caps.offloads in the following manner the
 * VF will be allowed to enable and/or disable 0x8100 XOR 0x88a8 outer VLAN
 * offloads. The ethertypes must match for stripping and insertion.
 *
 * virtchnl_vlan_caps.offloads.stripping_support.outer =
 *			VIRTCHNL_VLAN_TOGGLE |
 *			VIRTCHNL_VLAN_ETHERTYPE_8100 |
 *			VIRTCHNL_VLAN_ETHERTYPE_88A8 |
 *			VIRTCHNL_VLAN_ETHERTYPE_XOR;
 *
 * virtchnl_vlan_caps.offloads.insertion_support.outer =
 *			VIRTCHNL_VLAN_TOGGLE |
 *			VIRTCHNL_VLAN_ETHERTYPE_8100 |
 *			VIRTCHNL_VLAN_ETHERTYPE_88A8 |
 *			VIRTCHNL_VLAN_ETHERTYPE_XOR;
 *
 * virtchnl_vlan_caps.offloads.ethertype_match =
 *			VIRTCHNL_ETHERTYPE_STRIPPING_MATCHES_INSERTION;
 *
 * In order to enable outer VLAN stripping for 0x88a8 VLANs, the VF would
 * populate the virtchnl_vlan_setting structure in the following manner and send
 * the VIRTCHNL_OP_ENABLE_VLAN_STRIPPING_V2. Also, this will change the
 * ethertype for VLAN insertion if it's enabled. So, for completeness, a
 * VIRTCHNL_OP_ENABLE_VLAN_INSERTION_V2 with the same ethertype should be sent.
 *
 * virtchnl_vlan_setting.outer_ethertype_setting = VIRTHCNL_VLAN_ETHERTYPE_88A8;
 *
 * virtchnl_vlan_setting.vport_id = vport_id or vsi_id assigned to the VF on
 * initialization.
 *
 * VIRTCHNL_OP_ENABLE_VLAN_FILTERING_V2
 * VIRTCHNL_OP_DISABLE_VLAN_FILTERING_V2
 *
 * VF sends this message to enable or disable VLAN filtering. It also needs to
 * specify an ethertype. The VF knows which VLAN ethertypes are allowed and
 * whether or not it's allowed to enable/disable filtering via the
 * VIRTCHNL_OP_GET_OFFLOAD_VLAN_V2_CAPS message. The VF needs to
 * parse the virtchnl_vlan_caps.filtering fields to determine which, if any,
 * filtering messages are allowed.
 *
 * For example, if the PF populates the virtchnl_vlan_caps.filtering in the
 * following manner the VF will be allowed to enable/disable 0x8100 and 0x88a8
 * outer VLAN filtering together. Note, that the VIRTCHNL_VLAN_ETHERTYPE_AND
 * means that all filtering ethertypes will to be enabled and disabled together
 * regardless of the request from the VF. This means that the underlying
 * hardware only supports VLAN filtering for all VLAN the specified ethertypes
 * or none of them.
 *
 * virtchnl_vlan_caps.filtering.filtering_support.outer =
 *			VIRTCHNL_VLAN_TOGGLE |
 *			VIRTCHNL_VLAN_ETHERTYPE_8100 |
 *			VIRTHCNL_VLAN_ETHERTYPE_88A8 |
 *			VIRTCHNL_VLAN_ETHERTYPE_9100 |
 *			VIRTCHNL_VLAN_ETHERTYPE_AND;
 *
 * In order to enable outer VLAN filtering for 0x88a8 and 0x8100 VLANs (0x9100
 * VLANs aren't supported by the VF driver), the VF would populate the
 * virtchnl_vlan_setting structure in the following manner and send the
 * VIRTCHNL_OP_ENABLE_VLAN_FILTERING_V2. The same message format would be used
 * to disable outer VLAN filtering for 0x88a8 and 0x8100 VLANs, but the
 * VIRTCHNL_OP_DISABLE_VLAN_FILTERING_V2 opcode is used.
 *
 * virtchnl_vlan_setting.outer_ethertype_setting =
 *			VIRTCHNL_VLAN_ETHERTYPE_8100 |
 *			VIRTCHNL_VLAN_ETHERTYPE_88A8;
 *
 */
struct virtchnl_vlan_setting {
	u32 outer_ethertype_setting;
	u32 inner_ethertype_setting;
	u16 vport_id;
	u8 pad[6];
};

VIRTCHNL_CHECK_STRUCT_LEN(16, virtchnl_vlan_setting);

/* VIRTCHNL_OP_CONFIG_PROMISCUOUS_MODE
 * VF sends VSI id and flags.
 * PF returns status code in retval.
 * Note: we assume that broadcast accept mode is always enabled.
 */
struct virtchnl_promisc_info {
	u16 vsi_id;
	u16 flags;
};

VIRTCHNL_CHECK_STRUCT_LEN(4, virtchnl_promisc_info);

#define FLAG_VF_UNICAST_PROMISC	0x00000001
#define FLAG_VF_MULTICAST_PROMISC	0x00000002

/* VIRTCHNL_OP_GET_STATS
 * VF sends this message to request stats for the selected VSI. VF uses
 * the virtchnl_queue_select struct to specify the VSI. The queue_id
 * field is ignored by the PF.
 *
 * PF replies with struct virtchnl_eth_stats in an external buffer.
 */

struct virtchnl_eth_stats {
	u64 rx_bytes;			/* received bytes */
	u64 rx_unicast;			/* received unicast pkts */
	u64 rx_multicast;		/* received multicast pkts */
	u64 rx_broadcast;		/* received broadcast pkts */
	u64 rx_discards;
	u64 rx_unknown_protocol;
	u64 tx_bytes;			/* transmitted bytes */
	u64 tx_unicast;			/* transmitted unicast pkts */
	u64 tx_multicast;		/* transmitted multicast pkts */
	u64 tx_broadcast;		/* transmitted broadcast pkts */
	u64 tx_discards;
	u64 tx_errors;
};

/* VIRTCHNL_OP_CONFIG_RSS_KEY
 * VIRTCHNL_OP_CONFIG_RSS_LUT
 * VF sends these messages to configure RSS. Only supported if both PF
 * and VF drivers set the VIRTCHNL_VF_OFFLOAD_RSS_PF bit during
 * configuration negotiation. If this is the case, then the RSS fields in
 * the VF resource struct are valid.
 * Both the key and LUT are initialized to 0 by the PF, meaning that
 * RSS is effectively disabled until set up by the VF.
 */
struct virtchnl_rss_key {
	u16 vsi_id;
	u16 key_len;
	u8 key[1];         /* RSS hash key, packed bytes */
};

VIRTCHNL_CHECK_STRUCT_LEN(6, virtchnl_rss_key);

struct virtchnl_rss_lut {
	u16 vsi_id;
	u16 lut_entries;
	u8 lut[1];        /* RSS lookup table */
};

VIRTCHNL_CHECK_STRUCT_LEN(6, virtchnl_rss_lut);

/* VIRTCHNL_OP_GET_RSS_HENA_CAPS
 * VIRTCHNL_OP_SET_RSS_HENA
 * VF sends these messages to get and set the hash filter enable bits for RSS.
 * By default, the PF sets these to all possible traffic types that the
 * hardware supports. The VF can query this value if it wants to change the
 * traffic types that are hashed by the hardware.
 */
struct virtchnl_rss_hena {
	u64 hena;
};

VIRTCHNL_CHECK_STRUCT_LEN(8, virtchnl_rss_hena);

/* Type of RSS algorithm */
enum virtchnl_rss_algorithm {
	VIRTCHNL_RSS_ALG_TOEPLITZ_ASYMMETRIC	= 0,
	VIRTCHNL_RSS_ALG_R_ASYMMETRIC		= 1,
	VIRTCHNL_RSS_ALG_TOEPLITZ_SYMMETRIC	= 2,
	VIRTCHNL_RSS_ALG_XOR_SYMMETRIC		= 3,
};

/* This is used by PF driver to enforce how many channels can be supported.
 * When ADQ_V2 capability is negotiated, it will allow 16 channels otherwise
 * PF driver will allow only max 4 channels
 */
#define VIRTCHNL_MAX_ADQ_CHANNELS 4
#define VIRTCHNL_MAX_ADQ_V2_CHANNELS 16

/* VIRTCHNL_OP_ENABLE_CHANNELS
 * VIRTCHNL_OP_DISABLE_CHANNELS
 * VF sends these messages to enable or disable channels based on
 * the user specified queue count and queue offset for each traffic class.
 * This struct encompasses all the information that the PF needs from
 * VF to create a channel.
 */
struct virtchnl_channel_info {
	u16 count; /* number of queues in a channel */
	u16 offset; /* queues in a channel start from 'offset' */
	u32 pad;
	u64 max_tx_rate;
};

VIRTCHNL_CHECK_STRUCT_LEN(16, virtchnl_channel_info);

struct virtchnl_tc_info {
	u32	num_tc;
	u32	pad;
	struct	virtchnl_channel_info list[1];
};

VIRTCHNL_CHECK_STRUCT_LEN(24, virtchnl_tc_info);

/* VIRTCHNL_ADD_CLOUD_FILTER
 * VIRTCHNL_DEL_CLOUD_FILTER
 * VF sends these messages to add or delete a cloud filter based on the
 * user specified match and action filters. These structures encompass
 * all the information that the PF needs from the VF to add/delete a
 * cloud filter.
 */

struct virtchnl_l4_spec {
	u8	src_mac[ETH_ALEN];
	u8	dst_mac[ETH_ALEN];
	/* vlan_prio is part of this 16 bit field even from OS perspective
	 * vlan_id:12 is actual vlan_id, then vlanid:bit14..12 is vlan_prio
	 * in future, when decided to offload vlan_prio, pass that information
	 * as part of the "vlan_id" field, Bit14..12
	 */
	__be16	vlan_id;
	__be16	pad; /* reserved for future use */
	__be32	src_ip[4];
	__be32	dst_ip[4];
	__be16	src_port;
	__be16	dst_port;
};

VIRTCHNL_CHECK_STRUCT_LEN(52, virtchnl_l4_spec);

union virtchnl_flow_spec {
	struct	virtchnl_l4_spec tcp_spec;
	u8	buffer[128]; /* reserved for future use */
};

VIRTCHNL_CHECK_UNION_LEN(128, virtchnl_flow_spec);

enum virtchnl_action {
	/* action types */
	VIRTCHNL_ACTION_DROP = 0,
	VIRTCHNL_ACTION_TC_REDIRECT,
	VIRTCHNL_ACTION_PASSTHRU,
	VIRTCHNL_ACTION_QUEUE,
	VIRTCHNL_ACTION_Q_REGION,
	VIRTCHNL_ACTION_MARK,
	VIRTCHNL_ACTION_COUNT,
};

enum virtchnl_flow_type {
	/* flow types */
	VIRTCHNL_TCP_V4_FLOW = 0,
	VIRTCHNL_TCP_V6_FLOW,
	VIRTCHNL_UDP_V4_FLOW,
	VIRTCHNL_UDP_V6_FLOW,
};

struct virtchnl_filter {
	union	virtchnl_flow_spec data;
	union	virtchnl_flow_spec mask;

	/* see enum virtchnl_flow_type */
	s32 	flow_type;

	/* see enum virtchnl_action */
	s32	action;
	u32	action_meta;
	u8	field_flags;
};

VIRTCHNL_CHECK_STRUCT_LEN(272, virtchnl_filter);

struct virtchnl_shaper_bw {
	/* Unit is Kbps */
	u32 committed;
	u32 peak;
};

VIRTCHNL_CHECK_STRUCT_LEN(8, virtchnl_shaper_bw);

struct virtchnl_supported_rxdids {
	/* see enum virtchnl_rx_desc_id_bitmasks */
	u64 supported_rxdids;
};

VIRTCHNL_CHECK_STRUCT_LEN(8, virtchnl_supported_rxdids);

/* VIRTCHNL_OP_EVENT
 * PF sends this message to inform the VF driver of events that may affect it.
 * No direct response is expected from the VF, though it may generate other
 * messages in response to this one.
 */
enum virtchnl_event_codes {
	VIRTCHNL_EVENT_UNKNOWN = 0,
	VIRTCHNL_EVENT_LINK_CHANGE,
	VIRTCHNL_EVENT_RESET_IMPENDING,
	VIRTCHNL_EVENT_PF_DRIVER_CLOSE,
};

#define PF_EVENT_SEVERITY_INFO		0
#define PF_EVENT_SEVERITY_CERTAIN_DOOM	255

struct virtchnl_pf_event {
	/* see enum virtchnl_event_codes */
	s32 event;
	union {
		/* If the PF driver does not support the new speed reporting
		 * capabilities then use link_event else use link_event_adv to
		 * get the speed and link information. The ability to understand
		 * new speeds is indicated by setting the capability flag
		 * VIRTCHNL_VF_CAP_ADV_LINK_SPEED in vf_cap_flags parameter
		 * in virtchnl_vf_resource struct and can be used to determine
		 * which link event struct to use below.
		 */
		struct {
			enum virtchnl_link_speed link_speed;
			bool link_status;
			u8 pad[3];
		} link_event;
		struct {
			/* link_speed provided in Mbps */
			u32 link_speed;
			u8 link_status;
			u8 pad[3];
		} link_event_adv;
	} event_data;

	s32 severity;
};

VIRTCHNL_CHECK_STRUCT_LEN(16, virtchnl_pf_event);

/* used to specify if a ceq_idx or aeq_idx is invalid */
#define VIRTCHNL_RDMA_INVALID_QUEUE_IDX	0xFFFF
/* VIRTCHNL_OP_CONFIG_RDMA_IRQ_MAP
 * VF uses this message to request PF to map RDMA vectors to RDMA queues.
 * The request for this originates from the VF RDMA driver through
 * a client interface between VF LAN and VF RDMA driver.
 * A vector could have an AEQ and CEQ attached to it although
 * there is a single AEQ per VF RDMA instance in which case
 * most vectors will have an VIRTCHNL_RDMA_INVALID_QUEUE_IDX for aeq and valid
 * idx for ceqs There will never be a case where there will be multiple CEQs
 * attached to a single vector.
 * PF configures interrupt mapping and returns status.
 */
#define virtchnl_iwarp_qv_info virtchnl_rdma_qv_info
struct virtchnl_rdma_qv_info {
	u32 v_idx; /* msix_vector */
	u16 ceq_idx; /* set to VIRTCHNL_RDMA_INVALID_QUEUE_IDX if invalid */
	u16 aeq_idx; /* set to VIRTCHNL_RDMA_INVALID_QUEUE_IDX if invalid */
	u8 itr_idx;
};

VIRTCHNL_CHECK_STRUCT_LEN(12, virtchnl_rdma_qv_info);

#define virtchnl_iwarp_qvlist_info virtchnl_rdma_qvlist_info
struct virtchnl_rdma_qvlist_info {
	u32 num_vectors;
	struct virtchnl_rdma_qv_info qv_info[1];
};

VIRTCHNL_CHECK_STRUCT_LEN(16, virtchnl_rdma_qvlist_info);

/* VF reset states - these are written into the RSTAT register:
 * VFGEN_RSTAT on the VF
 * When the PF initiates a reset, it writes 0
 * When the reset is complete, it writes 1
 * When the PF detects that the VF has recovered, it writes 2
 * VF checks this register periodically to determine if a reset has occurred,
 * then polls it to know when the reset is complete.
 * If either the PF or VF reads the register while the hardware
 * is in a reset state, it will return DEADBEEF, which, when masked
 * will result in 3.
 */
enum virtchnl_vfr_states {
	VIRTCHNL_VFR_INPROGRESS = 0,
	VIRTCHNL_VFR_COMPLETED,
	VIRTCHNL_VFR_VFACTIVE,
};

#define VIRTCHNL_MAX_NUM_PROTO_HDRS	32
#define PROTO_HDR_SHIFT			5
#define PROTO_HDR_FIELD_START(proto_hdr_type) \
					(proto_hdr_type << PROTO_HDR_SHIFT)
#define PROTO_HDR_FIELD_MASK ((1UL << PROTO_HDR_SHIFT) - 1)

/* VF use these macros to configure each protocol header.
 * Specify which protocol headers and protocol header fields base on
 * virtchnl_proto_hdr_type and virtchnl_proto_hdr_field.
 * @param hdr: a struct of virtchnl_proto_hdr
 * @param hdr_type: ETH/IPV4/TCP, etc
 * @param field: SRC/DST/TEID/SPI, etc
 */
#define VIRTCHNL_ADD_PROTO_HDR_FIELD(hdr, field) \
	((hdr)->field_selector |= BIT((field) & PROTO_HDR_FIELD_MASK))
#define VIRTCHNL_DEL_PROTO_HDR_FIELD(hdr, field) \
	((hdr)->field_selector &= ~BIT((field) & PROTO_HDR_FIELD_MASK))
#define VIRTCHNL_TEST_PROTO_HDR_FIELD(hdr, val) \
	((hdr)->field_selector & BIT((val) & PROTO_HDR_FIELD_MASK))
#define VIRTCHNL_GET_PROTO_HDR_FIELD(hdr)	((hdr)->field_selector)

#define VIRTCHNL_ADD_PROTO_HDR_FIELD_BIT(hdr, hdr_type, field) \
	(VIRTCHNL_ADD_PROTO_HDR_FIELD(hdr, \
		VIRTCHNL_PROTO_HDR_ ## hdr_type ## _ ## field))
#define VIRTCHNL_DEL_PROTO_HDR_FIELD_BIT(hdr, hdr_type, field) \
	(VIRTCHNL_DEL_PROTO_HDR_FIELD(hdr, \
		VIRTCHNL_PROTO_HDR_ ## hdr_type ## _ ## field))

#define VIRTCHNL_SET_PROTO_HDR_TYPE(hdr, hdr_type) \
	((hdr)->type = VIRTCHNL_PROTO_HDR_ ## hdr_type)
#define VIRTCHNL_GET_PROTO_HDR_TYPE(hdr) \
	(((hdr)->type) >> PROTO_HDR_SHIFT)
#define VIRTCHNL_TEST_PROTO_HDR_TYPE(hdr, val) \
	((hdr)->type == ((s32)((val) >> PROTO_HDR_SHIFT)))
#define VIRTCHNL_TEST_PROTO_HDR(hdr, val) \
	(VIRTCHNL_TEST_PROTO_HDR_TYPE(hdr, val) && \
	 VIRTCHNL_TEST_PROTO_HDR_FIELD(hdr, val))

/* Protocol header type within a packet segment. A segment consists of one or
 * more protocol headers that make up a logical group of protocol headers. Each
 * logical group of protocol headers encapsulates or is encapsulated using/by
 * tunneling or encapsulation protocols for network virtualization.
 */
enum virtchnl_proto_hdr_type {
	VIRTCHNL_PROTO_HDR_NONE,
	VIRTCHNL_PROTO_HDR_ETH,
	VIRTCHNL_PROTO_HDR_S_VLAN,
	VIRTCHNL_PROTO_HDR_C_VLAN,
	VIRTCHNL_PROTO_HDR_IPV4,
	VIRTCHNL_PROTO_HDR_IPV6,
	VIRTCHNL_PROTO_HDR_TCP,
	VIRTCHNL_PROTO_HDR_UDP,
	VIRTCHNL_PROTO_HDR_SCTP,
	VIRTCHNL_PROTO_HDR_GTPU_IP,
	VIRTCHNL_PROTO_HDR_GTPU_EH,
	VIRTCHNL_PROTO_HDR_GTPU_EH_PDU_DWN,
	VIRTCHNL_PROTO_HDR_GTPU_EH_PDU_UP,
	VIRTCHNL_PROTO_HDR_PPPOE,
	VIRTCHNL_PROTO_HDR_L2TPV3,
	VIRTCHNL_PROTO_HDR_ESP,
	VIRTCHNL_PROTO_HDR_AH,
	VIRTCHNL_PROTO_HDR_PFCP,
	VIRTCHNL_PROTO_HDR_GTPC,
	VIRTCHNL_PROTO_HDR_ECPRI,
	VIRTCHNL_PROTO_HDR_L2TPV2,
	VIRTCHNL_PROTO_HDR_PPP,
	/* IPv4 and IPv6 Fragment header types are only associated to
	 * VIRTCHNL_PROTO_HDR_IPV4 and VIRTCHNL_PROTO_HDR_IPV6 respectively,
	 * cannot be used independently.
	 */
	VIRTCHNL_PROTO_HDR_IPV4_FRAG,
	VIRTCHNL_PROTO_HDR_IPV6_EH_FRAG,
	VIRTCHNL_PROTO_HDR_GRE,
};

/* Protocol header field within a protocol header. */
enum virtchnl_proto_hdr_field {
	/* ETHER */
	VIRTCHNL_PROTO_HDR_ETH_SRC =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_ETH),
	VIRTCHNL_PROTO_HDR_ETH_DST,
	VIRTCHNL_PROTO_HDR_ETH_ETHERTYPE,
	/* S-VLAN */
	VIRTCHNL_PROTO_HDR_S_VLAN_ID =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_S_VLAN),
	/* C-VLAN */
	VIRTCHNL_PROTO_HDR_C_VLAN_ID =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_C_VLAN),
	/* IPV4 */
	VIRTCHNL_PROTO_HDR_IPV4_SRC =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_IPV4),
	VIRTCHNL_PROTO_HDR_IPV4_DST,
	VIRTCHNL_PROTO_HDR_IPV4_DSCP,
	VIRTCHNL_PROTO_HDR_IPV4_TTL,
	VIRTCHNL_PROTO_HDR_IPV4_PROT,
	VIRTCHNL_PROTO_HDR_IPV4_CHKSUM,
	/* IPV6 */
	VIRTCHNL_PROTO_HDR_IPV6_SRC =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_IPV6),
	VIRTCHNL_PROTO_HDR_IPV6_DST,
	VIRTCHNL_PROTO_HDR_IPV6_TC,
	VIRTCHNL_PROTO_HDR_IPV6_HOP_LIMIT,
	VIRTCHNL_PROTO_HDR_IPV6_PROT,
	/* IPV6 Prefix */
	VIRTCHNL_PROTO_HDR_IPV6_PREFIX32_SRC,
	VIRTCHNL_PROTO_HDR_IPV6_PREFIX32_DST,
	VIRTCHNL_PROTO_HDR_IPV6_PREFIX40_SRC,
	VIRTCHNL_PROTO_HDR_IPV6_PREFIX40_DST,
	VIRTCHNL_PROTO_HDR_IPV6_PREFIX48_SRC,
	VIRTCHNL_PROTO_HDR_IPV6_PREFIX48_DST,
	VIRTCHNL_PROTO_HDR_IPV6_PREFIX56_SRC,
	VIRTCHNL_PROTO_HDR_IPV6_PREFIX56_DST,
	VIRTCHNL_PROTO_HDR_IPV6_PREFIX64_SRC,
	VIRTCHNL_PROTO_HDR_IPV6_PREFIX64_DST,
	VIRTCHNL_PROTO_HDR_IPV6_PREFIX96_SRC,
	VIRTCHNL_PROTO_HDR_IPV6_PREFIX96_DST,
	/* TCP */
	VIRTCHNL_PROTO_HDR_TCP_SRC_PORT =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_TCP),
	VIRTCHNL_PROTO_HDR_TCP_DST_PORT,
	VIRTCHNL_PROTO_HDR_TCP_CHKSUM,
	/* UDP */
	VIRTCHNL_PROTO_HDR_UDP_SRC_PORT =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_UDP),
	VIRTCHNL_PROTO_HDR_UDP_DST_PORT,
	VIRTCHNL_PROTO_HDR_UDP_CHKSUM,
	/* SCTP */
	VIRTCHNL_PROTO_HDR_SCTP_SRC_PORT =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_SCTP),
	VIRTCHNL_PROTO_HDR_SCTP_DST_PORT,
	VIRTCHNL_PROTO_HDR_SCTP_CHKSUM,
	/* GTPU_IP */
	VIRTCHNL_PROTO_HDR_GTPU_IP_TEID =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_GTPU_IP),
	/* GTPU_EH */
	VIRTCHNL_PROTO_HDR_GTPU_EH_PDU =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_GTPU_EH),
	VIRTCHNL_PROTO_HDR_GTPU_EH_QFI,
	/* PPPOE */
	VIRTCHNL_PROTO_HDR_PPPOE_SESS_ID =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_PPPOE),
	/* L2TPV3 */
	VIRTCHNL_PROTO_HDR_L2TPV3_SESS_ID =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_L2TPV3),
	/* ESP */
	VIRTCHNL_PROTO_HDR_ESP_SPI =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_ESP),
	/* AH */
	VIRTCHNL_PROTO_HDR_AH_SPI =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_AH),
	/* PFCP */
	VIRTCHNL_PROTO_HDR_PFCP_S_FIELD =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_PFCP),
	VIRTCHNL_PROTO_HDR_PFCP_SEID,
	/* GTPC */
	VIRTCHNL_PROTO_HDR_GTPC_TEID =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_GTPC),
	/* ECPRI */
	VIRTCHNL_PROTO_HDR_ECPRI_MSG_TYPE =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_ECPRI),
	VIRTCHNL_PROTO_HDR_ECPRI_PC_RTC_ID,
	/* IPv4 Dummy Fragment */
	VIRTCHNL_PROTO_HDR_IPV4_FRAG_PKID =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_IPV4_FRAG),
	/* IPv6 Extension Fragment */
	VIRTCHNL_PROTO_HDR_IPV6_EH_FRAG_PKID =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_IPV6_EH_FRAG),
	/* GTPU_DWN/UP */
	VIRTCHNL_PROTO_HDR_GTPU_DWN_QFI =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_GTPU_EH_PDU_DWN),
	VIRTCHNL_PROTO_HDR_GTPU_UP_QFI =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_GTPU_EH_PDU_UP),
};

struct virtchnl_proto_hdr {
	/* see enum virtchnl_proto_hdr_type */
	s32 type;
	u32 field_selector; /* a bit mask to select field for header type */
	u8 buffer[64];
	/**
	 * binary buffer in network order for specific header type.
	 * For example, if type = VIRTCHNL_PROTO_HDR_IPV4, a IPv4
	 * header is expected to be copied into the buffer.
	 */
};

VIRTCHNL_CHECK_STRUCT_LEN(72, virtchnl_proto_hdr);

struct virtchnl_proto_hdrs {
	u8 tunnel_level;
	/**
	 * specify where protocol header start from.
	 * 0 - from the outer layer
	 * 1 - from the first inner layer
	 * 2 - from the second inner layer
	 * ....
	 **/
	int count; /* the proto layers must < VIRTCHNL_MAX_NUM_PROTO_HDRS */
	struct virtchnl_proto_hdr proto_hdr[VIRTCHNL_MAX_NUM_PROTO_HDRS];
};

VIRTCHNL_CHECK_STRUCT_LEN(2312, virtchnl_proto_hdrs);

struct virtchnl_rss_cfg {
	struct virtchnl_proto_hdrs proto_hdrs;	   /* protocol headers */

	/* see enum virtchnl_rss_algorithm; rss algorithm type */
	s32 rss_algorithm;
	u8 reserved[128];                          /* reserve for future */
};

VIRTCHNL_CHECK_STRUCT_LEN(2444, virtchnl_rss_cfg);

/* action configuration for FDIR */
struct virtchnl_filter_action {
	/* see enum virtchnl_action type */
	s32 type;
	union {
		/* used for queue and qgroup action */
		struct {
			u16 index;
			u8 region;
		} queue;
		/* used for count action */
		struct {
			/* share counter ID with other flow rules */
			u8 shared;
			u32 id; /* counter ID */
		} count;
		/* used for mark action */
		u32 mark_id;
		u8 reserve[32];
	} act_conf;
};

VIRTCHNL_CHECK_STRUCT_LEN(36, virtchnl_filter_action);

#define VIRTCHNL_MAX_NUM_ACTIONS  8

struct virtchnl_filter_action_set {
	/* action number must be less then VIRTCHNL_MAX_NUM_ACTIONS */
	int count;
	struct virtchnl_filter_action actions[VIRTCHNL_MAX_NUM_ACTIONS];
};

VIRTCHNL_CHECK_STRUCT_LEN(292, virtchnl_filter_action_set);

/* pattern and action for FDIR rule */
struct virtchnl_fdir_rule {
	struct virtchnl_proto_hdrs proto_hdrs;
	struct virtchnl_filter_action_set action_set;
};

VIRTCHNL_CHECK_STRUCT_LEN(2604, virtchnl_fdir_rule);

/* Status returned to VF after VF requests FDIR commands
 * VIRTCHNL_FDIR_SUCCESS
 * VF FDIR related request is successfully done by PF
 * The request can be OP_ADD/DEL/QUERY_FDIR_FILTER.
 *
 * VIRTCHNL_FDIR_FAILURE_RULE_NORESOURCE
 * OP_ADD_FDIR_FILTER request is failed due to no Hardware resource.
 *
 * VIRTCHNL_FDIR_FAILURE_RULE_EXIST
 * OP_ADD_FDIR_FILTER request is failed due to the rule is already existed.
 *
 * VIRTCHNL_FDIR_FAILURE_RULE_CONFLICT
 * OP_ADD_FDIR_FILTER request is failed due to conflict with existing rule.
 *
 * VIRTCHNL_FDIR_FAILURE_RULE_NONEXIST
 * OP_DEL_FDIR_FILTER request is failed due to this rule doesn't exist.
 *
 * VIRTCHNL_FDIR_FAILURE_RULE_INVALID
 * OP_ADD_FDIR_FILTER request is failed due to parameters validation
 * or HW doesn't support.
 *
 * VIRTCHNL_FDIR_FAILURE_RULE_TIMEOUT
 * OP_ADD/DEL_FDIR_FILTER request is failed due to timing out
 * for programming.
 *
 * VIRTCHNL_FDIR_FAILURE_QUERY_INVALID
 * OP_QUERY_FDIR_FILTER request is failed due to parameters validation,
 * for example, VF query counter of a rule who has no counter action.
 */
enum virtchnl_fdir_prgm_status {
	VIRTCHNL_FDIR_SUCCESS = 0,
	VIRTCHNL_FDIR_FAILURE_RULE_NORESOURCE,
	VIRTCHNL_FDIR_FAILURE_RULE_EXIST,
	VIRTCHNL_FDIR_FAILURE_RULE_CONFLICT,
	VIRTCHNL_FDIR_FAILURE_RULE_NONEXIST,
	VIRTCHNL_FDIR_FAILURE_RULE_INVALID,
	VIRTCHNL_FDIR_FAILURE_RULE_TIMEOUT,
	VIRTCHNL_FDIR_FAILURE_QUERY_INVALID,
};

/* VIRTCHNL_OP_ADD_FDIR_FILTER
 * VF sends this request to PF by filling out vsi_id,
 * validate_only and rule_cfg. PF will return flow_id
 * if the request is successfully done and return add_status to VF.
 */
struct virtchnl_fdir_add {
	u16 vsi_id;  /* INPUT */
	/*
	 * 1 for validating a fdir rule, 0 for creating a fdir rule.
	 * Validate and create share one ops: VIRTCHNL_OP_ADD_FDIR_FILTER.
	 */
	u16 validate_only; /* INPUT */
	u32 flow_id;       /* OUTPUT */
	struct virtchnl_fdir_rule rule_cfg; /* INPUT */

	/* see enum virtchnl_fdir_prgm_status; OUTPUT */
	s32 status;
};

VIRTCHNL_CHECK_STRUCT_LEN(2616, virtchnl_fdir_add);

/* VIRTCHNL_OP_DEL_FDIR_FILTER
 * VF sends this request to PF by filling out vsi_id
 * and flow_id. PF will return del_status to VF.
 */
struct virtchnl_fdir_del {
	u16 vsi_id;  /* INPUT */
	u16 pad;
	u32 flow_id; /* INPUT */

	/* see enum virtchnl_fdir_prgm_status; OUTPUT */
	s32 status;
};

VIRTCHNL_CHECK_STRUCT_LEN(12, virtchnl_fdir_del);

/* VIRTCHNL_OP_GET_QOS_CAPS
 * VF sends this message to get its QoS Caps, such as
 * TC number, Arbiter and Bandwidth.
 */
struct virtchnl_qos_cap_elem {
	u8 tc_num;
	u8 tc_prio;
#define VIRTCHNL_ABITER_STRICT      0
#define VIRTCHNL_ABITER_ETS         2
	u8 arbiter;
#define VIRTCHNL_STRICT_WEIGHT      1
	u8 weight;
	enum virtchnl_bw_limit_type type;
	union {
		struct virtchnl_shaper_bw shaper;
		u8 pad2[32];
	};
};

VIRTCHNL_CHECK_STRUCT_LEN(40, virtchnl_qos_cap_elem);

struct virtchnl_qos_cap_list {
	u16 vsi_id;
	u16 num_elem;
	struct virtchnl_qos_cap_elem cap[1];
};

VIRTCHNL_CHECK_STRUCT_LEN(44, virtchnl_qos_cap_list);

/* VIRTCHNL_OP_CONFIG_QUEUE_TC_MAP
 * VF sends message virtchnl_queue_tc_mapping to set queue to tc
 * mapping for all the Tx and Rx queues with a specified VSI, and
 * would get response about bitmap of valid user priorities
 * associated with queues.
 */
struct virtchnl_queue_tc_mapping {
	u16 vsi_id;
	u16 num_tc;
	u16 num_queue_pairs;
	u8 pad[2];
	union {
		struct {
			u16 start_queue_id;
			u16 queue_count;
		} req;
		struct {
#define VIRTCHNL_USER_PRIO_TYPE_UP	0
#define VIRTCHNL_USER_PRIO_TYPE_DSCP	1
			u16 prio_type;
			u16 valid_prio_bitmap;
		} resp;
	} tc[1];
};

VIRTCHNL_CHECK_STRUCT_LEN(12, virtchnl_queue_tc_mapping);

/* queue types */
enum virtchnl_queue_type {
	VIRTCHNL_QUEUE_TYPE_TX			= 0,
	VIRTCHNL_QUEUE_TYPE_RX			= 1,
};

/* structure to specify a chunk of contiguous queues */
struct virtchnl_queue_chunk {
	/* see enum virtchnl_queue_type */
	s32 type;
	u16 start_queue_id;
	u16 num_queues;
};

VIRTCHNL_CHECK_STRUCT_LEN(8, virtchnl_queue_chunk);

/* structure to specify several chunks of contiguous queues */
struct virtchnl_queue_chunks {
	u16 num_chunks;
	u16 rsvd;
	struct virtchnl_queue_chunk chunks[1];
};

VIRTCHNL_CHECK_STRUCT_LEN(12, virtchnl_queue_chunks);

/* VIRTCHNL_OP_ENABLE_QUEUES_V2
 * VIRTCHNL_OP_DISABLE_QUEUES_V2
 *
 * These opcodes can be used if VIRTCHNL_VF_LARGE_NUM_QPAIRS was negotiated in
 * VIRTCHNL_OP_GET_VF_RESOURCES
 *
 * VF sends virtchnl_ena_dis_queues struct to specify the queues to be
 * enabled/disabled in chunks. Also applicable to single queue RX or
 * TX. PF performs requested action and returns status.
 */
struct virtchnl_del_ena_dis_queues {
	u16 vport_id;
	u16 pad;
	struct virtchnl_queue_chunks chunks;
};

VIRTCHNL_CHECK_STRUCT_LEN(16, virtchnl_del_ena_dis_queues);

/* Virtchannel interrupt throttling rate index */
enum virtchnl_itr_idx {
	VIRTCHNL_ITR_IDX_0	= 0,
	VIRTCHNL_ITR_IDX_1	= 1,
	VIRTCHNL_ITR_IDX_NO_ITR	= 3,
};

/* Queue to vector mapping */
struct virtchnl_queue_vector {
	u16 queue_id;
	u16 vector_id;
	u8 pad[4];

	/* see enum virtchnl_itr_idx */
	s32 itr_idx;

	/* see enum virtchnl_queue_type */
	s32 queue_type;
};

VIRTCHNL_CHECK_STRUCT_LEN(16, virtchnl_queue_vector);

/* VIRTCHNL_OP_MAP_QUEUE_VECTOR
 *
 * This opcode can be used only if VIRTCHNL_VF_LARGE_NUM_QPAIRS was negotiated
 * in VIRTCHNL_OP_GET_VF_RESOURCES
 *
 * VF sends this message to map queues to vectors and ITR index registers.
 * External data buffer contains virtchnl_queue_vector_maps structure
 * that contains num_qv_maps of virtchnl_queue_vector structures.
 * PF maps the requested queue vector maps after validating the queue and vector
 * ids and returns a status code.
 */
struct virtchnl_queue_vector_maps {
	u16 vport_id;
	u16 num_qv_maps;
	u8 pad[4];
	struct virtchnl_queue_vector qv_maps[1];
};

VIRTCHNL_CHECK_STRUCT_LEN(24, virtchnl_queue_vector_maps);

/* VIRTCHNL_VF_CAP_PTP
 *   VIRTCHNL_OP_1588_PTP_GET_CAPS
 *   VIRTCHNL_OP_1588_PTP_GET_TIME
 *   VIRTCHNL_OP_1588_PTP_SET_TIME
 *   VIRTCHNL_OP_1588_PTP_ADJ_TIME
 *   VIRTCHNL_OP_1588_PTP_ADJ_FREQ
 *   VIRTCHNL_OP_1588_PTP_TX_TIMESTAMP
 *   VIRTCHNL_OP_1588_PTP_GET_PIN_CFGS
 *   VIRTCHNL_OP_1588_PTP_SET_PIN_CFG
 *   VIRTCHNL_OP_1588_PTP_EXT_TIMESTAMP
 *
 * Support for offloading control of the device PTP hardware clock (PHC) is enabled
 * by VIRTCHNL_VF_CAP_PTP. This capability allows a VF to request that PF
 * enable Tx and Rx timestamps, and request access to read and/or write the
 * PHC on the device, as well as query if the VF has direct access to the PHC
 * time registers.
 *
 * The VF must set VIRTCHNL_VF_CAP_PTP in its capabilities when requesting
 * resources. If the capability is set in reply, the VF must then send
 * a VIRTCHNL_OP_1588_PTP_GET_CAPS request during initialization. The VF indicates
 * what extended capabilities it wants by setting the appropriate flags in the
 * caps field. The PF reply will indicate what features are enabled for
 * that VF.
 */
#define VIRTCHNL_1588_PTP_CAP_TX_TSTAMP		BIT(0)
#define VIRTCHNL_1588_PTP_CAP_RX_TSTAMP		BIT(1)
#define VIRTCHNL_1588_PTP_CAP_READ_PHC		BIT(2)
#define VIRTCHNL_1588_PTP_CAP_WRITE_PHC		BIT(3)
#define VIRTCHNL_1588_PTP_CAP_PHC_REGS		BIT(4)
#define VIRTCHNL_1588_PTP_CAP_PIN_CFG		BIT(5)

/**
 * virtchnl_phc_regs
 *
 * Structure defines how the VF should access PHC related registers. The VF
 * must request VIRTCHNL_1588_PTP_CAP_PHC_REGS. If the VF has access to PHC
 * registers, the PF will reply with the capability flag set, and with this
 * structure detailing what PCIe region and what offsets to use. If direct
 * access is not available, this entire structure is reserved and the fields
 * will be zero.
 *
 * If necessary in a future extension, a separate capability mutually
 * exclusive with VIRTCHNL_1588_PTP_CAP_PHC_REGS might be used to change the
 * entire format of this structure within virtchnl_ptp_caps.
 *
 * @clock_hi: Register offset of the high 32 bits of clock time
 * @clock_lo: Register offset of the low 32 bits of clock time
 * @pcie_region: The PCIe region the registers are located in.
 * @rsvd: Reserved bits for future extension
 */
struct virtchnl_phc_regs {
	u32 clock_hi;
	u32 clock_lo;
	u8 pcie_region;
	u8 rsvd[15];
};
VIRTCHNL_CHECK_STRUCT_LEN(24, virtchnl_phc_regs);

/* timestamp format enumeration
 *
 * VIRTCHNL_1588_PTP_TSTAMP_40BIT
 *
 *   This format indicates a timestamp that uses the 40bit format from the
 *   flexible Rx descriptors. It is also the default Tx timestamp format used
 *   today.
 *
 *   Such a timestamp has the following 40bit format:
 *
 *   *--------------------------------*-------------------------------*-----------*
 *   | 32 bits of time in nanoseconds | 7 bits of sub-nanosecond time | valid bit |
 *   *--------------------------------*-------------------------------*-----------*
 *
 *   The timestamp is passed in a u64, with the upper 24bits of the field
 *   reserved as zero.
 *
 *   With this format, in order to report a full 64bit timestamp to userspace
 *   applications, the VF is responsible for performing timestamp extension by
 *   carefully comparing the timestamp with the PHC time. This can correctly
 *   be achieved with a recent cached copy of the PHC time by doing delta
 *   comparison between the 32bits of nanoseconds in the timestamp with the
 *   lower 32 bits of the clock time. For this to work, the cached PHC time
 *   must be from within 2^31 nanoseconds (~2.1 seconds) of when the timestamp
 *   was captured.
 *
 * VIRTCHNL_1588_PTP_TSTAMP_64BIT_NS
 *
 *   This format indicates a timestamp that is 64 bits of nanoseconds.
 */
enum virtchnl_ptp_tstamp_format {
	VIRTCHNL_1588_PTP_TSTAMP_40BIT = 0,
	VIRTCHNL_1588_PTP_TSTAMP_64BIT_NS = 1,
};

/**
 * virtchnl_ptp_caps
 *
 * Structure that defines the PTP capabilities available to the VF. The VF
 * sends VIRTCHNL_OP_1588_PTP_GET_CAPS, and must fill in the ptp_caps field
 * indicating what capabilities it is requesting. The PF will respond with the
 * same message with the virtchnl_ptp_caps structure indicating what is
 * enabled for the VF.
 *
 * @phc_regs: If VIRTCHNL_1588_PTP_CAP_PHC_REGS is set, contains information
 *            on the PHC related registers available to the VF.
 * @caps: On send, VF sets what capabilities it requests. On reply, PF
 *        indicates what has been enabled for this VF. The PF shall not set
 *        bits which were not requested by the VF.
 * @max_adj: The maximum adjustment capable of being requested by
 *           VIRTCHNL_OP_1588_PTP_ADJ_FREQ, in parts per billion. Note that 1 ppb
 *           is approximately 65.5 scaled_ppm. The PF shall clamp any
 *           frequency adjustment in VIRTCHNL_op_1588_ADJ_FREQ to +/- max_adj.
 *           Use of ppb in this field allows fitting the value into 4 bytes
 *           instead of potentially requiring 8 if scaled_ppm units were used.
 * @tx_tstamp_idx: The Tx timestamp index to set in the transmit descriptor
 *                 when requesting a timestamp for an outgoing packet.
 *                 Reserved if VIRTCHNL_1588_PTP_CAP_TX_TSTAMP is not enabled.
 * @n_ext_ts: Number of external timestamp functions available. Reserved
 *            if VIRTCHNL_1588_PTP_CAP_PIN_CFG is not enabled.
 * @n_per_out: Number of periodic output functions available. Reserved if
 *             VIRTCHNL_1588_PTP_CAP_PIN_CFG is not enabled.
 * @n_pins: Number of physical programmable pins able to be controlled.
 *          Reserved if VIRTCHNL_1588_PTP_CAP_PIN_CFG is not enabled.
 * @tx_tstamp_format: Format of the Tx timestamps. Valid formats are defined
 *                    by the virtchnl_ptp_tstamp enumeration. Note that Rx
 *                    timestamps are tied to the descriptor format, and do not
 *                    have a separate format field.
 * @rsvd: Reserved bits for future extension.
 *
 * PTP capabilities
 *
 * VIRTCHNL_1588_PTP_CAP_TX_TSTAMP indicates that the VF can request transmit
 * timestamps for packets in its transmit descriptors. If this is unset,
 * transmit timestamp requests are ignored. Note that only one outstanding Tx
 * timestamp request will be honored at a time. The PF shall handle receipt of
 * the timestamp from the hardware, and will forward this to the VF by sending
 * a VIRTCHNL_OP_1588_TX_TIMESTAMP message.
 *
 * VIRTCHNL_1588_PTP_CAP_RX_TSTAMP indicates that the VF receive queues have
 * receive timestamps enabled in the flexible descriptors. Note that this
 * requires a VF to also negotiate to enable advanced flexible descriptors in
 * the receive path instead of the default legacy descriptor format.
 *
 * For a detailed description of the current Tx and Rx timestamp format, see
 * the section on virtchnl_phc_tx_tstamp. Future extensions may indicate
 * timestamp format in the capability structure.
 *
 * VIRTCHNL_1588_PTP_CAP_READ_PHC indicates that the VF may read the PHC time
 * via the VIRTCHNL_OP_1588_PTP_GET_TIME command, or by directly reading PHC
 * registers if VIRTCHNL_1588_PTP_CAP_PHC_REGS is also set.
 *
 * VIRTCHNL_1588_PTP_CAP_WRITE_PHC indicates that the VF may request updates
 * to the PHC time via VIRTCHNL_OP_1588_PTP_SET_TIME,
 * VIRTCHNL_OP_1588_PTP_ADJ_TIME, and VIRTCHNL_OP_1588_PTP_ADJ_FREQ.
 *
 * VIRTCHNL_1588_PTP_CAP_PHC_REGS indicates that the VF has direct access to
 * certain PHC related registers, primarily for lower latency access to the
 * PHC time. If this is set, the VF shall read the virtchnl_phc_regs section
 * of the capabilities to determine the location of the clock registers. If
 * this capability is not set, the entire 24 bytes of virtchnl_phc_regs is
 * reserved as zero. Future extensions define alternative formats for this
 * data, in which case they will be mutually exclusive with this capability.
 *
 * VIRTCHNL_1588_PTP_CAP_PIN_CFG indicates that the VF has the capability to
 * control software defined pins. These pins can be assigned either as an
 * input to timestamp external events, or as an output to cause a periodic
 * signal output.
 *
 * Note that in the future, additional capability flags may be added which
 * indicate additional extended support. All fields marked as reserved by this
 * header will be set to zero. VF implementations should verify this to ensure
 * that future extensions do not break compatibility.
 */
struct virtchnl_ptp_caps {
	struct virtchnl_phc_regs phc_regs;
	u32 caps;
	s32 max_adj;
	u8 tx_tstamp_idx;
	u8 n_ext_ts;
	u8 n_per_out;
	u8 n_pins;
	/* see enum virtchnl_ptp_tstamp_format */
	u8 tx_tstamp_format;
	u8 rsvd[11];
};
VIRTCHNL_CHECK_STRUCT_LEN(48, virtchnl_ptp_caps);

/**
 * virtchnl_phc_time
 * @time: PHC time in nanoseconds
 * @rsvd: Reserved for future extension
 *
 * Structure sent with VIRTCHNL_OP_1588_PTP_SET_TIME and received with
 * VIRTCHNL_OP_1588_PTP_GET_TIME. Contains the 64bits of PHC clock time in
 * nanoseconds.
 *
 * VIRTCHNL_OP_1588_PTP_SET_TIME may be sent by the VF if
 * VIRTCHNL_1588_PTP_CAP_WRITE_PHC is set. This will request that the PHC time
 * be set to the requested value. This operation is non-atomic and thus does
 * not adjust for the delay between request and completion. It is recommended
 * that the VF use VIRTCHNL_OP_1588_PTP_ADJ_TIME and
 * VIRTCHNL_OP_1588_PTP_ADJ_FREQ when possible to steer the PHC clock.
 *
 * VIRTCHNL_OP_1588_PTP_GET_TIME may be sent to request the current time of
 * the PHC. This op is available in case direct access via the PHC registers
 * is not available.
 */
struct virtchnl_phc_time {
	u64 time;
	u8 rsvd[8];
};
VIRTCHNL_CHECK_STRUCT_LEN(16, virtchnl_phc_time);

/**
 * virtchnl_phc_adj_time
 * @delta: offset requested to adjust clock by
 * @rsvd: reserved for future extension
 *
 * Sent with VIRTCHNL_OP_1588_PTP_ADJ_TIME. Used to request an adjustment of
 * the clock time by the provided delta, with negative values representing
 * subtraction. VIRTCHNL_OP_1588_PTP_ADJ_TIME may not be sent unless
 * VIRTCHNL_1588_PTP_CAP_WRITE_PHC is set.
 *
 * The atomicity of this operation is not guaranteed. The PF should perform an
 * atomic update using appropriate mechanisms if possible. However, this is
 * not guaranteed.
 */
struct virtchnl_phc_adj_time {
	s64 delta;
	u8 rsvd[8];
};
VIRTCHNL_CHECK_STRUCT_LEN(16, virtchnl_phc_adj_time);

/**
 * virtchnl_phc_adj_freq
 * @scaled_ppm: frequency adjustment represented in scaled parts per million
 * @rsvd: Reserved for future extension
 *
 * Sent with the VIRTCHNL_OP_1588_PTP_ADJ_FREQ to request an adjustment to the
 * clock frequency. The adjustment is in scaled_ppm, which is parts per
 * million with a 16bit binary fractional portion. 1 part per billion is
 * approximately 65.5 scaled_ppm.
 *
 *  ppm = scaled_ppm / 2^16
 *
 *  ppb = scaled_ppm * 1000 / 2^16 or
 *
 *  ppb = scaled_ppm * 125 / 2^13
 *
 * The PF shall clamp any adjustment request to plus or minus the specified
 * max_adj in the PTP capabilities.
 *
 * Requests for adjustment are always based off of nominal clock frequency and
 * not compounding. To reset clock frequency, send a request with a scaled_ppm
 * of 0.
 */
struct virtchnl_phc_adj_freq {
	s64 scaled_ppm;
	u8 rsvd[8];
};
VIRTCHNL_CHECK_STRUCT_LEN(16, virtchnl_phc_adj_freq);

/**
 * virtchnl_phc_tx_stamp
 * @tstamp: timestamp value
 * @rsvd: Reserved for future extension
 *
 * Sent along with VIRTCHNL_OP_1588_PTP_TX_TIMESTAMP from the PF when a Tx
 * timestamp for the index associated with this VF in the tx_tstamp_idx field
 * is captured by hardware.
 *
 * If VIRTCHNL_1588_PTP_CAP_TX_TSTAMP is set, the VF may request a timestamp
 * for a packet in its transmit context descriptor by setting the appropriate
 * flag and setting the timestamp index provided by the PF. On transmission,
 * the timestamp will be captured and sent to the PF. The PF will forward this
 * timestamp to the VF via the VIRTCHNL_1588_PTP_CAP_TX_TSTAMP op.
 *
 * The timestamp format is defined by the tx_tstamp_format field of the
 * virtchnl_ptp_caps structure.
 */
struct virtchnl_phc_tx_tstamp {
	u64 tstamp;
	u8 rsvd[8];
};
VIRTCHNL_CHECK_STRUCT_LEN(16, virtchnl_phc_tx_tstamp);

enum virtchnl_phc_pin_func {
	VIRTCHNL_PHC_PIN_FUNC_NONE = 0, /* Not assigned to any function */
	VIRTCHNL_PHC_PIN_FUNC_EXT_TS = 1, /* Assigned to external timestamp */
	VIRTCHNL_PHC_PIN_FUNC_PER_OUT = 2, /* Assigned to periodic output */
};

/* Length of the pin configuration data. All pin configurations belong within
 * the same union and *must* have this length in bytes.
 */
#define VIRTCHNL_PIN_CFG_LEN 64

/* virtchnl_phc_ext_ts_mode
 *
 * Mode of the external timestamp, indicating which edges of the input signal
 * to timestamp.
 */
enum virtchnl_phc_ext_ts_mode {
	VIRTCHNL_PHC_EXT_TS_NONE = 0,
	VIRTCHNL_PHC_EXT_TS_RISING_EDGE = 1,
	VIRTCHNL_PHC_EXT_TS_FALLING_EDGE = 2,
	VIRTCHNL_PHC_EXT_TS_BOTH_EDGES = 3,
};

/**
 * virtchnl_phc_ext_ts
 * @mode: mode of external timestamp request
 * @rsvd: reserved for future extension
 *
 * External timestamp configuration. Defines the configuration for this
 * external timestamp function.
 *
 * If mode is VIRTCHNL_PHC_EXT_TS_NONE, the function is essentially disabled,
 * timestamping nothing.
 *
 * If mode is VIRTCHNL_PHC_EXT_TS_RISING_EDGE, the function shall timestamp
 * the rising edge of the input when it transitions from low to high signal.
 *
 * If mode is VIRTCHNL_PHC_EXT_TS_FALLING_EDGE, the function shall timestamp
 * the falling edge of the input when it transitions from high to low signal.
 *
 * If mode is VIRTCHNL_PHC_EXT_TS_BOTH_EDGES, the function shall timestamp
 * both the rising and falling edge of the signal whenever it changes.
 *
 * The PF shall return an error if the requested mode cannot be implemented on
 * the function.
 */
struct virtchnl_phc_ext_ts {
	u8 mode; /* see virtchnl_phc_ext_ts_mode */
	u8 rsvd[63];
};

VIRTCHNL_CHECK_STRUCT_LEN(VIRTCHNL_PIN_CFG_LEN, virtchnl_phc_ext_ts);

/* virtchnl_phc_per_out_flags
 *
 * Flags defining periodic output functionality.
 */
enum virtchnl_phc_per_out_flags {
	VIRTCHNL_PHC_PER_OUT_PHASE_START = BIT(0),
};

/**
 * virtchnl_phc_per_out
 * @start: absolute start time (if VIRTCHNL_PHC_PER_OUT_PHASE_START unset)
 * @phase: phase offset to start (if VIRTCHNL_PHC_PER_OUT_PHASE_START set)
 * @period: time to complete a full clock cycle (low - > high -> low)
 * @on: length of time the signal should stay high
 * @flags: flags defining the periodic output operation.
 * rsvd: reserved for future extension
 *
 * Configuration for a periodic output signal. Used to define the signal that
 * should be generated on a given function.
 *
 * The period field determines the full length of the clock cycle, including
 * both duration hold high transition and duration to hold low transition in
 * nanoseconds.
 *
 * The on field determines how long the signal should remain high. For
 * a traditional square wave clock that is on for some duration and off for
 * the same duration, use an on length of precisely half the period. The duty
 * cycle of the clock is period/on.
 *
 * If VIRTCHNL_PHC_PER_OUT_PHASE_START is unset, then the request is to start
 * a clock an absolute time. This means that the clock should start precisely
 * at the specified time in the start field. If the start time is in the past,
 * then the periodic output should start at the next valid multiple of the
 * period plus the start time:
 *
 *   new_start = (n * period) + start
 *     (choose n such that new start is in the future)
 *
 * Note that the PF should not reject a start time in the past because it is
 * possible that such a start time was valid when the request was made, but
 * became invalid due to delay in programming the pin.
 *
 * If VIRTCHNL_PHC_PER_OUT_PHASE_START is set, then the request is to start
 * the next multiple of the period plus the phase offset. The phase must be
 * less than the period. In this case, the clock should start as soon possible
 * at the next available multiple of the period. To calculate a start time
 * when programming this mode, use:
 *
 *   start = (n * period) + phase
 *     (choose n such that start is in the future)
 *
 * A period of zero should be treated as a request to disable the clock
 * output.
 */
struct virtchnl_phc_per_out {
	union {
		u64 start;
		u64 phase;
	};
	u64 period;
	u64 on;
	u32 flags;
	u8 rsvd[36];
};

VIRTCHNL_CHECK_STRUCT_LEN(VIRTCHNL_PIN_CFG_LEN, virtchnl_phc_per_out);

/* virtchnl_phc_pin_cfg_flags
 *
 * Definition of bits in the flags field of the virtchnl_phc_pin_cfg
 * structure.
 */
enum virtchnl_phc_pin_cfg_flags {
	/* Valid for VIRTCHNL_OP_1588_PTP_SET_PIN_CFG. If set, indicates this
	 * is a request to verify if the function can be assigned to the
	 * provided pin. In this case, the ext_ts and per_out fields are
	 * ignored, and the PF response must be an error if the pin cannot be
	 * assigned to that function index.
	 */
	VIRTCHNL_PHC_PIN_CFG_VERIFY = BIT(0),
};

/**
 * virtchnl_phc_set_pin
 * @pin_index: The pin to get or set
 * @func: the function type the pin is assigned to
 * @func_index: the index of the function the pin is assigned to
 * @ext_ts: external timestamp configuration
 * @per_out: periodic output configuration
 * @rsvd1: Reserved for future extension
 * @rsvd2: Reserved for future extension
 *
 * Sent along with the VIRTCHNL_OP_1588_PTP_SET_PIN_CFG op.
 *
 * The VF issues a VIRTCHNL_OP_1588_PTP_SET_PIN_CFG to assign the pin to one
 * of the functions. It must set the pin_index field, the func field, and
 * the func_index field. The pin_index must be less than n_pins, and the
 * func_index must be less than the n_ext_ts or n_per_out depending on which
 * function type is selected. If func is for an external timestamp, the
 * ext_ts field must be filled in with the desired configuration. Similarly,
 * if the function is for a periodic output, the per_out field must be
 * configured.
 *
 * If the VIRTCHNL_PHC_PIN_CFG_VERIFY bit of the flag field is set, this is
 * a request only to verify the configuration, not to set it. In this case,
 * the PF should simply report an error if the requested pin cannot be
 * assigned to the requested function. This allows VF to determine whether or
 * not a given function can be assigned to a specific pin. Other flag bits are
 * currently reserved and must be verified as zero on both sides. They may be
 * extended in the future.
 */
struct virtchnl_phc_set_pin {
	u32 flags; /* see virtchnl_phc_pin_cfg_flags */
	u8 pin_index;
	u8 func; /* see virtchnl_phc_pin_func */
	u8 func_index;
	u8 rsvd1;
	union {
		struct virtchnl_phc_ext_ts ext_ts;
		struct virtchnl_phc_per_out per_out;
	};
	u8 rsvd2[8];
};

VIRTCHNL_CHECK_STRUCT_LEN(80, virtchnl_phc_set_pin);

/**
 * virtchnl_phc_pin
 * @pin_index: The pin to get or set
 * @func: the function type the pin is assigned to
 * @func_index: the index of the function the pin is assigned to
 * @rsvd: Reserved for future extension
 * @name: human readable pin name, supplied by PF on GET_PIN_CFGS
 *
 * Sent by the PF as part of the VIRTCHNL_OP_1588_PTP_GET_PIN_CFGS response.
 *
 * The VF issues a VIRTCHNL_OP_1588_PTP_GET_PIN_CFGS request to the PF in
 * order to obtain the current pin configuration for all of the pins that were
 * assigned to this VF.
 *
 * This structure details the pin configuration state, including a pin name
 * and which function is assigned to the pin currently.
 */
struct virtchnl_phc_pin {
	u8 pin_index;
	u8 func; /* see virtchnl_phc_pin_func */
	u8 func_index;
	u8 rsvd[5];
	char name[64];
};

VIRTCHNL_CHECK_STRUCT_LEN(72, virtchnl_phc_pin);

/**
 * virtchnl_phc_pin_cfg
 * @len: length of the variable pin config array
 * @pins: variable length pin configuration array
 *
 * Variable structure sent by the PF in reply to
 * VIRTCHNL_OP_1588_PTP_GET_PIN_CFGS. The VF does not send this structure with
 * its request of the operation.
 *
 * It is possible that the PF may need to send more pin configuration data
 * than can be sent in one virtchnl message. To handle this, the PF should
 * issue multiple VIRTCHNL_OP_1588_PTP_GET_PIN_CFGS responses. Each response
 * will indicate the number of pins it covers. The VF should be ready to wait
 * for multiple responses until it has received a total length equal to the
 * number of n_pins negotiated during extended PTP capabilities exchange.
 */
struct virtchnl_phc_get_pins {
	u8 len;
	u8 rsvd[7];
	struct virtchnl_phc_pin pins[1];
};

VIRTCHNL_CHECK_STRUCT_LEN(80, virtchnl_phc_get_pins);

/**
 * virtchnl_phc_ext_stamp
 * @tstamp: timestamp value
 * @tstamp_rsvd: Reserved for future extension of the timestamp value.
 * @tstamp_format: format of the timstamp
 * @func_index: external timestamp function this timestamp is for
 * @rsvd2: Reserved for future extension
 *
 * Sent along with the VIRTCHNL_OP_1588_PTP_EXT_TIMESTAMP from the PF when an
 * external timestamp function is triggered.
 *
 * This will be sent only if one of the external timestamp functions is
 * configured by the VF, and is only valid if VIRTCHNL_1588_PTP_CAP_PIN_CFG is
 * negotiated with the PF.
 *
 * The timestamp format is defined by the tstamp_format field using the
 * virtchnl_ptp_tstamp_format enumeration. The tstamp_rsvd field is
 * exclusively reserved for possible future variants of the timestamp format,
 * and its access will be controlled by the tstamp_format field.
 */
struct virtchnl_phc_ext_tstamp {
	u64 tstamp;
	u8 tstamp_rsvd[8];
	u8 tstamp_format;
	u8 func_index;
	u8 rsvd2[6];
};

VIRTCHNL_CHECK_STRUCT_LEN(24, virtchnl_phc_ext_tstamp);

/* Since VF messages are limited by u16 size, precalculate the maximum possible
 * values of nested elements in virtchnl structures that virtual channel can
 * possibly handle in a single message.
 */
enum virtchnl_vector_limits {
	VIRTCHNL_OP_CONFIG_VSI_QUEUES_MAX	=
		((u16)(~0) - sizeof(struct virtchnl_vsi_queue_config_info)) /
		sizeof(struct virtchnl_queue_pair_info),

	VIRTCHNL_OP_CONFIG_IRQ_MAP_MAX		=
		((u16)(~0) - sizeof(struct virtchnl_irq_map_info)) /
		sizeof(struct virtchnl_vector_map),

	VIRTCHNL_OP_ADD_DEL_ETH_ADDR_MAX	=
		((u16)(~0) - sizeof(struct virtchnl_ether_addr_list)) /
		sizeof(struct virtchnl_ether_addr),

	VIRTCHNL_OP_ADD_DEL_VLAN_MAX		=
		((u16)(~0) - sizeof(struct virtchnl_vlan_filter_list)) /
		sizeof(u16),

	VIRTCHNL_OP_CONFIG_RDMA_IRQ_MAP_MAX	=
		((u16)(~0) - sizeof(struct virtchnl_rdma_qvlist_info)) /
		sizeof(struct virtchnl_rdma_qv_info),

	VIRTCHNL_OP_ENABLE_CHANNELS_MAX		=
		((u16)(~0) - sizeof(struct virtchnl_tc_info)) /
		sizeof(struct virtchnl_channel_info),

	VIRTCHNL_OP_ENABLE_DISABLE_DEL_QUEUES_V2_MAX	=
		((u16)(~0) - sizeof(struct virtchnl_del_ena_dis_queues)) /
		sizeof(struct virtchnl_queue_chunk),

	VIRTCHNL_OP_MAP_UNMAP_QUEUE_VECTOR_MAX	=
		((u16)(~0) - sizeof(struct virtchnl_queue_vector_maps)) /
		sizeof(struct virtchnl_queue_vector),

	VIRTCHNL_OP_ADD_DEL_VLAN_V2_MAX		=
		((u16)(~0) - sizeof(struct virtchnl_vlan_filter_list_v2)) /
		sizeof(struct virtchnl_vlan_filter),
};

/**
 * virtchnl_vc_validate_vf_msg
 * @ver: Virtchnl version info
 * @v_opcode: Opcode for the message
 * @msg: pointer to the msg buffer
 * @msglen: msg length
 *
 * validate msg format against struct for each opcode
 */
static inline int
virtchnl_vc_validate_vf_msg(struct virtchnl_version_info *ver, u32 v_opcode,
			    u8 *msg, u16 msglen)
{
	bool err_msg_format = false;
	u32 valid_len = 0;

	/* Validate message length. */
	switch (v_opcode) {
	case VIRTCHNL_OP_VERSION:
		valid_len = sizeof(struct virtchnl_version_info);
		break;
	case VIRTCHNL_OP_RESET_VF:
		break;
	case VIRTCHNL_OP_GET_VF_RESOURCES:
		if (VF_IS_V11(ver))
			valid_len = sizeof(u32);
		break;
	case VIRTCHNL_OP_CONFIG_TX_QUEUE:
		valid_len = sizeof(struct virtchnl_txq_info);
		break;
	case VIRTCHNL_OP_CONFIG_RX_QUEUE:
		valid_len = sizeof(struct virtchnl_rxq_info);
		break;
	case VIRTCHNL_OP_CONFIG_VSI_QUEUES:
		valid_len = sizeof(struct virtchnl_vsi_queue_config_info);
		if (msglen >= valid_len) {
			struct virtchnl_vsi_queue_config_info *vqc =
			    (struct virtchnl_vsi_queue_config_info *)msg;

			if (vqc->num_queue_pairs == 0 || vqc->num_queue_pairs >
			    VIRTCHNL_OP_CONFIG_VSI_QUEUES_MAX) {
				err_msg_format = true;
				break;
			}

			valid_len += (vqc->num_queue_pairs *
				      sizeof(struct
					     virtchnl_queue_pair_info));
		}
		break;
	case VIRTCHNL_OP_CONFIG_IRQ_MAP:
		valid_len = sizeof(struct virtchnl_irq_map_info);
		if (msglen >= valid_len) {
			struct virtchnl_irq_map_info *vimi =
			    (struct virtchnl_irq_map_info *)msg;

			if (vimi->num_vectors == 0 || vimi->num_vectors >
			    VIRTCHNL_OP_CONFIG_IRQ_MAP_MAX) {
				err_msg_format = true;
				break;
			}

			valid_len += (vimi->num_vectors *
				      sizeof(struct virtchnl_vector_map));
		}
		break;
	case VIRTCHNL_OP_ENABLE_QUEUES:
	case VIRTCHNL_OP_DISABLE_QUEUES:
		valid_len = sizeof(struct virtchnl_queue_select);
		break;
	case VIRTCHNL_OP_GET_MAX_RSS_QREGION:
		break;
	case VIRTCHNL_OP_ADD_ETH_ADDR:
	case VIRTCHNL_OP_DEL_ETH_ADDR:
		valid_len = sizeof(struct virtchnl_ether_addr_list);
		if (msglen >= valid_len) {
			struct virtchnl_ether_addr_list *veal =
			    (struct virtchnl_ether_addr_list *)msg;

			if (veal->num_elements == 0 || veal->num_elements >
			    VIRTCHNL_OP_ADD_DEL_ETH_ADDR_MAX) {
				err_msg_format = true;
				break;
			}

			valid_len += veal->num_elements *
			    sizeof(struct virtchnl_ether_addr);
		}
		break;
	case VIRTCHNL_OP_ADD_VLAN:
	case VIRTCHNL_OP_DEL_VLAN:
		valid_len = sizeof(struct virtchnl_vlan_filter_list);
		if (msglen >= valid_len) {
			struct virtchnl_vlan_filter_list *vfl =
			    (struct virtchnl_vlan_filter_list *)msg;

			if (vfl->num_elements == 0 || vfl->num_elements >
			    VIRTCHNL_OP_ADD_DEL_VLAN_MAX) {
				err_msg_format = true;
				break;
			}

			valid_len += vfl->num_elements * sizeof(u16);
		}
		break;
	case VIRTCHNL_OP_CONFIG_PROMISCUOUS_MODE:
		valid_len = sizeof(struct virtchnl_promisc_info);
		break;
	case VIRTCHNL_OP_GET_STATS:
		valid_len = sizeof(struct virtchnl_queue_select);
		break;
	case VIRTCHNL_OP_RDMA:
		/* These messages are opaque to us and will be validated in
		 * the RDMA client code. We just need to check for nonzero
		 * length. The firmware will enforce max length restrictions.
		 */
		if (msglen)
			valid_len = msglen;
		else
			err_msg_format = true;
		break;
	case VIRTCHNL_OP_RELEASE_RDMA_IRQ_MAP:
		break;
	case VIRTCHNL_OP_CONFIG_RDMA_IRQ_MAP:
		valid_len = sizeof(struct virtchnl_rdma_qvlist_info);
		if (msglen >= valid_len) {
			struct virtchnl_rdma_qvlist_info *qv =
				(struct virtchnl_rdma_qvlist_info *)msg;

			if (qv->num_vectors == 0 || qv->num_vectors >
			    VIRTCHNL_OP_CONFIG_RDMA_IRQ_MAP_MAX) {
				err_msg_format = true;
				break;
			}

			valid_len += ((qv->num_vectors - 1) *
				sizeof(struct virtchnl_rdma_qv_info));
		}
		break;
	case VIRTCHNL_OP_CONFIG_RSS_KEY:
		valid_len = sizeof(struct virtchnl_rss_key);
		if (msglen >= valid_len) {
			struct virtchnl_rss_key *vrk =
				(struct virtchnl_rss_key *)msg;

			if (vrk->key_len == 0) {
				/* zero length is allowed as input */
				break;
			}

			valid_len += vrk->key_len - 1;
		}
		break;
	case VIRTCHNL_OP_CONFIG_RSS_LUT:
		valid_len = sizeof(struct virtchnl_rss_lut);
		if (msglen >= valid_len) {
			struct virtchnl_rss_lut *vrl =
				(struct virtchnl_rss_lut *)msg;

			if (vrl->lut_entries == 0) {
				/* zero entries is allowed as input */
				break;
			}

			valid_len += vrl->lut_entries - 1;
		}
		break;
	case VIRTCHNL_OP_GET_RSS_HENA_CAPS:
		break;
	case VIRTCHNL_OP_SET_RSS_HENA:
		valid_len = sizeof(struct virtchnl_rss_hena);
		break;
	case VIRTCHNL_OP_ENABLE_VLAN_STRIPPING:
	case VIRTCHNL_OP_DISABLE_VLAN_STRIPPING:
		break;
	case VIRTCHNL_OP_REQUEST_QUEUES:
		valid_len = sizeof(struct virtchnl_vf_res_request);
		break;
	case VIRTCHNL_OP_ENABLE_CHANNELS:
		valid_len = sizeof(struct virtchnl_tc_info);
		if (msglen >= valid_len) {
			struct virtchnl_tc_info *vti =
				(struct virtchnl_tc_info *)msg;

			if (vti->num_tc == 0 || vti->num_tc >
			    VIRTCHNL_OP_ENABLE_CHANNELS_MAX) {
				err_msg_format = true;
				break;
			}

			valid_len += (vti->num_tc - 1) *
				     sizeof(struct virtchnl_channel_info);
		}
		break;
	case VIRTCHNL_OP_DISABLE_CHANNELS:
		break;
	case VIRTCHNL_OP_ADD_CLOUD_FILTER:
	case VIRTCHNL_OP_DEL_CLOUD_FILTER:
		valid_len = sizeof(struct virtchnl_filter);
		break;
	case VIRTCHNL_OP_GET_SUPPORTED_RXDIDS:
		break;
	case VIRTCHNL_OP_ADD_RSS_CFG:
	case VIRTCHNL_OP_DEL_RSS_CFG:
		valid_len = sizeof(struct virtchnl_rss_cfg);
		break;
	case VIRTCHNL_OP_ADD_FDIR_FILTER:
		valid_len = sizeof(struct virtchnl_fdir_add);
		break;
	case VIRTCHNL_OP_DEL_FDIR_FILTER:
		valid_len = sizeof(struct virtchnl_fdir_del);
		break;
	case VIRTCHNL_OP_GET_QOS_CAPS:
		break;
	case VIRTCHNL_OP_CONFIG_QUEUE_TC_MAP:
		valid_len = sizeof(struct virtchnl_queue_tc_mapping);
		if (msglen >= valid_len) {
			struct virtchnl_queue_tc_mapping *q_tc =
				(struct virtchnl_queue_tc_mapping *)msg;
			if (q_tc->num_tc == 0) {
				err_msg_format = true;
				break;
			}
			valid_len += (q_tc->num_tc - 1) *
					 sizeof(q_tc->tc[0]);
		}
		break;
	case VIRTCHNL_OP_GET_OFFLOAD_VLAN_V2_CAPS:
		break;
	case VIRTCHNL_OP_ADD_VLAN_V2:
	case VIRTCHNL_OP_DEL_VLAN_V2:
		valid_len = sizeof(struct virtchnl_vlan_filter_list_v2);
		if (msglen >= valid_len) {
			struct virtchnl_vlan_filter_list_v2 *vfl =
			    (struct virtchnl_vlan_filter_list_v2 *)msg;

			if (vfl->num_elements == 0 || vfl->num_elements >
			    VIRTCHNL_OP_ADD_DEL_VLAN_V2_MAX) {
				err_msg_format = true;
				break;
			}

			valid_len += (vfl->num_elements - 1) *
				sizeof(struct virtchnl_vlan_filter);
		}
		break;
	case VIRTCHNL_OP_ENABLE_VLAN_STRIPPING_V2:
	case VIRTCHNL_OP_DISABLE_VLAN_STRIPPING_V2:
	case VIRTCHNL_OP_ENABLE_VLAN_INSERTION_V2:
	case VIRTCHNL_OP_DISABLE_VLAN_INSERTION_V2:
	case VIRTCHNL_OP_ENABLE_VLAN_FILTERING_V2:
	case VIRTCHNL_OP_DISABLE_VLAN_FILTERING_V2:
		valid_len = sizeof(struct virtchnl_vlan_setting);
		break;
	case VIRTCHNL_OP_1588_PTP_GET_CAPS:
		valid_len = sizeof(struct virtchnl_ptp_caps);
		break;
	case VIRTCHNL_OP_1588_PTP_GET_TIME:
	case VIRTCHNL_OP_1588_PTP_SET_TIME:
		valid_len = sizeof(struct virtchnl_phc_time);
		break;
	case VIRTCHNL_OP_1588_PTP_ADJ_TIME:
		valid_len = sizeof(struct virtchnl_phc_adj_time);
		break;
	case VIRTCHNL_OP_1588_PTP_ADJ_FREQ:
		valid_len = sizeof(struct virtchnl_phc_adj_freq);
		break;
	case VIRTCHNL_OP_1588_PTP_TX_TIMESTAMP:
		valid_len = sizeof(struct virtchnl_phc_tx_tstamp);
		break;
	case VIRTCHNL_OP_1588_PTP_SET_PIN_CFG:
		valid_len = sizeof(struct virtchnl_phc_set_pin);
		break;
	case VIRTCHNL_OP_1588_PTP_GET_PIN_CFGS:
		break;
	case VIRTCHNL_OP_1588_PTP_EXT_TIMESTAMP:
		valid_len = sizeof(struct virtchnl_phc_ext_tstamp);
		break;
	case VIRTCHNL_OP_ENABLE_QUEUES_V2:
	case VIRTCHNL_OP_DISABLE_QUEUES_V2:
		valid_len = sizeof(struct virtchnl_del_ena_dis_queues);
		if (msglen >= valid_len) {
			struct virtchnl_del_ena_dis_queues *qs =
				(struct virtchnl_del_ena_dis_queues *)msg;
			if (qs->chunks.num_chunks == 0 ||
			    qs->chunks.num_chunks > VIRTCHNL_OP_ENABLE_DISABLE_DEL_QUEUES_V2_MAX) {
				err_msg_format = true;
				break;
			}
			valid_len += (qs->chunks.num_chunks - 1) *
				      sizeof(struct virtchnl_queue_chunk);
		}
		break;
	case VIRTCHNL_OP_MAP_QUEUE_VECTOR:
		valid_len = sizeof(struct virtchnl_queue_vector_maps);
		if (msglen >= valid_len) {
			struct virtchnl_queue_vector_maps *v_qp =
				(struct virtchnl_queue_vector_maps *)msg;
			if (v_qp->num_qv_maps == 0 ||
			    v_qp->num_qv_maps > VIRTCHNL_OP_MAP_UNMAP_QUEUE_VECTOR_MAX) {
				err_msg_format = true;
				break;
			}
			valid_len += (v_qp->num_qv_maps - 1) *
				      sizeof(struct virtchnl_queue_vector);
		}
		break;
	/* These are always errors coming from the VF. */
	case VIRTCHNL_OP_EVENT:
	case VIRTCHNL_OP_UNKNOWN:
	default:
		return VIRTCHNL_STATUS_ERR_PARAM;
	}
	/* few more checks */
	if (err_msg_format || valid_len != msglen)
		return VIRTCHNL_STATUS_ERR_OPCODE_MISMATCH;

	return 0;
}
#endif /* _VIRTCHNL_H_ */
