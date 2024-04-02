## Contributions

Contributions are only accepted under the following conditions:

- The contribution have certified origin and give us your permission. To manage this process we use
  [Developer Certificate of Origin (DCO) V1.1](https://developercertificate.org/).
  To indicate that contributors agree to the terms of the DCO, it's necessary "sign off" the
  contribution by adding a line with name and e-mail address to every git commit message:

  ```log
  Signed-off-by: John Doe <john.doe@example.org>
  ```

  This can be done automatically by adding the `-s` option to your `git commit` command.
  You must use your real name, no pseudonyms or anonymous contributions are accepted.

- You give permission according to the [Apache License 2.0](../../LICENSE_APACHE_2.0.txt).

  In each source file, include the following copyright notice:

  ```copyright
  /*
  * SPDX-FileCopyrightText: Copyright <years additions were made to project> <your name>, Arm Limited and/or its affiliates <open-source-office@arm.com>
  * SPDX-License-Identifier: Apache-2.0
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *     http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  */
  ```

### Coding standards and guidelines

This repository follows a set of guidelines, best practices, programming styles and conventions,
see:

- [Coding standards and guidelines](./docs/sections/coding_guidelines.md#Coding-standards-and-guidelines)
    - [Introduction](./docs/sections/coding_guidelines.md#introduction)
    - [Language version](./docs/sections/coding_guidelines.md#language-version)
    - [File naming](./docs/sections/coding_guidelines.md#file-naming)
    - [File layout](./docs/sections/coding_guidelines.md#file-layout)
    - [Block Management](./docs/sections/coding_guidelines.md#block-management)
    - [Naming Conventions](./docs/sections/coding_guidelines.md#naming-conventions)
        - [C++ language naming conventions](./docs/sections/coding_guidelines.md#c_language-naming-conventions)
        - [C language naming conventions](./docs/sections/coding_guidelines.md#c-language-naming-conventions)
    - [Layout and formatting conventions](./docs/sections/coding_guidelines.md#layout-and-formatting-conventions)
    - [Language usage](./docs/sections/coding_guidelines.md#language-usage)

### Code Reviews

Contributions must go through code review. Code reviews are performed through the
[mlplatform.org Gerrit server](https://review.mlplatform.org). Contributors need to sign up to this
Gerrit server with their GitHub account credentials.
In order to be merged a patch needs to:

- get a "+1 Verified" from the pre-commit job.
- get a "+2 Code-review" from a reviewer, it means the patch has the final approval.

### Testing

Prior to submitting a patch for review please make sure that all build variants works and unit tests pass.
Contributions go through testing at the continuous integration system. All builds, tests and checks must pass before a
contribution gets merged to the main branch.
