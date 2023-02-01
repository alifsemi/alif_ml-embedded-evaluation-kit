/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement 
 *
 * You should have received a copy of the Alif Semiconductor Software 
 * License Agreement with this file. If not, please write to: 
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef LV_PORT_H_
#define LV_PORT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t lv_port_get_ticks(void);

/**
 * @brief   Initialise LVGL and the display. Clears the display if called a second time,
 *          so can be used from a use case to remove the default GLCD canvas.
 **/
void lv_port_disp_init(void);

/**
 * @brief   Protective lock to be used around LVGL API calls, to avoid race conditions
 *          with interrupt-based refresh
 * @return  restore state value to pass to lv_port_unlock.
 **/
uint32_t lv_port_lock(void);

/**
 * @brief   Protective lock to be used around LVGL API calls, to avoid race conditions
 *          with interrupt-based refresh
 * @param[in] state   state value returned from lv_port_lock.
 **/
void lv_port_unlock(uint32_t state);

#ifdef __cplusplus
}

/* C++11 Lockable object for the lock */
class LVGLLock {
private:
    uint32_t m_oldState;
public:
    void lock() {
        m_oldState = lv_port_lock();
    }

    bool try_lock() {
        lock();
        return true;
    }

    void unlock() {
        lv_port_unlock(m_oldState);
    }
};

class ScopedLVGLLock {
private:
    uint32_t m_oldState;
public:
    ScopedLVGLLock() {
        m_oldState = lv_port_lock();
    }

    ~ScopedLVGLLock() {
        lv_port_unlock(m_oldState);
    }

    ScopedLVGLLock(const ScopedLVGLLock &) = delete;
    ScopedLVGLLock &operator=(const ScopedLVGLLock &) = delete;
};

#endif

#endif /* LV_PORT_H_ */
