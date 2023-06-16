# Use case APIs

- [Use case APIs](#use-case-apis)
  - [Anomaly detection](#anomaly-detection)
  - [Automatic speech recognition](#automatic-speech-recognition)
  - [Image classification](#image-classification)
  - [Inference runner](#inference-runner)
  - [Keyword spotting](#keyword-spotting)
  - [Noise reduction](#noise-reduction)
  - [Object detection](#object-detection)
  - [Visual wake word](#visual-wake-word)
  - [Useful links](#useful-links)

The directories in here are individual CMake projects to generate use case API static libraries.
These libraries are intended to be used by external projects that only want access to the
ready-to-use ML use case pipelines implemented in this repository. This can be as CMake projects,
but also in the form of CMSIS-packs.

## Anomaly detection

This use case takes an audio clip of a machine at work as input and indicates whether there is an
anomaly to suggest that the machine is not performing normally and might need attention.

## Automatic speech recognition

This use case takes in an audio clip as an input and returns a transcript of what was being said
in text format.

## Image classification

This use case takes an image as input and classifies it into one of the categories the neural
network model supports. For example, the default Mobilenet V2 based model will be able to
classify the images into 1000 different classes.

## Inference runner

The inference runner is a generic use case which can run any model. It has a bigger memory
footprint because it includes support for all possible ML operators. This use case is useful for
checking if a given neural network model can be run with TensorFlow Lite Micro. The application
can also be used to get performance metrics for executing the ML workload for a given model.

## Keyword spotting

This use case takes an audio clip as an input, divides it into smaller sub-clips, and indicates which
keyword has been spotted for each sub-clip.

## Noise reduction

This use case removes noise from an audio clip. Instead of replicating a "noisy audio in" and
"clean audio out" problem, a simpler version is defined. Given an audio clip, the use case produces
gains to be applied to the input audio to reduce the noise.

## Object detection

This use case takes image data as input and performs object detection on them. The default model is
set up to detect faces of size 20x20 and above. The output represents the coordinates of the
bounding boxes that encapsulate the faces.

## Visual wake word

This use case takes images as input and determines, with a certain probability, whether the image
contains a person.

## Useful links

- [Project ReadMe](../../../../Readme.md)
- [Reusable Software](../../../../Readme.md#reusable-software)
