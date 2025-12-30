# Zephyr module for MLEK libraries

This directory exposes the MLEK libraries as a Zephyr module. The Zephyr build picks up the port via `module.yml` in this directory.

To add the module to a Zephyr build, append the path to this folder to `EXTRA_ZEPHYR_MODULES` (recommended for ad-hoc adds) or `ZEPHYR_MODULES`. Use the absolute path to your checkout root (`MLEK_ROOT`), for example:

```bash
export EXTRA_ZEPHYR_MODULES="${EXTRA_ZEPHYR_MODULES}:${MLEK_ROOT}/source/lib/ports/zephyr"
```

Then run your usual `west build ...` command. The Kconfig options under `MLEK_EXAMPLES_*` control which framework (TensorFlow Lite Micro or ExecuTorch) and logging settings are enabled. CMake consumes the resulting `CONFIG_*` definitions to configure the underlying MLEK build.
