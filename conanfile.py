from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.files import copy


class RslUtilRecipe(ConanFile):
    name = "rsl-util"
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
    default_options = {"coverage": False, "examples": False}
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
                    "BUILD_TESTING": not self.conf.get("tools.build:skip_test", default=False),
                    "ENABLE_COVERAGE": self.options.coverage,
                    "BUILD_EXAMPLES": self.options.examples
                })
            cmake.build()

    def package(self):
        # TODO verify this works
        copy(self, "include/rsl/*", self.source_folder, self.package_folder)

    def package_info(self):
        self.cpp_info.components["util"].set_property("cmake_target_name", "rsl::util")
        self.cpp_info.components["util"].includedirs = ["include"]
        self.cpp_info.components["util"].bindirs = []
        self.cpp_info.components["util"].libdirs = []
        self.cpp_info.components["util"].libs = []

    def package_id(self):
        self.info.clear()
