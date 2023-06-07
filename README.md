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
You need to define the platform, in this case KENGINE_WIN32 as this will resolve macros to the correct platform specific calls. I'd rather this be explicit for now, as we could figure out the platform based on the compiler. However later I'd like to include the idea of specifying KENGINE_LIB or similar, whereby the translation unit has zero references to any specific platform and all of these platform macros will resolve to calling function pointers.

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
This way the entire of the kengine library can be compiled once and reused in incremental builds. Personally I stick to unity builds that will just have a single translation unit, unless the project reaches 500 million lines of code the compile time can of a pure C project can still be a few seconds and what you're usually waiting for is the linker.

## API calls you
This may seem backwards to some, but platforms can be quite different and trying to design an engine that supports all of them forces you to include platform specific concepts into your library. For example creating a window or the message pump, while this is boilerplate code on some platforms its not even the case at all on others. So rather than focing consumers to write this plumming of create window, handle message, etc, all of this logic is buried in the engine. Instead you can just focus on what you need for your application/game, such as what needs to be rendered this frame? What sounds need to be playing?

## Console

[console_hello.c](https://github.com/kstandbridge/kengine_examples/blob/main/console_hello/console_hello.c)                 
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
                  
Notice we have a KENGINE_CONSOLE define, this tells kengine to do the boilerplate code for creating a console application, then you are expecting to create the main application loop:
```
s32 MainLoop(app_memory *AppMemory);
```

## Window

[window_hello.c](https://github.com/kstandbridge/kengine_examples/blob/main/window_hello/window_hello.c)
```
#define KENGINE_WIN32
#define KENGINE_WINDOW
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

typedef struct app_state
{
    memory_arena Arena;
} app_state;

void
InitApp(app_memory *AppMemory)
{
    AppMemory->AppState = BootstrapPushStruct(app_state, Arena);
}

LRESULT
MainWindowCallback(app_memory *AppMemory, HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = DefWindowProcA(Window, Message, WParam, LParam);
    return Result;
}
```
Here we define KENGINE_WINDOW, which when combined with KENGINE_WIN32 creates the basic Win32. Theres two places the engine calls you

```
void InitApp(app_memory *AppMemory);
```
This is called once at the start on the application, this is a good place to setup your application state, create any controls, etc.

```
LRESULT MainWindowCallback(app_memory *AppMemory, HWND Window, UINT Message, WPARAM WParam, LPARAM LParam);
```
This is the message pump, you can handle any message or pass them to the DefWindowProc. The additional first parameter app_memory also contains your app_state defined in InitApp

## Headless
[headless_hello.c](https://github.com/kstandbridge/kengine_examples/blob/main/headless_hello/headless_hello.c)
                   
Similar to KENGINE_WINDOW mentioned above, the KENGINE_HEADLESS will be almost identical however this doesn't create a visible window for the user. Handy for a none visual application.

## Unit Tests
                   
                   

## Kengine Unit Tests

## Preprocessor
## Kengine Preprocessor

## Sim8086
## Minesweeper

## DirectX