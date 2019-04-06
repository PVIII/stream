from conans import ConanFile, CMake, tools

class Stream(ConanFile):
    name = "stream"
    version = "0.0.2"
    url = "https://github.com/PVIII/stream.git"
    license = "MIT"
    author = "Patrick WANG-FRENINGER <github@freninger.at>"
    description = "Combining ranges and asynchronous IO."
    settings = "os", "compiler", "arch", "build_type"
    generators = "cmake"
    requires = "cmcstl2/bee0705@pviii/stable", "delegate/1.0.0@pviii/stable"
    build_requires = "Catch2/2.7.0@catchorg/stable"
    exports_sources = "**"
    no_copy_source = True

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        self.run("ctest -VV")

    def package(self):
        self.copy("*.hpp")
        self.copy("CMakeLists.txt")
        self.copy("LICENSE.txt")
        self.copy("README.md")

    def package_id(self):
        self.info.header_only()
