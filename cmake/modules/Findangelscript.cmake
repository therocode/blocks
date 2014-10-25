#    OSRail -- a network enabled railroad operations simulator and utilities
#    Copyright (C) 2009 Samuel E. Henley sehenley@comcast.net
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License along
#    with this program; if not, write to the Free Software Foundation, Inc.,
#    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
#
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
#----------------------------------------------------------------------
## \file
## Finds angelscript sdk.
## \page findangelscriptcmake Finds angelscript sdk

# - Find angelscript
# Find angelscript headers and libraries.
#  SET ANGELSCRIPT_SEARCH_SDK_DIRECTORY ${SIMULATOR_SDK_ROOT_DIRECTORY}
#  SET ANGELSCRIPT_SEARCH_BINARY_DIRECTORY ${SIMULATOR_WORKING_BINARY_DIRECTORY}
#  SET ANGELSCRIPT_SEARCH_LIB_DIRECTORY ${SIMULATOR_WORKING_LIB_DIRECTORY}
#  SET ANGELSCRIPT_DIRECTORY_NAME ${ANGELSCRIPT_VERSION_DIRECTORY_NAME}
#
#  ANGELSCRIPT_ROOT_DIRECTORY
#  ANGELSCRIPT_INCLUDE_DIRECTORY
#  ANGELSCRIPT_RELEASE_LIBRARY
#  ANGELSCRIPT_DEBUG_LIBRARY
#  ANGELSCRIPT_FOUND

# Look for the header file.

    FIND_PATH( ANGELSCRIPT_INCLUDE_DIRECTORY
            angelscript.h
            ${ANGELSCRIPT_ROOT_DIRECTORY}/angelscript/include
            ${ANGELSCRIPT_ROOT_DIRECTORY}/include
            ${ANGELSCRIPT_ROOT_DIRECTORY}/sdk/angelscript/include
            DOC "Should be in ${ANGELSCRIPT_ROOT_DIRECTORY}/angelscript/include."
            )

    FIND_LIBRARY( ANGELSCRIPT_LIBRARY
            Angelscript
            ${ANGELSCRIPT_SEARCH_LIB_DIRECTORY}
            ${ANGELSCRIPT_ROOT_DIRECTORY}/angelscript/lib
            ${ANGELSCRIPT_ROOT_DIRECTORY}/lib
            ${ANGELSCRIPT_ROOT_DIRECTORY}/sdk/angelscript/lib
            DOC "Should be in ${ANGELSCRIPT_SEARCH_LIB_DIRECTORY}."
            )

IF( NOT ANGELSCRIPT_LIBRARY )

    FIND_LIBRARY( ANGELSCRIPT_DEBUG_LIBRARY
            Angelscript
            ${ANGELSCRIPT_SEARCH_LIB_DIRECTORY}/debug
            ${ANGELSCRIPT_ROOT_DIRECTORY}/angelscript/lib
            ${ANGELSCRIPT_ROOT_DIRECTORY}/lib
            ${ANGELSCRIPT_ROOT_DIRECTORY}/sdk/angelscript/lib
            DOC "Should be in ${ANGELSCRIPT_SEARCH_LIB_DIRECTORY}/debug."
            )

    FIND_LIBRARY( ANGELSCRIPT_RELEASE_LIBRARY
            Angelscript
            ${ANGELSCRIPT_SEARCH_LIB_DIRECTORY}/release
            ${ANGELSCRIPT_ROOT_DIRECTORY}/angelscript/lib
            ${ANGELSCRIPT_ROOT_DIRECTORY}/lib
            ${ANGELSCRIPT_ROOT_DIRECTORY}/sdk/angelscript/lib
            DOC "${ANGELSCRIPT_SEARCH_LIB_DIRECTORY}/release."
            )

ELSE( NOT ANGELSCRIPT_LIBRARY )

    FIND_LIBRARY( ANGELSCRIPT_DEBUG_LIBRARY
            Angelscript
            ${ANGELSCRIPT_SEARCH_LIB_DIRECTORY}
            ${ANGELSCRIPT_ROOT_DIRECTORY}/angelscript/lib
            ${ANGELSCRIPT_ROOT_DIRECTORY}/lib
            ${ANGELSCRIPT_ROOT_DIRECTORY}/sdk/angelscript/lib
            DOC "Should be in ${ANGELSCRIPT_SEARCH_LIB_DIRECTORY}/debug."
            )

    FIND_LIBRARY( ANGELSCRIPT_RELEASE_LIBRARY
            Angelscript
            ${ANGELSCRIPT_SEARCH_LIB_DIRECTORY}
            ${ANGELSCRIPT_ROOT_DIRECTORY}/angelscript/lib
            ${ANGELSCRIPT_ROOT_DIRECTORY}/lib
            ${ANGELSCRIPT_ROOT_DIRECTORY}/sdk/angelscript/lib
            DOC "${ANGELSCRIPT_SEARCH_LIB_DIRECTORY}/release."
            )

ENDIF( NOT ANGELSCRIPT_LIBRARY )

INCLUDE( FindPackageHandleStandardArgs )

    FIND_PACKAGE_HANDLE_STANDARD_ARGS( ANGELSCRIPT DEFAULT_MSG ANGELSCRIPT_ROOT_DIRECTORY
            ANGELSCRIPT_INCLUDE_DIRECTORY
            ANGELSCRIPT_RELEASE_LIBRARY
            ANGELSCRIPT_DEBUG_LIBRARY
            )
