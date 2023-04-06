# Copyright (C) 2023  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

cmake_minimum_required(VERSION 3.8)
project(CADRepresentations)

find_package(ParaView REQUIRED)

if (NOT PARAVIEW_USE_QT)
  message(ERROR
    ": Please make sure ParaView has been built with PARAVIEW_USE_QT enabled.")
  return ()
endif()

include(GNUInstallDirs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

option(BUILD_SHARED_LIBS "Build shared libraries" ON)

paraview_plugin_scan(
  PLUGIN_FILES
    cadRepresentations/paraview.plugin
  PROVIDES_PLUGINS plugins
  ENABLE_BY_DEFAULT ON
  HIDE_PLUGINS_FROM_CACHE ON)

paraview_plugin_build(
  RUNTIME_DESTINATION "${CMAKE_INSTALL_BINDIR}"
  LIBRARY_DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  #LIBRARY_SUBDIRECTORY "${PARAVIEW_PLUGIN_SUBDIR}"
  #INSTALL_EXPORT CADViewerParaViewPlugins
  CMAKE_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CADRepresentations
  PLUGINS ${plugins}
  TARGET CADRepresentationPlugin
  #cadviewer_paraview_plugins
  )
