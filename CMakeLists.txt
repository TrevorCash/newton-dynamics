#
# Copyright (c) 2008-2018 the Urho3D project.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#


# Define source files
file (GLOB_RECURSE SOURCE_FILES *.h *.cpp)
set (SOURCE_FILES ${SOURCE_FILES})
if (BUILD_SHARED_LIBS)
    set (LIB_TYPE STATIC)
else ()
    set (LIB_TYPE OBJECT)
endif ()


add_library (NewtonDynamics ${LIB_TYPE} ${SOURCE_FILES})
target_include_directories(NewtonDynamics
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sdk>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sdk/dAnimation>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sdk/dContainers>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sdk/dCustomJoints>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sdk/dgCore>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sdk/dgMeshUtil>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sdk/dgNewton>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sdk/dgPhysics>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sdk/dMath>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sdk/dNewton>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sdk/dScene>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sdk/thirdParty/tinyxml>
           $<INSTALL_INTERFACE:${DEST_THIRDPARTY_HEADERS}/sdk>
)


target_compile_definitions(NewtonDynamics PRIVATE _CNEWTON_STATIC_LIB _CUSTOM_JOINTS_STATIC_LIB _NEWTON_STATIC_LIB)




install (DIRECTORY NewtonDynamics DESTINATION ${DEST_THIRDPARTY_HEADERS} FILES_MATCHING PATTERN *.h)
if (BUILD_SHARED_LIBS)
    install(TARGETS NewtonDynamics EXPORT Urho3D ARCHIVE DESTINATION ${DEST_LIBRARY_DIR})
endif ()
