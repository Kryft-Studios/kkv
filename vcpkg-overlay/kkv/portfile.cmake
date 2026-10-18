
vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

# The overlay is inside the KKV repository.
get_filename_component(
    SOURCE_PATH
    "${CMAKE_CURRENT_LIST_DIR}/../.."
    ABSOLUTE
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DBUILD_TESTING=OFF
        -DKKV_BUILD_DOCUMENTATION=OFF
        -DKKV_BUILD_FORMAT_TARGET=OFF
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(
    CONFIG_PATH lib/cmake/kkv
)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

vcpkg_install_copyright(
    FILE_LIST "${SOURCE_PATH}/LICENSE"
)
