<div align="center">
<img src="https://raw.githubusercontent.com/Kidsunbo/static_file/main/reddish_hermit/reddish%20hermit.png" style="width:100px; height:100px">
<h1>Reddish Hermit</h1>

[![GitHub Workflow Status](https://img.shields.io/github/actions/workflow/status/Kidsunbo/reddish_hermit/cmake.yml?label=Build&logo=CMake&style=flat-square)](https://github.com/Kidsunbo/reddish_hermit/actions/workflows/cmake.yml)
[![C++](https://img.shields.io/badge/C%2B%2B-20-brightgreen?style=flat-square&logo=cplusplus)](https://isocpp.org)
[![Codecov](https://img.shields.io/codecov/c/github/Kidsunbo/reddish_hermit?logo=codecov&style=flat-square)](https://codecov.io/gh/Kidsunbo/reddish_hermit)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)


`reddish_hermit` is a redis like kv database with client and server which is powered by Boost.ASIO with C++20 coroutine.
The client can be used individually as a redis client.
</div>

## Usage
### Conan
`reddish_hermit` uses `conan` as its package manager, and provided as a conan package as well. So you could just use `conan` to install the package.

Download the source code into your folder and enter to the root of the source code.

```
conan create .
```

and then add 
```
reddish_hermit/0.1.0
```
to your `conanfile.txt` file.

If you need this library to handle `TLS` connection, add 
```
reddish_hermit:with_ssl=True
```
to the `options` section of your `conanfile.txt` file.

### Other way
If you would like to use the traditional way to install this library, you will have to install `Boost` first and `OpenSSL` if you need TLS support. Don't forget set `WITH_SSL` to `ON` if TLS support is needed.
