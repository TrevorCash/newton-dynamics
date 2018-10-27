# Copyright (c) <2014-2017> <Newton Game Dynamics>
#
# This software is provided 'as-is', without any express or implied
# warranty. In no event will the authors be held liable for any damages
# arising from the use of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely.

cmake_minimum_required(VERSION 3.12.0)

set (projectName "newton")
#message (${projectName})

# low level core
file(GLOB CPP_SOURCE *.cpp)
file(GLOB HEADERS *.h)

if(NEWTON_BUILD_SHARED_LIBS)
    if(MSVC)
        add_definitions(-D_NEWTON_BUILD_DLL)
    endif(MSVC)
    add_library(${projectName} SHARED ${CPP_SOURCE})
else(NEWTON_BUILD_SHARED_LIBS)
    if (MSVC)
        add_definitions(-D_NEWTON_STATIC_LIB)
    endif(MSVC)

if (GENERATE_DLL)
	add_definitions(-D_NEWTON_BUILD_DLL)
	add_library(${projectName} SHARED ${source})
	target_link_libraries (${projectName} dgCore dgPhysics dTimeTracker)
	install(TARGETS ${projectName} RUNTIME DESTINATION ${dllPath})
	if (BUILD_SANDBOX_DEMOS)
		add_custom_command(
			TARGET ${projectName} POST_BUILD
			COMMAND ${CMAKE_COMMAND}
			ARGS -E copy $(TargetPath) ${PROJECT_BINARY_DIR}/applications/demosSandbox/${CMAKE_CFG_INTDIR}/$(TargetFileName))
	endif ()
endif(MSVC)

file(GLOB includes *.h)
install(FILES ${includes} DESTINATION "${includePath}/${projectName}")
install(TARGETS ${projectName} ARCHIVE DESTINATION ${libraryPath})
