function(python_install_modules)
	set(path lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages)
	install(FILES ${ARGN} DESTINATION ${path})
endfunction()
