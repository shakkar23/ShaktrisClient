file(GLOB plugin_dirs "${CMAKE_CURRENT_LIST_DIR}/*")

foreach(item IN LISTS plugin_dirs)
		if(IS_DIRECTORY "${item}")
			get_filename_component(item ${item} NAME)
			list(APPEND plugins ${item})
		endif()
endforeach()

foreach(plugin IN LISTS plugins)
	add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/${plugin}")

	set_target_properties(${plugin}
	    PROPERTIES
			ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/archive"
			LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins"
			RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins"
			PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/pdb"
	)

endforeach()
