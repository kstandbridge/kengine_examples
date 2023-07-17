# kengine_examples

This repository is used both for the development of kengine as well as demonstrating how to make use of the framework. Idearly kengine should be usable with a single include, perhaps with a few #defines

For ease of development kengine is split into multiple files, however I plan to have a build task that will generate a single header file which includes kengine in its entirety. For now the kengine.h files acts the same way by having all the includes.

## Basic usage
You can get access to a lot of the functionality by simply including kengine.h in an existing project.
```
#define KENGINE_WIN32
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

int main()
{
    PlatformConsoleOut("Hello, world!");
    return 0;
}
```
You need to define the platform, in this case KENGINE_WIN32 as this will resolve macros to the correct platform specific calls. I'd rather this be explicit for now, as we could figure out the platform based on the compiler. However later I'd like to include the idea of specifying KENGINE_LIB or similar, whereby the translation unit has zero references to any specific platform and all of these PlatformXXX macros would resolve to calling function pointers.

## Multiple translation units
You can include kengine.h in multiple translation units as by default it will only include protos, etc. You need to specify KENGINE_IMPLEMENTATION in a single translation unit which is where the definitions will be defined.

pch.c
```
#define KENGINE_IMPLEMENTATION
#include "kengine.h"
```

main.c
```
#define KENGINE_WIN32
#include "kengine.h"

int main()
{
    PlatformConsoleOut("Hello, world!");
    return 0;
}
```
This way the entire of the kengine library can be compiled once and reused in incremental builds.

## API calls you
This may seem backwards to some, but platforms can be quite different and trying to design an engine that supports all of them forces you to include platform specific concepts into your library. For example creating a window and the message pump, while this is boilerplate code on some platforms its not even the case at all on others. So rather than focing consumers to write this plumming of create window, handle message, etc, all of this logic is buried in the engine. Instead you can just focus on what you need for your application/game, such as what needs to be rendered this frame? What sounds need to be playing?

## Console

[hello_console.c](https://github.com/kstandbridge/kengine_examples/blob/main/hello_console/hello_console.c)
```
#define KENGINE_WIN32
#define KENGINE_CONSOLE
#define KENGINE_IMPLEMENTATION
#include "kengine.h"
                  
s32
MainLoop(app_memory *AppMemory)
{
    s32 Result = 0;
    PlatformConsoleOut("Hello, world!");                  
    return Result;
}
```
                  
Notice we have a KENGINE_CONSOLE define, this tells kengine to do the boilerplate code for creating a console application, then you are exptected to create the main application loop:
```
s32 MainLoop(app_memory *AppMemory);
```
## Preprocessor
[hello_preprocessor.c](https://github.com/kstandbridge/kengine_examples/blob/main/hello_preprocessor/hello_preprocessor.c)
```
#define KENGINE_PREPROCESSOR
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

void
GenerateCodeFor(memory_arena *Arena, c_struct Struct, string_list *Options)
{
    for(string_list *Option = Options;
        Option;
        Option = Option->Next)
    {
        // Output C code for this type
        PlatformConsoleOut("\n// Skipping unsupported option \"%S\" for type \"%S\"", Option->Entry, Struct.Type);
    }
    PlatformConsoleOut("\n");
}
```
Here we defined KENGINE_PREPROCESSOR, this is a console app that takes source files as arguments parses them looking for the introspect tag and lets you generate code. For example generate a ToJson method for a struct. You are expected to implement the GenerateCodeFor function:
```
void
GenerateCodeFor(memory_arena *Arena, c_struct Struct, string_list *Options);
```
Some options already built into kengine are constructors, math operations and linked list, though this will expand over time. Take a look at [kengine_preprocessor.c](https://github.com/kstandbridge/kengine/blob/master/kengine/kengine_preprocessor.c) to see how these are implemented.

### kengine Preprocessor
[kengine_preprocessor.c](https://github.com/kstandbridge/kengine_examples/blob/main/kengine_preprocessor/kengine_preprocessor.c)
We make use of the preprocessor to generate code for kengine, [build.sh](https://github.com/kstandbridge/kengine_examples/blob/main/kengine_preprocessor/build.sh) shows which files are parsed and the result is piped into [kengine_generated.h](https://github.com/kstandbridge/kengine/blob/master/kengine/kengine_generated.h) which is commited to the repository, so consumers don't need to run any preprocessing steps.

## Unit Tests
[hello_test.c](https://github.com/kstandbridge/kengine_examples/blob/main/hello_test/hello_test.c)

```
#define KENGINE_TEST
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

void
RunAllTests(memory_arena *Arena)
{
    AssertTrue(Arena);
    
    string Expected = String("Hello, world!");
    string Actual = String("Hello, world!");
    
    AssertEqualString(Expected, Actual);
}
```
Here we define KENGINE_TEST, which in a sense just creates a console application so we can output text if a test fails. You are expected to create the RunAllTests function:
```
void
RunAllTests(memory_arena *Arena);
```
The AssertEqualX macros will output the file, line etc if the assertion fails, at the end of execution the total and failed number of tests are displayed.

### kengine Unit Tests
[kengine_test.c](https://github.com/kstandbridge/kengine_examples/blob/main/kengine_test/kengine_test.c)
Rather than having the kengine repository grow with test code, I decided to instead keep it as lean as possible and any code used to develop be placed within these examples projects, here is actually all of the unit tests for kengine. If I'm working on an area of the engine I feel will benefit from tests, this file will continue to grow.

## ComputerEnhance
I've been following a course [ComputerEnhance](https://www.computerenhance.com/) and making use of kengine for the coding [exercises](https://github.com/kstandbridge/kengine_examples/tree/main/computer_enhance), the course focuses on performance aware programming.

## _deprecated
These [examples](https://github.com/kstandbridge/kengine_examples/tree/main/_deprecated) may no longer even compile as I'm in the process of migrating a lot of code.
