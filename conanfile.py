from conans import ConanFile, CMake, tools

class LibStream(ConanFile):
    name = "libstream"
    version = "0.0.2"
    url = "https://github.com/PVIII/stream.git"
    license = "MIT"
    author = "Patrick WANG-FRENINGER <github@freninger.at>"
    description = "Combining ranges and asynchronous IO."
    settings = "os", "compiler", "arch", "build_type"
    generators = "cmake"
    requires = "cmcstl2/bee0705@pviii/stable", "delegate/1.0.0@pviii/stable"
    build_requires = "prebuilt-catch2/2.7.1@pviii/stable", "trompeloeil/v34@rollbear/stable"
    exports_sources = "**"
    no_copy_source = True

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        cmake.test(output_on_failure=True)

    def package(self):
        self.copy("*.hpp", excludes="test/*")
        self.copy("CMakeLists.txt")
        self.copy("LICENSE.txt")
        self.copy("README.md")

    def package_id(self):
        self.info.header_only()
