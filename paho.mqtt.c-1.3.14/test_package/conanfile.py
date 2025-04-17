from conans import ConanFile, CMake, tools, RunEnvironment  # Import necessary Conan modules
import os  # For handling file paths


class TestPackageConan(ConanFile):  # Define a Conan package recipe for testing
    settings = "os", "compiler", "build_type", "arch"  # Define which settings affect the package
    generators = "cmake"  # Use CMake generator to create build files

    def build(self):  # Step to build the test package
        cmake = CMake(self)          # Create a CMake build helper
        cmake.configure()            # Run cmake configuration
        cmake.build()                # Compile the project

    def imports(self):  # Step to copy required shared libs into test/bin
        self.copy("*paho*.dll", dst="bin", src="bin")       # Copy DLLs on Windows
        self.copy("*paho*.dylib*", dst="bin", src="lib")    # Copy dylibs on macOS

    def test(self):  # Step to run the compiled test binary
        with tools.environment_append(RunEnvironment(self).vars):  # Ensure runtime paths are set correctly
            bin_path = os.path.join("bin", "test_package")  # Build path to test binary

            if self.settings.os == "Windows":
                self.run(bin_path)  # Run directly on Windows
            elif self.settings.os == "Macos":
                self.run("DYLD_LIBRARY_PATH=%s %s" % (
                    os.environ.get('DYLD_LIBRARY_PATH', ''), bin_path))  # Set macOS dynamic lib path
            else:
                self.run("LD_LIBRARY_PATH=%s %s" % (
                    os.environ.get('LD_LIBRARY_PATH', ''), bin_path))  # Set Linux/Unix lib path
