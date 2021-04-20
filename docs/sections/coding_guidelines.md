# Coding standards and guidelines

## Contents

- [Introduction](#introduction)
- [Language version](#language-version)
- [File naming](#file-naming)
- [File layout](#file-layout)
- [Block Management](#block-management)
- [Naming Conventions](#naming-conventions)
  - [C++ language naming conventions](#c_language-naming-conventions)
  - [C language naming conventions](#c-language-naming-conventions)
- [Layout and formatting conventions](#layout-and-formatting-conventions)
- [Language usage](#language-usage)

## Introduction

This document presents some standard coding guidelines to be followed for contributions to this repository. Most of the
code is written in C++, but there is some written in C as well. There is a clear C/C++ boundary at the Hardware
Abstraction Layer (HAL). Both these languages follow different naming conventions within this repository, by design, to:

- have clearly distinguishable C and C++ sources.
- make cross language function calls stand out. Mostly these will be C++ function calls to the HAL functions written in C.
However, because we also issue function calls to third party API's (and they may not follow these conventions), the
intended outcome may not be fully realised in all of the cases.

## Language version

For this project, code written in C++ shall use a subset of the C++11 feature set and software
may be written using the C++11 language standard. Code written in C should be compatible
with the C99 standard.

Software components written in C/C++ may use the language features allowed and encouraged by this documentation.

## File naming

- C files should have `.c` extension
- C++ files should have `.cc` or `.cpp` extension.
- Header files for functions implemented in C should have `.h` extension.
- Header files for functions implemented in C++ should have `.hpp` extension.

## File layout

- Standard copyright notice must be included in all files:

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

- Source lines must be no longer than 120 characters. Prefer to spread code out vertically rather than horizontally,
  wherever it makes sense:

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

- Block indentation should use 4 characters, no tabs.

- Each statement must be on a separate line.

  ```C++
  int a, b; // Error prone
  int c, *d;

  int e = 0; // GOOD
  int *p = nullptr; // GOOD
  ```

- Source must not contain commented out code or unreachable code

## Block Management

- Blocks must use braces and braces location must be consistent.
  - Each function has its opening brace at the next line on the same indentation level as its header, the code within
  the braces is indented and the closing brace at the end is on the same level as the opening.
  For compactness, if the class/function body is empty braces are accepted on the same line.

  - Conditional statements and loops, even if are just single-statement body, needs to be surrounded by braces, the
opening brace is at the same line, the closing brace is at the next line on the same indentation level as its header;
the same rule is applied to classes.

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

### C++ language naming conventions

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

- Macros, pre-processor definitions, and enumeration values should use upper case names:

  ```C++
  #define SOME_DEFINE

  enum class SomeEnum
  {
      ENUM_VALUE_1,
      ENUM_VALUE_2
  };
  ```

- Namespace names must be lower case

  ```C++
  namespace nspace
  {
  void FunctionInNamespace();
  };
  ```

- Source code should use Hungarian notation to annotate the name of a variable with information about its meaning.

  | Prefix | Class | Description |
  | ------ | ----- | ----------- |
  | p | Type      | Pointer to any other type |
  | k | Qualifier | Constant |
  | v | Qualifier | Volatile |
  | m | Scope     | Member of a class or struct |
  | s | Scope     | Static |
  | g | Scope     | Used to indicate variable has scope beyond the current function: file-scope or externally visible scope|

The following examples  of Hungarian notation are one possible set of uses:

  ```C++
  int g_GlobalInt=123;
  char* m_pNameOfMemberPointer=nullptr;
  const float g_kSomeGlobalConstant = 1.234f;
  static float ms_MyStaticMember =  4.321f;
  bool myLocalVariable=true;
  ```

### C language naming conventions

For C sources, we follow the Linux variant of the K&R style wherever possible.

- For function and variable names we use `snake_case` convention:

  ```C
  int some_variable;

  void some_function(int some_parameter) {}
  ```

- Macros, pre-processor definitions, and enumeration values should use upper case names:

  ```C
  #define SOME_DEFINE

  enum some_enum
  {
      ENUM_VALUE_1,
      ENUM_VALUE_2
  };
  ```

## Layout and formatting conventions

- C++ class code layout
  Public function definitions should be at the top of a class definition, since they are things most likely to be used
by other people.
  Private functions and member variables should be last.
  Class functions and member variables should be laid out logically in blocks of related functionality.

- Class  inheritance keywords are not indented.

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

- Don't leave trailing spaces at the end of lines.

- Empty lines should have no trailing spaces.

- For pointers and references, the symbols `*` and `&` should be adjacent to the name of the type, not the name
  of the variable.

  ```C++
  char* someText = "abc";

  void SomeFunction(const SomeObject& someObject) {}
  ```

## Language usage

- Header `#include` statements should be minimized.
  Inclusion of unnecessary headers slows down compilation, and can hide errors where a function calls a
  subroutine which it should not be using if the unnecessary header defining this subroutine is included.

  Header statements should be included in the following order:

  - Header file corresponding to the current source file (if applicable)
  - Headers from the same component
  - Headers from other components
  - Third-party headers
  - System headers

  > **Note:** Leave one blank line between each of these groups for readability.
  >Use quotes for headers from within the same project and angle brackets for third-party and system headers.
  >Do not use paths relative to the current source file, such as `../Header.hpp`. Instead configure your include paths
  >in the project makefiles.

  ```C++
  #include "ExampleClass.hpp"     // Own header

  #include "Header1.hpp"          // Header from same component
  #include "Header1.hpp"          // Header from same component

  #include "other/Header3.hpp"    // Header from other component

  #include <ThirdParty.hpp>       // Third-party headers

  #include <vector>               // System  header

  // [...]
  ```

- C++ casts should use the template-styled case syntax

  ```C++
  int a = 100;
  float b = (float)a; // Not OK
  float c = static_cast<float>(a); // OK
  ```

- Use the const keyword to declare constants instead of define.

- Should use `nullptr` instead of `NULL`,
  C++11 introduced the `nullptr` type to distinguish null pointer constants from the integer 0.
