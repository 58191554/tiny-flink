| Milestone                             | Implementation | Test | Document |
| ------------------------------------- | -------------- | ---- | -------- |
| M01 — Single-Process Dataflow Runtime | 🏗️              | 🏗️    | 🏗️        |
|                                       |                |      |          |
|                                       |                |      |          |

Unit tests use [GoogleTest](https://google.github.io/googletest/) and live beside
the corresponding headers (for example, `src/core/id_test.cpp`). CMake downloads
the pinned GoogleTest 1.18.0 release on the first configuration, which requires
network access. Use CMake 3.16 or newer and a C++20-capable compiler.

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Run only the ID tests:

```sh
ctest --test-dir build -R '^core\.id\.' --output-on-failure
```

Run only the type descriptor tests:

```sh
ctest --test-dir build -R '^core\.type_descriptor\.' --output-on-failure
```

Run only the record tests:

```sh
ctest --test-dir build -R '^core\.record\.' --output-on-failure
```

Run only the stream message tests:

```sh
ctest --test-dir build -R '^core\.stream_message\.' --output-on-failure
```

After editing code, repeat the build and test commands.
