# OpenPano
Re-implementation of OpenPano by @ppwwyyxx, but with efficient tricks and modern C++20

## Project Structure
```
.
├── build                   # Compiled files (alternatively `dist`)
├── docs                    # Documentation files (alternatively `doc`)
├── src                     # Source files (alternatively `lib` or `app`)
    ├── common
    ├── utils                  
    └── main.cpp
├── test                    # Automated tests (alternatively `spec` or `tests`)
├── tools                   # Tools and utilities
├── LICENSE
├── README.md
├── .gitignore
└── CMakeLists.txt

```

##### TODO
- [ ] split sub folders into libraries and link them using cmake instead of local imports
- [ ] header protections
- [ ] fix relative path issue with `std::filename::path`
- [ ] change the binary output file to the src folder instead of the build
- [ ] add author tag on top of the page
- [ ] after finishing, follow notes from API Design document in Obsidian
- [ ] Polish CMake files
- [ ] add unit testing to CMAKE files