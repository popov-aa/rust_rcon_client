from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps


class pkgRecipe(ConanFile):
    name = "rust_rcon_client"
    version = "0.1"
    package_type = "application"

    # Optional metadata
    license = "MIT"
    author = "Popov Aleksey popov.spb@gmail.com"
    url = "https://github.com/popov-aa/rust_rcon_client"
    description = "Yet another rcon client for rust (survival game)"
    topics = ("rcon", "websocket")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*"

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def requirements(self):
        self.requires("websocketpp/0.8.2")
        self.requires("nlohmann_json/3.11.3")
        self.requires("boost/1.83.0")
