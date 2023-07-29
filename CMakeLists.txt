# Copyright (c) 2021-present, Zejun Wang (wangzejunscut@126.com).
# All rights reserved.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.14)
cmake_policy(SET CMP0079 NEW)

set(CMAKE_CXX_STANDARD 11)

option(USE_PREFIX_TRIE  "Use Prefix Trie"   OFF)

if (USE_PREFIX_TRIE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3 -DUSE_PREFIX_TRIE")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O3 -DUSE_PREFIX_TRIE")
else ()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O3") 
endif ()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(test tests/test.cpp)

