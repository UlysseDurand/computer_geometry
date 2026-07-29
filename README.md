# File structure

- The subjects of the labs are in `sujets/`
- The source main files are in `projects/`
- The input files are in `data/`
- The output files are in `out/`

# Build the code

```
mkdir build
cd build
cmake ..
cmake --build .
```

# Run the tests (once built)
```
cd build
ctest
```

# Run a tp
```
./build/tp1
```