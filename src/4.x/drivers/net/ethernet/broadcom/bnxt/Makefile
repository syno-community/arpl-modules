#!/usr/bin/make
# Makefile for building Linux Broadcom Gigabit ethernet driver as a module.
# $id$

LINUXSRC=$(KSRC)

ifeq ($(shell ls $(LINUXSRC)/include/uapi/linux > /dev/null 2>&1 && echo uapi),)
  UAPI=
else
  UAPI=uapi
endif

ifneq ($(shell grep -o "pci_enable_msix_range" $(LINUXSRC)/include/linux/pci.h),)
  DISTRO_CFLAG = -DHAVE_MSIX_RANGE
else
  DISTRO_CFLAG =
endif

ifneq ($(shell grep -o "msix_cap" $(LINUXSRC)/include/linux/pci.h),)
  ifeq ($(shell grep -o "pci_dev_rh1" $(LINUXSRC)/include/linux/pci.h),)
    DISTRO_CFLAG += -DHAVE_MSIX_CAP
  endif
endif

ifneq ($(shell grep -o "module_pci_driver" $(LINUXSRC)/include/linux/pci.h),)
  DISTRO_CFLAG += -DHAVE_MODULE_PCI_DRIVER
endif

ifneq ($(shell grep "hlist_for_each_entry_safe" $(LINUXSRC)/include/linux/list.h | grep "tpos" > /dev/null 2>&1 && echo tpos),)
  DISTRO_CFLAG += -DHAVE_OLD_HLIST
endif

ifneq ($(shell grep -o "csum_level" $(LINUXSRC)/include/linux/skbuff.h),)
  DISTRO_CFLAG += -DHAVE_CSUM_LEVEL
endif

ifneq ($(shell grep -o "build_skb" $(LINUXSRC)/include/linux/skbuff.h),)
  DISTRO_CFLAG += -DHAVE_BUILD_SKB
  ifneq ($(shell grep "build_skb" $(LINUXSRC)/include/linux/skbuff.h | grep "int frag_size" > /dev/null 2>&1 && echo frag_size),)
    DISTRO_CFLAG += -DHAVE_NEW_BUILD_SKB
  endif
endif

ifneq ($(shell grep -o "inner_network_offset" $(LINUXSRC)/include/linux/skbuff.h),)
  DISTRO_CFLAG += -DHAVE_INNER_NETWORK_OFFSET
endif

ifeq ($(shell grep -o "skb_frag_size" $(LINUXSRC)/include/linux/skbuff.h),)
  DISTRO_CFLAG += -DNO_SKB_FRAG_SIZE
endif

ifneq ($(shell grep -so "n_proto" $(LINUXSRC)/include/net/flow_keys.h),)
  DISTRO_CFLAG += -DHAVE_N_PROTO
endif

ifneq ($(shell grep -so "flow_keys" $(LINUXSRC)/include/net/flow_keys.h),)
  DISTRO_CFLAG += -DHAVE_FLOW_KEYS
endif

ifneq ($(shell grep -o "skb_get_hash_raw" $(LINUXSRC)/include/linux/skbuff.h),)
  DISTRO_CFLAG += -DHAVE_GET_HASH_RAW
endif

ifneq ($(shell grep -o "PKT_HASH_TYPE" $(LINUXSRC)/include/linux/skbuff.h),)
  DISTRO_CFLAG += -DHAVE_SKB_HASH_TYPE
endif

ifneq ($(shell grep -o "SKB_GSO_UDP_TUNNEL_CSUM" $(LINUXSRC)/include/linux/skbuff.h),)
  DISTRO_CFLAG += -DHAVE_SKB_GSO_UDP_TUNNEL_CSUM
else
ifneq ($(shell grep -o "SKB_GSO_UDP_TUNNEL" $(LINUXSRC)/include/linux/skbuff.h),)
  DISTRO_CFLAG += -DHAVE_SKB_GSO_UDP_TUNNEL
endif
endif

ifneq ($(shell grep -o "skb_frag_page" $(LINUXSRC)/include/linux/skbuff.h),)
  DISTRO_CFLAG += -DHAVE_SKB_FRAG_PAGE
endif

ifneq ($(shell grep "skb_checksum_none_assert" $(LINUXSRC)/include/linux/skbuff.h > /dev/null 2>&1 && echo skb_cs_none_assert),)
  DISTRO_CFLAG += -DHAVE_SKB_CHECKSUM_NONE_ASSERT
endif

ifneq ($(shell grep -o "xmit_more" $(LINUXSRC)/include/linux/skbuff.h),)
  DISTRO_CFLAG += -DHAVE_SKB_XMIT_MORE
endif

ifneq ($(shell grep -so "min_tx_rate" $(LINUXSRC)/include/$(UAPI)/linux/if_link.h),)
  DISTRO_CFLAG += -DHAVE_IFLA_TX_RATE
endif

ifneq ($(shell grep -so "IFLA_XDP_PROG_ID" $(LINUXSRC)/include/$(UAPI)/linux/if_link.h),)
  DISTRO_CFLAG += -DHAVE_IFLA_XDP_PROG_ID
endif

ifneq ($(shell grep -o "dma_set_mask_and_coherent" $(LINUXSRC)/include/linux/dma-mapping.h),)
  DISTRO_CFLAG += -DHAVE_SET_MASK_AND_COHERENT
endif

ifneq ($(shell grep -o "dma_set_coherent_mask" $(LINUXSRC)/include/linux/dma-mapping.h),)
  DISTRO_CFLAG += -DHAVE_SET_COHERENT_MASK
endif

ifneq ($(shell ls $(LINUXSRC)/include/linux/dma-attrs.h > /dev/null 2>&1 && echo dma_attrs),)
  DISTRO_CFLAG += -DHAVE_DMA_ATTRS_H
endif

ifneq ($(shell grep -o "dma_map_page_attrs" $(LINUXSRC)/include/linux/dma-mapping.h),)
  DISTRO_CFLAG += -DHAVE_DMA_MAP_PAGE_ATTRS
else
  ifneq ($(shell grep -so "dma_map_page_attrs" $(LINUXSRC)/include/asm-generic/dma-mapping-common.h),)
    DISTRO_CFLAG += -DHAVE_DMA_MAP_PAGE_ATTRS
  endif
endif

ifneq ($(shell grep -o "dma_zalloc_coherent" $(LINUXSRC)/include/linux/dma-mapping.h),)
  DISTRO_CFLAG += -DHAVE_DMA_ZALLOC_COHERENT
endif

ifneq ($(shell grep -o "ndo_add_vxlan_port" $(LINUXSRC)/include/linux/netdevice.h),)
  DISTRO_CFLAG += -DHAVE_NDO_ADD_VXLAN
endif

ifneq ($(shell grep -o "struct dev_addr_list" $(LINUXSRC)/include/linux/netdevice.h),)
  DISTRO_CFLAG += -DHAVE_DEV_ADDR_LIST
endif

ifneq ($(shell grep "netif_set_real_num_tx" $(LINUXSRC)/include/linux/netdevice.h > /dev/null 2>&1 && echo real_tx),)
  DISTRO_CFLAG += -DHAVE_NETIF_SET_REAL_NUM_TX
else
  DISTRO_CFLAG += -DVOID_NETIF_SET_NUM_TX
endif

ifneq ($(shell grep "netif_set_real_num_tx" $(LINUXSRC)/include/linux/netdevice.h | grep void > /dev/null 2>&1 && echo netif_set_real),)
  DISTRO_CFLAG += -DVOID_NETIF_SET_NUM_TX
endif

ifneq ($(shell grep -o "netdev_tx_sent_queue" $(LINUXSRC)/include/linux/netdevice.h),)
  DISTRO_CFLAG += -DHAVE_NETDEV_TX_QUEUE_CTRL
endif

ifneq ($(shell ls $(LINUXSRC)/include/net/flow_dissector.h > /dev/null 2>&1 && echo flow),)
  DISTRO_CFLAG += -DNEW_FLOW_KEYS -DHAVE_FLOW_KEYS
  ifneq ($(shell grep -so "static inline bool skb_flow_dissect_flow_keys" $(LINUXSRC)/include/linux/skbuff.h),)
    ifneq ($(shell grep -A 2 "static inline bool skb_flow_dissect_flow_keys" $(LINUXSRC)/include/linux/skbuff.h | grep -o "unsigned int flags"),)
      DISTRO_CFLAG += -DHAVE_NEW_FLOW_DISSECTOR_WITH_FLAGS
    endif
    ifneq ($(shell grep -o "FLOW_DIS_ENCAPSULATION" $(LINUXSRC)/include/net/flow_dissector.h),)
      DISTRO_CFLAG += -DHAVE_NEW_FLOW_DISSECTOR
    endif
  endif
endif

ifneq ($(shell ls $(LINUXSRC)/include/net/udp_tunnel.h > /dev/null 2>&1 && echo udp_tunnel),)
  DISTRO_CFLAG += -DHAVE_UDP_TUNNEL_H
endif

ifneq ($(shell grep -o "ether_addr_equal" $(LINUXSRC)/include/linux/etherdevice.h),)
  DISTRO_CFLAG += -DHAVE_ETHER_ADDR_EQUAL
endif

ifneq ($(shell grep -o "ether_addr_copy" $(LINUXSRC)/include/linux/etherdevice.h),)
  DISTRO_CFLAG += -DHAVE_ETHER_ADDR_COPY
endif

ifneq ($(shell grep -o "eth_broadcast_addr" $(LINUXSRC)/include/linux/etherdevice.h),)
  DISTRO_CFLAG += -DHAVE_ETH_BROADCAST_ADDR
endif

ifneq ($(shell grep -o "eth_get_headlen" $(LINUXSRC)/include/linux/etherdevice.h),)
  DISTRO_CFLAG += -DHAVE_ETH_GET_HEADLEN
endif

ifneq ($(shell grep -o "eth_hw_addr_random" $(LINUXSRC)/include/linux/etherdevice.h),)
  DISTRO_CFLAG += -DHAVE_ETH_HW_ADDR_RANDOM
endif

ifneq ($(shell grep -o "get_rxnfc" $(LINUXSRC)/include/linux/ethtool.h),)
  DISTRO_CFLAG += -DHAVE_RXNFC
  ifneq ($(shell grep -A 2 "get_rxnfc" $(LINUXSRC)/include/linux/ethtool.h | grep -o "void"),)
    DISTRO_CFLAG += -DHAVE_RXNFC_VOID
  endif
endif

ifneq ($(shell grep -o "get_rxfh_key_size" $(LINUXSRC)/include/linux/ethtool.h),)
  ifneq ($(shell grep -o "ETH_RSS_HASH_TOP" $(LINUXSRC)/include/linux/ethtool.h),)
    DISTRO_CFLAG += -DHAVE_GET_RXFH_KEY_SIZE
  endif
endif

ifneq ($(shell grep -o "get_rxfh_indir_size" $(LINUXSRC)/include/linux/ethtool.h),)
  DISTRO_CFLAG += -DHAVE_RXFH_INDIR_SIZE
endif

ifneq ($(shell grep -o "set_phys_id" $(LINUXSRC)/include/linux/ethtool.h),)
  DISTRO_CFLAG += -DHAVE_SET_PHYS_ID
endif

ifneq ($(shell grep -so "ETHTOOL_LINK_MODE_25000baseCR_Full_BIT" $(LINUXSRC)/include/$(UAPI)/linux/ethtool.h),)
  DISTRO_CFLAG += -DHAVE_ETHTOOL_GLINKSETTINGS_25G
endif

ifneq ($(shell grep -so "ethtool_tcpip6_spec" $(LINUXSRC)/include/$(UAPI)/linux/ethtool.h),)
  DISTRO_CFLAG += -DHAVE_ETHTOOL_IP6_SPEC
endif

ifeq ($(shell grep -o "rx_cpu_rmap" $(LINUXSRC)/include/linux/netdevice.h),)
  DISTRO_CFLAG += -DNO_NETDEV_CPU_RMAP
else
  ifneq ($(shell grep -o "irq_run_affinity_notifiers" $(LINUXSRC)/include/linux/interrupt.h),)
    DISTRO_CFLAG += -DNO_NETDEV_CPU_RMAP
  endif
endif

ifneq ($(shell grep -o "hw_features" $(LINUXSRC)/include/linux/netdevice.h),)
  ifeq ($(shell grep -o "get_netdev_hw_features" $(LINUXSRC)/include/linux/netdevice.h),)
    DISTRO_CFLAG += -DNETDEV_HW_FEATURES
  endif
endif

ifneq ($(shell grep -o "hw_enc_features" $(LINUXSRC)/include/linux/netdevice.h),)
  DISTRO_CFLAG += -DNETDEV_HW_ENC_FEATURES
endif

ifneq ($(shell grep -o "sriov_configure" $(LINUXSRC)/include/linux/pci.h),)
  DISTRO_CFLAG += -DPCIE_SRIOV_CONFIGURE
  ifneq ($(shell grep -A 2 "pci_driver_rh" $(LINUXSRC)/include/linux/pci.h | \
                 grep -o "sriov_configure"),)
    DISTRO_CFLAG += -DSRIOV_CONF_DEF_IN_PCI_DRIVER_RH
  endif
endif

ifneq ($(shell grep -o "pci_vfs_assigned" $(LINUXSRC)/include/linux/pci.h),)
  DISTRO_CFLAG += -DHAVE_PCI_VFS_ASSIGNED
endif

ifneq ($(shell grep -o "pci_num_vf" $(LINUXSRC)/include/linux/pci.h),)
  DISTRO_CFLAG += -DHAVE_PCI_NUM_VF
endif

ifneq ($(shell grep -o "ndo_fix_features" $(LINUXSRC)/include/linux/netdevice.h),)
  ifeq ($(shell grep -o "net_device_ops_ext" $(LINUXSRC)/include/linux/netdevice.h),)
    DISTRO_CFLAG += -DNETDEV_FEATURE_CONTROL
  endif
  ifneq ($(shell grep -o "net_device_ops_extended" $(LINUXSRC)/include/linux/netdevice.h),)
    DISTRO_CFLAG += -DNETDEV_FEATURE_CONTROL
  endif
endif

ifneq ($(shell grep -o "ndo_rx_flow_steer" $(LINUXSRC)/include/linux/netdevice.h),)
  ifeq ($(shell grep -o "netdev_rfs_info" $(LINUXSRC)/include/linux/netdevice.h),)
    DISTRO_CFLAG += -DNETDEV_RX_FLOW_STEER
  endif
endif

ifneq ($(shell grep -o "ndo_busy_poll" $(LINUXSRC)/include/linux/netdevice.h),)
  ifeq ($(shell grep -o "net_device_extended" $(LINUXSRC)/include/linux/netdevice.h),)
    DISTRO_CFLAG += -DNETDEV_BUSY_POLL
  endif
endif

ifneq ($(shell grep -o "ndo_get_stats64" $(LINUXSRC)/include/linux/netdevice.h),)
  ifeq ($(shell grep -o "net_device_ops_ext" $(LINUXSRC)/include/linux/netdevice.h),)
    DISTRO_CFLAG += -DNETDEV_GET_STATS64
  endif
  ifneq ($(shell grep -o "net_device_ops_extended" $(LINUXSRC)/include/linux/netdevice.h),)
    DISTRO_CFLAG += -DNETDEV_GET_STATS64
  endif
  ifneq ($(shell grep "ndo_get_stats64" $(LINUXSRC)/include/linux/netdevice.h | grep -o "void"),)
    DISTRO_CFLAG += -DNETDEV_GET_STATS64_VOID
  endif
endif

ifneq ($(shell grep -o "ndo_get_vf_config" $(LINUXSRC)/include/linux/netdevice.h),)
  DISTRO_CFLAG += -DHAVE_NDO_GET_VF_CONFIG
endif

ifneq ($(shell grep -A 2 "ndo_bridge_getlink" $(LINUXSRC)/include/linux/netdevice.h | grep -o "nlflags"),)
  ifneq ($(shell grep -A 3 "ndo_dflt_bridge_getlink" $(LINUXSRC)/include/linux/rtnetlink.h | grep -o "filter_mask"),)
    DISTRO_CFLAG += -DHAVE_NDO_BRIDGE_GETLINK
  endif
endif

ifneq ($(shell grep -o "ndo_set_vf_link_state" $(LINUXSRC)/include/linux/netdevice.h),)
  ifeq ($(shell grep -o "net_device_ops_ext" $(LINUXSRC)/include/linux/netdevice.h),)
    DISTRO_CFLAG += -DHAVE_NDO_SET_VF_LINK_STATE
  endif
  ifneq ($(shell grep -o "net_device_ops_extended" $(LINUXSRC)/include/linux/netdevice.h),)
    DISTRO_CFLAG += -DHAVE_NDO_SET_VF_LINK_STATE
  endif
endif

ifneq ($(shell grep -o "ndo_set_vf_spoofchk" $(LINUXSRC)/include/linux/netdevice.h),)
  ifeq ($(shell grep -o "net_device_ops_ext" $(LINUXSRC)/include/linux/netdevice.h),)
    DISTRO_CFLAG += -DHAVE_VF_SPOOFCHK
  endif
  ifneq ($(shell grep -o "net_device_ops_extended" $(LINUXSRC)/include/linux/netdevice.h),)
    DISTRO_CFLAG += -DHAVE_VF_SPOOFCHK
  endif
endif

ifneq ($(shell grep -A 1 "ndo_set_vf_vlan" $(LINUXSRC)/include/linux/netdevice.h | grep -o "proto"),)
  ifeq ($(shell grep -o "net_device_ops_extended" $(LINUXSRC)/include/linux/netdevice.h),)
    DISTRO_CFLAG += -DNEW_NDO_SET_VF_VLAN
  endif
endif

ifneq ($(shell grep -o "ndo_set_vf_vlan_rh73" $(LINUXSRC)/include/linux/netdevice.h),)
  DISTRO_CFLAG += -DHAVE_NDO_SET_VF_VLAN_RH73
endif

ifneq ($(shell grep -o "ndo_set_vf_trust" $(LINUXSRC)/include/linux/netdevice.h),)
  ifeq ($(shell grep -o "net_device_ops_ext" $(LINUXSRC)/include/linux/netdevice.h),)
    DISTRO_CFLAG += -DHAVE_NDO_SET_VF_TRUST
  endif
endif

ifneq ($(shell grep -o "ndo_setup_tc" $(LINUXSRC)/include/linux/netdevice.h),)
  DISTRO_CFLAG += -DHAVE_SETUP_TC
  ifneq ($(shell grep -o "struct tc_to_netdev" $(LINUXSRC)/include/linux/netdevice.h),)
    DISTRO_CFLAG += -DHAVE_TC_TO_NETDEV
    ifneq ($(shell grep -o "struct tc_mqprio_qopt" $(LINUXSRC)/include/linux/netdevice.h),)
      DISTRO_CFLAG += -DHAVE_MQPRIO_QOPT
    endif
    ifneq ($(shell grep -A 1 "ndo_setup_tc" $(LINUXSRC)/include/linux/netdevice.h | grep -o "u32 chain_index"),)
      DISTRO_CFLAG += -DHAVE_CHAIN_INDEX
    endif
  endif
  ifneq ($(shell grep -o "enum tc_setup_type" $(LINUXSRC)/include/linux/netdevice.h),)
    DISTRO_CFLAG += -DHAVE_TC_SETUP_TYPE
  endif
  ifneq ($(shell grep -so "tc_cls_flower_offload" $(LINUXSRC)/include/net/pkt_cls.h),)
    DISTRO_CFLAG += -DHAVE_TC_CLS_FLOWER_OFFLOAD
  endif
  ifneq ($(shell grep -o "TC_SETUP_BLOCK" $(LINUXSRC)/include/linux/netdevice.h),)
    DISTRO_CFLAG += -DHAVE_TC_SETUP_BLOCK
  endif
endif

ifneq ($(shell grep -so "FLOW_DISSECTOR_KEY_ICMP" $(LINUXSRC)/include/net/flow_dissector.h),)
  DISTRO_CFLAG += -DHAVE_FLOW_DISSECTOR_KEY_ICMP
endif

ifneq ($(shell grep -so "rhashtable" $(LINUXSRC)/include/linux/rhashtable.h),)
    DISTRO_CFLAG += -DHAVE_RHASHTABLE
endif

ifneq ($(shell grep -so "tcf_exts_to_list" $(LINUXSRC)/include/net/pkt_cls.h),)
    DISTRO_CFLAG += -DHAVE_TCF_EXTS_TO_LIST
endif

ifneq ($(shell grep -so "tcf_exts_stats_update" $(LINUXSRC)/include/net/pkt_cls.h),)
    DISTRO_CFLAG += -DHAVE_TCF_STATS_UPDATE
endif

ifneq ($(shell grep -so "tcf_exts_has_actions" $(LINUXSRC)/include/net/pkt_cls.h),)
    DISTRO_CFLAG += -DHAVE_TCF_EXTS_HAS_ACTIONS
endif

ifneq ($(shell grep -so "is_tcf_tunnel_set" $(LINUXSRC)/include/net/tc_act/tc_tunnel_key.h),)
  DISTRO_CFLAG += -DHAVE_TCF_TUNNEL
endif

ifneq ($(shell grep -o "netdev_get_num_tc" $(LINUXSRC)/include/linux/netdevice.h),)
  DISTRO_CFLAG += -DHAVE_GET_NUM_TC
endif

ifneq ($(shell grep -so "netdev_features_t" $(LINUXSRC)/include/linux/netdev_features.h ||	\
	 grep -o "netdev_features_t" $(LINUXSRC)/include/linux/netdevice.h),)
  DISTRO_CFLAG += -DHAVE_NETDEV_FEATURES_T
endif

ifneq ($(shell grep -o "ndo_fix_features" $(LINUXSRC)/include/linux/netdevice.h),)
  DISTRO_CFLAG += -DHAVE_NDO_FIX_FEATURES
endif

ifneq ($(shell grep -o "netif_set_real_num_rx" $(LINUXSRC)/include/linux/netdevice.h),)
  DISTRO_CFLAG += -DHAVE_NETIF_SET_REAL_NUM_RX
endif

ifneq ($(shell grep -o "netif_get_num_default_rss_queues" $(LINUXSRC)/include/linux/netdevice.h),)
  DISTRO_CFLAG += -DHAVE_NETIF_GET_DEFAULT_RSS
endif

ifneq ($(shell grep -o "ndo_vlan_rx_register" $(LINUXSRC)/include/linux/netdevice.h),)
  DISTRO_CFLAG += -DHAVE_VLAN_RX_REGISTER
endif

ifneq ($(shell grep -o "ndo_xdp" $(LINUXSRC)/include/linux/netdevice.h),)
  DISTRO_CFLAG += -DHAVE_NDO_XDP
  ifneq ($(shell ls $(LINUXSRC)/include/net/bpf_trace.h > /dev/null 2>&1 && echo bpf_trace),)
    DISTRO_CFLAG += -DHAVE_BPF_TRACE
  endif
  ifneq ($(shell grep -o "ndo_xdp_xmit" $(LINUXSRC)/include/linux/netdevice.h),)
    DISTRO_CFLAG += -DHAVE_NDO_XDP_XMIT
  endif
  ifneq ($(shell grep -o "xdp_set_data_meta_invalid" $(LINUXSRC)/include/linux/filter.h),)
    DISTRO_CFLAG += -DHAVE_XDP_SET_DATA_META_INVALID
  endif
endif

ifneq ($(shell grep -o "netdev_name" $(LINUXSRC)/include/linux/netdevice.h),)
  DISTRO_CFLAG += -DHAVE_NETDEV_NAME
endif

ifneq ($(shell grep -o "netdev_update_features" $(LINUXSRC)/include/linux/netdevice.h),)
  DISTRO_CFLAG += -DHAVE_NETDEV_UPDATE_FEATURES
endif

ifneq ($(shell grep -o "dev_port" $(LINUXSRC)/include/linux/netdevice.h),)
  DISTRO_CFLAG += -DHAVE_DEV_PORT
endif

ifneq ($(shell grep -o "napi_hash_add" $(LINUXSRC)/include/linux/netdevice.h),)
  DISTRO_CFLAG += -DHAVE_NAPI_HASH_ADD
endif

ifneq ($(shell grep -o "napi_hash_del" $(LINUXSRC)/include/linux/netdevice.h),)
  DISTRO_CFLAG += -DHAVE_NAPI_HASH_DEL
endif

ifneq ($(shell grep "napi_complete_done" $(LINUXSRC)/include/linux/netdevice.h | grep -o "bool"),)
  DISTRO_CFLAG += -DHAVE_NEW_NAPI_COMPLETE_DONE
endif

ifneq ($(shell grep -o "min_mtu" $(LINUXSRC)/include/linux/netdevice.h),)
  DISTRO_CFLAG += -DHAVE_MIN_MTU
endif

ifneq ($(shell grep -o "prandom_bytes" $(LINUXSRC)/include/linux/random.h),)
  DISTRO_CFLAG += -DHAVE_PRANDOM_BYTES
endif

ifneq ($(shell grep -o "tcp_v6_check" $(LINUXSRC)/include/net/ip6_checksum.h),)
  DISTRO_CFLAG += -DHAVE_TCP_V6_CHECK
endif

ifneq ($(shell grep -o "usleep_range" $(LINUXSRC)/include/linux/delay.h),)
  DISTRO_CFLAG += -DHAVE_USLEEP_RANGE
endif

ifneq ($(shell grep -o "vzalloc" $(LINUXSRC)/include/linux/vmalloc.h),)
  DISTRO_CFLAG += -DHAVE_VZALLOC
endif

ifneq ($(shell grep -o "pcie_get_minimum_link" $(LINUXSRC)/include/linux/pci.h),)
  DISTRO_CFLAG += -DHAVE_PCIE_GET_MINIMUM_LINK
endif

ifneq ($(shell grep -o "pcie_capability_read_word" $(LINUXSRC)/include/linux/pci.h),)
  DISTRO_CFLAG += -DHAVE_PCIE_CAPABILITY_READ_WORD
endif

ifneq ($(shell grep -o "PCIE_SPEED_2_5GT" $(LINUXSRC)/include/linux/pci.h),)
  DISTRO_CFLAG += -DHAVE_PCIE_BUS_SPEED
endif

ifneq ($(shell grep -o "pci_is_bridge" $(LINUXSRC)/include/linux/pci.h),)
  DISTRO_CFLAG += -DHAVE_PCI_IS_BRIDGE
endif

ifneq ($(shell grep -o "pci_physfn" $(LINUXSRC)/include/linux/pci.h),)
  DISTRO_CFLAG += -DHAVE_PCI_PHYSFN
endif

ifneq ($(shell ls $(LINUXSRC)/include/$(UAPI)/linux/net_tstamp.h > /dev/null 2>&1 && echo net_tstamp),)
  ifneq ($(shell ls $(LINUXSRC)/include/linux/timecounter.h > /dev/null 2>&1 && echo timecounter),)
    ifneq ($(shell ls $(LINUXSRC)/include/linux/timekeeping.h > /dev/null 2>&1 && echo timekeeping),)
      ifneq ($(shell grep -o "HWTSTAMP_FILTER_PTP_V2_EVENT" $(LINUXSRC)/include/$(UAPI)/linux/net_tstamp.h),)
        DISTRO_CFLAG += -DHAVE_IEEE1588_SUPPORT
      endif
    endif
  endif
endif

ifneq ($(shell grep -s "devlink_ops" $(LINUXSRC)/include/net/devlink.h),)
  DISTRO_CFLAG += -DHAVE_DEVLINK
endif

# Check if the file exists or not
ifneq ($(shell grep -s "switchdev_ops" $(LINUXSRC)/include/net/switchdev.h),)
  DISTRO_CFLAG += -DHAVE_SWITCHDEV
endif

ifneq ($(shell grep -s "METADATA_HW_PORT_MUX" $(LINUXSRC)/include/net/dst_metadata.h),)
  DISTRO_CFLAG += -DHAVE_METADATA_HW_PORT_MUX
endif

ifneq ($(shell grep -so "(*ieee_delapp)" $(LINUXSRC)/include/net/dcbnl.h),)
  DISTRO_CFLAG += -DHAVE_IEEE_DELAPP
endif

ifneq ($(shell grep -o cpumask_local_spread $(LINUXSRC)/include/linux/cpumask.h),)
  DISTRO_CFLAG += -DHAVE_CPUMASK_LOCAL_SPREAD
endif

ifneq ($(shell grep -o cpumask_set_cpu_local_first $(LINUXSRC)/include/linux/cpumask.h),)
  DISTRO_CFLAG += -DHAVE_CPUMASK_LOCAL_FIRST
endif

EXTRA_CFLAGS += ${DISTRO_CFLAG} -g -DCHIMP_FW -D__LINUX -DCONFIG_BNXT_SRIOV -DCONFIG_BNXT_DCB -DCONFIG_BNXT_FLASHDEV -DHSI_DBG_DISABLE -DCONFIG_BNXT_LFC -DBNXT_TIMER_POLLED_VF

cflags-y += $(EXTRA_CFLAGS)

BCM_DRV = bnxt_en.ko


obj-$(CONFIG_BNXT) += bnxt_en.o
bnxt_en-y := bnxt.o bnxt_ethtool.o bnxt_sriov.o bnxt_dcb.o bnxt_ulp.o bnxt_xdp.o bnxt_ptp.o bnxt_vfr.o bnxt_tc.o bnxt_devlink.o bnxt_lfc.o#decode_hsi.o
