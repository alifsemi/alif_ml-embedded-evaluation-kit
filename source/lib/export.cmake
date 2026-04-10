#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2025-2026 Arm Limited and/or its
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
#----------------------------------------------------------------------------
include_guard(GLOBAL)

include(GNUInstallDirs)

set(MLEK_API_EXPORT_SET mlek-api-targets)

set(MLEK_API_INSTALL_TARGETS
    mlek_log
    arm_math
    common_api
    ml_framework_iface)

# Add the use case specific targets.
foreach(_api_name ${MLEK_API_LIST})
    set(_target_name ${_api_name}_api)
    if (TARGET ${_target_name})
        get_target_property(_aliased_target ${_target_name} ALIASED_TARGET)
        if (NOT _aliased_target)
            list(APPEND MLEK_API_INSTALL_TARGETS ${_target_name})
        else()
            list(APPEND MLEK_API_INSTALL_TARGETS ${_aliased_target})
        endif()
    endif()
    if (TARGET ${_api_name}_impl)
        list(APPEND MLEK_API_INSTALL_TARGETS ${_api_name}_impl)
    endif()
endforeach()

list(REMOVE_DUPLICATES MLEK_API_INSTALL_TARGETS)
set_target_properties(${MLEK_API_INSTALL_TARGETS}
                      PROPERTIES EXCLUDE_FROM_ALL FALSE)

# Align exported target names with the existing mlek:: aliases.
set_target_properties(mlek_log PROPERTIES EXPORT_NAME log)
if (TARGET ml_framework_tflm)
    list(APPEND MLEK_API_INSTALL_TARGETS ml_framework_tflm)
    set_target_properties(ml_framework_tflm PROPERTIES EXPORT_NAME ml_framework_impl)
endif()
if (TARGET ml_framework_et)
    list(APPEND MLEK_API_INSTALL_TARGETS ml_framework_et)
    set_target_properties(ml_framework_et PROPERTIES EXPORT_NAME ml_framework_impl)
endif()

if (TARGET CMSISDSP)
    list(APPEND MLEK_API_INSTALL_TARGETS CMSISDSP)
endif()

# Export targets for build-tree consumption.
export(TARGETS ${MLEK_API_INSTALL_TARGETS}
       NAMESPACE mlek::
       FILE ${CMAKE_CURRENT_BINARY_DIR}/${MLEK_API_EXPORT_SET}.cmake)

# Installation flow For native buids.
if (NOT CMAKE_CROSSCOMPILING)
    install(TARGETS ${MLEK_API_INSTALL_TARGETS}
            EXPORT ${MLEK_API_EXPORT_SET}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    install(EXPORT ${MLEK_API_EXPORT_SET}
            NAMESPACE mlek::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mlek-api)

    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/mlek/
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mlek
            FILES_MATCHING
                PATTERN "*.h"
                PATTERN "*.hpp")
endif()
