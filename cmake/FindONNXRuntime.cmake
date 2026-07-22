# Findonnxruntime.cmake

if(DEFINED ONNXRUNTIME_ROOT)
  set(_ONNXRUNTIME_HINTS
    ${ONNXRUNTIME_ROOT}
    ${ONNXRUNTIME_ROOT}/lib
    ${ONNXRUNTIME_ROOT}/lib64
    ${ONNXRUNTIME_ROOT}/include
  )
else()
  set(_ONNXRUNTIME_HINTS
    /usr/local
    /opt/homebrew
    ${CMAKE_SOURCE_DIR}/onnxruntime-*
  )
endif()

find_path(ONNXRuntime_INCLUDE_DIR onnxruntime_cxx_api.h
  HINTS ${_ONNXRUNTIME_HINTS}
  PATH_SUFFIXES include
)

find_library(ONNXRuntime_LIBRARY NAMES onnxruntime
  HINTS ${_ONNXRUNTIME_HINTS}
  PATH_SUFFIXES lib lib64
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ONNXRuntime DEFAULT_MSG
  ONNXRuntime_INCLUDE_DIR ONNXRuntime_LIBRARY)

mark_as_advanced(ONNXRuntime_INCLUDE_DIR ONNXRuntime_LIBRARY)
