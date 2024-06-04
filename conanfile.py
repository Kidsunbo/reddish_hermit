from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps


class ReddishHermitConan(ConanFile):
    name = "reddish_hermit"
    version = "0.1.0"

    # Optional metadata
    license = "MIT"
    author = "Kie <qiongxiaozi158@sina.com>"
    url = "https://github.com/Kidsunbo/reddish_hermit"
    description = "A kv database powered by Boost.ASIO"
    topics = ("async", "redis", "asio", "database")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "with_ssl": [True, False],
        "enable_test": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "with_ssl": False,
        "enable_test": True,
    }

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "test/*", "LICENSE"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        self.requires("boost/1.81.0")
        if self.options.with_ssl:
            self.requires("openssl/3.0.5")

    def build_requirements(self):
        if self.options.enable_test:
            self.test_requires("gtest/cci.20210126")

    def _configurate(self, tc):
        tc.variables["WITH_SSL"] = self.options.with_ssl
        tc.variables["ENABLE_TEST"] = self.options.enable_test

    def generate(self):
        tc = CMakeToolchain(self)
        self._configurate(tc)
        tc.generate()

        tc = CMakeDeps(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["reddish_hermit"]
        if self.options.enable_test:
            self.cpp_info.cxxflags = ["--coverage", "-fsanitize=address"]
            self.cpp_info.exelinkflags = ["--coverage", "-fsanitize=address"]
