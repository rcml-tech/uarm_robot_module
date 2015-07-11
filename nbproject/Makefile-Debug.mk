#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=g++
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=MinGW-Windows
CND_DLIB_EXT=dll
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1902510628/SerialClass.o \
	${OBJECTDIR}/uarm_robot_module.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/C/Programs/boost_1_58_0/stage/lib -lboost_system-mgw48-mt-d-1_58 -lboost_thread-mgw48-mt-d-1_58

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../../bin/robot_modules/uarm/uarm_module.${CND_DLIB_EXT}

../../bin/robot_modules/uarm/uarm_module.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ../../bin/robot_modules/uarm
	${LINK.cc} -o ../../bin/robot_modules/uarm/uarm_module.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -shared

${OBJECTDIR}/_ext/1902510628/SerialClass.o: ../../SerialClass/SerialClass.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1902510628
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../module_headers -I../../simpleini -I../../SerialClass -I/C/Programs/boost_1_58_0 -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1902510628/SerialClass.o ../../SerialClass/SerialClass.cpp

${OBJECTDIR}/uarm_robot_module.o: uarm_robot_module.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../module_headers -I../../simpleini -I../../SerialClass -I/C/Programs/boost_1_58_0 -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/uarm_robot_module.o uarm_robot_module.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ../../bin/robot_modules/uarm/uarm_module.${CND_DLIB_EXT}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
