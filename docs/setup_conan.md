# Conan setup

## Get conan
You can install `conan` for your OS from [https://conan.io/downloads](https://conan.io/downloads).

## Post-install

### Remotes
Check that `conan` has access to its remotes:
```sh
conan remote list
```
You should get:
```
conancenter: https://center2.conan.io [Verify SSL: True, Enabled: True]
```

If not, run:
```sh
conan remote add conancenter https://center.conan.io
```
### Profile
Now you can initiate your conan profile:
```sh
conan profile detect
```

### Windows Configuration

If you are building on Windows, strictly enforce the usage of MSVC to avoid 
compatibility issues. Check your profile 
(usually located at `C:\Users\<YOU>\.conan2\profiles\default`) and ensure it
matches the configuration below:

```
[settings]
arch=x86_64
build_type=Debug
compiler=msvc
compiler.cppstd=23
compiler.runtime=dynamic
compiler.runtime_type=Debug
compiler.version=195
os=Windows
```

> [!NOTE]
> If conan profile detect generated a profile using `compiler=gcc` (MinGW) or 
> clang, you must edit the file manually to match the settings above.
