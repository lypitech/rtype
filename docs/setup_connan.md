# Setup `connan`

## Get conan
You can install `conan` for your os from [https://conan.io/downloads](https://conan.io/downloads).

## Post-install

### Remotes
Check that `conan` has acces to its remotes:
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
