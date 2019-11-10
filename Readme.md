# Program *range*

**Using:**

`range < [data_file_pathname]`

[data_file_pathname] - data file in format like ip_filter.tsv

-------------------------
**Tests:**

 - local development:
```
mkdir build
cd build
conan install ..
cmake ..
cmake --build . --target tests
./Debug/tests
```

 - into os where it have installed:
```
cd /usr/lib/range/tests
./bin/tests
```