from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps


class ReddishHermitConan(ConanFile):
    name = "reddish_hermit"
    version = "0.1.0"

    # Optional metadata
    license = "MIT"
    author = "Kie <qiongxiaozi158@sina.com>"
    url = "https://github.com/Kidsunbo/reddish_hermit"
    description = "A redis client powered by Boost.ASIO"
    topics = ("async", "redis", "asio")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "with_ssl": [True, False],
    }
    default_options = {"shared": False, "fPIC": True, "with_ssl": False}

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "LICENSE"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)

    def configure(self):
        self.options["kie_toolbox"].with_context = True
        self.options["kie_toolbox"].enable_test = False

    def requirements(self):
        self.requires("kie_toolbox/0.1.1")
        if self.options.with_ssl:
            self.requires("openssl/3.0.5")

    def _configurate(self, tc):
        tc.variables["WITH_SSL"] = self.options.with_ssl

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
