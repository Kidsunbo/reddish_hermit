# AGENTS.md

C++20 Redis-like key/value database (server) with a standalone Redis client, both powered by Boost.ASIO coroutines. CMake + Conan, gtest, C++20.

## Build & test

There is **no checked-in `CMakePresets.json`**. Conan generates presets (`conan-<buildtype>`, e.g. `conan-debug`, `conan-release`) during `conan install`, so you must run Conan before any CMake configure.

```bash
conan install . --build=missing -s build_type=Debug --output-folder=build
cmake . --preset conan-debug
cmake --build --preset conan-debug
ctest --preset conan-debug
```

- CI (`.github/workflows/cmake.yml`) uses `gcc-11`/`g++-11` on Ubuntu 22.04, Debug build.
- Helper scripts `script/build_client.sh` / `script/build_server.sh <TYPE>` run the full Conan+configure+build flow for a single component.
- Conan options: `with_ssl` (adds OpenSSL 3.0.5 requirement), `enable_test`, `shared`, `fPIC`. TLS only when `WITH_SSL=ON`.

## Structure

- `common/` — header-only-ish library `reddish_hermit_common`: network (`Connection`), RESP `protocol`, `command`, `utils`. Pure library, no standalone executable. All three components depend on it.
- `client/` — `reddish_hermit_client` lib (installable standalone); loop/Awaitable-async. Include prefix `client/include/reddish/...`.
- `server/` — `reddish_hermit_server` executable; `kiedis`, `storage`, `commands`. Include prefix `server/include/server/...`.
- Sources are gathered with `aux_source_directory` in each `src/CMakeLists.txt` — adding a `.cpp` under an existing src dir is picked up automatically; add new dirs explicitly.

## Tests

- Only real, registered test: `common/test/connection/connection_test.cpp`. `client/test` and `server/test` CMakeLists are effectively empty (just `find_package(GTest)`); don't expect executables there.
- Tests use `boost::asio` coroutines + gtest, gated behind `ENABLE_TEST` (default ON).
- When `ENABLE_TEST` is ON, targets get `-Wall -Wextra -Werror`, `-fsanitize=address`, and `--coverage` — build will hard-fail on any warning via `-Werror`.

## Style

- clang-format: `BasedOnStyle: WebKit`, `ColumnLimit: 0` (no line-length limit), 4-space indent.
- Namespaces: `reddish::common::...`, `reddish::server::...`, `reddish::client::...`.
- Common naming: `snake_case`. Code is coroutine-heavy; `co_await` on Boost.ASIO awaitables, errors returned as expected/error_code.