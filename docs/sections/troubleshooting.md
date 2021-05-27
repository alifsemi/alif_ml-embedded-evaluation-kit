# Troubleshooting

- [Troubleshooting](#troubleshooting)
  - [Inference results are incorrect for my custom files](#inference-results-are-incorrect-for-my-custom-files)
  - [The application does not work with my custom model](#the-application-does-not-work-with-my-custom-model)

## Inference results are incorrect for my custom files

Ensure that the files you are using match the requirements of the model you are using. Ensure that cmake parameters are
set accordingly. More information on these cmake parameters is detailed in their separate sections.

> **Note:** Preprocessing of the files can also affect the inference result, such as the rescaling and padding
> operations performed for image classification.

## The application does not work with my custom model

Ensure that your model is in a fully quantized `.tflite` file format, either `uint8` or `int8`, and that it has
successfully been run through the Vela compiler.

Also, please check that the cmake parameters used match the input requirements of your new model.

> **Note:** The Vela tool is not available within this software project. It is a separate Python tool that is available
> from: <https://pypi.org/project/ethos-u-vela/>. The source code is hosted on
> <https://git.mlplatform.org/ml/ethos-u/ethos-u-vela.git/>.

The next section of the documentation is: [Appendix](appendix.md).
