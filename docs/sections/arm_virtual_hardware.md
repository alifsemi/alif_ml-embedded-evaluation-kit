# Arm Virtual Hardware

- [Overview](./arm_virtual_hardware.md#overview)
  - [Getting started](./arm_virtual_hardware.md#getting-started)
  - [Useful Links](./arm_virtual_hardware.md#useful-links)

## Overview

Arm® Virtual Hardware (AVH) is an evolution of Arm’s modelling technology delivering models of Arm-based processors,
systems, third party hardware for application developers and SoC designers to build and test software before silicon
and hardware availability. It is an accurate representation of a physical System on Chip and runs as a simple
application in a Linux environment for easy scalability in the cloud. Powered by Amazon Web Services (AWS), developers
can launch Amazon Machine Image (AMI) running as a virtual server in the cloud. The Arm Virtual Hardware is configured
with the Arm® Corstone™-300 and Corstone™-310 MPS3 based Fixed Virtual Platform (FVP), compiler and other tools.

### Getting started

To take advantage of Arm Virtual Hardware, you would need to have an AWS account and follow the steps below:

 1. Login to your AWS account.
 2. Go to the AWS Marketplace and search for [Arm Virtual Hardware](https://aws.amazon.com/marketplace/pp/prodview-urbpq7yo5va7g).
 3. Subscribe to the Arm Virtual Hardware Amazon Machine Image.
 4. Launch the AWS instance.

 To access the Arm Virtual Hardware AWS instance via ssh, accept the prompt  to generate a *.pem* key
 while creating the instance or add it later.
 You can then access the AWS instance over ssh: `$ ssh -i <mykey.pem> ubuntu@<ec2-ip-address>`.
 It may be necessary to change the permissions for mykey.pem with `$ chmod 400 mykey.pem`.

### Useful Links

Note that you can register to receive free AWS credits to use Arm Virtual Hardware from
[here](https://www.arm.com/company/contact-us/virtual-hardware).

You can find more information about Arm Virtual Hardware [here](https://arm-software.github.io/AVH/main/overview/html/index.html).

Once you have access to the AWS instance, we recommend starting from the
[quick start guide](../quick_start.md#Quick-start-example-ML-application) in order to get familiar
with the ml-embedded-evaluation-kit. Note that on the AWS instance, the FVPs are available under:
 - `/opt/VHT/VHT_Corstone_SSE-300_Ethos-U55`
 - `/opt/VHT/VHT_Corstone_SSE-300_Ethos-U65`
 - `/opt/VHT/VHT_Corstone_SSE-310`

In order to view the FVP window when launching on the AWS instance a VNC is required.
See relevant section [here](https://aws.amazon.com/premiumsupport/knowledge-center/ec2-linux-2-install-gui/).
Alternatively, the FVPs can be given certain command line arguments to make them execute without the front-end. See
[Running the FVP without the UI](./deployment.md#running-the-fvp-without-the-ui).
