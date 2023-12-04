#  SPDX-FileCopyrightText: Copyright 2022-2023 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
ENV EVAL_KIT_DIR "/home/ml-embedded-evaluation-kit"

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

# Download and install the Arm Corstone_SSE-300 FVP
RUN wget https://developer.arm.com/-/media/Arm%20Developer%20Community/Downloads/OSS/FVP/Corstone-300/FVP_Corstone_SSE-300_11.16_26.tgz 2>/dev/null && \
    echo "e26139be756b5003a30d978c629de638aed1934d597dc24a17043d4708e934d7 FVP_Corstone_SSE-300_11.16_26.tgz" | sha256sum -c && \
    mkdir -p /home/FVP_Corstone_SSE-300/ && \
    tar -xf FVP_Corstone_SSE-300_11.16_26.tgz -C /home/FVP_Corstone_SSE-300/ && \
    bash /home/FVP_Corstone_SSE-300/FVP_Corstone_SSE-300.sh --no-interactive --i-agree-to-the-contained-eula -d /home/FVP_Corstone_SSE-300 && \
    rm FVP_Corstone_SSE-300_11.16_26.tgz

# Clone the ml-embedded-evaluation-kit repository
RUN git clone "https://review.mlplatform.org/ml/ethos-u/ml-embedded-evaluation-kit" ${EVAL_KIT_DIR}

# Change working directory
WORKDIR ${EVAL_KIT_DIR}

# Initialize submodules
RUN git submodule update --init

# Download and install resources required for ml-embedded-evaluation-kit
RUN python3 ./set_up_default_resources.py

# Update PATH to make the required version of CMake and GNU embedded toolchain is available:
ENV PATH="${EVAL_KIT_DIR}/resources_downloaded/env/bin:/opt/gcc-arm-none-eabi/bin:${PATH}"

# FVP binary paths:
ENV FVP_ETHOS_U55="/home/FVP_Corstone_SSE-300/models/Linux64_GCC-6.4/FVP_Corstone_SSE-300_Ethos-U55"
ENV FVP_ETHOS_U65="/home/FVP_Corstone_SSE-300/models/Linux64_GCC-6.4/FVP_Corstone_SSE-300_Ethos-U65"

# Default FVP arguments for no GUI and telnet pop ups:
ENV FVP_ARGS="-C mps3_board.telnetterminal0.start_telnet=0 -C mps3_board.uart0.out_file='-' -C mps3_board.uart0.shutdown_on_eot=1 -C mps3_board.visualisation.disable-visualisation=1"
