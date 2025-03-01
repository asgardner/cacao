set(PROJECTNAME "cacao")

# list source files (.c) other than modulename.c
set(SOURCEFILES
	AOloopControl_perfTest_DM.c
	AOloopControl_perfTest_status.c
	AOloopControl_perfTest_LinSim.c
	compRMsensitivity.c
	mlat.c
)

# list include files (.h) that should be installed on system
set(INCLUDEFILES
	mlat.h
)

# list scripts that should be installed on system
set(SCRIPTFILES
	scripts/cacao-compRMsens
	scripts/cacao-selectWFSframes
	scripts/cacao-syncStreams
	scripts/cacao-WFSrefTunePSFloop
)


# DEFAULT SETTINGS
# Do not change unless needed
# =====================================================================


# SRCNAME is current directory (last subdir)
set(NAME0 "${CMAKE_CURRENT_SOURCE_DIR}")
string(REPLACE "/" " " NAME1 ${NAME0})
string(REGEX MATCH "[a-zA-Z0-9_]+$" SRCNAME "${NAME0}")


# LIBNAME constructed by removing spaces
string(REPLACE "_" "" LIBNAME "${PROJECTNAME}${SRCNAME}")


message("")
message(" SRCNAME = ${SRCNAME} -> LIBNAME = ${LIBNAME}")


project(lib_${LIBNAME}_project)

# Library can be compiled from multiple source files
# Convention: the main souce file is named <libname>.c
#
add_library(${LIBNAME} SHARED ${SRCNAME}.c ${SOURCEFILES})

target_link_libraries(${LIBNAME} PUBLIC CLIcore milkstatistic cacaoAOloopControl)

install(TARGETS ${LIBNAME} DESTINATION lib)
install(FILES ${SRCNAME}.h ${INCLUDEFILES} DESTINATION include/${SRCNAME})

install(PROGRAMS
	${SCRIPTFILES}
	DESTINATION bin)
