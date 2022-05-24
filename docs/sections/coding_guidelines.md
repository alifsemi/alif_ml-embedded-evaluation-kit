# Coding standards and guidelines

- [Coding standards and guidelines](./coding_guidelines.md#coding-standards-and-guidelines)
  - [Introduction](./coding_guidelines.md#introduction)
  - [Language version](./coding_guidelines.md#language-version)
  - [File naming](./coding_guidelines.md#file-naming)
  - [File layout](./coding_guidelines.md#file-layout)
  - [Block Management](./coding_guidelines.md#block-management)
  - [Naming Conventions](./coding_guidelines.md#naming-conventions)
    - [CPP language naming conventions](./coding_guidelines.md#cpp-language-naming-conventions)
    - [C language naming conventions](./coding_guidelines.md#c-language-naming-conventions)
  - [Layout and formatting conventions](./coding_guidelines.md#layout-and-formatting-conventions)
  - [Language usage](./coding_guidelines.md#language-usage)

## Introduction

This document presents some standard coding guidelines to be followed for contributions to this repository. Most of the
code is written in C++, but there is also some written in C. There is a clear C/C++ boundary at the Hardware Abstraction
Layer (HAL). Both of these languages follow different naming conventions within this repository, by design, to:

- Have clearly distinguishable C and C++ sources.
- Make cross language function calls stand out. These are mainly C++ function calls to the HAL functions, that were
  written in C.

However, because we also issue function calls to third-party APIs, and they are not guaranteed to follow these
conventions, the intended outcome could be different for every case.

## Pre-commit formatting
To help with the adherence of the coding guidelines, we have provided a clang-format file. When commiting, please run 
the following command, post-staging but pre-commit.


  ```Git
  git-clang-format
  ```

This will modify the staged changes, to adhere to the guidelines as described in the 
.clang-format file in the root of the repo. Please note that the clang-format tool must be installed to run this 
step and can be installed using the following command on Ubuntu: 

  ```
  sudo apt install clang-format
  ```


## Language version

For this project, code written in C++ uses a subset of the `C++14` feature set and software may be written using the
`C++14` language standard. Code written in C is compatible with the `C99` standard.

Software components written in C/C++ may use the language features allowed and is encouraged.

## File naming

- C files must have a `.c` extension
- C++ files must have a `.cc` or `.cpp` extension.
- Header files for functions implemented in C must have a `.h` extension.
- Header files for functions implemented in C++ must have a `.hpp` extension.

## File layout

- The standard copyright notice must be included in all files:

  ```copyright
  /*
  * Copyright (c) <years additions were made to project> <your name>, Arm Limited. All rights reserved.
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

- Source lines must be no longer than 120 characters. You can spread the code out vertically, rather than horizontally,
  if required. For example:

  ```C++
  # This is significantly easier to read
  enum class SomeEnum1
  {
      ENUM_VALUE_1,
      ENUM_VALUE_2,
      ENUM_VALUE_3
  };

  # than this
  enum class SomeEnum2 { ENUM_VALUE_1, ENUM_VALUE_2, ENUM_VALUE_3 };
  ```

- Block indentation must use 4 characters and not use tabs.

- Each statement must be on a separate line. For example:

  ```C++
  int a, b; // Error prone
  int c, *d;

  int e = 0; // GOOD
  int *p = nullptr; // GOOD
  ```

- Also, the source code must not contain code that has been commented out or is unreachable.

## Block Management

- Blocks must use braces and the brace location must be consistent throughout.
  - Therefore, each function has its opening brace at the next line on the same indentation level as its header. The
    code within the braces is indented and the closing brace at the end is on the same level as the opening. For
    compactness, if the class, or function, body is empty, then braces on the same line are acceptable.

  - Conditional statements and loops, even if they are just single-statement body, must be surrounded by braces. The
    opening brace is at the same line, the closing brace is at the next line, and on the same indentation level as its
    header. The same rule is applied to classes.

    ```C++
    class Class1 {
    public:
        Class1();
    private:
        int element;
    };

    void NotEmptyFunction()
    {
        if (condition) {
            // [...]
        } else {
            // [...]
        }
        // [...]
        for(start_cond; end_cond; step_cond) {
            // [...]
        }
    }

    void EmptyFunction() {}
    ```

  - Cases within switch are indented and enclosed in brackets:

    ```C++
    switch (option)
    {
        case 1:
        {
            // handle option 1
            break;
        }
        case 2:
        {
            // handle option 2
            break;
        }
        default:
        {
            break;
        }
    }
    ```

## Naming Conventions

### CPP language naming conventions

- Type (class, struct, enum) names must be `PascalCase`:

  ```C++
  class SomeClass
  {
      // [...]
  };
  void SomeFunction()
  {
      // [...]
  }
  ```

- Variables and parameter names must be `camelCase`:

  ```C++
  int someVariable;

  void SomeFunction(int someParameter) {}
  ```

- Use uppercase names for macros, pre-processor definitions, and enumeration values:

  ```C++
  #define SOME_DEFINE

  enum class SomeEnum
  {
      ENUM_VALUE_1,
      ENUM_VALUE_2
  };
  ```

- Namespace names must be lowercase

  ```C++
  namespace nspace
  {
  void FunctionInNamespace();
  }
  ```

- Source code must use Hungarian notation to annotate the name of a variable with information about its meaning.

  | Prefix | Class | Description |
  | ------ | ----- | ----------- |
  | `p` | Type      | Pointer to any other type |
  | `k` | Qualifier | Constant |
  | `v` | Qualifier | Volatile |
  | `m` | Scope     | Member of a class or struct |
  | `s` | Scope     | Static |
  | `g` | Scope     | Used to indicate variable has scope beyond the current function: file-scope or externally visible scope. |

The following examples of Hungarian notation are one possible set of uses:

  ```C++
  int g_GlobalInt=123;
  char* m_pNameOfMemberPointer=nullptr;
  const float g_kSomeGlobalConstant = 1.234f;
  static float ms_MyStaticMember =  4.321f;
  bool myLocalVariable=true;
  ```

### C language naming conventions

For C sources, we follow the Linux variant of the K&R style wherever possible.

- For function and variable names, we use the `snake_case` convention:

  ```C
  int some_variable;

  void some_function(int some_parameter) {}
  ```

- Use uppercase names for macros, pre-processor definitions, and enumeration values:

  ```C
  #define SOME_DEFINE

  enum some_enum
  {
      ENUM_VALUE_1,
      ENUM_VALUE_2
  };
  ```

## Layout and formatting conventions

- C++ class code layout: Public function definitions must be at the top of a class definition, since they are most
  likely to be used. Private functions and member variables are left to last. Lay out class functions and member
  variables logically in blocks of related functionality.

- Class inheritance keywords are not indented. For example:

  ```C++
  class MyClass
  {
  public:
    int m_PublicMember;
  protected:
    int m_ProtectedMember;
  private:
    int m_PrivateMember;
  };
  ```

- Do not leave trailing spaces at the end of lines.

- Empty lines do not have trailing spaces.

- For pointers and references, the symbols `*` and `&` must be next to the name of the type - *not* the name of the
  variable.

  ```C++
  char* someText = "abc";

  void SomeFunction(const SomeObject& someObject) {}
  ```

## Language usage

- Minimize header `#include` statements: The inclusion of unnecessary headers slows down compilation. If the unnecessary
  header defining this subroutine is included, then it can also hide errors where a function calls a subroutine that it
  must not use.

  Include header statements in the following order:

  - If applicable, begin with the header file corresponding to the current source file,
  - Headers from the same component,
  - Headers from other components,
  - Third-party headers,
  - System headers.

  > **Note:** Leave one blank line between each of these groups for readability. Use quotes for headers from within the
  > same project and angle brackets for third-party and system headers. Do not use paths relative to the current source
  > file, such as `../Header.hpp`. Instead, configure your include paths in the project makefiles.

  For example:

  ```C++
  #include "ExampleClass.hpp"     // Own header

  #include "Header1.hpp"          // Header from same component
  #include "Header1.hpp"          // Header from same component

  #include "other/Header3.hpp"    // Header from other component

  #include <ThirdParty.hpp>       // Third-party headers

  #include <vector>               // System  header

  // [...]
  ```

- Use the template-styled case syntax for C++ casts:

  ```C++
  int a = 100;
  float b = (float)a; // Not OK
  float c = static_cast<float>(a); // OK
  ```

- Use the `const` keyword to declare constants instead of `define`.

- Use `nullptr` instead of `NULL`. C++11 introduced the `nullptr` type to distinguish null pointer constants from the
  integer 0.
