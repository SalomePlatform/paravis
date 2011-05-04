project(vtkEDFOverloads)

cmake_minimum_required(VERSION 2.8)

option(GENERATE_PARAVIEW_PLUGIN "warp all classes for usage in paraview" ON)

if(${GENERATE_PARAVIEW_PLUGIN})
  find_package(ParaView REQUIRED)
  include(${PARAVIEW_USE_FILE})
else(${GENERATE_PARAVIEW_PLUGIN})
  find_package(VTK REQUIRED)
  include(${VTK_USE_FILE})
endif(${GENERATE_PARAVIEW_PLUGIN})

set(vtkEDFOverloads_CLASSES  
  vtkEDFCutter
  vtkEDFFactory
)

set(vtkEDFOverloads_CXX)
set(vtkEDFOverloads_H)

foreach(class ${vtkEDFOverloads_CLASSES})
  set(vtkEDFOverloads_CXX ${vtkEDFOverloads_CXX} ${class}.cxx)
  set(vtkEDFOverloads_H ${vtkEDFOverloads_H} ${class}.h)
endforeach(class ${vtkEDFOverloads_CLASSES})

if(${GENERATE_PARAVIEW_PLUGIN})
  add_paraview_plugin(vtkEDFOverloads "0.0"
        SERVER_MANAGER_SOURCES ${vtkEDFOverloads_CXX})
else(${GENERATE_PARAVIEW_PLUGIN})
  add_library(vtkEDFOverloads SHARED ${vtkEDFOverloads_CXX})
  link_libraries(vtkEDFOverloads ${VTK_LIBRARIES})
endif(${GENERATE_PARAVIEW_PLUGIN})

INSTALL(
    TARGETS vtkEDFOverloads
    RUNTIME DESTINATION .
    LIBRARY DESTINATION lib/paraview
    ARCHIVE DESTINATION .
)
