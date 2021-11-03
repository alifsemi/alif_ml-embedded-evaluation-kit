- [Overview](#overview)
  - [Getting started](#getting-started)

# Overview

Arm® Virtual Hardware is an accurate representation of a physical System on Chip and runs as a simple application in a Linux environment for easy
scalability in the cloud and removes dependency from silicon availability. Powered by Amazon Web Services (AWS), developers can launch Amazon Machine Image
(AMI) running as a virtual server in the cloud. The Arm Virtual Hardware is configured with the Corstone™-300 MPS3 based Fixed Virtual Platform (FVP),
compiler and other tools.

## Getting started

To take advantage of Arm Virtual Hardware, you would need to have an AWS account and follow the steps below:

 1. Login to your AWS account.
 2. Go to the AWS Marketplace and search for [Arm Virtual Hardware](https://aws.amazon.com/marketplace/pp/prodview-urbpq7yo5va7g).
 3. Subscribe to the Arm Virtual Hardware Amazon Machine Image.
 4. Launch the AWS instance.
 5. If you want to access the Arm Virtual Hardware AWS instance via ssh, at the moment of creating the instance you would need to generate a *.pem* key.
 You could then access the AWS instance over ssh: `$ ssh -i <mykey.pem> ubuntu@<ec2-ip-address>`.

Note that you can register to receive free AWS credits to use Arm Virtual Hardware from [here](https://www.arm.com/company/contact-us/virtual-hardware).

You can find more information about Arm Virtual Hardware [here](https://arm-software.github.io/VHT/main/overview/html/index.html).

Once you have access to the AWS instance, we recommend starting from the [quick start guide](../quick_start.md) in order to get familiar
with the ml-embedded-evaluation-kit. Note that on the AWS instance, the FVP is available under `/opt/FVP_Corstone_SSE-300`.
