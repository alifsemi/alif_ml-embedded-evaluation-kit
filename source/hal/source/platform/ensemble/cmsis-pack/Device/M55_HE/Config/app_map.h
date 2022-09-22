/* -----------------------------------------------------------------------------
 * Copyright (c) 2022 Alif Semiconductor Inc.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software. Permission is granted to anyone to use this
 * software for any purpose, including commercial applications, and to alter
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * $Date:        02. feb 2022
 * $Revision:    V1.0.0
 * Author:       Eugene Bonev
 * Project:      MRAM XIP Configurations
 * -------------------------------------------------------------------------- */

#ifndef APP_MAP_H
#define APP_MAP_H

#define _APP_ADDRESS			  MRAM_BASE

/* TODO: The macros below must be fixed for the E-series devices */
#define _PARTITION_ADDRESS		  0x8057F000
#define _PARTITION_SIZE			  0x1000
#define _APP_MAX_SIZE			  0x57F000

#endif
