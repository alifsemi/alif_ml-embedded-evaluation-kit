/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement 
 *
 * You should have received a copy of the Alif Semiconductor Software 
 * License Agreement with this file. If not, please write to: 
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef LV_PORT_DISP_H_
#define LV_PORT_DISP_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t lv_port_get_ticks(void);

void lv_port_disp_init(void);

void lv_port_lock(void);

void lv_port_unlock(void);

#ifdef __cplusplus
}

/* C++11 Lockable object for the lock */
class LVGLLock {
    void lock() {
        lv_port_lock();
    }

    bool try_lock() {
        lock();
        return true;
    }

    void unlock() {
        lv_port_unlock();
    }
};

#endif

#endif /* LV_PORT_DISP_H_ */
