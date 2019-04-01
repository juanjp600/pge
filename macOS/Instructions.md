Mostly writing this as instructions for myself later. ~Mark

# How to setup an Xcode project that uses PGE.

1. Make a new Xcode project with the game template (or any that spits out a .app).

2. Add `Engine.xcodeproj` as a project item.

3. Project Settings -> Build Settings -> Search Paths -> Header Search Paths. Add `../../Include/` and `../../Libraries/SDL2-2.0.9/include/`. Make sure it's both on the bundle and Xcode project.

4. Build Phases -> Link Binary With Libraries. Add `libEngine.a`, `SDL2.framework`, `OpenAL.framework` and `OpenGL.framework`.

5. Add a `New Copy Files Phase`. Set Destination to `Frameworks`. Add `SDL2.framework`. Call it `Copy Bundle Frameworks`.

6. Grab a .cpp with a defined `int PGE::Main()`. `#include <Init/Init.h>` has the declaration for this.

7. Add `Dummy.txt` as a project item.

8. Crush with `Guard Malloc`.