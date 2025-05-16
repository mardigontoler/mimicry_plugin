# mimicry_plugin
A pitch shifted multi-delay plugin


# Dependencies

General:
- [highway](https://github.com/google/highway)
- [JUCE (v 8.0.7)](https://github.com/juce-framework/JUCE)

Windows/Linux:
- [Intel OneMKL](https://www.intel.com/content/www/us/en/developer/tools/oneapi/onemkl-download.html)



# Building

On Win/Linux, Intel OneMKL needs to be installed (from the link above).
The CMake script should hopefully detect it with `find_package`.
This requires that certain environment variables are set up
from a script provided by intel. 
See [Windows Instructions](https://www.intel.com/content/www/us/en/docs/oneapi/programming-guide/2024-2/use-the-setvars-and-oneapi-vars-scripts-with-linux.html)
and  [Linux instructions](https://www.intel.com/content/www/us/en/docs/oneapi/programming-guide/2024-2/use-the-setvars-and-oneapi-vars-scripts-with-linux.html).

Note that on Linux, when trying to use CMake tools integrated into an IDE like CLion,
it's helpfule to add a toolchain Settings->Build, Execution, and Deployment->Toolchains
that runs the `/opt/intel/oneapi/setvars.sh` environment file.
Then, build configurations can be set to use this toolchain.

JUCE and Highway are included as git submodules. 
You can pull them with
`git submodule update --init`



Generate a build system using cmake (at least version 3.14). 
For example:
```bash
mkdir build
cd build
cmake ..

```

To generate a Release build, use 
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```

Check cmake docs for more info about selecting a generator.
For exmaple, to use Ninja:
```bash
cmake .. -G Ninja
```

Then build with
```bash
cmake --build . -t mimicry_VST3
```
