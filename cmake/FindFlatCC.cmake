# Copyright 2014 Stefan.Eilemann@epfl.ch
# Copyright 2014 Google Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Find the flatcc schema compiler
#
# Output Variables:
# * FLATCC_FOUND
# * FLATCC_EXECUTABLE the flatcc compiler executable
# * FLATCC_INCLUDE_DIR
# * FLATCC_LIBRARY
# * FLATCC_RUNTIME_LIBRARY
#
# Provides:
# * FLATCC_GENERATE_C_HEADERS(Name <files>) creates the C++ headers
#   for the given flatbuffer schema files.
#   Returns the header files in ${Name}_OUTPUTS

set(FLATCC_CMAKE_DIR ${CMAKE_CURRENT_LIST_DIR})

find_program(FLATCC_FLATC_EXECUTABLE NAMES flatcc)
find_path(FLATCC_INCLUDE_DIR NAMES flatcc/flatcc.h)
find_library(FLATCC_LIBRARY NAMES flatcc)
find_library(FLATCC_RUNTIME_LIBRARY NAMES flatccrt)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(flatcc
    DEFAULT_MSG FLATCC_FLATC_EXECUTABLE FLATCC_INCLUDE_DIR FLATCC_LIBRARY FLATCC_RUNTIME_LIBRARY)

if(FLATCC_FOUND)
  function(FLATCC_GENERATE_C_HEADERS Name)
    set(FLATCC_COMMON_READER "${CMAKE_CURRENT_BINARY_DIR}/flatbuffers_common_reader.h")
    add_custom_command(OUTPUT ${FLATCC_COMMON_READER}
      COMMAND ${FLATCC_FLATC_EXECUTABLE}
      ARGS -c -o "${CMAKE_CURRENT_BINARY_DIR}/"
      COMMENT "Building C common flatbuffer reader"
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

    set(FLATCC_OUTPUTS ${FLATCC_COMMON_READER})
    foreach(FILE ${ARGN})
      get_filename_component(FLATCC_OUTPUT ${FILE} NAME_WE)
      set(FLATCC_OUTPUT
        "${CMAKE_CURRENT_BINARY_DIR}/${FLATCC_OUTPUT}_reader.h")
      list(APPEND FLATCC_OUTPUTS ${FLATCC_OUTPUT})

      add_custom_command(OUTPUT ${FLATCC_OUTPUT}
        DEPENDS ${FILE}
        COMMAND ${FLATCC_FLATC_EXECUTABLE}
        ARGS -w -o "${CMAKE_CURRENT_BINARY_DIR}/" ${FILE}
        COMMENT "Building C header for ${FILE}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    endforeach()
    set(${Name}_OUTPUTS ${FLATCC_OUTPUTS} PARENT_SCOPE)
  endfunction()

  set(FLATCC_INCLUDE_DIRS ${FLATCC_INCLUDE_DIR})
  include_directories(${CMAKE_BINARY_DIR})
else()
  set(FLATCC_INCLUDE_DIR)
endif()
