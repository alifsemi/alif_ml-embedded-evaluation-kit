# Troubleshooting

## Contents

- [Inference results are incorrect for my custom files](#inference-results-are-incorrect-for-my-custom-files)
- [The application does not work with my custom model](#the-application-does-not-work-with-my-custom-model)

## Inference results are incorrect for my custom files

Ensure that the files you are using match the requirements of the model
you are using and that cmake parameters are set accordingly. More
information on these cmake parameters is detailed in their separate
sections. Note that preprocessing of the files could also affect the
inference result, such as the rescaling and padding operations done for
image classification.

## The application does not work with my custom model

Ensure that your model is in a fully quantized `.tflite` file format,
either uint8 or int8, and has successfully been run through the Vela
compiler.

Check that cmake parameters match your new models input requirements.

> **Note:** Vela tool is not available within this software project.
It is a python tool available from <https://pypi.org/project/ethos-u-vela/>.
The source code is hosted on <https://git.mlplatform.org/ml/ethos-u/ethos-u-vela.git/>.

Next section of the documentation: [Contribution guidelines](../documentation.md#Contribution-guidelines).
