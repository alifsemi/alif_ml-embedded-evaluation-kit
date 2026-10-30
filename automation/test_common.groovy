/**
 * Prefix for the log lines of an agent and a stage, see psh.
 */
def log_prefix(String stage_name) {
    return "[${env.NODE_NAME}|${stage_name}]"
}

/**
 * Same as the sh step but every output line gets the prefix of env.LOG_PREFIX (see log_prefix
 * and withEnv in build_stage and test_stage, the agent name only if not set). The console of a
 * parallel build mixes the output of all the agents, with the prefix the lines of different
 * agents can be told apart. The time stamp of each line is added by the timestamps() option of
 * the pipeline. The script is run with bash and the options -xe like the scripts of the callers
 * had in their shebang.
 *
 * @param script  The shell script.
 */
def psh(String script) {
    writeFile file: '.psh_script.sh', text: script + '\n'
    sh '''#!/bin/bash
bash -xe .psh_script.sh 2>&1 | awk 'BEGIN { p = ENVIRON["LOG_PREFIX"]; if (p == "") p = "[" ENVIRON["NODE_NAME"] "]" } { print p " " $0; fflush() }'
rc=${PIPESTATUS[0]}
rm -f .psh_script.sh
exit $rc
'''
}

def download_dependencies() {
    sh "rm -Rf dependencies"
    sh "python3.10 download_dependencies.py"
}

def setup_resources() {
    /* CI builds only use TensorFlow Lite Micro, so install the trimmed requirements (no torch,
     * CUDA or ExecuTorch, several GB) and run the downloads and Vela optimisations in parallel. */
    psh "#!/bin/bash\npython3.10 set_up_default_resources.py --additional-ethos-u-config-name ethos-u55-256 " +
        "--requirements-file scripts/py/requirements_tflm.txt --parallel \$(nproc)"

    /* Temp workaround for Jenkins encoding issue. These PDF filenames are written with some Chinese characters */
    sh "rm -rf dependencies/Arm-2D/examples/common/loader/zhrgb565_loader/zhrgb565/*.pdf"
}

/**@
* Builds project.
*
* @param build_type Release or Debug
* @param toolchain  Used toolchain, gcc or armclang
*/
def build_hp(String build_type, String toolchain, String board, String extra_cmake_options = "") {

    def build_path = "build_${toolchain}_hp_${build_type}_${board}".toLowerCase()
    def cmake_cmd = "cmake .. -DTARGET_PLATFORM=alif -DTARGET_SUBSYSTEM=RTSS-HP -DUSE_CASE_BUILD=alif_img_class\\;alif_object_detection\\;alif_ad\\;alif_vww -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-${toolchain}.cmake -DCMAKE_BUILD_TYPE=${build_type} -DMLEK_LOG_LEVEL=MLEK_LOG_LEVEL_INFO -DTARGET_BOARD=${board} ${extra_cmake_options}"

    psh """#!/bin/bash -xe
        export PATH=$PATH:/opt/arm-gnu-toolchain-12.3.rel1-x86_64-arm-none-eabi/bin
        export PATH=$PATH:/opt/ArmCompilerforEmbedded6.23/bin
        echo $PATH
        which armclang
        mkdir $build_path
        pushd $build_path
        $cmake_cmd &&
        make -j 4
        exit_code=\$?
        popd
        exit \$exit_code"""
}

def build_he_tcm(String build_type, String toolchain, String board) {

    def build_path = "build_${toolchain}_he_tcm_${build_type}_${board}".toLowerCase()
    def cmake_cmd = "cmake .. -DTARGET_PLATFORM=alif -DTARGET_SUBSYSTEM=RTSS-HE -DUSE_CASE_BUILD=alif_kws -DGLCD_UI=NO -DLINKER_SCRIPT_NAME=RTSS-HE-TCM -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-${toolchain}.cmake -DCMAKE_BUILD_TYPE=${build_type} -DMLEK_LOG_LEVEL=MLEK_LOG_LEVEL_INFO -DTARGET_BOARD=${board} -DINTERACTIVE_MODE=1"

    psh """#!/bin/bash -xe
        export PATH=$PATH:/opt/arm-gnu-toolchain-12.3.rel1-x86_64-arm-none-eabi/bin
        export PATH=$PATH:/opt/ArmCompilerforEmbedded6.23/bin
        echo $PATH
        which armclang
        mkdir $build_path
        pushd $build_path
        $cmake_cmd &&
        make -j 4
        exit_code=\$?
        popd
        exit \$exit_code"""
}

def build_inf_runner(String build_type, String toolchain, String board, String core) {

    def build_path = "build_${toolchain}_${core}_${build_type}_${board}_inf_runner".toLowerCase()
    def cmake_cmd = "cmake .. -DTARGET_PLATFORM=alif -DTARGET_SUBSYSTEM=RTSS-${core} -DUSE_CASE_BUILD=inference_runner -DLINKER_SCRIPT_NAME=RTSS-${core}-infrun -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-${toolchain}.cmake -DCMAKE_BUILD_TYPE=${build_type} -DMLEK_LOG_LEVEL=MLEK_LOG_LEVEL_INFO -DTARGET_BOARD=${board} -DGLCD_UI=NO"

    psh """#!/bin/bash -xe
        export PATH=$PATH:/opt/arm-gnu-toolchain-12.3.rel1-x86_64-arm-none-eabi/bin
        export PATH=$PATH:/opt/ArmCompilerforEmbedded6.23/bin
        echo $PATH
        which armclang
        mkdir $build_path
        pushd $build_path
        $cmake_cmd &&
        make -j 4
        exit_code=\$?
        popd
        exit \$exit_code"""
}

/**
 * Builds the CI power test binaries.
 *
 * @param extra_cmake_options  Optional extra CMake options, passed as they are, e.g.
 *                             '-DNO_HELIUM=ON -DETHOS_U_NPU_ENABLED=OFF'
 */
def build_ci_power_test(String core, String board, String toolchain, String npu_id, String linker_script_name, String ml_framework, String use_case="all", String extra_cmake_options="") {
    def build_path = "build_ci_power_test_${core}_${board}_${toolchain}_${npu_id}_${linker_script_name}_${ml_framework}".toLowerCase()

    /* Keep builds with different options in separate directories */
    if (extra_cmake_options.contains('-DNO_HELIUM=ON')) {
        build_path += "_no_helium"
    }
    if (extra_cmake_options.contains('-DETHOS_U_NPU_ENABLED=OFF')) {
        build_path += "_no_npu"
    }

    def use_case_build
    if (use_case == "all") {
        use_case_build = "alif_img_class\\;alif_object_detection\\;alif_ad\\;alif_vww\\;alif_asr\\;alif_kws"
    } else {
        use_case_build = use_case
    }

    def cmake_cmd = "cmake .. -DALIF_CAMERA_MODULE=OV5675 -DTARGET_PLATFORM=alif -DTARGET_SUBSYSTEM=RTSS-${core} -DUSE_CASE_BUILD=${use_case_build} -DETHOS_U_NPU_ID=${npu_id} -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-${toolchain}.cmake -DCMAKE_BUILD_TYPE=Release -DMLEK_LOG_LEVEL=MLEK_LOG_LEVEL_INFO -DTARGET_BOARD=${board} -DGPIO_PROFILING=ON -DGPIO_PROFILING_CI=ON -DLINKER_SCRIPT_NAME=${linker_script_name} -DML_FRAMEWORK=${ml_framework} -DGLCD_UI=NO -DNO_DISPLAY=ON ${extra_cmake_options}"

    psh """#!/bin/bash -xe
        export PATH=$PATH:/opt/arm-gnu-toolchain-12.3.rel1-x86_64-arm-none-eabi/bin
        export PATH=$PATH:/opt/ArmCompilerforEmbedded6.23/bin
        echo $PATH
        which armclang
        mkdir $build_path
        pushd $build_path
        $cmake_cmd && make -j 4
        exit_code=\$?
        popd
        exit \$exit_code"""
}

/**
 * Write the SE tools configuration file (json) of an application binary, which is given to
 * app-gen-toc. Used instead of a json file of the repository for each core, linker script and
 * binary combination.
 *
 * @param flash   Map with core (HE or HP), mram (MRAM address of the linker script, e.g.
 *                '0x80008000') and optionally device_config (default app-device-config.json).
 * @param binary  The name of the binary the application is flashed with.
 * @return        The name of the written file.
 */
def flash_config_file(Map flash, String binary) {
    def device_config = flash.device_config ?: 'app-device-config.json'
    def file_name = binary.replaceFirst(/\.bin$/, '') + '_' + flash.core.toLowerCase() + '.json'
    writeFile file: file_name, text: """{
    "${flash.core}_Image": {
        "binary": "${binary}",
        "version": "1.0.0",
        "mramAddress": "${flash.mram}",
        "cpu_id": "M55_${flash.core}",
        "flags": ["boot"],
        "signed": false
    },
    "DEVICE": {
        "disabled": false,
        "binary": "${device_config}",
        "version": "0.5.00",
        "signed": true
    }
}
"""
    return file_name
}

/**
 * Write a binary to the MRAM of the device under test with the SE tools. Fails if the device is
 * not in the maintenance mode.
 *
 * @param jsonfile             SE tools json file of the binary, with or without the directories.
 * @param build_dir            Directory of the binary.
 * @param source_binary_name   Name of the binary in build_dir.
 * @param target_binary_name   Name of the binary in the json file.
 */
def flash_mram(String jsonfile, String build_dir, String source_binary_name, String target_binary_name) {
    def json_name = jsonfile.substring(jsonfile.lastIndexOf('/') + 1)
    psh """#!/bin/bash -xe
        printenv NODE_NAME
        cat $ALIF_SETOOLS_ORIG/isp_config_data.cfg
        rsync -a --delete $ALIF_SETOOLS_ORIG $ALIF_SETOOLS_LOCATION
        cp $build_dir/$source_binary_name $ALIF_SETOOLS_LOCATION/build/images/$target_binary_name
        cp $jsonfile $ALIF_SETOOLS_LOCATION/build/config/
        pushd $ALIF_SETOOLS_LOCATION/
        {
            ./app-gen-toc --filename build/config/$json_name
            ./app-write-mram -e app
            ./app-write-mram -p
        } 2>&1 | tee $WORKSPACE/flash_device.log
        popd"""

    if (fileExists('flash_device.log') &&
        readFile('flash_device.log').contains('Maintenance Mode = Disabled')) {
        error "Failed, Maintenance Mode not Enabled... No reason to continue execution"
    }
}

def flash_and_run_pytest(String jsonfile, String build_dir, String source_binary_name, String target_binary_name, String test_name, String extra_pytest_options = "") {
    try {
        flash_mram(jsonfile, build_dir, source_binary_name, target_binary_name)

        def run_pytest = "\
        pytest \
        -s -v -k $test_name ${extra_pytest_options} \
        --tb=native \
        --root-logdir=pytest-logs \
        --junit-prefix=$target_binary_name \
        --html-report=html-report-${target_binary_name}.html \
        --junit-xml junit/junit-report-${target_binary_name}.xml"

        psh """#!/bin/bash -xe
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
    } catch (Exception e) {
        echo "${env.LOG_PREFIX} Exception occurred: " + e.toString()
    }
}

/**
 * Archive the result files of one pytest run (see flash_and_run_pytest).
 */
def archive_pytest_results(String jsonfile, String target_binary_name) {
    archiveArtifacts artifacts: jsonfile, fingerprint: false, allowEmptyArchive: true
    archiveArtifacts artifacts: '*/junit/*', fingerprint: false, allowEmptyArchive: true
    archiveArtifacts artifacts: 'pytest/pytest-logs/pytest-embedded/**', fingerprint: false, allowEmptyArchive: true
    archiveArtifacts artifacts: 'pytest/html-report-' + target_binary_name + '.html', fingerprint: false, allowEmptyArchive: true
    junit skipPublishingChecks: true, testResults: 'pytest/junit/junit-report-' + target_binary_name + '.xml'
}

def clean_workspace() {
    cleanWs(cleanWhenNotBuilt: false, deleteDirs: true, disableDeferredWipeout: true, notFailBuild: true)
}

/**
 * Unstash the given stashes, the ones which do not exist (the stage which should have
 * created it failed early) are skipped.
 */
def unstash_if_exists(List stash_names) {
    for (stash_name in stash_names) {
        try {
            unstash name: stash_name
        } catch (Exception e) {
            echo "Stash ${stash_name} not available: " + e.toString()
        }
    }
}

/**
 * Run one build of a build stage definition, see ci_stages.groovy.
 *
 * @param build_def  [type, arguments of the build function]
 */
def run_build(List build_def) {
    if (build_def[0] == 'hp') {
        build_hp(build_def[1], build_def[2], build_def[3], build_def.size() > 4 ? build_def[4] : "")
    } else if (build_def[0] == 'he_tcm') {
        build_he_tcm(build_def[1], build_def[2], build_def[3])
    } else if (build_def[0] == 'inf_runner') {
        build_inf_runner(build_def[1], build_def[2], build_def[3], build_def[4])
    } else if (build_def[0] == 'ci_power_test') {
        build_ci_power_test(build_def[1], build_def[2], build_def[3], build_def[4], build_def[5], build_def[6], build_def[7], build_def.size() > 8 ? build_def[8] : "")
    } else {
        error "Unknown build type ${build_def[0]}"
    }
}

/**
 * Run a build stage definition (see ci_stages.groovy) on a build agent. The results are archived
 * and stashed for the test stages only if all the builds succeed.
 */
def build_stage(Map stage_def) {
    node('docker_build2') {
        withEnv(["LOG_PREFIX=" + log_prefix(stage_def.name)]) {
            echo "${env.LOG_PREFIX} Running on ${env.NODE_NAME}"
            unstash name: "ml_base_stash"
            for (build_def in stage_def.builds) {
                run_build(build_def)
            }
            if (stage_def.artifact) {
                archiveArtifacts artifacts: '*/bin/*', fingerprint: true
                archiveArtifacts artifacts: '*/bin/sectors/**/' + stage_def.artifact, fingerprint: true
                for (stash_def in stage_def.stashes) {
                    stash name: stash_def[0], includes: stash_def[1]
                }
            }
        }
    }
}

/**
 * Write a file (e.g. the model of a use case which is too large for the MRAM) to the OSPI flash
 * of the device under test with the alif_usb-to-ospi-flasher. The flasher application
 * (automation/flasher, for DevKit-E8 HE) is first written to the MRAM, then the file is sent to
 * it over the USB serial port with XMODEM, see send_file_via_xmodem. The serial port of the
 * flasher is read from the environment variable OSPI_FLASHER_PORT of the agent. The test
 * binary written after this replaces the flasher in the MRAM, the OSPI flash content stays.
 *
 * @param file_path  Path of the file, including the directories (e.g. the ext_flash.bin of a build)
 */
def flash_ospi_file(String file_path) {
    if (!env.OSPI_FLASHER_PORT) {
        error "OSPI_FLASHER_PORT is not set on ${env.NODE_NAME}, cannot write ${file_path} to the OSPI flash"
    }
    if (!fileExists(file_path)) {
        error "${file_path} to be written to the OSPI flash does not exist, is it unstashed?"
    }
    echo "${env.LOG_PREFIX} Writing ${file_path} to the OSPI flash"
    flash_mram('automation/flasher/e8_flasher-cfg.json', 'automation/flasher', 'flasher_e8_he.bin', 'flasher_e8_he.bin')

    /* The USB serial port appears when the flasher has booted */
    psh """#!/bin/bash -xe
        for i in \$(seq 30); do
            [ -c '${env.OSPI_FLASHER_PORT}' ] && exit 0
            sleep 1
        done
        echo "${env.OSPI_FLASHER_PORT} did not appear, is the flasher running?"
        exit 1"""
    send_file_via_xmodem(env.OSPI_FLASHER_PORT, file_path)
}

/**
 * Run a test stage definition (see ci_stages_weekly.groovy) on the agent of the device under test.
 */
def test_stage(Map stage_def) {
    node(stage_def.label) {
        try {
            withEnv(["LOG_PREFIX=" + log_prefix(stage_def.name)]) {
                echo "${env.LOG_PREFIX} Running on ${env.NODE_NAME}"
                /* Either a json file of the repository or generated from the flash definition */
                def json_file = stage_def.json ?: flash_config_file(stage_def.flash, stage_def.bin)
                try {
                    if (stage_def.video) {
                        build job: 'camera_start_record'
                    }
                    unstash name: "ml_base_stash"
                    for (stash_name in stage_def.unstash) {
                        unstash name: stash_name
                    }
                    /* The model of the use case is in the OSPI flash, write it before the test */
                    if (stage_def.ospi_file) {
                        flash_ospi_file(stage_def.ospi_file)
                    }
                    def pytest_options = ""
                    if (stage_def.power) {
                        pytest_options = '--power_results ' + power_results_file(stage_def.power) +
                            ' --power_board "' + stage_def.power.board + '" --power_core ' + stage_def.power.core
                    }
                    flash_and_run_pytest(json_file, stage_def.dir, stage_def.src, stage_def.bin, stage_def.test, pytest_options)
                    if (stage_def.video) {
                        build job: 'camera_stop_record'
                        copyArtifacts(projectName: 'camera_stop_record', filter: 'recording.mov', target: stage_def.video, optional: true)
                        stash name: stage_def.video, includes: stage_def.video + '/recording.mov'
                    }
                } finally {
                    archive_pytest_results(json_file, stage_def.bin)
                    if (stage_def.power) {
                        archiveArtifacts artifacts: 'pytest/' + power_results_file(stage_def.power), fingerprint: false, allowEmptyArchive: true
                        stash name: power_stash_name(stage_def.power), includes: 'pytest/' + power_results_file(stage_def.power), allowEmpty: true
                    }
                    if (stage_def.video) {
                        archiveArtifacts artifacts: stage_def.video + '/recording.mov', fingerprint: false, allowEmptyArchive: true
                    }
                }
            }
        } finally {
            clean_workspace()
        }
    }
}

def power_results_file(Map power) {
    return 'power_results_' + power.id + '_' + power.suffix + '.md'
}

def power_stash_name(Map power) {
    return 'power_results_' + power.id
}

/**
 * Create the parallel branches of build stage definitions.
 */
def build_branches(List stage_defs) {
    def branches = [:]
    for (stage_def in stage_defs) {
        def current = stage_def
        branches[current.name] = { build_stage(current) }
    }
    return branches
}

/**
 * Create the parallel branches of test stage definitions.
 */
def test_branches(List stage_defs) {
    def branches = [:]
    for (stage_def in stage_defs) {
        def current = stage_def
        branches[current.name] = { test_stage(current) }
    }
    return branches
}

/**
 * Unstash the results of the power tests of the test stage definitions, for the merge.
 */
def unstash_power_results(List stage_defs) {
    def stash_names = []
    for (stage_def in stage_defs) {
        if (stage_def.power) {
            stash_names.add(power_stash_name(stage_def.power))
        }
    }
    unstash_if_exists(stash_names)
}

/**
 * Send a file to a device over a serial port using classic XMODEM. Runs
 * automation/send_file_via_xmodem.sh, which can also be used locally.
 *
 * @param port       Serial device, e.g. /dev/ttyACM1
 * @param file_path  Path of the file to send
 * @param timeout_s  Maximum duration of the transfer in seconds
 */
def send_file_via_xmodem(String port, String file_path, int timeout_s = 1800) {
    psh "bash automation/send_file_via_xmodem.sh '${port}' '${file_path}' ${timeout_s}"
}

return [
    download_dependencies: this.&download_dependencies,
    setup_resources: this.&setup_resources,
    build_hp: this.&build_hp,
    build_he_tcm: this.&build_he_tcm,
    build_inf_runner: this.&build_inf_runner,
    build_ci_power_test: this.&build_ci_power_test,
    flash_and_run_pytest: this.&flash_and_run_pytest,
    archive_pytest_results: this.&archive_pytest_results,
    clean_workspace: this.&clean_workspace,
    unstash_if_exists: this.&unstash_if_exists,
    build_branches: this.&build_branches,
    test_branches: this.&test_branches,
    unstash_power_results: this.&unstash_power_results,
    send_file_via_xmodem: this.&send_file_via_xmodem,
    flash_mram: this.&flash_mram,
    flash_ospi_file: this.&flash_ospi_file,
    flash_config_file: this.&flash_config_file,
]
