# syntax=docker/dockerfile:1
#  SPDX-FileCopyrightText: Copyright 2022-2026 Arm Limited and/or its
#  affiliates <open-source-office@arm.com>
#  SPDX-License-Identifier: Apache-2.0
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
#  Two-stage hierarchy:
#    base  – Ubuntu 24.04 + build tools
#    tools – base + Arm toolchains + Corstone FVPs
#
#  Build individual targets:
#    docker build --target base  -t mlek:base  .
#    docker build                -t mlek:tools .   (default = tools)
#
#  Override the default non-root user IDs when needed:
#    docker build --target tools -t mlek:tools \
#        --build-arg USER_UID="$(id -u)" --build-arg USER_GID="$(id -g)" .
#  The override values must remain non-root (do not pass 0:0).
#
#  Cross-compiling for arm64 (Podman does not auto-inject TARGETARCH from --platform):
#    podman build --target tools -t mlek:tools-arm64 \
#        --platform linux/arm64 --build-arg TARGETARCH=arm64 .

# ---------------------------------------------------------------------------
# Stage 1: base
# Ubuntu 24.04 with the build-essential packages needed by all stages.
# ---------------------------------------------------------------------------
FROM ubuntu:24.04 AS base

# BuildKit sets TARGETARCH automatically based on --platform (e.g. amd64, arm64).
ARG TARGETARCH
ARG USERNAME=mlek
ARG USER_UID=1000
ARG USER_GID=${USER_UID}

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        git \
        ninja-build \
        python3 \
        python3-pip \
        python3-dev \
        python3-venv \
        unzip \
        curl \
        gpg \
        libsndfile1 \
        sudo \
    && rm -rf /var/lib/apt/lists/*

# Create a non-root user so bind-mounted workspaces do not default to root ownership.
RUN set -eux; \
    if [ "${USER_UID}" = "0" ] || [ "${USER_GID}" = "0" ]; then \
        echo "USER_UID and USER_GID must be non-root values." >&2; \
        exit 1; \
    fi; \
    existingGroupName="$(getent group "${USER_GID}" | cut -d: -f1 || true)"; \
    existingUserName="$(getent passwd "${USER_UID}" | cut -d: -f1 || true)"; \
    if [ -n "${existingGroupName}" ] && [ "${existingGroupName}" != "${USERNAME}" ] && \
        ! getent group "${USERNAME}" >/dev/null; then \
        groupmod -n "${USERNAME}" "${existingGroupName}"; \
    elif [ -z "${existingGroupName}" ]; then \
        groupadd --gid "${USER_GID}" "${USERNAME}"; \
    fi; \
    if ! id -u "${USERNAME}" >/dev/null 2>&1 && [ -n "${existingUserName}" ]; then \
        usermod --login "${USERNAME}" --home "/home/${USERNAME}" --move-home "${existingUserName}"; \
    fi; \
    if id -u "${USERNAME}" >/dev/null 2>&1; then \
        usermod --uid "${USER_UID}" --gid "${USER_GID}" "${USERNAME}"; \
    else \
        useradd --uid "${USER_UID}" --gid "${USER_GID}" -m -s /bin/bash "${USERNAME}"; \
    fi; \
    mkdir -p "/home/${USERNAME}"; \
    chown -R "${USER_UID}:${USER_GID}" "/home/${USERNAME}"; \
    echo "${USERNAME} ALL=(root) NOPASSWD:ALL" > "/etc/sudoers.d/${USERNAME}"; \
    chmod 0440 "/etc/sudoers.d/${USERNAME}"

# Verify host toolchain and Python are present.
RUN gcc --version && g++ --version && python3 --version

# ---------------------------------------------------------------------------
# Stage 2: tools
# Adds the Arm cross-compilation toolchains and Corstone FVPs.
# ---------------------------------------------------------------------------
FROM base AS tools

ARG TARGETARCH
ARG USERNAME=mlek
ARG USER_UID=1000
ARG USER_GID=${USER_UID}

ARG FVP_BASE_URL="https://developer.arm.com/-/cdn-downloads/permalink/FVPs-Corstone-IoT"
ARG FVP_300="FVP_Corstone_SSE-300"
ARG FVP_VER_300="11.27_42"
ARG FVP_310="FVP_Corstone_SSE-310"
ARG FVP_VER_310="11.27_42"
ARG FVP_315="FVP_Corstone_SSE-315"
ARG FVP_VER_315="11.27_42"
ARG FVP_320="FVP_Corstone_SSE-320"
ARG FVP_VER_320="11.27_25"

# Download and install Arm GNU toolchain 14.2 (arm-none-eabi) for the host architecture.
# aarch64 SHA: verify at https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
RUN set -eux; \
    ARCH="${TARGETARCH:-$(dpkg --print-architecture)}"; \
    case "${ARCH}" in \
        amd64) \
            TOOLCHAIN_ARCH="x86_64"; \
            TOOLCHAIN_SHA="62a63b981fe391a9cbad7ef51b17e49aeaa3e7b0d029b36ca1e9c3b2a9b78823" \
            ;; \
        arm64) \
            TOOLCHAIN_ARCH="aarch64"; \
            TOOLCHAIN_SHA="87330bab085dd8749d4ed0ad633674b9dc48b237b61069e3b481abd364d0a684" \
            ;; \
        *) echo "Unsupported ARCH: ${ARCH}" >&2; exit 1 ;; \
    esac; \
    curl -fsSL \
        "https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/arm-gnu-toolchain-14.2.rel1-${TOOLCHAIN_ARCH}-arm-none-eabi.tar.xz" \
        -o gcc-arm-none-eabi.tar.xz; \
    echo "${TOOLCHAIN_SHA}  gcc-arm-none-eabi.tar.xz" | sha256sum -c; \
    mkdir /opt/gcc-arm-none-eabi; \
    tar -xf gcc-arm-none-eabi.tar.xz -C /opt/gcc-arm-none-eabi --strip-components 1; \
    rm gcc-arm-none-eabi.tar.xz

# Download and install Arm Toolchain for Embedded (ATfE) 20.1.0.
# aarch64 SHA: verify at https://github.com/arm/arm-toolchain/releases
RUN set -eux; \
    ARCH="${TARGETARCH:-$(dpkg --print-architecture)}"; \
    case "${ARCH}" in \
        amd64) \
            ATFE_ARCH="x86_64"; \
            ATFE_SHA="c1179396608c07bf68f3014923cfdfcd11c8402a3732f310c23d07c9a726b275" \
            ;; \
        arm64) \
            ATFE_ARCH="AArch64"; \
            ATFE_SHA="2fa9220f64097b71c07e6de2917f33fda1bb736964730786e90a430fdc0fa6be" \
            ;; \
        *) echo "Unsupported ARCH: ${ARCH}" >&2; exit 1 ;; \
    esac; \
    curl -fsSL \
        "https://github.com/arm/arm-toolchain/releases/download/release-20.1.0-ATfE/ATfE-20.1.0-Linux-${ATFE_ARCH}.tar.xz" \
        -o ATfE-20.1.0.tar.xz; \
    echo "${ATFE_SHA}  ATfE-20.1.0.tar.xz" | sha256sum -c; \
    mkdir /opt/ATfE; \
    tar -xf ATfE-20.1.0.tar.xz -C /opt/ATfE --strip-components 1; \
    rm ATfE-20.1.0.tar.xz

# Download and install Arm Corstone FVPs.
# amd64 packages use the "Linux64" suffix; aarch64 packages use "Linux64_armv8l".
# aarch64 MD5s: verify at https://developer.arm.com/downloads/-/arm-ecosystem-fvps
RUN set -eux; \
    ARCH="${TARGETARCH:-$(dpkg --print-architecture)}"; \
    case "${ARCH}" in \
        amd64) \
            FVP_PKG_SUFFIX="Linux64"; \
            FVP_300_MD5="0dc9538041296b8d479249e6fa5aab74"; \
            FVP_310_MD5="f6fabe78377245457b7de7e069a702d1"; \
            FVP_315_MD5="66883e7bed717c4f6645909ae013db36"; \
            FVP_320_MD5="3deb3c68f9b2d145833f15374203514d" \
            ;; \
        arm64) \
            FVP_PKG_SUFFIX="Linux64_armv8l"; \
            FVP_300_MD5="74e7952817b338c8479bcf66874b23e0"; \
            FVP_310_MD5="4ad6b4d173cc3765b08c8d1f308e4c2b"; \
            FVP_315_MD5="be7d543913f3d71883017a6783cb3305"; \
            FVP_320_MD5="3889f1d80a6d9861ea4aa6f1c88dd0ae" \
            ;; \
        *) echo "Unsupported ARCH: ${ARCH}" >&2; exit 1 ;; \
    esac; \
    \
    # Corstone-300
    curl -fsSL "${FVP_BASE_URL}/Corstone-300/${FVP_300}_${FVP_VER_300}_${FVP_PKG_SUFFIX}.tgz" \
        -o "${FVP_300}.tgz"; \
    echo "${FVP_300_MD5}  ${FVP_300}.tgz" | md5sum -c; \
    mkdir -p /opt/fvps/${FVP_300}; \
    tar -xf "${FVP_300}.tgz" -C /opt/fvps/${FVP_300}/; \
    bash "/opt/fvps/${FVP_300}/${FVP_300}.sh" --no-interactive --i-agree-to-the-contained-eula -d /opt/fvps/${FVP_300}; \
    rm "${FVP_300}.tgz"; \
    \
    # Corstone-310
    curl -fsSL "${FVP_BASE_URL}/Corstone-310/${FVP_310}_${FVP_VER_310}_${FVP_PKG_SUFFIX}.tgz" \
        -o "${FVP_310}.tgz"; \
    echo "${FVP_310_MD5}  ${FVP_310}.tgz" | md5sum -c; \
    mkdir -p /opt/fvps/${FVP_310}; \
    tar -xf "${FVP_310}.tgz" -C /opt/fvps/${FVP_310}/; \
    bash "/opt/fvps/${FVP_310}/${FVP_310}.sh" --no-interactive --i-agree-to-the-contained-eula -d /opt/fvps/${FVP_310}; \
    rm "${FVP_310}.tgz"; \
    \
    # Corstone-315
    curl -fsSL "${FVP_BASE_URL}/Corstone-315/${FVP_315}_${FVP_VER_315}_${FVP_PKG_SUFFIX}.tgz" \
        -o "${FVP_315}.tgz"; \
    echo "${FVP_315_MD5}  ${FVP_315}.tgz" | md5sum -c; \
    mkdir -p /opt/fvps/${FVP_315}; \
    tar -xf "${FVP_315}.tgz" -C /opt/fvps/${FVP_315}/; \
    bash "/opt/fvps/${FVP_315}/${FVP_315}.sh" --no-interactive --i-agree-to-the-contained-eula -d /opt/fvps/${FVP_315}; \
    rm "${FVP_315}.tgz"; \
    \
    # Corstone-320
    curl -fsSL "${FVP_BASE_URL}/Corstone-320/${FVP_320}_${FVP_VER_320}_${FVP_PKG_SUFFIX}.tgz" \
        -o "${FVP_320}.tgz"; \
    echo "${FVP_320_MD5}  ${FVP_320}.tgz" | md5sum -c; \
    mkdir -p /opt/fvps/${FVP_320}; \
    tar -xf "${FVP_320}.tgz" -C /opt/fvps/${FVP_320}/; \
    bash "/opt/fvps/${FVP_320}/${FVP_320}.sh" --no-interactive --i-agree-to-the-contained-eula -d /opt/fvps/${FVP_320}; \
    rm "${FVP_320}.tgz"

# Create arch-specific symlinks for FVP binaries in /usr/local/bin so that
# the FVP_* ENV vars below can use static paths available to all processes.
RUN set -eux; \
    ARCH="${TARGETARCH:-$(dpkg --print-architecture)}"; \
    case "${ARCH}" in \
        amd64)  FVP_MODEL_ARCH_DIR="Linux64_GCC-9.3" ;; \
        arm64)  FVP_MODEL_ARCH_DIR="Linux64_armv8l_GCC-9.3" ;; \
        *) echo "Unsupported ARCH: ${ARCH}" >&2; exit 1 ;; \
    esac; \
    ln -s "/opt/fvps/${FVP_300}/models/${FVP_MODEL_ARCH_DIR}/${FVP_300}_Ethos-U55" /usr/local/bin/FVP_Corstone_SSE-300_Ethos-U55; \
    ln -s "/opt/fvps/${FVP_300}/models/${FVP_MODEL_ARCH_DIR}/${FVP_300}_Ethos-U65" /usr/local/bin/FVP_Corstone_SSE-300_Ethos-U65; \
    ln -s "/opt/fvps/${FVP_310}/models/${FVP_MODEL_ARCH_DIR}/${FVP_310}"            /usr/local/bin/FVP_Corstone_SSE-310; \
    ln -s "/opt/fvps/${FVP_310}/models/${FVP_MODEL_ARCH_DIR}/${FVP_310}_Ethos-U65" /usr/local/bin/FVP_Corstone_SSE-310_Ethos-U65; \
    ln -s "/opt/fvps/${FVP_315}/models/${FVP_MODEL_ARCH_DIR}/${FVP_315}"            /usr/local/bin/FVP_Corstone_SSE-315; \
    ln -s "/opt/fvps/${FVP_320}/models/${FVP_MODEL_ARCH_DIR}/${FVP_320}"            /usr/local/bin/FVP_Corstone_SSE-320

# All FVPs bundle the same Python shared library; only one path is needed.
ENV LD_LIBRARY_PATH="/opt/fvps/${FVP_300}/python/lib" \
    PATH="/opt/gcc-arm-none-eabi/bin:/opt/ATfE/bin:${PATH}" \
    FVP_300_U55="/usr/local/bin/FVP_Corstone_SSE-300_Ethos-U55" \
    FVP_300_U65="/usr/local/bin/FVP_Corstone_SSE-300_Ethos-U65" \
    FVP_310_U55="/usr/local/bin/FVP_Corstone_SSE-310" \
    FVP_310_U65="/usr/local/bin/FVP_Corstone_SSE-310_Ethos-U65" \
    FVP_315_U65="/usr/local/bin/FVP_Corstone_SSE-315" \
    FVP_320_U85="/usr/local/bin/FVP_Corstone_SSE-320" \
    FVP_300_ARGS="-C mps3_board.telnetterminal0.start_telnet=0 -C mps3_board.uart0.out_file='-' -C mps3_board.uart0.shutdown_on_eot=1 -C mps3_board.visualisation.disable-visualisation=1" \
    FVP_310_ARGS="-C mps3_board.telnetterminal0.start_telnet=0 -C mps3_board.uart0.out_file='-' -C mps3_board.uart0.shutdown_on_eot=1 -C mps3_board.visualisation.disable-visualisation=1" \
    FVP_315_ARGS="-C mps4_board.telnetterminal0.start_telnet=0 -C mps4_board.uart0.out_file='-' -C mps4_board.uart0.shutdown_on_eot=1 -C mps4_board.visualisation.disable-visualisation=1 -C vis_hdlcd.disable_visualisation=1" \
    FVP_320_ARGS="-C mps4_board.telnetterminal0.start_telnet=0 -C mps4_board.uart0.out_file='-' -C mps4_board.uart0.shutdown_on_eot=1 -C mps4_board.visualisation.disable-visualisation=1 -C vis_hdlcd.disable_visualisation=1"

# Add convenience aliases for running each FVP with its default arguments.
# Written to /etc/bash.bashrc so they are available in all interactive bash sessions.
RUN { \
    echo "alias run_fvp_300_u55='\"${FVP_300_U55}\" ${FVP_300_ARGS}'"; \
    echo "alias run_fvp_300_u65='\"${FVP_300_U65}\" ${FVP_300_ARGS}'"; \
    echo "alias run_fvp_310_u55='\"${FVP_310_U55}\" ${FVP_310_ARGS}'"; \
    echo "alias run_fvp_310_u65='\"${FVP_310_U65}\" ${FVP_310_ARGS}'"; \
    echo "alias run_fvp_315_u65='\"${FVP_315_U65}\" ${FVP_315_ARGS}'"; \
    echo "alias run_fvp_320_u85='\"${FVP_320_U85}\" ${FVP_320_ARGS}'"; \
    } >> /etc/bash.bashrc

USER ${USERNAME}
