/*
 * SPDX-FileCopyrightText: Copyright 2024 Arm Limited and/or its
 * affiliates <open-source-office@arm.com>
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
#ifndef HAL_CAMERA_H
#define HAL_CAMERA_H

#if defined(__cplusplus)
extern "C" {
#endif // defined(__cplusplus)

#include <stdint.h>
#include <stdbool.h>

/**< Camera device's operating modes */
typedef enum hal_camera_mode_ {
    HAL_CAMERA_MODE_SINGLE_FRAME = 0,
    HAL_CAMERA_MODE_CONTINUOUS,
    HAL_CAMERA_MODE_INVALID
} hal_cam_mode;

/**< Image colour formats supported by this interface */
typedef enum hal_camera_colour_format_ {
    HAL_CAMERA_COLOUR_FORMAT_RGB888 = 0,
    HAL_CAMERA_COLOUR_FORMAT_RGB565,
    HAL_CAMERA_COLOUR_FORMAT_INVALID
} hal_cam_clr_format;

/**< Camera device status */
typedef enum hal_camera_status_ {
    HAL_CAMERA_STATUS_ERROR = -1,
    HAL_CAMERA_STATUS_STOPPED = 0,
    HAL_CAMERA_STATUS_RUNNING,
    HAL_CAMERA_STATUS_INVALID
} hal_cam_status;

/**
* @brief  Initialises the camera interface.
* @return true if successful, false otherwise.
*/
bool hal_camera_init(void);

/**
 *  @brief Configure camera device.
 *  @param[in]  width           Image width
 *  @param[in]  height          Image height
 *  @param[in]  mode            Camera mode
 *  @param[in]  colour_format   Colour format
 *  @return     true if successful, false otherwise.
 */
bool hal_camera_configure(uint32_t width,
                          uint32_t height,
                          hal_cam_mode mode,
                          hal_cam_clr_format colour_format);

/**
 *  @brief Sets the image buffer location.
 *  @param[in]  buffer      Buffer to be used for populating image data.
 *  @param[in]  size        Size of the buffer provided.
 *  @return     true if successful, false otherwise.
 */
bool hal_camera_set_buffer(uint8_t* buffer, const uint32_t size);

/**
 *  @brief Starts capturing data from camera device.
 *  @return     true if successful, false otherwise.
 */
bool hal_camera_start(void);

/**
 *  @brief Gets pointer to the captured frame.
 *  @param[out] size Bytes occupied by the current frame.
 *  @return     Pointer to 8-bit unsigned data.
 */
const uint8_t* hal_camera_get_captured_frame(uint32_t* size);

/**
 * @brief  Stops the camera device.
 * @return true if successful, false otherwise.
 */
bool hal_camera_stop(void);

/**
 * @brief  Gets camera device status.
 * @return Status represented as the status enumeration.
 */
hal_cam_status hal_camera_get_status(void);

/**
 * @brief  Releases the camera device.
 */
void hal_camera_release(void);

/**
 * @brief  Gets the human friendly name of the camera device.
 * @return Name expressed as a C string.
 */
const char* hal_camera_get_device_name(void);

#if defined(__cplusplus)
}
#endif // defined(__cplusplus)
#endif /* HAL_CAMERA_H */
