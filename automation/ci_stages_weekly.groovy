/* Copyright (C) 2026 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 */

/*
 * Definitions of the stages run in parallel by the Jenkinsfile. The Jenkinsfile creates the
 * parallel branches from these lists (see build_branches() and test_branches() in
 * test_common.groovy), so adding a build or a test means adding an entry here instead of
 * copy-pasting a stage to the Jenkinsfile. That keeps the Jenkinsfile small: a declarative
 * pipeline is compiled to one method and the JVM limits the size of a method to 64 KB.
 *
 * The lists are in separate functions to keep the size of each function small too. If a
 * function grows large, add a new one and return it from the same group.
 *
 * Build stage entries:
 *   name:      Name of the parallel branch.
 *   builds:    Builds to run in order, each [<type>, <arguments of the build function>] where the
 *              type is hp, he_tcm, inf_runner or ci_power_test, see test_common.groovy.
 *   artifact:  Name pattern of the binaries archived from bin/sectors of the build directories
 *              (*.bin or ER_ROM), if the results are kept.
 *   stashes:   [stash name, path] pairs to stash for the test stages, if the results are kept.
 *
 * Test stage entries:
 *   name:      Name of the parallel branch.
 *   label:     Agent label of the device under test.
 *   unstash:   Stashes of the binaries to test.
 *   flash:     Flash configuration of the binary: core (HE or HP), mram (the MRAM address of the
 *              linker script) and optionally device_config. The SE tools json file is generated
 *              from it, see flash_config_file in test_common.groovy.
 *   dir, src, bin:  Binary directory, name of the binary there and name it is flashed with.
 *   test:      pytest -k expression.
 *   ospi_file: Optional. Path of a file (including the directories, from the workspace after the
 *              unstash) to write to the OSPI flash before the test, e.g. the ext_flash.bin of a
 *              use case with the model in the OSPI flash. The usb-to-ospi-flasher from
 *              automation/flasher is flashed to the device first, see flash_ospi_file in
 *              test_common.groovy.
 *   video:     Optional. Record the device with the camera job, the video is stashed with this name.
 *   power:     Optional. Power test, the results are written to
 *              power_results_<id>_<suffix>.md and stashed as power_results_<id>.
 */


def initial_weekly_builds() {
    return [
        /* U55 builds HP-E8 */
        [name: 'Build HP GCC Release CI Power Test for DevKit-e8',
         builds: [['ci_power_test', 'HP', 'DevKit-e8', 'gcc', 'U55', 'RTSS-HP', 'TensorFlowLiteMicro', 'all']],
         artifact: '*.bin',
         stashes: [
             ['test_build_ci_power_test_img_hp_devkit_e8', 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro/bin/sectors/alif_img_class/mram.bin'],
             ['test_build_ci_power_test_obj_hp_devkit_e8', 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro/bin/sectors/alif_object_detection/mram.bin'],
             ['test_build_ci_power_test_ad_hp_devkit_e8', 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro/bin/sectors/alif_ad/mram.bin'],
             ['test_build_ci_power_test_vww_hp_devkit_e8', 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro/bin/sectors/alif_vww/mram.bin'],
             ['test_build_ci_power_test_asr_hp_devkit_e8', 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro/bin/sectors/alif_asr/mram.bin'],
             ['test_build_ci_power_test_asr_hp_devkit_e8_ext_flash', 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro/bin/sectors/alif_asr/ext_flash.bin'],
             ['test_build_ci_power_test_kws_hp_devkit_e8', 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro/bin/sectors/alif_kws/mram.bin']]],
        /* U55 builds HE-E1C */
        [name: 'Build HE GCC Release CI Power Test for DevKit-E1C',
         builds: [['ci_power_test', 'HE', 'DevKit-e1c', 'gcc', 'U55', 'RTSS-HE-TCM', 'TensorFlowLiteMicro', 'alif_kws']],
         artifact: '*.bin',
         stashes: [
             ['test_build_ci_power_test_kws_he_devkit_e1c', 'build_ci_power_test_he_devkit-e1c_gcc_u55_rtss-he-tcm_tensorflowlitemicro/bin/sectors/alif_kws/mram.bin']]],
    ]
}

def more_weekly_builds() {
    return [
        /* U55 builds HE-E8 */
        [name: 'Build HE GCC Release CI Power Test for DevKit-e8',
         builds: [['ci_power_test', 'HE', 'DevKit-e8', 'gcc', 'U55', 'RTSS-HE', 'TensorFlowLiteMicro', 'all'],
                  ['ci_power_test', 'HE', 'DevKit-e8', 'gcc', 'U55', 'RTSS-HE-TCM', 'TensorFlowLiteMicro', 'alif_kws']],
         artifact: '*.bin',
         stashes: [
             ['test_build_ci_power_test_img_he_devkit_e8', 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he_tensorflowlitemicro/bin/sectors/alif_img_class/mram.bin'],
             ['test_build_ci_power_test_obj_he_devkit_e8', 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he_tensorflowlitemicro/bin/sectors/alif_object_detection/mram.bin'],
             ['test_build_ci_power_test_ad_he_devkit_e8', 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he_tensorflowlitemicro/bin/sectors/alif_ad/mram.bin'],
             ['test_build_ci_power_test_vww_he_devkit_e8', 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he_tensorflowlitemicro/bin/sectors/alif_vww/mram.bin'],
             ['test_build_ci_power_test_asr_he_devkit_e8', 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he_tensorflowlitemicro/bin/sectors/alif_asr/mram.bin'],
             ['test_build_ci_power_test_asr_he_devkit_e8_ext_flash', 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he_tensorflowlitemicro/bin/sectors/alif_asr/ext_flash.bin'],
             ['test_build_ci_power_test_kws_he_devkit_e8', 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he-tcm_tensorflowlitemicro/bin/sectors/alif_kws/mram.bin']]],
        /* U85 builds HP-E8 */
        [name: 'Build HP GCC Release CI Power Test for DevKit-e8 U85',
         builds: [['ci_power_test', 'HP', 'DevKit-e8', 'gcc', 'U85', 'RTSS-HP', 'TensorFlowLiteMicro', 'all']],
         artifact: '*.bin',
         stashes: [
             ['test_build_ci_power_test_img_hp_devkit_e8_u85', 'build_ci_power_test_hp_devkit-e8_gcc_u85_rtss-hp_tensorflowlitemicro/bin/sectors/alif_img_class/mram.bin'],
             ['test_build_ci_power_test_obj_hp_devkit_e8_u85', 'build_ci_power_test_hp_devkit-e8_gcc_u85_rtss-hp_tensorflowlitemicro/bin/sectors/alif_object_detection/mram.bin'],
             ['test_build_ci_power_test_ad_hp_devkit_e8_u85', 'build_ci_power_test_hp_devkit-e8_gcc_u85_rtss-hp_tensorflowlitemicro/bin/sectors/alif_ad/mram.bin'],
             ['test_build_ci_power_test_vww_hp_devkit_e8_u85', 'build_ci_power_test_hp_devkit-e8_gcc_u85_rtss-hp_tensorflowlitemicro/bin/sectors/alif_vww/mram.bin'],
             ['test_build_ci_power_test_asr_hp_devkit_e8_u85', 'build_ci_power_test_hp_devkit-e8_gcc_u85_rtss-hp_tensorflowlitemicro/bin/sectors/alif_asr/mram.bin'],
             ['test_build_ci_power_test_asr_hp_devkit_e8_u85_ext_flash', 'build_ci_power_test_hp_devkit-e8_gcc_u85_rtss-hp_tensorflowlitemicro/bin/sectors/alif_asr/ext_flash.bin'],
             ['test_build_ci_power_test_kws_hp_devkit_e8_u85', 'build_ci_power_test_hp_devkit-e8_gcc_u85_rtss-hp_tensorflowlitemicro/bin/sectors/alif_kws/mram.bin']]],
        /* U85 builds HE-E8 */
        [name: 'Build HE GCC Release CI Power Test for DevKit-e8 U85',
         builds: [['ci_power_test', 'HE', 'DevKit-e8', 'gcc', 'U85', 'RTSS-HE', 'TensorFlowLiteMicro', 'all'],
                  ['ci_power_test', 'HE', 'DevKit-e8', 'gcc', 'U85', 'RTSS-HE-TCM', 'TensorFlowLiteMicro', 'alif_kws']],
         artifact: '*.bin',
         stashes: [
             ['test_build_ci_power_test_img_he_devkit_e8_u85', 'build_ci_power_test_he_devkit-e8_gcc_u85_rtss-he_tensorflowlitemicro/bin/sectors/alif_img_class/mram.bin'],
             ['test_build_ci_power_test_obj_he_devkit_e8_u85', 'build_ci_power_test_he_devkit-e8_gcc_u85_rtss-he_tensorflowlitemicro/bin/sectors/alif_object_detection/mram.bin'],
             ['test_build_ci_power_test_ad_he_devkit_e8_u85', 'build_ci_power_test_he_devkit-e8_gcc_u85_rtss-he_tensorflowlitemicro/bin/sectors/alif_ad/mram.bin'],
             ['test_build_ci_power_test_vww_he_devkit_e8_u85', 'build_ci_power_test_he_devkit-e8_gcc_u85_rtss-he_tensorflowlitemicro/bin/sectors/alif_vww/mram.bin'],
             ['test_build_ci_power_test_asr_he_devkit_e8_u85', 'build_ci_power_test_he_devkit-e8_gcc_u85_rtss-he_tensorflowlitemicro/bin/sectors/alif_asr/mram.bin'],
             ['test_build_ci_power_test_asr_he_devkit_e8_u85_ext_flash', 'build_ci_power_test_he_devkit-e8_gcc_u85_rtss-he_tensorflowlitemicro/bin/sectors/alif_asr/ext_flash.bin'],
             ['test_build_ci_power_test_kws_he_devkit_e8_u85', 'build_ci_power_test_he_devkit-e8_gcc_u85_rtss-he-tcm_tensorflowlitemicro/bin/sectors/alif_kws/mram.bin']]],
        /* Builds without Helium and NPU HE-E8. Use less inference rounds as without NPU and Helium inference is so much slower (-DGPIO_PROFILING_CI_SKIP_ROUNDS=10) */
        [name: 'Build HE GCC Release CI Power Without Helium and NPU for DevKit-e8',
         builds: [['ci_power_test', 'HE', 'DevKit-e8', 'gcc', 'U55', 'RTSS-HE', 'TensorFlowLiteMicro', 'all', '-DNO_HELIUM=ON -DETHOS_U_NPU_ENABLED=OFF -DGPIO_PROFILING_CI_SKIP_ROUNDS=10'],
                  ['ci_power_test', 'HE', 'DevKit-e8', 'gcc', 'U55', 'RTSS-HE-TCM', 'TensorFlowLiteMicro', 'alif_kws', '-DNO_HELIUM=ON -DETHOS_U_NPU_ENABLED=OFF']],
         artifact: '*.bin',
         stashes: [
             ['test_build_ci_power_test_img_he_devkit_e8_no_helium_no_npu', 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he_tensorflowlitemicro_no_helium_no_npu/bin/sectors/alif_img_class/mram.bin'],
             ['test_build_ci_power_test_obj_he_devkit_e8_no_helium_no_npu', 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he_tensorflowlitemicro_no_helium_no_npu/bin/sectors/alif_object_detection/mram.bin'],
             ['test_build_ci_power_test_ad_he_devkit_e8_no_helium_no_npu', 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he_tensorflowlitemicro_no_helium_no_npu/bin/sectors/alif_ad/mram.bin'],
             ['test_build_ci_power_test_vww_he_devkit_e8_no_helium_no_npu', 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he_tensorflowlitemicro_no_helium_no_npu/bin/sectors/alif_vww/mram.bin'],
             ['test_build_ci_power_test_asr_he_devkit_e8_no_helium_no_npu', 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he_tensorflowlitemicro_no_helium_no_npu/bin/sectors/alif_asr/mram.bin'],
             ['test_build_ci_power_test_kws_he_devkit_e8_no_helium_no_npu', 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he-tcm_tensorflowlitemicro_no_helium_no_npu/bin/sectors/alif_kws/mram.bin']]],
        /* Builds without Helium and NPU HP-E8. Use less inference rounds as without NPU and Helium inference is so much slower (-DGPIO_PROFILING_CI_SKIP_ROUNDS=10) */
        [name: 'Build HP GCC Release CI Power Without Helium and NPU for DevKit-e8',
         builds: [['ci_power_test', 'HP', 'DevKit-e8', 'gcc', 'U55', 'RTSS-HP', 'TensorFlowLiteMicro', 'all', '-DNO_HELIUM=ON -DETHOS_U_NPU_ENABLED=OFF -DGPIO_PROFILING_CI_SKIP_ROUNDS=10']],
         artifact: '*.bin',
         stashes: [
             ['test_build_ci_power_test_img_hp_devkit_e8_no_helium_no_npu', 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro_no_helium_no_npu/bin/sectors/alif_img_class/mram.bin'],
             ['test_build_ci_power_test_obj_hp_devkit_e8_no_helium_no_npu', 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro_no_helium_no_npu/bin/sectors/alif_object_detection/mram.bin'],
             ['test_build_ci_power_test_ad_hp_devkit_e8_no_helium_no_npu', 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro_no_helium_no_npu/bin/sectors/alif_ad/mram.bin'],
             ['test_build_ci_power_test_vww_hp_devkit_e8_no_helium_no_npu', 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro_no_helium_no_npu/bin/sectors/alif_vww/mram.bin'],
             ['test_build_ci_power_test_asr_hp_devkit_e8_no_helium_no_npu', 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro_no_helium_no_npu/bin/sectors/alif_asr/mram.bin'],
             ['test_build_ci_power_test_kws_hp_devkit_e8_no_helium_no_npu', 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro_no_helium_no_npu/bin/sectors/alif_kws/mram.bin']]],

        /* ASR Use case with model in OSPI Flash takes so much time without Helium and NPU that is must start from the first inference round */
        [name: 'Build HE GCC ASR Release CI Power Without Helium and NPU for DevKit-e8',
         builds: [['ci_power_test', 'HE', 'DevKit-e8', 'gcc', 'U55', 'RTSS-HE', 'TensorFlowLiteMicro', 'alif_asr', '-DNO_HELIUM=ON -DETHOS_U_NPU_ENABLED=OFF -DGPIO_PROFILING_CI_SKIP_ROUNDS=10'],
         artifact: '*.bin',
         stashes: [
             ['test_build_ci_power_test_asr_he_devkit_e8_no_helium_no_npu_ext_flash', 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he_tensorflowlitemicro_no_helium_no_npu/bin/sectors/alif_asr/ext_flash.bin']]],
        /* ASR Use case with model in OSPI Flash takes so much time without Helium and NPU that is must start from the first inference round */
        [name: 'Build HP GCC ASR Release CI Power Without Helium and NPU for DevKit-e8',
         builds: [['ci_power_test', 'HP', 'DevKit-e8', 'gcc', 'U55', 'RTSS-HP', 'TensorFlowLiteMicro', 'alif_asr', '-DNO_HELIUM=ON -DETHOS_U_NPU_ENABLED=OFF -DGPIO_PROFILING_CI_SKIP_ROUNDS=10']],
         artifact: '*.bin',
         stashes: [
             ['test_build_ci_power_test_asr_hp_devkit_e8_no_helium_no_npu_ext_flash', 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro_no_helium_no_npu/bin/sectors/alif_asr/ext_flash.bin']]],
    ]
}

def tests() {
    /* Flash configurations (see flash_config_file in test_common.groovy) of the cores and linker scripts */
    def hp = [core: 'HP', mram: '0x80008000']
    def he = [core: 'HE', mram: '0x80008000']
    def he_tcm = [core: 'HE', mram: '0x80480000']
    def he_e1c = [core: 'HE', mram: '0x80000000', device_config: 'app-device-config-1c.json']
    return [
        /* KWS tests */
        /* HE-E8 U55 tests */
        [name: 'Test KWS HE U55 GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_kws_he_devkit_e8'],
         flash: he_tcm,
         dir: 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he-tcm_tensorflowlitemicro/bin/sectors/alif_kws',
         src: 'mram.bin', bin: 'mlek_alif_kws.bin', test: 'test_power_kws',
         power: [id: 'kws_he_devkit_e8', suffix: 'gcc_u55_rtss-he-tcm_tensorflowlitemicro', core: 'HE_U55', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HE-E1C U55 tests */
        [name: 'Test KWS HE U55 GCC Release CI Power Test for DevKit-E1C',
         label: 'E1C_JS', unstash: ['test_build_ci_power_test_kws_he_devkit_e1c'],
         flash: he_e1c,
         dir: 'build_ci_power_test_he_devkit-e1c_gcc_u55_rtss-he-tcm_tensorflowlitemicro/bin/sectors/alif_kws',
         src: 'mram.bin', bin: 'mlek_alif_kws.bin', test: 'test_power_kws',
         power: [id: 'kws_he_devkit_e1c', suffix: 'gcc_u55_rtss-he-tcm_tensorflowlitemicro', core: 'HE_E1C_U55', board: 'DevKit-E1C, SE v112.0']],
        /* HP-E8 U55 tests */
        [name: 'Test KWS HP U55 GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_kws_hp_devkit_e8'],
         flash: hp,
         dir: 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro/bin/sectors/alif_kws',
         src: 'mram.bin', bin: 'mlek_alif_kws.bin', test: 'test_power_kws',
         power: [id: 'kws_hp_devkit_e8', suffix: 'gcc_u55_rtss-hp_tensorflowlitemicro', core: 'HP_U55', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HE-E8 U85 tests */
        [name: 'Test KWS HE U85 GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_kws_he_devkit_e8_u85'],
         flash: he_tcm,
         dir: 'build_ci_power_test_he_devkit-e8_gcc_u85_rtss-he-tcm_tensorflowlitemicro/bin/sectors/alif_kws',
         src: 'mram.bin', bin: 'mlek_alif_kws.bin', test: 'test_power_kws',
         power: [id: 'kws_he_devkit_e8_u85', suffix: 'gcc_u85_rtss-he-tcm_tensorflowlitemicro', core: 'HE_U85', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HP-E8 U85 tests */
        [name: 'Test KWS HP U85 GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_kws_hp_devkit_e8_u85'],
         flash: hp,
         dir: 'build_ci_power_test_hp_devkit-e8_gcc_u85_rtss-hp_tensorflowlitemicro/bin/sectors/alif_kws',
         src: 'mram.bin', bin: 'mlek_alif_kws.bin', test: 'test_power_kws',
         power: [id: 'kws_hp_devkit_e8_u85', suffix: 'gcc_u85_rtss-hp_tensorflowlitemicro', core: 'HP_U85', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HE-E8 without Helium and NPU tests */
        [name: 'Test KWS HE without Helium and NPU GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_kws_he_devkit_e8_no_helium_no_npu'],
         flash: he_tcm,
         dir: 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he-tcm_tensorflowlitemicro_no_helium_no_npu/bin/sectors/alif_kws',
         src: 'mram.bin', bin: 'mlek_alif_kws.bin', test: 'test_power_kws',
         power: [id: 'kws_he_devkit_e8_no_helium_no_npu', suffix: 'gcc_u55_rtss-he-tcm_tensorflowlitemicro_no_helium_no_npu', core: 'HE_NO_HELIUM_NO_NPU', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HP-E8 without Helium and NPU tests */
        [name: 'Test KWS HP without Helium and NPU GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_kws_hp_devkit_e8_no_helium_no_npu'],
         flash: hp,
         dir: 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro_no_helium_no_npu/bin/sectors/alif_kws',
         src: 'mram.bin', bin: 'mlek_alif_kws.bin', test: 'test_power_kws',
         power: [id: 'kws_hp_devkit_e8_no_helium_no_npu', suffix: 'gcc_u55_rtss-hp_tensorflowlitemicro_no_helium_no_npu', core: 'HP_NO_HELIUM_NO_NPU', board: 'DevKit-E8 rev b, SE v112.0']],

        /* Object detection tests */
        /* HP-E8 U55 tests */
        [name: 'Test Object Detection HP U55 GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_obj_hp_devkit_e8'],
         flash: hp,
         dir: 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro/bin/sectors/alif_object_detection',
         src: 'mram.bin', bin: 'mlek_alif_object_detection.bin', test: 'test_power_object_detection',
         power: [id: 'obj_detect_hp_devkit_e8', suffix: 'gcc_u55_rtss-hp_tensorflowlitemicro', core: 'HP_U55', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HE-E8 U55 tests */
        [name: 'Test Object Detection HE U55 GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_obj_he_devkit_e8'],
         flash: he,
         dir: 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he_tensorflowlitemicro/bin/sectors/alif_object_detection',
         src: 'mram.bin', bin: 'mlek_alif_object_detection.bin', test: 'test_power_object_detection',
         power: [id: 'obj_detect_he_devkit_e8', suffix: 'gcc_u55_rtss-he_tensorflowlitemicro', core: 'HE_U55', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HP-E8 U85 tests */
         [name: 'Test Object Detection HP U85 GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_obj_hp_devkit_e8_u85'],
         flash: hp,
         dir: 'build_ci_power_test_hp_devkit-e8_gcc_u85_rtss-hp_tensorflowlitemicro/bin/sectors/alif_object_detection',
         src: 'mram.bin', bin: 'mlek_alif_object_detection.bin', test: 'test_power_object_detection',
         power: [id: 'obj_detect_hp_devkit_e8_u85', suffix: 'gcc_u85_rtss-hp_tensorflowlitemicro', core: 'HP_U85', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HE-E8 U85 tests */
         [name: 'Test Object Detection HE U85 GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_obj_he_devkit_e8_u85'],
         flash: he,
         dir: 'build_ci_power_test_he_devkit-e8_gcc_u85_rtss-he_tensorflowlitemicro/bin/sectors/alif_object_detection',
         src: 'mram.bin', bin: 'mlek_alif_object_detection.bin', test: 'test_power_object_detection',
         power: [id: 'obj_detect_he_devkit_e8_u85', suffix: 'gcc_u85_rtss-he_tensorflowlitemicro', core: 'HE_U85', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HE-E8 without Helium and NPU tests */
         [name: 'Test Object Detection HE without Helium and NPU GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_obj_he_devkit_e8_no_helium_no_npu'],
         flash: he,
         dir: 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he_tensorflowlitemicro_no_helium_no_npu/bin/sectors/alif_object_detection',
         src: 'mram.bin', bin: 'mlek_alif_object_detection.bin', test: 'test_power_object_detection',
         power: [id: 'obj_detect_he_devkit_e8_no_helium_no_npu', suffix: 'gcc_u55_rtss-he_tensorflowlitemicro_no_helium_no_npu', core: 'HE_NO_HELIUM_NO_NPU', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HP-E8 without Helium and NPU tests */
         [name: 'Test Object Detection HP without Helium and NPU GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_obj_hp_devkit_e8_no_helium_no_npu'],
         flash: hp,
         dir: 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro_no_helium_no_npu/bin/sectors/alif_object_detection',
         src: 'mram.bin', bin: 'mlek_alif_object_detection.bin', test: 'test_power_object_detection',
         power: [id: 'obj_detect_hp_devkit_e8_no_helium_no_npu', suffix: 'gcc_u55_rtss-hp_tensorflowlitemicro_no_helium_no_npu', core: 'HP_NO_HELIUM_NO_NPU', board: 'DevKit-E8 rev b, SE v112.0']],


        /* Image classification tests */
        /* HP-E8 U55 tests */
        [name: 'Test Image Classification HP GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_img_hp_devkit_e8'],
         flash: hp,
         dir: 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro/bin/sectors/alif_img_class',
         src: 'mram.bin', bin: 'mlek_alif_img_class.bin', test: 'test_power_img_class',
         power: [id: 'img_class_hp_devkit_e8', suffix: 'gcc_u55_rtss-hp_tensorflowlitemicro', core: 'HP_U55', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HE-E8 U55 tests */
        [name: 'Test Image Classification HE U55 GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_img_he_devkit_e8'],
         flash: he,
         dir: 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he_tensorflowlitemicro/bin/sectors/alif_img_class',
         src: 'mram.bin', bin: 'mlek_alif_img_class.bin', test: 'test_power_img_class',
         power: [id: 'img_class_he_devkit_e8', suffix: 'gcc_u55_rtss-he_tensorflowlitemicro', core: 'HE_U55', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HP-E8 U85 tests */
        [name: 'Test Image Classification HP U85 GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_img_hp_devkit_e8_u85'],
         flash: hp,
         dir: 'build_ci_power_test_hp_devkit-e8_gcc_u85_rtss-hp_tensorflowlitemicro/bin/sectors/alif_img_class',
         src: 'mram.bin', bin: 'mlek_alif_img_class.bin', test: 'test_power_img_class',
         power: [id: 'img_class_hp_devkit_e8_u85', suffix: 'gcc_u85_rtss-hp_tensorflowlitemicro', core: 'HP_U85', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HE-E8 U85 tests */
        [name: 'Test Image Classification HE U85 GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_img_he_devkit_e8_u85'],
         flash: he,
         dir: 'build_ci_power_test_he_devkit-e8_gcc_u85_rtss-he_tensorflowlitemicro/bin/sectors/alif_img_class',
         src: 'mram.bin', bin: 'mlek_alif_img_class.bin', test: 'test_power_img_class',
         power: [id: 'img_class_he_devkit_e8_u85', suffix: 'gcc_u85_rtss-he_tensorflowlitemicro', core: 'HE_U85', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HE-E8 without Helium and NPU tests */
        [name: 'Test Image Classification HE without Helium and NPU GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_img_he_devkit_e8_no_helium_no_npu'],
         flash: he,
         dir: 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he_tensorflowlitemicro_no_helium_no_npu/bin/sectors/alif_img_class',
         src: 'mram.bin', bin: 'mlek_alif_img_class.bin', test: 'test_power_img_class',
         power: [id: 'img_class_he_devkit_e8_no_helium_no_npu', suffix: 'gcc_u55_rtss-he_tensorflowlitemicro_no_helium_no_npu', core: 'HE_NO_HELIUM_NO_NPU', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HP-E8 without Helium and NPU tests */
        [name: 'Test Image Classification HP without Helium and NPU GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_img_hp_devkit_e8_no_helium_no_npu'],
         flash: hp,
         dir: 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro_no_helium_no_npu/bin/sectors/alif_img_class',
         src: 'mram.bin', bin: 'mlek_alif_img_class.bin', test: 'test_power_img_class',
         power: [id: 'img_class_hp_devkit_e8_no_helium_no_npu', suffix: 'gcc_u55_rtss-hp_tensorflowlitemicro_no_helium_no_npu', core: 'HP_NO_HELIUM_NO_NPU', board: 'DevKit-E8 rev b, SE v112.0']],

        /* Visual Wake Word tests */
        /* HP-E8 U55 tests */
        [name: 'Test Visual Wake Word HP GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_vww_hp_devkit_e8'],
         flash: hp,
         dir: 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro/bin/sectors/alif_vww',
         src: 'mram.bin', bin: 'mlek_alif_vww.bin', test: 'test_power_vww',
         power: [id: 'vww_hp_devkit_e8', suffix: 'gcc_u55_rtss-hp_tensorflowlitemicro', core: 'HP_U55', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HE-E8 U55 tests */
        [name: 'Test Visual Wake Word HE U55 GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_vww_he_devkit_e8'],
         flash: he,
         dir: 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he_tensorflowlitemicro/bin/sectors/alif_vww',
         src: 'mram.bin', bin: 'mlek_alif_vww.bin', test: 'test_power_vww',
         power: [id: 'vww_he_devkit_e8', suffix: 'gcc_u55_rtss-he_tensorflowlitemicro', core: 'HE_U55', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HP-E8 U85 tests */
        [name: 'Test Visual Wake Word HP U85 GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_vww_hp_devkit_e8_u85'],
         flash: hp,
         dir: 'build_ci_power_test_hp_devkit-e8_gcc_u85_rtss-hp_tensorflowlitemicro/bin/sectors/alif_vww',
         src: 'mram.bin', bin: 'mlek_alif_vww.bin', test: 'test_power_vww',
         power: [id: 'vww_hp_devkit_e8_u85', suffix: 'gcc_u85_rtss-hp_tensorflowlitemicro', core: 'HP_U85', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HE-E8 U85 tests */
        [name: 'Test Visual Wake Word HE U85 GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_vww_he_devkit_e8_u85'],
         flash: he,
         dir: 'build_ci_power_test_he_devkit-e8_gcc_u85_rtss-he_tensorflowlitemicro/bin/sectors/alif_vww',
         src: 'mram.bin', bin: 'mlek_alif_vww.bin', test: 'test_power_vww',
         power: [id: 'vww_he_devkit_e8_u85', suffix: 'gcc_u85_rtss-he_tensorflowlitemicro', core: 'HE_U85', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HE-E8 without Helium and NPU tests */
        [name: 'Test Visual Wake Word HE without Helium and NPU GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_vww_he_devkit_e8_no_helium_no_npu'],
         flash: he,
         dir: 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he_tensorflowlitemicro_no_helium_no_npu/bin/sectors/alif_vww',
         src: 'mram.bin', bin: 'mlek_alif_vww.bin', test: 'test_power_vww',
         power: [id: 'vww_he_devkit_e8_no_helium_no_npu', suffix: 'gcc_u55_rtss-he_tensorflowlitemicro_no_helium_no_npu', core: 'HE_NO_HELIUM_NO_NPU', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HP-E8 without Helium and NPU tests */
        [name: 'Test Visual Wake Word HP without Helium and NPU GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_vww_hp_devkit_e8_no_helium_no_npu'],
         flash: hp,
         dir: 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro_no_helium_no_npu/bin/sectors/alif_vww',
         src: 'mram.bin', bin: 'mlek_alif_vww.bin', test: 'test_power_vww',
         power: [id: 'vww_hp_devkit_e8_no_helium_no_npu', suffix: 'gcc_u55_rtss-hp_tensorflowlitemicro_no_helium_no_npu', core: 'HP_NO_HELIUM_NO_NPU', board: 'DevKit-E8 rev b, SE v112.0']],

        /* Anomaly detection (alif_ad) tests */
        /* HP-E8 U55 tests */
        [name: 'Test Anomaly Detection HP GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_ad_hp_devkit_e8'],
         flash: hp,
         dir: 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro/bin/sectors/alif_ad',
         src: 'mram.bin', bin: 'mlek_alif_ad.bin', test: 'test_power_ad',
         power: [id: 'ad_hp_devkit_e8', suffix: 'gcc_u55_rtss-hp_tensorflowlitemicro', core: 'HP_U55', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HE-E8 U55 tests */
        [name: 'Test Anomaly Detection HE GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_ad_he_devkit_e8'],
         flash: he,
         dir: 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he_tensorflowlitemicro/bin/sectors/alif_ad',
         src: 'mram.bin', bin: 'mlek_alif_ad.bin', test: 'test_power_ad',
         power: [id: 'ad_he_devkit_e8', suffix: 'gcc_u55_rtss-he_tensorflowlitemicro', core: 'HE_U55', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HP-E8 U85 tests */
        [name: 'Test Anomaly Detection HP U85 GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_ad_hp_devkit_e8_u85'],
         flash: hp,
         dir: 'build_ci_power_test_hp_devkit-e8_gcc_u85_rtss-hp_tensorflowlitemicro/bin/sectors/alif_ad',
         src: 'mram.bin', bin: 'mlek_alif_ad.bin', test: 'test_power_ad',
         power: [id: 'ad_hp_devkit_e8_u85', suffix: 'gcc_u85_rtss-hp_tensorflowlitemicro', core: 'HP_U85', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HE-E8 U85 tests */
        [name: 'Test Anomaly Detection HE U85 GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_ad_he_devkit_e8_u85'],
         flash: he,
         dir: 'build_ci_power_test_he_devkit-e8_gcc_u85_rtss-he_tensorflowlitemicro/bin/sectors/alif_ad',
         src: 'mram.bin', bin: 'mlek_alif_ad.bin', test: 'test_power_ad',
         power: [id: 'ad_he_devkit_e8_u85', suffix: 'gcc_u85_rtss-he_tensorflowlitemicro', core: 'HE_U85', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HE-E8 without Helium and NPU tests */
        [name: 'Test Anomaly Detection HE without Helium and NPU GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_ad_he_devkit_e8_no_helium_no_npu'],
         flash: he,
         dir: 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he_tensorflowlitemicro_no_helium_no_npu/bin/sectors/alif_ad',
         src: 'mram.bin', bin: 'mlek_alif_ad.bin', test: 'test_power_ad',
         power: [id: 'ad_he_devkit_e8_no_helium_no_npu', suffix: 'gcc_u55_rtss-he_tensorflowlitemicro_no_helium_no_npu', core: 'HE_NO_HELIUM_NO_NPU', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HP-E8 without Helium and NPU tests */
        [name: 'Test Anomaly Detection HP without Helium and NPU GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_ad_hp_devkit_e8_no_helium_no_npu'],
         flash: hp,
         dir: 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro_no_helium_no_npu/bin/sectors/alif_ad',
         src: 'mram.bin', bin: 'mlek_alif_ad.bin', test: 'test_power_ad',
         power: [id: 'ad_hp_devkit_e8_no_helium_no_npu', suffix: 'gcc_u55_rtss-hp_tensorflowlitemicro_no_helium_no_npu', core: 'HP_NO_HELIUM_NO_NPU', board: 'DevKit-E8 rev b, SE v112.0']],
    ]
}

/* Tests of the use cases with the model in the OSPI flash. The model (ext_flash.bin) is written
 * to the OSPI flash with the usb-to-ospi-flasher before the application (mram.bin) is flashed
 * and the test is run, see ospi_file in the description of the test stage entries. */
def tests_withmodel_in_ospi_flash() {
    /* Flash configurations (see flash_config_file in test_common.groovy) of the cores */
    def hp = [core: 'HP', mram: '0x80008000']
    def he = [core: 'HE', mram: '0x80008000']
    return [
        /* ASR tests (TensorFlow Lite Micro) */
        /* HP-E8 U55 tests */
        [name: 'Test ASR HP U55 GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_asr_hp_devkit_e8', 'test_build_ci_power_test_asr_hp_devkit_e8_ext_flash'],
         flash: hp,
         dir: 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro/bin/sectors/alif_asr',
         ospi_file: 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro/bin/sectors/alif_asr/ext_flash.bin',
         src: 'mram.bin', bin: 'mlek_alif_asr.bin', test: 'test_power_asr',
         power: [id: 'asr_hp_devkit_e8', suffix: 'gcc_u55_rtss-hp_tensorflowlitemicro', core: 'HP_U55', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HE-E8 U55 tests */
        [name: 'Test ASR HE U55 GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_asr_he_devkit_e8', 'test_build_ci_power_test_asr_he_devkit_e8_ext_flash'],
         flash: he,
         dir: 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he_tensorflowlitemicro/bin/sectors/alif_asr',
         ospi_file: 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he_tensorflowlitemicro/bin/sectors/alif_asr/ext_flash.bin',
         src: 'mram.bin', bin: 'mlek_alif_asr.bin', test: 'test_power_asr',
         power: [id: 'asr_he_devkit_e8', suffix: 'gcc_u55_rtss-he_tensorflowlitemicro', core: 'HE_U55', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HP-E8 U85 tests */
        [name: 'Test ASR HP U85 GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_asr_hp_devkit_e8_u85', 'test_build_ci_power_test_asr_hp_devkit_e8_u85_ext_flash'],
         flash: hp,
         dir: 'build_ci_power_test_hp_devkit-e8_gcc_u85_rtss-hp_tensorflowlitemicro/bin/sectors/alif_asr',
         ospi_file: 'build_ci_power_test_hp_devkit-e8_gcc_u85_rtss-hp_tensorflowlitemicro/bin/sectors/alif_asr/ext_flash.bin',
         src: 'mram.bin', bin: 'mlek_alif_asr.bin', test: 'test_power_asr',
         power: [id: 'asr_hp_devkit_e8_u85', suffix: 'gcc_u85_rtss-hp_tensorflowlitemicro', core: 'HP_U85', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HE-E8 U85 tests */
        [name: 'Test ASR HE U85 GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_asr_he_devkit_e8_u85', 'test_build_ci_power_test_asr_he_devkit_e8_u85_ext_flash'],
         flash: he,
         dir: 'build_ci_power_test_he_devkit-e8_gcc_u85_rtss-he_tensorflowlitemicro/bin/sectors/alif_asr',
         ospi_file: 'build_ci_power_test_he_devkit-e8_gcc_u85_rtss-he_tensorflowlitemicro/bin/sectors/alif_asr/ext_flash.bin',
         src: 'mram.bin', bin: 'mlek_alif_asr.bin', test: 'test_power_asr',
         power: [id: 'asr_he_devkit_e8_u85', suffix: 'gcc_u85_rtss-he_tensorflowlitemicro', core: 'HE_U85', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HE-E8 without Helium and NPU tests */
        [name: 'Test ASR HE without Helium and NPU GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_asr_he_devkit_e8_no_helium_no_npu', 'test_build_ci_power_test_asr_he_devkit_e8_no_helium_no_npu_ext_flash'],
         flash: he,
         dir: 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he_tensorflowlitemicro_no_helium_no_npu/bin/sectors/alif_asr',
         ospi_file: 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he_tensorflowlitemicro_no_helium_no_npu/bin/sectors/alif_asr/ext_flash.bin',
         src: 'mram.bin', bin: 'mlek_alif_asr.bin', test: 'test_power_asr',
         power: [id: 'asr_he_devkit_e8_no_helium_no_npu', suffix: 'gcc_u55_rtss-he_tensorflowlitemicro_no_helium_no_npu', core: 'HE_NO_HELIUM_NO_NPU', board: 'DevKit-E8 rev b, SE v112.0']],
        /* HP-E8 without Helium and NPU tests */
        [name: 'Test ASR HP without Helium and NPU GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_asr_hp_devkit_e8_no_helium_no_npu', 'test_build_ci_power_test_asr_hp_devkit_e8_no_helium_no_npu_ext_flash'],
         flash: hp,
         dir: 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro_no_helium_no_npu/bin/sectors/alif_asr',
         ospi_file: 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro_no_helium_no_npu/bin/sectors/alif_asr/ext_flash.bin',
         src: 'mram.bin', bin: 'mlek_alif_asr.bin', test: 'test_power_asr',
         power: [id: 'asr_hp_devkit_e8_no_helium_no_npu', suffix: 'gcc_u55_rtss-hp_tensorflowlitemicro_no_helium_no_npu', core: 'HP_NO_HELIUM_NO_NPU', board: 'DevKit-E8 rev b, SE v112.0']],
    ]
}

return [
    initial_weekly_builds: this.&initial_weekly_builds,
    more_weekly_builds: this.&more_weekly_builds,
    tests: this.&tests,
    tests_withmodel_in_ospi_flash: this.&tests_withmodel_in_ospi_flash,
]
