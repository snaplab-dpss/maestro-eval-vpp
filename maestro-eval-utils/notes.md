# Notes

## Commands

- Generate plugin tree graph in graphviz format: `show vlib graphviz file file.gv`
- Show interfaces: `show int`

## Typical NAT44-ei in2out trace

**Reproducing:**
1. Add trace: `trace add dpdk-input 1`
2. Show trace: `show trace`

```
------------------- Start of thread 0 vpp_main -------------------
No packets in trace buffer
------------------- Start of thread 1 vpp_wk_0 -------------------
Packet 1

18:00:26:018141: handoff_trace
  HANDED-OFF: from thread 4 trace index 0
18:00:26:018141: nat44-ei-in2out
  NAT44_IN2OUT_FAST_PATH: sw_if_index 1, next index 0, session 0
18:00:26:018146: ip4-lookup
  fib 0 dpo-idx 4 flow hash: 0x00000000
  UDP: 10.0.10.1 -> 1.2.3.4
    tos 0x00, ttl 64, length 28, checksum 0x62ca dscp CS0 ecn NON_ECN
    fragment id 0x0001
  UDP: 11570 -> 8080
    length 8, checksum 0x9b15
18:00:26:018148: ip4-rewrite
  tx_sw_if_index 2 dpo-idx 4 : ipv4 via 10.0.10.1 wan: mtu:9000 next:3 flags:[] deadbeefbabeb49691b3a7510800 flow hash: 0x00000000
  00000000: deadbeefbabeb49691b3a75108004500001c000100003f1163ca0a000a010102
  00000020: 03042d321f9000089b15000000000000000000000000000000000000
18:00:26:018149: wan-output
  wan
  IP4: b4:96:91:b3:a7:51 -> de:ad:be:ef:ba:be
  UDP: 10.0.10.1 -> 1.2.3.4
    tos 0x00, ttl 63, length 28, checksum 0x63ca dscp CS0 ecn NON_ECN
    fragment id 0x0001
  UDP: 11570 -> 8080
    length 8, checksum 0x9b15
18:00:26:018150: wan-tx
  wan tx queue 1
  buffer 0x97759f: current data 0, length 60, buffer-pool 1, ref-count 1, totlen-nifb 0, trace handle 0x1000000
                   ext-hdr-valid
                   l4-cksum-computed l4-cksum-correct natted l2-hdr-offset 0 l3-hdr-offset 14
  PKT MBUF: port 0, nb_segs 1, pkt_len 60
    buf_len 2176, data_len 60, ol_flags 0x400182, data_off 128, phys_addr 0x767d6840
    packet_type 0x291 l2_len 0 l3_len 0 outer_l2_len 0 outer_l3_len 0
    rss 0xcb1f81e7 fdir.hi 0x0 fdir.lo 0xcb1f81e7
    Packet Offload Flags
      PKT_RX_RSS_HASH (0x0002) RX packet with RSS hash result
      PKT_RX_IP_CKSUM_GOOD (0x0080) IP cksum of RX pkt. is valid
      PKT_RX_L4_CKSUM_GOOD (0x0100) L4 cksum of RX pkt. is valid
    Packet Types
      RTE_PTYPE_L2_ETHER (0x0001) Ethernet packet
      RTE_PTYPE_L3_IPV4_EXT_UNKNOWN (0x0090) IPv4 packet with or without extension headers
      RTE_PTYPE_L4_UDP (0x0200) UDP packet
  IP4: b4:96:91:b3:a7:51 -> de:ad:be:ef:ba:be
  UDP: 10.0.10.1 -> 1.2.3.4
    tos 0x00, ttl 63, length 28, checksum 0x63ca dscp CS0 ecn NON_ECN
    fragment id 0x0001
  UDP: 11570 -> 8080
    length 8, checksum 0x9b15
------------------- Start of thread 2 vpp_wk_1 -------------------
No packets in trace buffer
------------------- Start of thread 3 vpp_wk_2 -------------------
No packets in trace buffer
------------------- Start of thread 4 vpp_wk_3 -------------------
Packet 1

18:00:26:018112: dpdk-input
  lan rx queue 3
  buffer 0x97759f: current data 0, length 60, buffer-pool 1, ref-count 1, totlen-nifb 0, trace handle 0x4000000
                   ext-hdr-valid
                   l4-cksum-computed l4-cksum-correct
  PKT MBUF: port 0, nb_segs 1, pkt_len 60
    buf_len 2176, data_len 60, ol_flags 0x400182, data_off 128, phys_addr 0x767d6840
    packet_type 0x291 l2_len 0 l3_len 0 outer_l2_len 0 outer_l3_len 0
    rss 0xcb1f81e7 fdir.hi 0x0 fdir.lo 0xcb1f81e7
    Packet Offload Flags
      PKT_RX_RSS_HASH (0x0002) RX packet with RSS hash result
      PKT_RX_IP_CKSUM_GOOD (0x0080) IP cksum of RX pkt. is valid
      PKT_RX_L4_CKSUM_GOOD (0x0100) L4 cksum of RX pkt. is valid
    Packet Types
      RTE_PTYPE_L2_ETHER (0x0001) Ethernet packet
      RTE_PTYPE_L3_IPV4_EXT_UNKNOWN (0x0090) IPv4 packet with or without extension headers
      RTE_PTYPE_L4_UDP (0x0200) UDP packet
  IP4: 00:01:02:03:04:05 -> ca:ff:ee:ba:be:ee
  UDP: 8.8.8.8 -> 1.2.3.4
    tos 0x00, ttl 64, length 28, checksum 0x66bb dscp CS0 ecn NON_ECN
    fragment id 0x0001
  UDP: 1234 -> 8080
    length 8, checksum 0xc766
18:00:26:018130: ethernet-input
  frame: flags 0x3, hw-if-index 1, sw-if-index 1
  IP4: 00:01:02:03:04:05 -> ca:ff:ee:ba:be:ee
18:00:26:018133: ip4-input-no-checksum
  UDP: 8.8.8.8 -> 1.2.3.4
    tos 0x00, ttl 64, length 28, checksum 0x66bb dscp CS0 ecn NON_ECN
    fragment id 0x0001
  UDP: 1234 -> 8080
    length 8, checksum 0xc766
18:00:26:018135: ip4-sv-reassembly-feature
  [not-fragmented]
18:00:26:018136: nat44-ei-in2out-worker-handoff
  NAT44_EI_IN2OUT_WORKER_HANDOFF : next-worker 1 trace index 0
```

**Packet plugin-path traversed:**
1. dpdk-input
2. ethernet-input
3. ip4-input-no-checksum
4. ip4-sv-reassembly-feature
5. nat44-ei-in2out-worker-handoff
6. handoff_trace
7. nat44-ei-in2out
8. ip4-lookup
9. ip4-rewrite
10. wan-output
11. wan-tx
