#  SPDX-FileCopyrightText: Copyright 2022-2024 Arm Limited and/or its
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

FROM ubuntu:22.04

ENV DEBIAN_FRONTEND noninteractive

ENV EVAL_KIT_DIR="/home/ml-embedded-evaluation-kit" \
    FVP_BASE_URL="https://developer.arm.com/-/media/Arm%20Developer%20Community/Downloads/OSS/" \
    FVP_300="FVP_Corstone_SSE-300" \
    FVP_VER_300="11.24_13" \
    FVP_300_SHA="6ea4096ecf8a8c06d6e76e21cae494f0c7139374cb33f6bc3964d189b84539a9" \
    FVP_310="FVP_Corstone_SSE-310" \
    FVP_VER_310="11.24_13" \
    FVP_310_SHA="616ecc0e82067fe0684790cf99638b3496f9ead11051a58d766e8258e766c556" \
    FVP_315="FVP_Corstone_SSE-315" \
    FVP_VER_315="11.24_22" \
    FVP_315_SHA="e105569b159e42a5557baf15cc980a62427b2de3bf17aaaa72de6d218bb3a2eb"

RUN apt-get update && \
    apt-get install -y \
    make \
    git \
    python3.10 \
    python3-pip \
    python3.10-dev \
    python3.10-venv \
    unzip \
    curl \
    wget \
    gpg \
    libsndfile1 \
    sudo \
    telnet

# Check versions
RUN gcc --version && g++ --version && python3 --version

# Download and install GNU GCC 13.2 embedded toolchain
RUN curl -L https://developer.arm.com/-/media/Files/downloads/gnu/13.2.rel1/binrel/arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz -o gcc-arm-none-eabi.tar.xz && \
    echo "6cd1bbc1d9ae57312bcd169ae283153a9572bd6a8e4eeae2fedfbc33b115fdbb gcc-arm-none-eabi.tar.xz" | sha256sum -c && \
    mkdir /opt/gcc-arm-none-eabi && \
    tar -xf  gcc-arm-none-eabi.tar.xz -C /opt/gcc-arm-none-eabi --strip-components 1 && \
    rm gcc-arm-none-eabi.tar.xz

# Download and install Arm Corstone-300 FVP
RUN wget "${FVP_BASE_URL}/FVP/Corstone-300/${FVP_300}_${FVP_VER_300}_Linux64.tgz" 2>/dev/null && \
    echo "${FVP_300_SHA} ${FVP_300}_${FVP_VER_300}_Linux64.tgz" | sha256sum -c && \
    mkdir -p /home/${FVP_300}/ && \
    tar -xf ${FVP_300}_${FVP_VER_300}_Linux64.tgz -C /home/${FVP_300}/ && \
    bash /home/${FVP_300}/${FVP_300}.sh --no-interactive --i-agree-to-the-contained-eula -d /home/${FVP_300} && \
    rm "${FVP_300}_${FVP_VER_300}_Linux64.tgz"

# Download and install Arm Corstone-310 FVP
RUN wget ${FVP_BASE_URL}/FVP/Corstone-310/${FVP_310}_${FVP_VER_310}_Linux64.tgz 2>/dev/null && \
    echo "${FVP_310_SHA} ${FVP_310}_${FVP_VER_310}_Linux64.tgz" | sha256sum -c && \
    mkdir -p /home/${FVP_310}/ && \
    tar -xf ${FVP_310}_${FVP_VER_310}_Linux64.tgz -C /home/${FVP_310}/ && \
    bash /home/${FVP_310}/${FVP_310}.sh --no-interactive --i-agree-to-the-contained-eula -d /home/${FVP_310} && \
    rm "${FVP_310}_${FVP_VER_310}_Linux64.tgz"

# Download and install Arm Corstone-315 FVP
RUN wget "${FVP_BASE_URL}/FVP/Corstone-315/${FVP_315}_${FVP_VER_315}_Linux64.tgz" 2>/dev/null && \
    echo "${FVP_315_SHA} ${FVP_315}_${FVP_VER_315}_Linux64.tgz" | sha256sum -c && \
    mkdir -p /home/${FVP_315}/ && \
    tar -xf ${FVP_315}_${FVP_VER_315}_Linux64.tgz -C /home/${FVP_315}/ && \
    bash /home/${FVP_315}/${FVP_315}.sh --no-interactive --i-agree-to-the-contained-eula -d /home/${FVP_315} && \
    rm "${FVP_315}_${FVP_VER_315}_Linux64.tgz"

# Clone the ml-embedded-evaluation-kit repository
RUN git clone "https://review.mlplatform.org/ml/ethos-u/ml-embedded-evaluation-kit" ${EVAL_KIT_DIR}

# Change working directory
WORKDIR ${EVAL_KIT_DIR}

# Initialize submodules
RUN git submodule update --init

# Download and install resources required for ml-embedded-evaluation-kit
RUN python3 ./set_up_default_resources.py

# Update PATH to make the required version of CMake and GNU embedded toolchain is available.
# And add env variables to make access to the different FVPs easier.
ENV PATH="${EVAL_KIT_DIR}/resources_downloaded/env/bin:/opt/gcc-arm-none-eabi/bin:${PATH}" \
    FVP_300_U55="/home/${FVP_300}/models/Linux64_GCC-9.3/${FVP_300}_Ethos-U55" \
    FVP_300_U65="/home/${FVP_300}/models/Linux64_GCC-9.3/${FVP_300}_Ethos-U65" \
    FVP_310_U55="/home/${FVP_310}/models/Linux64_GCC-9.3/${FVP_310}" \
    FVP_310_U65="/home/${FVP_310}/models/Linux64_GCC-9.3/${FVP_310}_Ethos-U65" \
    FVP_315_U65="/home/${FVP_315}/models/Linux64_GCC-9.3/${FVP_315}" \
    FVP_300_ARGS="-C mps3_board.telnetterminal0.start_telnet=0 -C mps3_board.uart0.out_file='-' -C mps3_board.uart0.shutdown_on_eot=1 -C mps3_board.visualisation.disable-visualisation=1" \
    FVP_310_ARGS="-C mps3_board.telnetterminal0.start_telnet=0 -C mps3_board.uart0.out_file='-' -C mps3_board.uart0.shutdown_on_eot=1 -C mps3_board.visualisation.disable-visualisation=1" \
    FVP_315_ARGS="-C mps4_board.telnetterminal0.start_telnet=0 -C mps4_board.uart0.out_file='-' -C mps4_board.uart0.shutdown_on_eot=1 -C mps4_board.visualisation.disable-visualisation=1 -C vis_hdlcd.disable_visualisation=1"
