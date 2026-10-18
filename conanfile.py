
import os
from pathlib import Path

from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout


def project_version():
    if os.getenv("kkv_ver"):
        return os.environ["kkv_ver"]

    config_path = Path(__file__).parent / "project-config.env"
    for line in config_path.read_text(encoding="utf-8").splitlines():
        key, separator, value = line.partition("=")
        if separator and key.strip() == "kkv_ver":
            return value.strip()

    raise RuntimeError(f"kkv_ver is not defined in {config_path}")


class KKVConan(ConanFile):
    name = "kkv"
    version = project_version()
    package_type = "static-library"

    license = "Apache-2.0"
    description = (
        "Kryft Key-Value, a configuration and data format library"
    )
    topics = ("kkv", "configuration", "parser", "cpp")
    exports = "project-config.env"

    settings = "os", "compiler", "build_type", "arch"

    requires = "rang/3.2"

    exports_sources = (
        "CMakeLists.txt",
        "CMakePresets.json",
        "project-config.env",
        "Doxyfile",
        "cmake/**",
        "include/**",
        "src/**",
        "private/**",
        "examples/**",
        "tests/**",
        "emscripten/**",
        ".clang-format",
    )

    def layout(self):
        cmake_layout(self)

    def validate(self):
        try:
            check_min_cppstd(self, "20")
        except ConanInvalidConfiguration as exc:
            raise ConanInvalidConfiguration(
                "KKV requires C++20 or newer."
            ) from exc

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()

        toolchain = CMakeToolchain(self)
        toolchain.generator = "Ninja"
        toolchain.user_presets_path = ""
        toolchain.absolute_paths = True
        toolchain.cache_variables["BUILD_TESTING"] = False
        toolchain.cache_variables["CMAKE_CXX_STANDARD"] = 20
        toolchain.cache_variables["CMAKE_CXX_STANDARD_REQUIRED"] = True
        toolchain.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["kkv"]

        self.cpp_info.set_property(
            "cmake_file_name",
            "kkv",
        )
        self.cpp_info.set_property(
            "cmake_target_name",
            "kkv::kkv",
        )
