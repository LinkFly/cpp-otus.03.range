Program *ip_filter*
-------------

*Using:*

`ip_filter < [data_file_pathname]`

data_file_pathname - data file in format like ip_filter.tsv

*Tests:*

 - local development:
```
mkdir build
cd build
cmake ..
cmake --build . --target tests
./Debug/tests
```

 - into os where it have installed:
```
cd /usr/lib/ip_filter/tests
./bin/tests
```