# FAQ

**Q: I'm unable to clone the ML embedded evaluation kit. When I run the command
`git clone "ssh://review.mlplatform.org:29418/ml/ethos-u/ml-embedded-evaluation-kit"` and get a permission denied
(publickey) error. What should I do to get the code base?**

**A:**  When cloning the repository, it's recommended to use https protocol command instead of ssh, use:

`git clone "https://review.mlplatform.org/ml/ethos-u/ml-embedded-evaluation-kit"`

A good starting point to explore the repository is the
[quick starting guide](https://review.mlplatform.org/plugins/gitiles/ml/ethos-u/ml-embedded-evaluation-kit/+/HEAD/docs/quick_start.md).

----

**Q: I’m running through the quick-start guide and I’m running into an error with pip. When I run `./build_default.py`,
I get an error `ImportError: No module named pip`, but pip is installed on my machine.**

**A:** Network or third party repository issues can cause the `build_default` script to fail and leave build environment in a
broken inconsistent state. Removing the `build` and `resources_downloaded` folders and running the script again may help.
If the problem persist contact your Arm representative or open a discussion on
[https://discuss.mlplatform.org/c/ml-embedded-evaluation-kit](https://discuss.mlplatform.org/c/ml-embedded-evaluation-kit/).

----

**Q: When pointing to the TensorFlow Lite file explicitly in the cmake command, I get the following error message:**

```log
CMake Error at scripts/cmake/util_functions.cmake:73 (message): Invalid file path.
Description: NN models file to be used in the evaluation application. Model files must be in tflite format.
```

**A:** This issue is usually caused by an incorrect path to the model file, pointed by the `-D<use_case>_MODEL_TFLITE_PATH`
parameter. Check that the path is correct, clean the build folder and re-run the `cmake` command.

----

**Q: How can we interpret the NPU and CPU cycles in terms of latency? Is the latency a summation of the total cycles
(idle and active NPU, active CPU)?**

**A:** For Fast Model simulations, active NPU cycles should be representative of a real system.
However, when running code samples on Corstone-300 FVP, active CPU cycles should not be used for any performance analysis or interpretation.
The Cortex-M part of the Fast Model **is not** cycle accurate or approximate, meanwhile NPU (Ethos-U) part **is** cycle approximate.
If you need to interpret cycles for Cortex-M part, you need to use FPGA system (based on MPS3) or cycle accurate modelling environment.

----

**Q: I changed the model for a use case but when running the application, tensor allocation fails with an error that an op for a
builtin opcode could not be found and the application failed to get registration from that op code.**

**A:** If you change the default model for a use case to a new model then you should update the corresponding
*<model_name>Model.cc* source file to include any new operators that this model has.

For example, if you changed the **img_class** model you would need to modify the **arm::app::MobileNetModel::EnlistOperations()**
function in *source/use_case/img_class/src/MobileNetModel.cc* to enlist any new operators in the model.

Don't forget to also increment the **ms_maxOpCnt** variable in the corresponding header file
(e.g. *source/use_case/img_class/include/MobileNetModel.hpp* if we are changing **img_class**).
