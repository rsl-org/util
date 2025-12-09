from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.files import copy


class RslUtilRecipe(ConanFile):
    name = "rsl-util"
    version = "0.1"
    package_type = "header-library"
    no_copy_source = True

    # Optional metadata
    license = "MIT"
    author = "Tsche che@pydong.org"
    description = "Reflective reimplementations of various standard library types and other utilities."
    topics = () # TODO

    settings = "os", "arch", "compiler", "build_type"
    options = {
        "examples": [True, False], "tests": [True, False]
    }
    default_options = {"examples": False, "tests": False}
    generators = "CMakeToolchain", "CMakeDeps"

    exports_sources = "CMakeLists.txt", "include/*", "test/*", "cmake/*"
    def requirements(self):
        if self.options.tests:
            self.requires("gtest/1.14.0")

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure(
            variables={
                "BUILD_TESTING": self.options.tests,
                "BUILD_EXAMPLES": self.options.examples
            })

        cmake.build()
        cmake.install()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "rsl-util")

        util = self.cpp_info.components["util"]
        util.set_property("cmake_target_name", "rsl::util")
        util.includedirs = ["include"]
        util.libs = []