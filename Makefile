BUILD_FOLDER:=./build
CMAKE_MAC:=/usr/local/bin/cmake
CMAKE_LINUX:=/usr/bin/cmake

TARGET_MAC:=-DTARGET_MAC=True
TARGET_LINUX:=-DTARGET_LINUX=True
CMAKE_OPTIONS_MAC:=-DCMAKE_C_COMPILER:FILEPATH=/usr/bin/clang -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/clang++ ${TARGET_MAC} -B ./build/mac_x64/
CMAKE_OPTIONS_LINUX:=-DCMAKE_C_COMPILER:FILEPATH=/usr/bin/gcc -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/g++ ${TARGET_LINUX} -B ./build/linux_x64/
CMAKE_OPTIONS:=--no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=Debug -S ./ -G "Unix Makefiles"
CMAKE_BUILD_OPTIONS:=--config Debug --target all -j 14 --

cleanup:
	rm -rf ${BUILD_FOLDER}/*
	rm -rf Resources/compiledShaders/*.spv

mac: cleanup compileShaders
	${CMAKE_MAC} ${CMAKE_OPTIONS} ${CMAKE_OPTIONS_MAC} \
	&& ${CMAKE_MAC} --build ${BUILD_FOLDER}/mac_x64 ${CMAKE_BUILD_OPTIONS}

linux: cleanup compileShaders
	${CMAKE_LINUX} ${CMAKE_OPTIONS} ${CMAKE_OPTIONS_LINUX} \
	&& ${CMAKE_LINUX} --build ${BUILD_FOLDER}/linux_x64 ${CMAKE_BUILD_OPTIONS}	

compileShaders:
	./bin/compileShaders.sh