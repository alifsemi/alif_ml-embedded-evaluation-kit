<!--
    SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or its affiliates <open-source-office@arm.com>
    SPDX-License-Identifier: Apache-2.0
-->

# Framework Abstraction Layer

## Overview

This module provides a **framework-agnostic interface layer** for inference backends. Its purpose is to isolate 
higher-level logic (like common APIs and use-case implementations) from the specifics of any single ML framework,
such as TensorFlow Lite Micro or ExecuTorch.

## Design Principles

- **Modularity:** Framework-specific logic is isolated into backend modules (e.g., `tflm` and `executorch`), while
                  `fwk` defines generic interfaces.
- **Minimal Intrusion:** Existing use-case and model logic can progressively migrate to use `fwk` interfaces.

## Namespaces

### `fwk::iface`

This namespace contains **pure virtual interfaces** for components like `Tensors` and `Model`. These interfaces
are used by [`common_api`](../common) and [`use_case` APIs](../use_case). The application logic should also rely
on this interface wherever possible.

### `fwk::tflm`

This namespace contains TensorFlow Lite Micro specific implementation of `Tensors` as `TflmTensor` class and
`Model` as `TflmModel` class. These, specifically `TflmModel`, are used to instantiate TensorFlow Lite Micro
specific models in the examples. However, once instantiated, the other orchestration logic relies on interface
exposed by `fwk::iface` namespace.


### `fwk::et`

This namespace contains ExecuTorch specific implementation of `Tensors` as `EtTensor` class and
`Model` as `EtModel` class. These, specifically `EtModel`, are used to instantiate ExecuTorch
specific models in the examples. However, once instantiated, the other orchestration logic relies on interface
exposed by `fwk::iface` namespace.
