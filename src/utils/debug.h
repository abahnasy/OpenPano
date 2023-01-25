#include <iostream>
#include <filesystem>

#define DEBUG_MARKER "==== DEBUG === "

inline void print_path_info(const std::filesystem::path& path) {
    std::cout << DEBUG_MARKER << "\n";
    std::cout << "exists() = " << std::filesystem::exists(path) << "\n"
        << "root_name() = " << path.root_name() << "\n"
        << "root_path() = " << path.root_path() << "\n"
        << "relative_path() = " << path.relative_path() << "\n"
        << "parent_path() = " << path.parent_path() << "\n"
        << "filename() = " << path.filename() << "\n"
        << "stem() = " << path.stem() << "\n"
        << "extension() = " << path.extension() << "\n";
    std::cout << DEBUG_MARKER << "\n";
}