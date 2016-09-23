# plugins

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_MAIN ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

get_target_property(annotator_config annotator::annotatorplugin "IMPORTED_CONFIGURATIONS")
foreach(CONFIG ${annotator_config})
get_target_property(annotator_so annotator::annotatorplugin "IMPORTED_LOCATION_${CONFIG}")
endforeach()

get_filename_component(annotator_bin_dir ${annotator_so} DIRECTORY)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${annotator_bin_dir}/plugins")

add_subdirectory(camshift)
add_subdirectory(kalmanfilter)
add_subdirectory(templatematcher)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY_MAIN})
