# Streaming the AEC output over Ethernet

The `alif_aec` sample can send the audio it plays on the speaker to another
device over Ethernet. Supported on **AppKit-E8** (RTSS-HE).

- Format: RTP (RFC 3550) with L16 payload: 16-bit big-endian PCM, stereo,
  16 kHz, payload type 96.
- Transport: UDP/IPv4, port 5004, 256 frames (16 ms) per packet,
  62.5 packets/s, ~67 kB/s.
- One packet per 16 ms processing stride (`alif_aec_FRAME_STRIDE`). Each
  stride is sent as soon as it is ready, before it is queued to the speaker,
  and the capture loop never waits for the network.

## Network setup

```
  AppKit-E8 (target)                              PC (host)
  192.168.1.10                                    192.168.1.100/24
  MAC 02:00:00:ae:c0:01                           wired adapter
  RTL8201F PHY, 100 Mbit/s full duplex
  RJ45 ──────────── Ethernet cable (direct, or via a switch) ──────────── RJ45
        UDP 5004 → 5004, RTP L16/16000/2
```

There is no DHCP, DNS or gateway support. Both ends use static addresses on
the same subnet.

### Target (board)

Everything is set at build time:

| CMake option | Default | Description |
|---|---|---|
| `ETH_STREAM` | `OFF` | Enables streaming |
| `ETH_STREAM_SRC_IP` | `192.168.1.10` | Board IPv4 address |
| `ETH_STREAM_DEST_IP` | `192.168.1.100` | Receiver: unicast on the same subnet, multicast (224.0.0.0–239.255.255.255) or `255.255.255.255` |
| `ETH_STREAM_PORT` | `5004` | UDP source and destination port |
| `ETH_STREAM_MAC` | `02:00:00:ae:c0:01` | Board MAC address |
| `ETH_STREAM_FRAMES_PER_PACKET` | `256` | Stereo frames per packet (max 365). Keep it equal to `alif_aec_FRAME_STRIDE` for one packet per stride |

At startup the board:

- enables the ETH clock in the Secure Enclave run profile;
- resets the PHY through its reset line on P11_6. The board pin table routes
  P11_6 to the MAC's `ETH_RST` output, which holds the PHY in reset, so the
  sample drives it as a GPIO;
- starts the MAC (the Alif CMSIS `Driver_ETH_MAC0`) at a fixed 100 Mbit/s
  full duplex. The PHY auto-negotiates on its own;
- finds the receiver's MAC address with ARP (retried every 0.5 s), and
  answers ARP requests for its own address.

Audio is dropped until the receiver answers ARP. Multicast and broadcast
destinations don't need ARP.

### Host (PC)

Any OS works. The host needs:

1. **A wired Ethernet port** connected to the board, directly or through a
   switch.
2. **A static IPv4 address equal to `ETH_STREAM_DEST_IP`** (default
   `192.168.1.100`), subnet mask `255.255.255.0`, no gateway or DNS. The
   board finds the host by asking for this address (ARP), so audio only
   starts once the host has it.
3. **Link speed and duplex left on automatic** (auto-negotiation). The link
   comes up at 100 Mbit/s full duplex. Don't force a fixed speed/duplex: the
   board PHY would fall back to half duplex.
4. **Incoming UDP port 5004 allowed** through the firewall.
5. **A player that understands RTP with L16 audio**, such as GStreamer, VLC
   or ffmpeg (see [Listening on the host](#listening-on-the-host)).

Where to set the address:

- **Windows:** Settings → Network & internet → Ethernet → IP assignment →
  Edit → Manual, IPv4 on: IP `192.168.1.100`, subnet mask `255.255.255.0`,
  gateway empty. Windows Defender Firewall asks the first time a player
  listens; allow it on private networks.
- **macOS:** System Settings → Network → (USB) Ethernet → Details → TCP/IP:
  Configure IPv4 *Manually*, IP `192.168.1.100`, subnet mask
  `255.255.255.0`, router empty.
- **Linux:** see below.

#### Linux example

With NetworkManager:

```sh
nmcli con add type ethernet ifname <adapter> con-name aec-stream \
  ipv4.method manual ipv4.addresses 192.168.1.100/24 ipv4.never-default yes \
  ipv6.method disabled \
  802-3-ethernet.auto-negotiate yes 802-3-ethernet.speed 100 802-3-ethernet.duplex full
nmcli con up aec-stream
```

- `ipv4.never-default yes` keeps your normal internet route on the other
  interfaces.
- The speed/duplex settings keep auto-negotiation on but advertise only
  100 Mbit/s full duplex. Don't force duplex with auto-negotiation off: the
  board PHY would then fall back to half duplex.
- If a firewall is active, allow incoming UDP on port 5004
  (e.g. `sudo ufw allow 5004/udp`).

Find the adapter name with `ip -br link`.

## Build and flash

Configure with `ETH_STREAM=ON` next to the usual `alif_aec` options:

```sh
cmake -B build_aec_eth \
  -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-gcc.cmake \
  -DTARGET_PLATFORM=alif -DTARGET_BOARD=AppKit-e8 -DTARGET_SUBSYSTEM=RTSS-HE \
  -DUSE_CASE_BUILD=alif_aec -DTARGET_MICS=LPPDM_PDM -DUSE_STEREO=ON \
  -DSE_SERVICES_SUPPORT=ON -DGLCD_UI=OFF -DCMAKE_BUILD_TYPE=Release \
  -DETH_STREAM=ON \
  -DETH_STREAM_SRC_IP=192.168.1.10 -DETH_STREAM_DEST_IP=192.168.1.100
cmake --build build_aec_eth -j$(nproc) --target mlek_alif_aec
```

Convert `build_aec_eth/bin/mlek_alif_aec.axf` to a binary
(`arm-none-eabi-objcopy -O binary`). Flash it like the plain `alif_aec`
sample with the Alif Security Toolkit (`app-gen-toc`, `app-write-mram`).

On boot the console shows:

```
ETH stream: RTP L16/16000/2 PT 96, 256 frames/packet, 192.168.1.10 -> 192.168.1.100:5004
ETH stream: receiver at 3C:49:37:08:AD:F0
```

## Listening on the host

The player has to receive RTP on UDP port 5004 and decode it as **L16,
16000 Hz, 2 channels, payload type 96**. RTP doesn't carry the format, so the
player has to be told. Packets arrive every 16 ms; a jitter buffer of ~50 ms
is enough.

### VLC or ffmpeg (any OS)

Save this as `aec.sdp`, with the host address on the `c=` line:

```
v=0
o=- 0 0 IN IP4 192.168.1.10
s=AEC stream
c=IN IP4 192.168.1.100
t=0 0
m=audio 5004 RTP/AVP 96
a=rtpmap:96 L16/16000/2
```

- **Play:** open `aec.sdp` in VLC (Media → Open File). Raise the network
  caching to ~50 ms for low latency (raise it if the audio breaks up).
- **Record to WAV:**
  `ffmpeg -protocol_whitelist file,udp,rtp -i aec.sdp aec.wav`. Stop with
  `q`.

### GStreamer

GStreamer is available for Linux, Windows and macOS, and the commands are
the same on all of them. On Windows, put the pipeline on one line instead of
using `\`.

#### Play live

```sh
gst-launch-1.0 udpsrc port=5004 \
  caps="application/x-rtp,media=audio,clock-rate=16000,encoding-name=L16,channels=2,payload=96" \
  ! rtpjitterbuffer latency=50 ! rtpL16depay ! audioconvert ! autoaudiosink
```

#### Record to a WAV file (for Audacity)

```sh
gst-launch-1.0 -e udpsrc port=5004 \
  caps="application/x-rtp,media=audio,clock-rate=16000,encoding-name=L16,channels=2,payload=96" \
  ! rtpjitterbuffer latency=50 ! rtpL16depay ! audioconvert ! wavenc \
  ! filesink location=aec.wav
```

Stop with Ctrl-C. `-e` makes GStreamer finish the WAV header properly. Then
open the file in Audacity with **File → Open**.

### Record directly in Audacity (Linux)

Send the stream to a virtual sound device and record that device's monitor.
On Windows and macOS, record to a WAV file as above instead, or use a
virtual audio cable (e.g. VB-Cable or BlackHole) as the player's output
device.

```sh
pactl load-module module-null-sink sink_name=aec_stream
gst-launch-1.0 udpsrc port=5004 \
  caps="application/x-rtp,media=audio,clock-rate=16000,encoding-name=L16,channels=2,payload=96" \
  ! rtpjitterbuffer latency=50 ! rtpL16depay ! audioconvert \
  ! pulsesink device=aec_stream
```

In Audacity, select the **PulseAudio** host and the recording device
**Monitor of aec_stream**, then press Record. Remove the device afterwards
with `pactl unload-module module-null-sink`.

## Troubleshooting

| Symptom | Check |
|---|---|
| Link LED off, host shows the cable as disconnected | Cable, board running the `ETH_STREAM=ON` image |
| `ETH stream: MAC init failed (no RMII reference clock from the PHY?)` | PHY not running: PHY reset line (P11_6) or PHY power |
| No `receiver at …` line | Host adapter IP must equal `ETH_STREAM_DEST_IP`, mask `255.255.255.0` |
| `receiver at …` printed but no audio | Firewall must allow incoming UDP 5004. Wireshark (any OS) or `tcpdump` (Linux/macOS) should show ~62 packets/s from the board |
| Audio has gaps | Jitter buffer too small: raise `latency` |
