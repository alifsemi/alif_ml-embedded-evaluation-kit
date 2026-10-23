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
 *   json:      Flash configuration file.
 *   dir, src, bin:  Binary directory, name of the binary there and name it is flashed with.
 *   test:      pytest -k expression.
 *   video:     Optional. Record the device with the camera job, the video is stashed with this name.
 *   power:     Optional. Power test, the results are written to
 *              power_results_<id>_<suffix>.md and stashed as power_results_<id>.
 */


def initial_builds() {
    return [
        [name: 'Build HP GCC Release DevKit-e7',
         builds: [['hp', 'Release', 'gcc', 'DevKit-e7']],
         artifact: '*.bin',
         stashes: [
             ['test_build_gnu_img_hp_release_devkit_e7', 'build_gcc_hp_release_devkit-e7/bin/sectors/alif_img_class/mram.bin'],
             ['test_build_gnu_obj_hp_release_devkit_e7', 'build_gcc_hp_release_devkit-e7/bin/sectors/alif_object_detection/mram.bin'],
             ['test_build_gnu_vww_hp_release_devkit_e7', 'build_gcc_hp_release_devkit-e7/bin/sectors/alif_vww/mram.bin']]],
        [name: 'Build HP GCC Release DevKit-e8',
         builds: [['hp', 'Release', 'gcc', 'DevKit-e8']],
         artifact: '*.bin',
         stashes: [
             ['test_build_gnu_img_hp_release_devkit_e8', 'build_gcc_hp_release_devkit-e8/bin/sectors/alif_img_class/mram.bin'],
             ['test_build_gnu_obj_hp_release_devkit_e8', 'build_gcc_hp_release_devkit-e8/bin/sectors/alif_object_detection/mram.bin'],
             ['test_build_gnu_vww_hp_release_devkit_e8', 'build_gcc_hp_release_devkit-e8/bin/sectors/alif_vww/mram.bin']]],
        [name: 'Build HP GCC Release AppKit-e8',
         builds: [['hp', 'Release', 'gcc', 'AppKit-e8']],
         artifact: '*.bin',
         stashes: [
             ['test_build_gnu_img_hp_release_appkit_e8', 'build_gcc_hp_release_appkit-e8/bin/sectors/alif_img_class/mram.bin'],
             ['test_build_gnu_obj_hp_release_appkit_e8', 'build_gcc_hp_release_appkit-e8/bin/sectors/alif_object_detection/mram.bin'],
             ['test_build_gnu_vww_hp_release_appkit_e8', 'build_gcc_hp_release_appkit-e8/bin/sectors/alif_vww/mram.bin']]],
    ]
}

def more_builds() {
    return [
        [name: 'Build HP GCC Release CI Power Test for DevKit-e8',
         builds: [['ci_power_test', 'HP', 'DevKit-e8', 'gcc', 'U55', 'RTSS-HP', 'TensorFlowLiteMicro', 'all']],
         artifact: '*.bin',
         stashes: [
             ['test_build_ci_power_test_img_hp_devkit_e8', 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro/bin/sectors/alif_img_class/mram.bin'],
             ['test_build_ci_power_test_obj_hp_devkit_e8', 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro/bin/sectors/alif_object_detection/mram.bin'],
             ['test_build_ci_power_test_ad_hp_devkit_e8', 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro/bin/sectors/alif_ad/mram.bin'],
             ['test_build_ci_power_test_vww_hp_devkit_e8', 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro/bin/sectors/alif_vww/mram.bin'],
             ['test_build_ci_power_test_asr_hp_devkit_e8', 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro/bin/sectors/alif_asr/mram.bin'],
             ['test_build_ci_power_test_kws_hp_devkit_e8', 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro/bin/sectors/alif_kws/mram.bin']]],
        [name: 'Build HE GCC Release CI Power Test for DevKit-e8',
         builds: [['ci_power_test', 'HE', 'DevKit-e8', 'gcc', 'U55', 'RTSS-HE', 'TensorFlowLiteMicro', 'all'], ['ci_power_test', 'HE', 'DevKit-e8', 'gcc', 'U55', 'RTSS-HE-TCM', 'TensorFlowLiteMicro', 'alif_kws']],
         artifact: '*.bin',
         stashes: [
             ['test_build_ci_power_test_img_he_devkit_e8', 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he_tensorflowlitemicro/bin/sectors/alif_img_class/mram.bin'],
             ['test_build_ci_power_test_obj_he_devkit_e8', 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he_tensorflowlitemicro/bin/sectors/alif_object_detection/mram.bin'],
             ['test_build_ci_power_test_ad_he_devkit_e8', 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he_tensorflowlitemicro/bin/sectors/alif_ad/mram.bin'],
             ['test_build_ci_power_test_vww_he_devkit_e8', 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he_tensorflowlitemicro/bin/sectors/alif_vww/mram.bin'],
             ['test_build_ci_power_test_asr_he_devkit_e8', 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he_tensorflowlitemicro/bin/sectors/alif_asr/mram.bin'],
             ['test_build_ci_power_test_kws_he_devkit_e8', 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he-tcm_tensorflowlitemicro/bin/sectors/alif_kws/mram.bin']]],
        [name: 'Build HP ROTATE_DISPLAY=180 Release for DevKit-e8',
         builds: [['hp', 'Release', 'gcc', 'DevKit-e8', '-DROTATE_DISPLAY=180'], ['hp', 'Release', 'armclang', 'DevKit-e8', '-DROTATE_DISPLAY=180']]],
        [name: 'Build HP ROTATE_DISPLAY=180 Release for AppKit-e7',
         builds: [['hp', 'Release', 'gcc', 'AppKit-e7', '-DROTATE_DISPLAY=180'], ['hp', 'Release', 'armclang', 'AppKit-e7', '-DROTATE_DISPLAY=180']]],
        [name: 'Build inference runner GCC Release DevKit-e7',
         builds: [['inf_runner', 'Release', 'gcc', 'DevKit-e7', 'HP'], ['inf_runner', 'Release', 'gcc', 'DevKit-e7', 'HE']],
         artifact: '*.bin',
         stashes: [
             ['test_build_gnu_inf_hp_release_devkit_e7', 'build_gcc_hp_release_devkit-e7_inf_runner/bin/sectors/inference_runner/mram.bin'],
             ['test_build_gnu_inf_he_release_devkit_e7', 'build_gcc_he_release_devkit-e7_inf_runner/bin/sectors/inference_runner/mram.bin']]],
        [name: 'Build inference runner ARMCLANG Release DevKit-e7/e8',
         builds: [['inf_runner', 'Release', 'armclang', 'DevKit-e7', 'HP'], ['inf_runner', 'Release', 'armclang', 'DevKit-e7', 'HE'], ['inf_runner', 'Release', 'armclang', 'DevKit-e8', 'HP'], ['inf_runner', 'Release', 'armclang', 'DevKit-e8', 'HE']],
         artifact: 'ER_ROM',
         stashes: [
             ['test_build_armclang_inf_hp_release_devkit_e7', 'build_armclang_hp_release_devkit-e7_inf_runner/bin/sectors/inference_runner/ER_ROM'],
             ['test_build_armclang_inf_he_release_devkit_e7', 'build_armclang_he_release_devkit-e7_inf_runner/bin/sectors/inference_runner/ER_ROM'],
             ['test_build_armclang_inf_hp_release_devkit_e8', 'build_armclang_hp_release_devkit-e8_inf_runner/bin/sectors/inference_runner/ER_ROM'],
             ['test_build_armclang_inf_he_release_devkit_e8', 'build_armclang_he_release_devkit-e8_inf_runner/bin/sectors/inference_runner/ER_ROM']]],
        [name: 'Build inference runner GCC Release DevKit-e8',
         builds: [['inf_runner', 'Release', 'gcc', 'DevKit-e8', 'HP'], ['inf_runner', 'Release', 'gcc', 'DevKit-e8', 'HE']],
         artifact: '*.bin',
         stashes: [
             ['test_build_gnu_inf_hp_release_devkit_e8', 'build_gcc_hp_release_devkit-e8_inf_runner/bin/sectors/inference_runner/mram.bin'],
             ['test_build_gnu_inf_he_release_devkit_e8', 'build_gcc_he_release_devkit-e8_inf_runner/bin/sectors/inference_runner/mram.bin']]],
        [name: 'Build HP GCC Debug DevKit-e7/e8',
         builds: [['hp', 'Debug', 'gcc', 'DevKit-e7'], ['hp', 'Debug', 'gcc', 'DevKit-e8']]],
        [name: 'Build HP ARMCLANG Release DevKit-e7/e8/AppKit-e8',
         builds: [['hp', 'Release', 'armclang', 'DevKit-e7'], ['hp', 'Release', 'armclang', 'DevKit-e8'], ['hp', 'Release', 'armclang', 'AppKit-e8']],
         artifact: 'ER_ROM',
         stashes: [
             ['test_build_armclang_img_hp_release_devkit_e7', 'build_armclang_hp_release_devkit-e7/bin/sectors/alif_img_class/ER_ROM'],
             ['test_build_armclang_obj_hp_release_devkit_e7', 'build_armclang_hp_release_devkit-e7/bin/sectors/alif_object_detection/ER_ROM'],
             ['test_build_armclang_vww_hp_release_devkit_e7', 'build_armclang_hp_release_devkit-e7/bin/sectors/alif_vww/ER_ROM'],
             ['test_build_armclang_img_hp_release_devkit_e8', 'build_armclang_hp_release_devkit-e8/bin/sectors/alif_img_class/ER_ROM'],
             ['test_build_armclang_obj_hp_release_devkit_e8', 'build_armclang_hp_release_devkit-e8/bin/sectors/alif_object_detection/ER_ROM'],
             ['test_build_armclang_vww_hp_release_devkit_e8', 'build_armclang_hp_release_devkit-e8/bin/sectors/alif_vww/ER_ROM'],
             ['test_build_armclang_img_hp_release_appkit_e8', 'build_armclang_hp_release_appkit-e8/bin/sectors/alif_img_class/ER_ROM'],
             ['test_build_armclang_obj_hp_release_appkit_e8', 'build_armclang_hp_release_appkit-e8/bin/sectors/alif_object_detection/ER_ROM'],
             ['test_build_armclang_vww_hp_release_appkit_e8', 'build_armclang_hp_release_appkit-e8/bin/sectors/alif_vww/ER_ROM']]],
        [name: 'Build HE-TCM GCC Release DevKit-e1c/e7/e8/e1c-sk/AppKit-e8',
         builds: [['he_tcm', 'Release', 'gcc', 'DevKit-e1c'], ['he_tcm', 'Release', 'gcc', 'DevKit-e7'], ['he_tcm', 'Release', 'gcc', 'DevKit-e8'], ['he_tcm', 'Release', 'gcc', 'StartKit-e1c'], ['he_tcm', 'Release', 'gcc', 'AppKit-e8']],
         artifact: '*.bin',
         stashes: [
             ['test_build_gnu_kws_he_release_devkit_e1c', 'build_gcc_he_tcm_release_devkit-e1c/bin/sectors/alif_kws/mram.bin'],
             ['test_build_gnu_kws_he_release_devkit_e7', 'build_gcc_he_tcm_release_devkit-e7/bin/sectors/alif_kws/mram.bin'],
             ['test_build_gnu_kws_he_release_devkit_e8', 'build_gcc_he_tcm_release_devkit-e8/bin/sectors/alif_kws/mram.bin'],
             ['test_build_gnu_kws_he_release_startkit_e1c', 'build_gcc_he_tcm_release_startkit-e1c/bin/sectors/alif_kws/mram.bin'],
             ['test_build_gnu_kws_he_release_appkit_e8', 'build_gcc_he_tcm_release_appkit-e8/bin/sectors/alif_kws/mram.bin']]],
        [name: 'Build HE-TCM ARMCLANG Release DevKit-e1c/e7/e8/e1c-sk/AppKit-e8',
         builds: [['he_tcm', 'Release', 'armclang', 'DevKit-e1c'], ['he_tcm', 'Release', 'armclang', 'DevKit-e7'], ['he_tcm', 'Release', 'armclang', 'DevKit-e8'], ['he_tcm', 'Release', 'armclang', 'StartKit-e1c'], ['he_tcm', 'Release', 'armclang', 'AppKit-e8']],
         artifact: 'ER_ROM',
         stashes: [
             ['test_build_armclang_kws_he_release_devkit_e1c', 'build_armclang_he_tcm_release_devkit-e1c/bin/sectors/alif_kws/ER_ROM'],
             ['test_build_armclang_kws_he_release_devkit_e7', 'build_armclang_he_tcm_release_devkit-e7/bin/sectors/alif_kws/ER_ROM'],
             ['test_build_armclang_kws_he_release_devkit_e8', 'build_armclang_he_tcm_release_devkit-e8/bin/sectors/alif_kws/ER_ROM'],
             ['test_build_armclang_kws_he_release_startkit_e1c', 'build_armclang_he_tcm_release_startkit-e1c/bin/sectors/alif_kws/ER_ROM'],
             ['test_build_armclang_kws_he_release_appkit_e8', 'build_armclang_he_tcm_release_appkit-e8/bin/sectors/alif_kws/ER_ROM']]],
        [name: 'Build HE-TCM GCC Debug DevKit-e1c/e7/e8/e1c-sk',
         builds: [['he_tcm', 'Debug', 'gcc', 'DevKit-e1c'], ['he_tcm', 'Debug', 'gcc', 'DevKit-e7'], ['he_tcm', 'Debug', 'gcc', 'DevKit-e8'], ['he_tcm', 'Debug', 'gcc', 'StartKit-e1c']]],
        [name: 'Build HE GCC Release CI Power Test for DevKit-e1c',
         builds: [['ci_power_test', 'HE', 'DevKit-e1c', 'gcc', 'U55', 'RTSS-HE-TCM', 'TensorFlowLiteMicro', 'alif_kws']],
         artifact: '*.bin',
         stashes: [
             ['test_build_ci_power_test_kws_he_devkit_e1c', 'build_ci_power_test_he_devkit-e1c_gcc_u55_rtss-he-tcm_tensorflowlitemicro/bin/sectors/alif_kws/mram.bin']]],
    ]
}

def tests() {
    return [
        [name: 'Test Inference runner HP GCC Release DevKit-E7',
         label: 'E7', unstash: ['test_build_gnu_inf_hp_release_devkit_e7'],
         json: 'inference_runner_hp.json',
         dir: 'build_gcc_hp_release_devkit-e7_inf_runner/bin/sectors/inference_runner',
         src: 'mram.bin', bin: 'mlek_inference_runner.bin', test: 'test_inference_runner'],
        [name: 'Test KWS HE GCC Release DevKit-E7',
         label: 'E7', unstash: ['test_build_gnu_kws_he_release_devkit_e7'],
         json: 'alif_kws_he.json',
         dir: 'build_gcc_he_tcm_release_devkit-e7/bin/sectors/alif_kws',
         src: 'mram.bin', bin: 'mlek_alif_kws.bin', test: 'test_kws'],
        [name: 'Test KWS HE ARMCLANG Release DevKit-E7',
         label: 'E7', unstash: ['test_build_armclang_kws_he_release_devkit_e7'],
         json: 'alif_kws_he.json',
         dir: 'build_armclang_he_tcm_release_devkit-e7/bin/sectors/alif_kws',
         src: 'ER_ROM', bin: 'mlek_alif_kws.bin', test: 'test_kws'],
        [name: 'Test Image Classification HP GCC Release DevKit-E7',
         label: 'Blinky', unstash: ['test_build_gnu_img_hp_release_devkit_e7'],
         json: 'alif_img_class_hp.json',
         dir: 'build_gcc_hp_release_devkit-e7/bin/sectors/alif_img_class',
         src: 'mram.bin', bin: 'mlek_alif_img_class.bin', test: 'test_img_class',
         video: 'img_class_gcc_video'],
        [name: 'Test Image Classification HP ARMCLANG Release DevKit-E7',
         label: 'Blinky', unstash: ['test_build_armclang_img_hp_release_devkit_e7'],
         json: 'alif_img_class_hp.json',
         dir: 'build_armclang_hp_release_devkit-e7/bin/sectors/alif_img_class',
         src: 'ER_ROM', bin: 'mlek_alif_img_class.bin', test: 'test_img_class',
         video: 'img_class_armclang_video'],
        [name: 'Test Object Detection HP GCC Release DevKit-E7',
         label: 'Blinky', unstash: ['test_build_gnu_obj_hp_release_devkit_e7'],
         json: 'alif_obj_detect_hp.json',
         dir: 'build_gcc_hp_release_devkit-e7/bin/sectors/alif_object_detection',
         src: 'mram.bin', bin: 'mlek_alif_object_detection.bin', test: 'test_object_detection',
         video: 'obj_det_gcc_video'],
        [name: 'Test Object Detection HP ARMCLANG Release DevKit-E7',
         label: 'Blinky', unstash: ['test_build_armclang_obj_hp_release_devkit_e7'],
         json: 'alif_obj_detect_hp.json',
         dir: 'build_armclang_hp_release_devkit-e7/bin/sectors/alif_object_detection',
         src: 'ER_ROM', bin: 'mlek_alif_object_detection.bin', test: 'test_object_detection',
         video: 'obj_det_armclang_video'],
        [name: 'Test Visual Wake Word HP GCC Release DevKit-E7',
         label: 'Blinky', unstash: ['test_build_gnu_vww_hp_release_devkit_e7'],
         json: 'alif_vww.json',
         dir: 'build_gcc_hp_release_devkit-e7/bin/sectors/alif_vww',
         src: 'mram.bin', bin: 'mlek_alif_vww.bin', test: 'test_vww',
         video: 'vww_gcc_video'],
        [name: 'Test Visual Wake Word HP ARMCLANG Release DevKit-E7',
         label: 'Blinky', unstash: ['test_build_armclang_vww_hp_release_devkit_e7'],
         json: 'alif_vww.json',
         dir: 'build_armclang_hp_release_devkit-e7/bin/sectors/alif_vww',
         src: 'ER_ROM', bin: 'mlek_alif_vww.bin', test: 'test_vww',
         video: 'vww_armclang_video'],
        [name: 'Test KWS HE GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_kws_he_devkit_e8'],
         json: 'alif_kws_he.json',
         dir: 'build_ci_power_test_he_devkit-e8_gcc_u55_rtss-he-tcm_tensorflowlitemicro/bin/sectors/alif_kws',
         src: 'mram.bin', bin: 'mlek_alif_kws.bin', test: 'test_power_kws',
         power: [id: 'kws_he_devkit_e8', suffix: 'gcc_u55_rtss-he-tcm_tensorflowlitemicro', core: 'HE_U55', board: 'DevKit-E8 rev b, SE v112.0']],
        [name: 'Test Object Detection HP GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_obj_hp_devkit_e8'],
         json: 'alif_obj_detect_hp.json',
         dir: 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro/bin/sectors/alif_object_detection',
         src: 'mram.bin', bin: 'mlek_alif_object_detection.bin', test: 'test_power_object_detection',
         power: [id: 'obj_detect_hp_devkit_e8', suffix: 'gcc_u55_rtss-hp_tensorflowlitemicro', core: 'HP_U55', board: 'DevKit-E8 rev b, SE v112.0']],
        [name: 'Test Image Classification HP GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_img_hp_devkit_e8'],
         json: 'alif_img_class_hp.json',
         dir: 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro/bin/sectors/alif_img_class',
         src: 'mram.bin', bin: 'mlek_alif_img_class.bin', test: 'test_power_img_class',
         power: [id: 'img_class_hp_devkit_e8', suffix: 'gcc_u55_rtss-hp_tensorflowlitemicro', core: 'HP_U55', board: 'DevKit-E8 rev b, SE v112.0']],
        [name: 'Test Visual Wake Word HP GCC Release CI Power Test for DevKit-e8',
         label: 'E8_JS', unstash: ['test_build_ci_power_test_vww_hp_devkit_e8'],
         json: 'alif_vww.json',
         dir: 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro/bin/sectors/alif_vww',
         src: 'mram.bin', bin: 'mlek_alif_vww.bin', test: 'test_power_vww',
         power: [id: 'vww_hp_devkit_e8', suffix: 'gcc_u55_rtss-hp_tensorflowlitemicro', core: 'HP_U55', board: 'DevKit-E8 rev b, SE v112.0']],
        [name: 'Test KWS HE GCC Release DevKit-E1C',
         label: 'E1C', unstash: ['test_build_gnu_kws_he_release_devkit_e1c'],
         json: 'alif_kws_he_e1c.json',
         dir: 'build_gcc_he_tcm_release_devkit-e1c/bin/sectors/alif_kws',
         src: 'mram.bin', bin: 'mlek_alif_kws.bin', test: 'test_kws'],
        [name: 'Test KWS HE ARMCLANG Release DevKit-E1C',
         label: 'E1C', unstash: ['test_build_armclang_kws_he_release_devkit_e1c'],
         json: 'alif_kws_he_e1c.json',
         dir: 'build_armclang_he_tcm_release_devkit-e1c/bin/sectors/alif_kws',
         src: 'ER_ROM', bin: 'mlek_alif_kws.bin', test: 'test_kws'],
        [name: 'Test KWS HE GCC Release CI Power Test for DevKit-E1C',
         label: 'E1C_JS', unstash: ['test_build_ci_power_test_kws_he_devkit_e1c'],
         json: 'alif_kws_he_e1c.json',
         dir: 'build_ci_power_test_he_devkit-e1c_gcc_u55_rtss-he-tcm_tensorflowlitemicro/bin/sectors/alif_kws',
         src: 'mram.bin', bin: 'mlek_alif_kws.bin', test: 'test_power_kws',
         power: [id: 'kws_he_devkit_e1c', suffix: 'gcc_u55_rtss-he-tcm_tensorflowlitemicro', core: 'HE_U55', board: 'DevKit-E1C rev b, SE v112.0']],


        // ASR has the model in OSPI Flash, not supported yet
        // [name: 'Test ASR HP GCC Release CI Power Test for DevKit-e8',
        //  label: 'E8_JS', unstash: ['test_build_ci_power_test_asr_hp_devkit_e8'],
        //  json: 'alif_asr_hp.json',
        //  dir: 'build_ci_power_test_hp_devkit-e8_gcc_u55_rtss-hp_tensorflowlitemicro/bin/sectors/alif_asr',
        //  src: 'mram.bin', bin: 'mlek_alif_asr.bin', test: 'test_power_asr',
        //  power: [id: 'asr_hp_devkit_e8', suffix: 'gcc_u55_rtss-hp_tensorflowlitemicro', core: 'HP_U55', board: 'DevKit-E8 rev b, SE v112.0']],
    ]
}

return [
    initial_builds: this.&initial_builds,
    more_builds: this.&more_builds,
    tests: this.&tests,
]
