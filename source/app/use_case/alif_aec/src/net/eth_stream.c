/* Copyright (C) 2026 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#if defined(ETH_STREAM)

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "RTE_Components.h"
#include CMSIS_device_header
#include "Driver_ETH_MAC.h"
#include "delay.h"
#include "gpio.h"
#include "pinconf.h"
#include "platform_drivers.h"
#include "timer_alif.h"

#if defined(__ARM_FEATURE_MVE) && (__ARM_FEATURE_MVE & 1)
#include <arm_mve.h>
#endif

#include "eth_stream.h"

/* Build time configuration, normally set from usecase.cmake. */
#ifndef ETH_STREAM_MAC
#define ETH_STREAM_MAC 0x02, 0x00, 0x00, 0xAE, 0xC0, 0x01
#endif
#ifndef ETH_STREAM_SRC_IP
#define ETH_STREAM_SRC_IP 192, 168, 1, 10
#endif
/* Unicast (resolved with ARP, must be on the same subnet), multicast
 * (224.0.0.0 - 239.255.255.255) or 255.255.255.255. */
#ifndef ETH_STREAM_DEST_IP
#define ETH_STREAM_DEST_IP 192, 168, 1, 100
#endif
#ifndef ETH_STREAM_PORT
#define ETH_STREAM_PORT 5004
#endif
/* 160 frames = 10 ms at 16 kHz. */
#ifndef ETH_STREAM_FRAMES_PER_PACKET
#define ETH_STREAM_FRAMES_PER_PACKET 160
#endif
/* Dynamic RTP payload type, there is no static one for L16 at 16 kHz. */
#ifndef ETH_STREAM_RTP_PT
#define ETH_STREAM_RTP_PT 96
#endif
/* DSCP EF (46), the usual class for real-time audio. */
#ifndef ETH_STREAM_TOS
#define ETH_STREAM_TOS 0xB8
#endif

/* PHY reset line (active low). AppKit-e8: RTL8201F PHYRSTB on P11_6. The pin
 * table muxes it to the MAC's ETH_RST output, which stays low and keeps the
 * PHY in reset. Without the PHY there is no 50 MHz RMII reference clock and
 * the MAC never leaves reset, so drive the line as a GPIO instead. */
#ifndef ETH_STREAM_PHY_RST_PORT
#define ETH_STREAM_PHY_RST_PORT     PORT_11
#define ETH_STREAM_PHY_RST_PIN      PIN_6
#define ETH_STREAM_PHY_RST_GPIO     GPIO11
#define ETH_STREAM_PHY_RST_GPIO_IDX 11
#endif

#define ETH_STREAM_CHANNELS 2
#define BYTES_PER_FRAME     (ETH_STREAM_CHANNELS * sizeof(int16_t))

#define ETH_HDR_LEN 14
#define IP_HDR_LEN  20
#define UDP_HDR_LEN 8
#define RTP_HDR_LEN 12
#define ARP_LEN     28
#define IP_OFS      ETH_HDR_LEN
#define UDP_OFS     (IP_OFS + IP_HDR_LEN)
#define RTP_OFS     (UDP_OFS + UDP_HDR_LEN)
#define PAYLOAD_OFS (RTP_OFS + RTP_HDR_LEN)

#define ETHERTYPE_IPV4 0x0800
#define ETHERTYPE_ARP  0x0806
#define ETHERTYPE_VLAN 0x8100
#define ARP_OP_REQUEST 1
#define ARP_OP_REPLY   2

#define PAYLOAD_MAX (ETH_STREAM_FRAMES_PER_PACKET * BYTES_PER_FRAME)

_Static_assert(IP_HDR_LEN + UDP_HDR_LEN + RTP_HDR_LEN + PAYLOAD_MAX <= 1500,
               "ETH_STREAM_FRAMES_PER_PACKET does not fit in the Ethernet MTU");

/* Retry interval for ARP requests while the receiver is not resolved. */
#define ARP_RETRY_MS 500U

/* Receive diagnostics: printed once per second while the receiver is not
 * resolved, and once when it is. */
#ifndef ETH_STREAM_DIAG
#define ETH_STREAM_DIAG 1
#endif

#if ETH_STREAM_DIAG
#define DIAG_PERIOD_MS 1000U
/* PHY address of the AppKit-e8 RTL8201F. */
#define DIAG_PHY_ADDR  1U

/* Frames seen by rx_poll(). The MAC passes frames with errors and undersized
 * frames up (MTL FEP/FUP set by the pack driver), so these cover them too;
 * this MAC has no MMC counters. */
static struct {
    uint32_t frames;
    uint32_t errors;
    uint32_t vlan;
    uint32_t arp;
    uint32_t arp_from_dest;
    uint32_t other;
    uint16_t last_type;
    uint32_t last_len;
    uint8_t last_src[6];
} rx_stats;
static uint32_t diag_last_ms;
#endif

#if defined(SE_SERVICES_SUPPORT)
/* Run profile applied by the platform at boot, see platform_drivers.c. */
extern run_profile_t default_runprof;
#endif

extern ARM_DRIVER_ETH_MAC Driver_ETH_MAC0;
static ARM_DRIVER_ETH_MAC* const mac = &Driver_ETH_MAC0;

static const uint8_t our_mac[6] = {ETH_STREAM_MAC};
static const uint8_t our_ip[4] = {ETH_STREAM_SRC_IP};
static const uint8_t dest_ip[4] = {ETH_STREAM_DEST_IP};
static const uint8_t bcast_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static uint8_t dest_mac[6];
static bool dest_mac_valid;
static bool arp_requested;
static uint32_t arp_last_ms;
static bool inited;

/* The ETH IRQ (TX complete) and eth_stream_send() both send frames. Main only
 * touches the state below with the ETH IRQ masked. */

/* Frame being built, the MAC driver copies it into its own TX buffers. */
static uint8_t frame_buf[PAYLOAD_OFS + PAYLOAD_MAX];
/* Constant Ethernet/IP/UDP/RTP headers, fields that change are patched per
 * packet. IP and UDP checksums are inserted by the MAC. */
static uint8_t hdr_template[PAYLOAD_OFS];
static uint16_t ip_id;
static uint16_t rtp_seq;
static uint32_t rtp_ts;
static bool rtp_marker;

/* Audio not sent yet: points into the caller's buffer. */
static const int16_t* tx_src;
static uint32_t tx_left;
static uint32_t tx_ts;

/* Debug counters, frames are stereo audio frames. */
static struct {
    uint32_t packets_sent;
    uint32_t frames_sent;
    uint32_t frames_dropped;
} stats;

static void put16(uint8_t* p, uint16_t v)
{
    p[0] = (uint8_t)(v >> 8);
    p[1] = (uint8_t)v;
}

static void put32(uint8_t* p, uint32_t v)
{
    put16(p, (uint16_t)(v >> 16));
    put16(p + 2, (uint16_t)v);
}

static uint16_t get16(const uint8_t* p)
{
    return (uint16_t)((p[0] << 8) | p[1]);
}

static void put_eth_header(uint8_t* f, const uint8_t* dst, uint16_t type)
{
    memcpy(&f[0], dst, 6);
    memcpy(&f[6], our_mac, 6);
    put16(&f[12], type);
}

/* Copy samples converting them to network (big-endian) byte order. */
static void copy_be16(uint8_t* dst, const int16_t* src, uint32_t count)
{
    const uint8_t* s = (const uint8_t*)src;
    const uint32_t bytes = count * sizeof(int16_t);
    uint32_t i = 0;

#if defined(__ARM_FEATURE_MVE) && (__ARM_FEATURE_MVE & 1)
    for (; i + 16U <= bytes; i += 16U) {
        vst1q_u8(&dst[i], vrev16q_u8(vld1q_u8(&s[i])));
    }
#endif
    for (; i < bytes; i += 2U) {
        dst[i] = s[i + 1U];
        dst[i + 1U] = s[i];
    }
}

static void phy_reset(void)
{
#if SOC_FEAT_GPIO_HAS_CLOCK_ENABLE
    CLKCTL_PER_SLV->GPIO_CTRL[ETH_STREAM_PHY_RST_GPIO_IDX] |= (1U << 16); /* GPIO clock enable */
#endif
    gpio_set_value_low(ETH_STREAM_PHY_RST_GPIO, ETH_STREAM_PHY_RST_PIN);
    gpio_set_direction_output(ETH_STREAM_PHY_RST_GPIO, ETH_STREAM_PHY_RST_PIN);
    pinconf_set(ETH_STREAM_PHY_RST_PORT, ETH_STREAM_PHY_RST_PIN, PINMUX_ALTERNATE_FUNCTION_0,
                PADCTRL_OUTPUT_DRIVE_STRENGTH_4MA);
    /* RTL8201F: reset low >= 10 ms, then wait for its PLL and the RMII
     * reference clock output to settle. */
    sleep_or_wait_msec(10);
    gpio_set_value_high(ETH_STREAM_PHY_RST_GPIO, ETH_STREAM_PHY_RST_PIN);
    sleep_or_wait_msec(100);
}

static void dest_mac_init(void)
{
    if (dest_ip[0] >= 224U && dest_ip[0] <= 239U) {
        /* IPv4 multicast MAC, RFC 1112. */
        const uint8_t mc[6] = {0x01, 0x00, 0x5E, (uint8_t)(dest_ip[1] & 0x7FU), dest_ip[2],
                               dest_ip[3]};
        memcpy(dest_mac, mc, sizeof(dest_mac));
        dest_mac_valid = true;
    } else if (dest_ip[0] == 255U && dest_ip[1] == 255U && dest_ip[2] == 255U &&
               dest_ip[3] == 255U) {
        memcpy(dest_mac, bcast_mac, sizeof(dest_mac));
        dest_mac_valid = true;
    } else {
        dest_mac_valid = false;
    }
}

static void hdr_template_init(void)
{
    uint8_t* ip = &hdr_template[IP_OFS];
    uint8_t* udp = &hdr_template[UDP_OFS];
    uint8_t* rtp = &hdr_template[RTP_OFS];

    memset(hdr_template, 0, sizeof(hdr_template));
    put_eth_header(hdr_template, dest_mac, ETHERTYPE_IPV4);

    ip[0] = 0x45; /* IPv4, 20 byte header */
    ip[1] = ETH_STREAM_TOS;
    put16(&ip[6], 0x4000); /* don't fragment */
    ip[8] = 64;            /* TTL */
    ip[9] = 17;            /* UDP */
    memcpy(&ip[12], our_ip, 4);
    memcpy(&ip[16], dest_ip, 4);

    put16(&udp[0], ETH_STREAM_PORT);
    put16(&udp[2], ETH_STREAM_PORT);

    rtp[0] = 0x80; /* version 2 */
    rtp[1] = ETH_STREAM_RTP_PT;
    /* SSRC from our MAC address so that it is stable between boots. */
    put32(&rtp[8], ((uint32_t)our_mac[2] << 24) | ((uint32_t)our_mac[3] << 16) |
                       ((uint32_t)our_mac[4] << 8) | our_mac[5]);
}

static void arp_send(uint16_t op, const uint8_t* eth_dst, const uint8_t* tha, const uint8_t* tpa)
{
    uint8_t f[ETH_HDR_LEN + ARP_LEN];
    uint8_t* arp = &f[ETH_HDR_LEN];

    put_eth_header(f, eth_dst, ETHERTYPE_ARP);
    put16(&arp[0], 1); /* Ethernet */
    put16(&arp[2], ETHERTYPE_IPV4);
    arp[4] = 6;
    arp[5] = 4;
    put16(&arp[6], op);
    memcpy(&arp[8], our_mac, 6);
    memcpy(&arp[14], our_ip, 4);
    memcpy(&arp[18], tha, 6);
    memcpy(&arp[24], tpa, 4);

    /* Dropped if the TX ring is full, ARP is retried anyway. */
    mac->SendFrame(f, sizeof(f), 0);
}

/* Handle received ARP: learn the receiver's MAC and answer requests for our
 * address. Everything else is dropped. */
static void rx_poll(void)
{
    /* Minimum Ethernet frame without FCS: room for an 802.1Q tag and ARP. */
    uint8_t f[60];

    for (;;) {
        const uint32_t len = mac->GetRxFrameSize();
        if (len == 0U) {
            break;
        }
        if (len == (uint32_t)ARM_DRIVER_ERROR) {
            mac->ReadFrame(NULL, 0); /* drop a bad frame */
#if ETH_STREAM_DIAG
            rx_stats.errors++;
#endif
            continue;
        }
        /* Only the start of the frame is needed, the rest is dropped. */
        const uint32_t n = (len < sizeof(f)) ? len : sizeof(f);
        mac->ReadFrame(f, n);

        /* Skip an 802.1Q tag, some host NIC drivers add a priority tag. */
        uint32_t type_ofs = 12U;
        if (n >= 16U && get16(&f[type_ofs]) == ETHERTYPE_VLAN) {
            type_ofs += 4U;
#if ETH_STREAM_DIAG
            rx_stats.vlan++;
#endif
        }
#if ETH_STREAM_DIAG
        rx_stats.frames++;
        rx_stats.last_len = len;
        memcpy(rx_stats.last_src, &f[6], 6);
        rx_stats.last_type = (n >= type_ofs + 2U) ? get16(&f[type_ofs]) : 0U;
#endif
        if (n < type_ofs + 2U + ARP_LEN || get16(&f[type_ofs]) != ETHERTYPE_ARP) {
#if ETH_STREAM_DIAG
            rx_stats.other++;
#endif
            continue;
        }

        const uint8_t* arp = &f[type_ofs + 2U];
        if (get16(&arp[0]) != 1 || get16(&arp[2]) != ETHERTYPE_IPV4) {
            continue;
        }
        const uint8_t* sha = &arp[8];
        const uint8_t* spa = &arp[14];
        const uint8_t* tpa = &arp[24];
#if ETH_STREAM_DIAG
        rx_stats.arp++;
        if (memcmp(spa, dest_ip, 4) == 0) {
            rx_stats.arp_from_dest++;
        }
#endif

        if (!dest_mac_valid && memcmp(spa, dest_ip, 4) == 0) {
            memcpy(dest_mac, sha, 6);
            dest_mac_valid = true;
            hdr_template_init();
            printf("ETH stream: receiver at %02X:%02X:%02X:%02X:%02X:%02X\n", sha[0], sha[1],
                   sha[2], sha[3], sha[4], sha[5]);
        }
        if (get16(&arp[6]) == ARP_OP_REQUEST && memcmp(tpa, our_ip, 4) == 0) {
            arp_send(ARP_OP_REPLY, sha, sha, spa);
        }
    }
}

#if ETH_STREAM_DIAG
static void diag_print(void)
{
    uint16_t bmsr = 0;
    uint16_t anlpar = 0;

    /* Link status is latched low, the second read gives the current state. */
    mac->PHY_Read(DIAG_PHY_ADDR, 1, &bmsr);
    mac->PHY_Read(DIAG_PHY_ADDR, 1, &bmsr);
    mac->PHY_Read(DIAG_PHY_ADDR, 5, &anlpar);
    printf("ETH diag: PHY link %u aneg done %u, partner%s%s%s%s\n", (bmsr >> 2) & 1U,
           (bmsr >> 5) & 1U, (anlpar & 0x100U) ? " 100FD" : "", (anlpar & 0x080U) ? " 100HD" : "",
           (anlpar & 0x040U) ? " 10FD" : "", (anlpar & 0x020U) ? " 10HD" : "");

    printf("ETH diag: missed %" PRIu32 "/%" PRIu32 ", driver rx %" PRIu32 " err %" PRIu32
           " vlan %" PRIu32 " arp %" PRIu32 " (from %d.%d.%d.%d: %" PRIu32 ") other %" PRIu32
           ", last %" PRIu32 " B type 0x%04X from %02X:%02X:%02X:%02X:%02X:%02X\n",
           ETH->ETH_MTL_RXQ0_MISSED_PACKET_OVERFLOW_CNT, ETH->ETH_DMA_CH0_MISS_FRAME_CNT,
           rx_stats.frames, rx_stats.errors, rx_stats.vlan, rx_stats.arp, dest_ip[0], dest_ip[1],
           dest_ip[2], dest_ip[3], rx_stats.arp_from_dest, rx_stats.other, rx_stats.last_len,
           rx_stats.last_type,
           rx_stats.last_src[0], rx_stats.last_src[1], rx_stats.last_src[2], rx_stats.last_src[3],
           rx_stats.last_src[4], rx_stats.last_src[5]);
}
#endif

/* Send as much of the pending audio as the MAC TX ring takes. Called from
 * eth_stream_send() and from the ETH IRQ when a frame has been sent. */
static void tx_pump(void)
{
    uint8_t* ip = &frame_buf[IP_OFS];
    uint8_t* udp = &frame_buf[UDP_OFS];
    uint8_t* rtp = &frame_buf[RTP_OFS];

    while (tx_left) {
        const uint32_t n =
            (tx_left > ETH_STREAM_FRAMES_PER_PACKET) ? ETH_STREAM_FRAMES_PER_PACKET : tx_left;
        const uint32_t payload = n * BYTES_PER_FRAME;

        memcpy(frame_buf, hdr_template, sizeof(hdr_template));
        put16(&ip[2], (uint16_t)(IP_HDR_LEN + UDP_HDR_LEN + RTP_HDR_LEN + payload));
        put16(&ip[4], ip_id);
        put16(&udp[4], (uint16_t)(UDP_HDR_LEN + RTP_HDR_LEN + payload));
        if (rtp_marker) {
            rtp[1] |= 0x80; /* first packet after a gap */
        }
        put16(&rtp[2], rtp_seq);
        put32(&rtp[4], tx_ts);
        copy_be16(&frame_buf[PAYLOAD_OFS], tx_src, n * ETH_STREAM_CHANNELS);

        if (mac->SendFrame(frame_buf, PAYLOAD_OFS + payload, 0) != ARM_DRIVER_OK) {
            /* Ring full: continue from the TX complete interrupt. */
            break;
        }

        ip_id++;
        rtp_seq++;
        rtp_marker = false;
        tx_src += n * ETH_STREAM_CHANNELS;
        tx_left -= n;
        tx_ts += n;
        stats.packets_sent++;
        stats.frames_sent += n;
    }
}

static void mac_event(uint32_t event)
{
    if (event & ARM_ETH_MAC_EVENT_TX_FRAME) {
        tx_pump();
    }
}

int32_t eth_stream_init(uint32_t sample_rate)
{
#if defined(SE_SERVICES_SUPPORT)
    /* The SE gates the clock of every IP not listed in the run profile, and
     * the platform default does not list ETH. */
    default_runprof.ip_clock_gating |= ETH_MASK;
    if (set_power_run_profile(default_runprof)) {
        printf("ETH stream: failed to enable the ETH clock in the run profile\n");
        return -1;
    }
#endif

    phy_reset();

    /* Like Zephyr, the PHY is not configured: it auto-negotiates on its own
     * and the MAC is fixed to 100 Mbit/s full duplex. */
    ARM_ETH_MAC_ADDR addr;
    memcpy(addr.b, our_mac, sizeof(addr.b));
    if (mac->Initialize(mac_event) || mac->PowerControl(ARM_POWER_FULL)) {
        printf("ETH stream: MAC init failed (no RMII reference clock from the PHY?)\n");
        return -1;
    }
    if (mac->SetMacAddress(&addr) ||
        mac->Control(ARM_ETH_MAC_CONFIGURE, ARM_ETH_MAC_SPEED_100M | ARM_ETH_MAC_DUPLEX_FULL |
                                                ARM_ETH_MAC_ADDRESS_BROADCAST |
                                                ARM_ETH_MAC_CHECKSUM_OFFLOAD_TX) ||
        mac->Control(ARM_ETH_MAC_CONTROL_TX, 1) || mac->Control(ARM_ETH_MAC_CONTROL_RX, 1)) {
        printf("ETH stream: MAC configuration failed\n");
        return -1;
    }
    /* The MAC latches the address only on a low register write, and
     * SetMacAddress() writes low before high. Rewrite low so that unicast
     * frames to our MAC pass the RX filter. */
    ETH->ETH_MAC_ADDRESS0_LOW = ((uint32_t)our_mac[3] << 24) | ((uint32_t)our_mac[2] << 16) |
                                ((uint32_t)our_mac[1] << 8) | our_mac[0];

    dest_mac_init();
    hdr_template_init();
    arp_requested = false;
    rtp_marker = true;
    tx_left = 0;
    /* Random-ish start values as recommended by RFC 3550. */
    rtp_seq = (uint16_t)Get_SysTick_Cycle_Count32();
    rtp_ts = Get_SysTick_Cycle_Count32();
    memset(&stats, 0, sizeof(stats));
    inited = true;

    printf("ETH stream: RTP L16/%" PRIu32 "/%d PT %d, %d frames/packet, "
           "%d.%d.%d.%d -> %d.%d.%d.%d:%d\n",
           sample_rate, ETH_STREAM_CHANNELS, ETH_STREAM_RTP_PT, ETH_STREAM_FRAMES_PER_PACKET,
           our_ip[0], our_ip[1], our_ip[2], our_ip[3], dest_ip[0], dest_ip[1], dest_ip[2],
           dest_ip[3], ETH_STREAM_PORT);
    return 0;
}

void eth_stream_send(const int16_t* frames, uint32_t num_frames)
{
    if (!inited) {
        return;
    }

    NVIC_DisableIRQ(ETH_SBD_IRQ_IRQn);

    /* The RTP clock runs with the audio even when nothing is sent, so the
     * receiver sees dropped audio as a gap and not as a time shift. */
    const uint32_t ts = rtp_ts;
    rtp_ts += num_frames;

#if ETH_STREAM_DIAG
    const bool was_resolved = dest_mac_valid;
#endif
    rx_poll();

#if ETH_STREAM_DIAG
    if (!was_resolved) {
        const uint32_t now = Get_SysTick_Count();
        if (dest_mac_valid || (now - diag_last_ms) >= DIAG_PERIOD_MS) {
            diag_print();
            diag_last_ms = now;
        }
    }
#endif

    if (!dest_mac_valid) {
        const uint32_t now = Get_SysTick_Count();
        if (!arp_requested || (now - arp_last_ms) >= ARP_RETRY_MS) {
            static const uint8_t zero_mac[6] = {0};
            arp_send(ARP_OP_REQUEST, bcast_mac, zero_mac, dest_ip);
            arp_requested = true;
            arp_last_ms = now;
        }
        stats.frames_dropped += num_frames;
        rtp_marker = true;
    } else {
        if (tx_left) {
            /* Previous audio not sent yet (link slower than the audio). */
            stats.frames_dropped += tx_left;
            rtp_marker = true;
        }
        tx_src = frames;
        tx_left = num_frames;
        tx_ts = ts;
        tx_pump();
    }

    NVIC_EnableIRQ(ETH_SBD_IRQ_IRQn);
}

#endif /* ETH_STREAM */
