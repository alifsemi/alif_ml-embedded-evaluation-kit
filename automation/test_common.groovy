def download_dependencies() {
    sh "rm -Rf dependencies"
    sh "python3.10 download_dependencies.py"
}

def setup_resources() {

    sh "python3.10 set_up_default_resources.py --additional-ethos-u-config-name ethos-u55-256"
}

/**@
* Builds project.
*
* @param build_type Release or Debug
* @param toolchain  Used toolchain, gcc or armclang
*/
def build_hp(String build_type, String toolchain, String silicon_rev) {

    build_path = "build_${toolchain}_hp_${build_type}_rev_${silicon_rev}".toLowerCase()
    cmake_cmd = "cmake .. -DTARGET_PLATFORM=ensemble -DTARGET_SUBSYSTEM=RTSS-HP -DUSE_CASE_BUILD=alif_img_class\\;alif_object_detection\\;alif_ad\\;alif_vww -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-${toolchain}.cmake -DCMAKE_BUILD_TYPE=${build_type} -DLOG_LEVEL=LOG_LEVEL_DEBUG -DTARGET_REVISION=${silicon_rev} -DTARGET_BOARD=DevKit"

    sh """#!/bin/bash -xe
        export PATH=$PATH:/opt/arm-gnu-toolchain-12.3.rel1-x86_64-arm-none-eabi/bin
        mkdir $build_path
        pushd $build_path
        $cmake_cmd &&
        make -j 4
        exit_code=\$?
        popd
        exit \$exit_code"""
}

def build_he_tcm(String build_type, String toolchain, String silicon_rev) {

    build_path = "build_${toolchain}_he_tcm_${build_type}_rev_${silicon_rev}".toLowerCase()
    cmake_cmd = "cmake .. -DTARGET_PLATFORM=ensemble -DTARGET_SUBSYSTEM=RTSS-HE -DUSE_CASE_BUILD=alif_kws -DGLCD_UI=NO -DLINKER_SCRIPT_NAME=ensemble-RTSS-HE-TCM -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-${toolchain}.cmake -DCMAKE_BUILD_TYPE=${build_type} -DLOG_LEVEL=LOG_LEVEL_DEBUG -DTARGET_REVISION=${silicon_rev} -DTARGET_BOARD=DevKit -Dalif_kws_USE_APP_MENU=1"

    sh """#!/bin/bash -xe
        export PATH=$PATH:/opt/arm-gnu-toolchain-12.3.rel1-x86_64-arm-none-eabi/bin
        mkdir $build_path
        pushd $build_path
        $cmake_cmd &&
        make -j 4
        exit_code=\$?
        popd
        exit \$exit_code"""
}

def flash_and_run_pytest(String jsonfile, String build_dir, String source_binary_name, String target_binary_name) {
    sh """#!/bin/bash -xe
        printenv NODE_NAME
        cat $ALIF_SETOOLS_ORIG/isp_config_data.cfg
        rsync -a --delete $ALIF_SETOOLS_ORIG $ALIF_SETOOLS_LOCATION
        cp $build_dir/$source_binary_name $ALIF_SETOOLS_LOCATION/build/images/$target_binary_name
        cp $jsonfile $ALIF_SETOOLS_LOCATION/build/config/
        pushd $ALIF_SETOOLS_LOCATION/
        ./app-gen-toc --filename build/config/$jsonfile
        ./app-write-mram -p
        popd"""

    logs = currentBuild.rawBuild.getLog(10000).join('\n').toString()
    //if (logs.contains('Maintenance Mode = Disabled')) {
    //    error "Failed, Maintenance Mode not Enabled... No reason to continue execution"
    //}

    def run_pytest = "\
    pytest \
    -s \
    --tb=native \
    --root-logdir=pytest-logs \
    --junit-prefix=$target_binary_name \
    --html-report=html-report-${target_binary_name}.html \
    --junit-xml junit/junit-report-${target_binary_name}.xml"

    sh """#!/bin/bash -xe
        set +e
        pushd pytest
        sed -e "s/ttyUSB0/ttyAlifUART2$DUT_NUMBER/g" pytest.ini.template > pytest.ini
        cat pytest.ini
        mkdir junit
        mkdir pytest-logs
        mkdir venv
        python3 -m venv venv
        . venv/bin/activate
        pip install -r requirements.txt
        $run_pytest
        deactivate
        rsync -a --delete $ALIF_SETOOLS_ORIG $ALIF_SETOOLS_LOCATION
        pushd $ALIF_SETOOLS_LOCATION/
        ./app-gen-toc --filename build/config/app-cpu-stubs.json
        ./app-write-mram
        popd"""
}

return [
    download_dependencies: this.&download_dependencies,
    setup_resources: this.&setup_resources,
    build_hp: this.&build_hp,
    build_he_tcm: this.&build_he_tcm,
    flash_and_run_pytest: this.&flash_and_run_pytest,
]
