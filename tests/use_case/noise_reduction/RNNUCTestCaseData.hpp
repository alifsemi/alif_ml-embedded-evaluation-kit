/*
 * SPDX-FileCopyrightText: Copyright 2021 Arm Limited and/or its affiliates <open-source-office@arm.com>
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef RNNUC_TEST_DATA
#define RNNUC_TEST_DATA

#include <cstdint>

/* 1st inference denoised output. */
int16_t denoisedInf0 [512] = {
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x1, 0x1,
    0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
    0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, -0x1, -0x1, -0x1, -0x1, -0x1, -0x1, -0x1,
    -0x1, -0x1, -0x1, -0x1, -0x1, -0x1, -0x1, 0x0,
    -0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, -0x1, -0x1, -0x1,
    -0x1, -0x1, -0x1, -0x1, -0x1, -0x1, -0x1, -0x1,
    -0x1, -0x1, -0x1, -0x1, -0x1, -0x1, -0x1, -0x1,
    -0x1, -0x1, -0x1, 0x0, -0x1, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
    0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
    0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x2, 0x2,
    0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2,
    0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2,
    0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2,
    0x2, 0x2, 0x2, 0x3, 0x2, 0x3, 0x3, 0x3,
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x4,
    0x3, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4,
    0x4, 0x4, 0x3, 0x4, 0x4, 0x3, 0x3, 0x3,
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x2, 0x2,
    0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x1, 0x2,
    0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
    0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x2,
    0x1, 0x2, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
    0x1, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, -0x1,
    0x0, 0x0, -0x1, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x1, 0x1, 0x1, 0x2, 0x2, 0x3, 0x3, 0x3,
    0x3, 0x3, 0x3, 0x3, 0x3, 0x4, 0x3, 0x3,
    0x3, 0x3, 0x3, 0x2, 0x2, 0x2, 0x2, 0x2,
    0x2, 0x2, 0x3, 0x3, 0x3, 0x4, 0x4, 0x5,
    0x5, 0x6, 0x7, 0x7, 0x8, 0x8, 0x9, 0xa,
    0xa, 0xb, 0xb, 0xb, 0xc, 0xc, 0xd, 0xd,
    0xd, 0xe, 0xd, 0xe, 0xe, 0xd, 0xe, 0xd,
    0xd, 0xd, 0xc, 0xc, 0xc, 0xc, 0xc, 0xb,
    0xc, 0xc, 0xc, 0xd, 0xe, 0xf, 0x10, 0x11,
    0x12, 0x13, 0x15, 0x16, 0x17, 0x19, 0x19, 0x1a,
    0x1b, 0x1b, 0x1b, 0x1a, 0x19, 0x19, 0x17, 0x16,
    0x15, 0x13, 0x12, 0x10, 0x10, 0x10, 0xf, 0x10,
    0x10, 0x11, 0x12, 0x13, 0x15, 0x16, 0x18, 0x1b,
    0x1b, 0x1f, 0x20, 0x21, 0x24, 0x20, 0x23, 0x21
};

/* 2nd inference denoised output. */
int16_t denoisedInf1 [512] = {
    0x13, 0x1f, 0x31, 0x2b, 0x3c, 0x4d, 0x4c, 0x5c,
    0x61, 0x54, 0x4e, 0x3a, 0x26, 0x19, -0x6, -0x22,
    -0x36, -0x3c, -0x30, -0x36, -0x3e, -0x38, -0x3c, -0x35,
    -0x3a, -0x57, -0x41, -0x1c, -0x26, -0x26, -0x15, -0x17,
    -0x17, -0x1b, -0x2b, -0x2f, -0x37, -0x3b, -0x30, -0x2e,
    -0x2f, -0x23, -0x24, -0x2b, -0x2d, -0x35, -0x2e, -0x18,
    -0x2e, -0x5a, -0x60, -0x5d, -0x63, -0x6c, -0x8f, -0xad,
    -0xaa, -0xbf, -0xd3, -0xbf, -0xb7, -0xa8, -0x8c, -0xa0,
    -0xb2, -0xa3, -0xa7, -0xab, -0xae, -0xb5, -0xa5, -0x98,
    -0x9c, -0xa9, -0xc3, -0xca, -0xc9, -0xde, -0xdf, -0xc0,
    -0x9f, -0x8d, -0x92, -0x91, -0x90, -0x9f, -0x9c, -0x99,
    -0xac, -0xae, -0xa4, -0xa6, -0xa2, -0x9c, -0x97, -0x81,
    -0x7c, -0x91, -0x99, -0x95, -0x90, -0x90, -0x91, -0x7f,
    -0x70, -0x63, -0x4b, -0x50, -0x52, -0x38, -0x44, -0x50,
    -0x40, -0x3f, -0x2a, -0xd, -0x17, -0x21, -0x29, -0x36,
    -0x35, -0x38, -0x42, -0x3f, -0x31, -0x30, -0x3d, -0x38,
    -0x34, -0x38, -0x2f, -0x40, -0x4e, -0x3f, -0x4f, -0x5d,
    -0x5b, -0x60, -0x4f, -0x51, -0x5e, -0x51, -0x60, -0x70,
    -0x6f, -0x7d, -0x7b, -0x7f, -0x89, -0x70, -0x64, -0x59,
    -0x3e, -0x34, -0x27, -0x25, -0x26, -0x15, -0x18, -0x1b,
    -0x13, -0xb, 0xa, 0xb, 0xb, 0x10, -0x17, -0x28,
    -0x15, -0x16, -0x13, -0x15, -0x18, -0x12, -0x24, -0x2d,
    -0x40, -0x69, -0x57, -0x4a, -0x69, -0x4a, -0x18, -0x24,
    -0x30, -0x36, -0x44, -0x38, -0x35, -0x4b, -0x46, -0x3c,
    -0x3d, -0x35, -0x46, -0x5a, -0x55, -0x5e, -0x5b, -0x47,
    -0x48, -0x45, -0x4c, -0x64, -0x5a, -0x48, -0x55, -0x57,
    -0x3a, -0x23, -0x1b, -0xf, -0x1, 0x15, 0x2f, 0x33,
    0x35, 0x43, 0x51, 0x5d, 0x54, 0x49, 0x5d, 0x67,
    0x6c, 0x7a, 0x6f, 0x61, 0x5a, 0x4c, 0x49, 0x42,
    0x36, 0x44, 0x4c, 0x40, 0x44, 0x53, 0x59, 0x55,
    0x58, 0x62, 0x65, 0x6d, 0x74, 0x67, 0x5b, 0x50,
    0x2d, 0x2, -0x1b, -0x1d, -0x1e, -0x2d, -0x25, -0x1e,
    -0x2c, -0x29, -0x21, -0x25, -0x29, -0x27, -0x19, -0x16,
    -0x21, -0x1d, -0x16, -0x17, -0x13, -0x9, -0x9, -0x1c,
    -0x1f, -0x1e, -0x30, -0x2f, -0x2c, -0x37, -0x32, -0x3f,
    -0x53, -0x55, -0x6c, -0x82, -0x84, -0x93, -0xa3, -0xb1,
    -0xba, -0xab, -0xa1, -0x96, -0x7c, -0x6e, -0x5d, -0x4d,
    -0x56, -0x47, -0x39, -0x52, -0x4f, -0x4a, -0x69, -0x75,
    -0x83, -0x97, -0x97, -0xa1, -0xa8, -0xa6, -0xb1, -0xab,
    -0x98, -0x83, -0x65, -0x52, -0x44, -0x37, -0x34, -0x34,
    -0x3b, -0x35, -0x2a, -0x34, -0x37, -0x41, -0x5c, -0x57,
    -0x50, -0x53, -0x3c, -0x29, -0x23, -0x13, -0x1a, -0x2b,
    -0x24, -0x26, -0x31, -0x2c, -0x27, -0x2e, -0x2b, -0x1c,
    -0x25, -0x26, -0x4, 0x6, 0xd, 0x29, 0x2e, 0x43,
    0x5e, 0x54, 0x5d, 0x64, 0x63, 0x6c, 0x59, 0x5e,
    0x6d, 0x62, 0x7b, 0x84, 0x76, 0x8e, 0x9a, 0x9b,
    0xa0, 0xa3, 0xac, 0x9d, 0x99, 0xa1, 0x88, 0x7c,
    0x65, 0x44, 0x4a, 0x3d, 0x12, -0x18, -0x2c, -0x28,
    -0x3b, -0x45, -0x3d, -0x3b, -0x20, -0x1e, -0x2f, -0x1e,
    -0x27, -0x38, -0x3e, -0x4f, -0x54, -0x5e, -0x57, -0x3e,
    -0x3b, -0x2c, -0x1e, -0x19, 0xa, 0x21, 0x29, 0x35,
    0x3c, 0x3c, 0x28, 0x12, -0x4, -0x1d, -0x27, -0x2e,
    -0x2e, -0x30, -0x2c, -0x1, 0x18, 0x17, 0x2a, 0x46,
    0x58, 0x4a, 0x51, 0x72, 0x5d, 0x58, 0x68, 0x5a,
    0x5d, 0x4e, 0x3a, 0x3e, 0x31, 0x36, 0x2f, 0x19,
    0x30, 0x2e, 0x27, 0x38, 0x29, 0x2f, 0x44, 0x48,
    0x59, 0x57, 0x49, 0x47, 0x42, 0x3c, 0x33, 0x36,
    0x3e, 0x45, 0x4e, 0x4d, 0x5c, 0x62, 0x56, 0x6a,
    0x6a, 0x5b, 0x64, 0x4e, 0x39, 0x30, 0x23, 0x30,
    0x31, 0x2a, 0x38, 0x40, 0x53, 0x62, 0x6b, 0x80,
    0x84, 0x85, 0x7e, 0x6c, 0x6d, 0x5f, 0x4b, 0x49,
    0x43, 0x3b, 0x30, 0x30, 0x31, 0x25, 0x36, 0x38,
    0x25, 0x30, 0x21, 0x7, 0x14, 0x17, 0xc, 0x2,
    -0x1, 0xf, 0x10, 0x4, 0x8, 0x16, 0x1f, 0x21
};

/* Final denoised results after 128 steps */
int16_t denoisedInf2 [512] = {
    -0x1a1, -0x19b, -0x1ad, -0x1b3, -0x1c1, -0x1d1, -0x1c2, -0x1b5,
    -0x1bb, -0x1d0, -0x1d8, -0x1c7, -0x1d2, -0x1d6, -0x1bf, -0x1c6,
    -0x1c4, -0x1b9, -0x1c1, -0x1ba, -0x1c9, -0x1e0, -0x1d2, -0x1d6,
    -0x1e7, -0x1e8, -0x1ef, -0x1f2, -0x1eb, -0x1e2, -0x1f0, -0x209,
    -0x205, -0x205, -0x20c, -0x1f9, -0x1df, -0x1de, -0x1fc, -0x20d,
    -0x1f5, -0x1ed, -0x20f, -0x225, -0x21a, -0x20b, -0x209, -0x205,
    -0x1fd, -0x1ef, -0x1ec, -0x1f5, -0x1e2, -0x1dc, -0x1ef, -0x1f5,
    -0x20d, -0x21d, -0x211, -0x20b, -0x1f9, -0x202, -0x225, -0x219,
    -0x20f, -0x213, -0x20d, -0x21f, -0x226, -0x210, -0x209, -0x21d,
    -0x236, -0x238, -0x22b, -0x228, -0x23f, -0x245, -0x225, -0x22e,
    -0x24b, -0x246, -0x246, -0x23b, -0x23c, -0x255, -0x243, -0x22a,
    -0x231, -0x23e, -0x235, -0x209, -0x218, -0x24d, -0x242, -0x242,
    -0x25c, -0x26b, -0x27b, -0x25a, -0x24a, -0x26d, -0x26d, -0x272,
    -0x26f, -0x243, -0x253, -0x27b, -0x26c, -0x25a, -0x25b, -0x270,
    -0x28a, -0x26d, -0x253, -0x269, -0x259, -0x24d, -0x270, -0x274,
    -0x272, -0x27a, -0x26e, -0x27e, -0x279, -0x262, -0x285, -0x28d,
    -0x27a, -0x289, -0x280, -0x27a, -0x282, -0x270, -0x265, -0x272,
    -0x27b, -0x269, -0x263, -0x27a, -0x277, -0x275, -0x285, -0x27f,
    -0x287, -0x28b, -0x288, -0x29d, -0x28d, -0x274, -0x275, -0x25e,
    -0x249, -0x260, -0x281, -0x27c, -0x25f, -0x25f, -0x261, -0x254,
    -0x25a, -0x264, -0x261, -0x260, -0x26e, -0x278, -0x279, -0x281,
    -0x26c, -0x25d, -0x278, -0x264, -0x258, -0x281, -0x274, -0x25b,
    -0x267, -0x266, -0x26c, -0x25e, -0x243, -0x258, -0x270, -0x272,
    -0x270, -0x261, -0x261, -0x266, -0x251, -0x24d, -0x261, -0x254,
    -0x254, -0x27c, -0x278, -0x26b, -0x279, -0x260, -0x250, -0x264,
    -0x25b, -0x250, -0x252, -0x258, -0x25b, -0x242, -0x243, -0x251,
    -0x241, -0x242, -0x23d, -0x241, -0x268, -0x254, -0x230, -0x23a,
    -0x231, -0x226, -0x22c, -0x222, -0x211, -0x20f, -0x214, -0x203,
    -0x1f1, -0x1f5, -0x1eb, -0x1f1, -0x207, -0x1fe, -0x200, -0x1ff,
    -0x1e1, -0x1e9, -0x1fb, -0x1ea, -0x1e1, -0x1ec, -0x1e8, -0x1d3,
    -0x1c7, -0x1bd, -0x1a7, -0x19d, -0x1b1, -0x1c0, -0x19e, -0x182,
    -0x1a6, -0x1b8, -0x1a2, -0x18d, -0x17e, -0x194, -0x1a1, -0x188,
    -0x18d, -0x181, -0x15e, -0x169, -0x166, -0x14e, -0x14b, -0x155,
    -0x170, -0x16c, -0x151, -0x158, -0x15e, -0x15b, -0x14e, -0x138,
    -0x13e, -0x144, -0x139, -0x134, -0x12c, -0x131, -0x137, -0x131,
    -0x134, -0x12d, -0x122, -0x125, -0x126, -0x12b, -0x119, -0xfa,
    -0x101, -0x105, -0xeb, -0xe2, -0xe6, -0xe4, -0xef, -0xff,
    -0x102, -0xfd, -0xf1, -0xdc, -0xd8, -0xda, -0xbe, -0xa4,
    -0xad, -0xb9, -0xad, -0x95, -0x8c, -0x90, -0x90, -0x9a,
    -0x99, -0x80, -0x88, -0xa0, -0x95, -0x8c, -0x86, -0x72,
    -0x6e, -0x6e, -0x66, -0x65, -0x61, -0x60, -0x5f, -0x52,
    -0x50, -0x4c, -0x40, -0x51, -0x56, -0x37, -0x3a, -0x4d,
    -0x3d, -0x2a, -0x24, -0x2a, -0x33, -0x2e, -0x27, -0x12,
    -0x2, -0xf, -0x8, -0x7, -0x17, -0x1, 0x8, -0x6,
    0xc, 0xc, 0x3, 0x11, 0x18, 0x24, 0x22, 0x16,
    0x2b, 0x37, 0x32, 0x2e, 0x25, 0x39, 0x45, 0x34,
    0x47, 0x66, 0x5a, 0x40, 0x39, 0x48, 0x53, 0x4d,
    0x49, 0x44, 0x43, 0x4d, 0x4e, 0x4b, 0x55, 0x54,
    0x3f, 0x38, 0x4d, 0x60, 0x55, 0x48, 0x5b, 0x66,
    0x56, 0x55, 0x63, 0x6a, 0x63, 0x5e, 0x62, 0x61,
    0x62, 0x5c, 0x4f, 0x61, 0x79, 0x77, 0x69, 0x6a,
    0x84, 0x95, 0x95, 0x90, 0x8d, 0xb2, 0xbc, 0x8c,
    0x9e, 0xb2, 0x9c, 0xb8, 0xb4, 0x8f, 0xa8, 0xb2,
    0xa2, 0xa9, 0xb7, 0xcf, 0xd9, 0xd5, 0xd7, 0xdc,
    0xf1, 0xf5, 0xe6, 0xf9, 0x107, 0xf6, 0xf6, 0xfa,
    0xf6, 0x106, 0x116, 0x112, 0x106, 0x104, 0x10c, 0x102,
    0xef, 0xfd, 0x110, 0x107, 0xf6, 0xfa, 0x112, 0x119,
    0x10e, 0x115, 0x11e, 0x11d, 0x119, 0x11e, 0x12f, 0x12c,
    0x128, 0x135, 0x137, 0x136, 0x127, 0x11d, 0x13b, 0x143,
    0x142, 0x142, 0x137, 0x15c, 0x166, 0x145, 0x150, 0x156,
    0x15e, 0x167, 0x149, 0x157, 0x162, 0x14a, 0x159, 0x158,
    0x14d, 0x152, 0x13d, 0x142, 0x160, 0x166, 0x163, 0x16a,
    0x18a, 0x195, 0x17e, 0x184, 0x19d, 0x1a4, 0x193, 0x17e,
    0x188, 0x198, 0x193, 0x184, 0x194, 0x1ac, 0x196, 0x18c,
};

static int16_t* ofms[3] = {denoisedInf0, denoisedInf1, denoisedInf2};

#endif  /* RNNUC_TEST_DATA */