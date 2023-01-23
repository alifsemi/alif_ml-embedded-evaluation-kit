
/**@
* Builds project.
*
* @param build_type Release or Debug
* @param toolchain  Used toolchain, gcc or armclang
*/
def build_img(String build_type, String toolchain) {

    build_path = "build_${toolchain}_hp_${build_type}".toLowerCase()
    cmake_cmd = "cmake .. -DTARGET_PLATFORM=ensemble -DTARGET_SUBSYSTEM=RTSS-HP -DUSE_CASE_BUILD=alif_img_class -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-${toolchain}.cmake -DCMAKE_BUILD_TYPE=${build_type} -DLOG_LEVEL=LOG_LEVEL_DEBUG"

    sh """#!/bin/bash
        export PATH=$PATH:/opt/arm-gnu-toolchain-11.3.rel1-x86_64-arm-none-eabi/bin
        python3 set_up_default_resources.py --additional-ethos-u-config-name ethos-u55-256
        mkdir $build_path
        pushd $build_path
        $cmake_cmd
        make -j 4
        popd"""
}

def build_kws(String build_type, String toolchain) {

    build_path = "build_${toolchain}_he_${build_type}".toLowerCase()
    cmake_cmd = "cmake .. -DTARGET_PLATFORM=ensemble -DTARGET_SUBSYSTEM=RTSS-HE -DUSE_CASE_BUILD=alif_kws -DGLCD_UI=NO -DLINKER_SCRIPT_NAME=ensemble-RTSS-HE-TCM -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-${toolchain}.cmake -DCMAKE_BUILD_TYPE=${build_type} -DLOG_LEVEL=LOG_LEVEL_DEBUG"

    sh """#!/bin/bash
        export PATH=$PATH:/opt/arm-gnu-toolchain-11.3.rel1-x86_64-arm-none-eabi/bin
        python3 set_up_default_resources.py --additional-ethos-u-config-name ethos-u55-256
        mkdir $build_path
        pushd $build_path
        $cmake_cmd
        make -j 4
        popd"""
}

return [
    build_img: this.&build_img,
    build_kws: this.&build_kws,
]
