#!/usr/bin/env bash
# Send a file to a device over a serial port using classic XMODEM (requires lrzsz).
#
# Usage: send_file_via_xmodem.sh <port> <file_path> [timeout_s]
#   port        Serial device, e.g. /dev/ttyACM1
#   file_path   File to send
#   timeout_s   Maximum duration of the transfer in seconds (default: 1800)
#
# Example: send_file_via_xmodem.sh /dev/ttyACM1 build/bin/app.bin 3600
#
# Environment:
#   PROGRESS_STEP   Print the progress every this many percent (default: 5)
set -euo pipefail

if [ "$#" -lt 2 ] || [ "$#" -gt 3 ]; then
    echo "usage: $0 <port> <file_path> [timeout_s]" >&2
    exit 2
fi

PORT=$1
FILE=$2
TIMEOUT_S=${3:-1800}
BAUD=115200
PROGRESS_STEP=${PROGRESS_STEP:-5}

if [ ! -c "$PORT" ]; then
    echo "ERROR: $PORT is not a character device" >&2
    exit 1
fi

if [ ! -f "$FILE" ]; then
    echo "ERROR: $FILE does not exist" >&2
    exit 1
fi

if ! [[ "$TIMEOUT_S" =~ ^[0-9]+$ ]]; then
    echo "ERROR: timeout_s must be a number of seconds, got '$TIMEOUT_S'" >&2
    exit 2
fi

# XMODEM sends 128-byte blocks
FILE_SIZE=$(stat -c %s "$FILE")
TOTAL_BLOCKS=$(( (FILE_SIZE + 127) / 128 ))

echo "Port:    $PORT ($BAUD baud)"
echo "File:    $FILE ($FILE_SIZE bytes, $TOTAL_BLOCKS blocks)"
echo "Timeout: ${TIMEOUT_S}s"

# Print each command before running it
run() {
    echo "+ $*"
    "$@"
}

# Configure the port: raw mode, no echo, no flow control, no hangup on close
run stty -F "$PORT" "$BAUD" raw -echo -echoe -echok -crtscts -hupcl

# Make sure nothing else is holding the port
echo "+ fuser $PORT"
if fuser "$PORT" >/dev/null 2>&1; then
    echo "ERROR: $PORT is in use" >&2
    exit 1
fi

# sx writes its progress to stderr using carriage returns, which would flood a CI log. Turn them
# into lines and print the progress only every PROGRESS_STEP percent. Other lines (retries,
# errors) are printed as they are.
filter_progress() {
    local last=-$PROGRESS_STEP line sent rest pct
    local start=$SECONDS
    tr '\r' '\n' | while IFS= read -r line; do
        if [[ "$line" =~ ^Xmodem\ sectors/kbytes\ sent:\ *([0-9]+)/\ *[0-9]+k(.*)$ ]]; then
            sent=${BASH_REMATCH[1]}
            rest=${BASH_REMATCH[2]}
            # sx may append a retry message to the progress text
            if [ -n "$rest" ]; then
                echo "$rest"
            fi
            pct=$(( sent * 100 / TOTAL_BLOCKS ))
            if [ "$pct" -ge $(( last + PROGRESS_STEP )) ]; then
                printf 'Progress: %3d%% (%d/%d blocks), %ds elapsed\n' \
                    "$pct" "$sent" "$TOTAL_BLOCKS" $(( SECONDS - start ))
                last=$pct
            fi
        elif [ -n "$line" ]; then
            echo "$line"
        fi
    done
}

# Send using classic XMODEM (128-byte blocks). The alif_usb-to-ospi-flasher does not support
# XMODEM-1K, so do not add -k. `timeout` keeps CI from hanging if the device never sends 'C'/NAK.
# stderr goes to the progress filter, stdout and stdin are the serial port. With pipefail the
# exit code of sx (or timeout) fails the script.
echo "+ timeout $TIMEOUT_S sx -vv --xmodem $FILE < $PORT > $PORT"
timeout "$TIMEOUT_S" sx -vv --xmodem "$FILE" < "$PORT" 2>&1 > "$PORT" | filter_progress
echo "Transfer OK"
