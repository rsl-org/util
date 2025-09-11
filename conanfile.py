from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.files import copy


class RslUtilRecipe(ConanFile):
    name = "rsl-util"
    version = "0.1"
    package_type = "library"

    # Optional metadata
    license = "MIT"
    author = "Tsche che@pydong.org"
    description = "Reflective reimplementations of various standard library types and other utilities."
    topics = () # TODO

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False], "fPIC": [True, False],
        "examples": [True, False], "tests": [True, False]
    }
    default_options = {"shared": False, "fPIC": True,
                       "examples": False, "tests": False}
    generators = "CMakeToolchain", "CMakeDeps"

    exports_sources = "CMakeLists.txt", "include/*", "test/*", "cmake/*"

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

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
        util.libs = ["rsl-util"]