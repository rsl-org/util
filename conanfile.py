from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.files import copy


class RslRecipe(ConanFile):
    name = "rsl"
    version = "0.1"
    package_type = "header-library"

    # Optional metadata
    license = "MIT"
    author = "Tsche che@pydong.org"
    description = "Reflective header-only reimplementations of various standard library types."
    topics = () # TODO

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "coverage": [True, False],
        "examples": [True, False]
    }
    default_options = {"coverage": False, "examples": True}
    generators = "CMakeToolchain", "CMakeDeps"

    exports_sources = "CMakeLists.txt", "include/*"

    def requirements(self):
        self.test_requires("gtest/1.14.0")

    def layout(self):
        cmake_layout(self)

    def build(self):
        if not self.conf.get("tools.build:skip_test", default=False):
            cmake = CMake(self)
            cmake.configure(
                variables={
                    "ENABLE_COVERAGE": self.options.coverage,
                    "ENABLE_EXAMPLES": self.options.examples
                }
            )
            cmake.build()

    def package(self):
        # TODO verify this works
        copy(self, "include/rsl/*", self.source_folder, self.package_folder)

    def package_info(self):
        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []

    def package_id(self):
        self.info.clear()
