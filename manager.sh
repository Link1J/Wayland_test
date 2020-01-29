#!/bin/bash

if [ "$1" == "GLFW" ]; then
	shift
	while [ "$1" != "" ]; do
		case $1 in
			"build")
				rm -rf build/glfw
				mkdir -p build/glfw

				echo Building object files
				gcc -Wall -g -I ./glfw -c -o build/glfw/glfw.c.obj glfw/glfw.c
				gcc -Wall -g -I ./glfw -c -o build/glfw/glad_gl.c.obj glfw/glad_gl.c
				gcc -Wall -g -I ./glfw -c -o build/glfw/glad_vulkan.c.obj glfw/glad_vulkan.c

				echo Linking exe
				g++ -std=c++2a -Wall -lglfw -lGL -g -I ./glfw -o build/glfw-test build/glfw/*
			;;
			"run")
				echo Running
				./build/glfw-test
			;;
			"debug")
				echo Debuging
				gdb ./build/glfw-test
			;;
	    esac
	    shift
	done
elif [ "$1" == "SHM" ]; then
	shift
	while [ "$1" != "" ]; do
		case $1 in
			"wayland")
				wayland-scanner client-header /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml shm/xdg-shell-protocol.h
				wayland-scanner private-code /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml shm/xdg-shell-protocol.c
				wayland-scanner client-header /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml shm/xdg-decoration-protocol.h
				wayland-scanner private-code /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml shm/xdg-decoration-protocol.c
			;;
			"build")
				rm -rf build/shm
				mkdir -p build/shm

				echo Building object files
				g++ -std=c++2a -Wall -g -I ./shm -c -o build/shm/main.cpp.obj shm/main.cpp
				g++ -std=c++2a -Wall -g -I ./shm -c -o build/shm/Wayland.cpp.obj shm/Wayland.cpp
				g++ -std=c++2a -Wall -g -I ./shm -c -o build/shm/format.cc.obj shm/fmt/format.cc
				gcc -Wall -g -I ./shm -c -o build/shm/xdg-shell-protocol.c.obj shm/xdg-shell-protocol.c 
				gcc -Wall -g -I ./shm -c -o build/shm/xdg-decoration-protocol.c.obj shm/xdg-decoration-protocol.c

				echo Linking exe
				g++ -std=c++2a -Wall -lwayland-client -g -I ./ -o build/shm-test build/shm/*
			;;
			"run")
				echo Running
				./build/shm-test
			;;
			"debug")
				echo Debuging
				gdb ./build/shm-test
			;;
	    esac
	    shift
	done
elif [ "$1" == "EGL" ]; then
	shift
	while [ "$1" != "" ]; do
		case $1 in
			"wayland")
				wayland-scanner client-header /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml egl/xdg-shell-protocol.h
				wayland-scanner private-code /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml egl/xdg-shell-protocol.c
				wayland-scanner client-header /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml egl/xdg-decoration-protocol.h
				wayland-scanner private-code /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml egl/xdg-decoration-protocol.c
			;;
			"build")
				rm -rf build/egl
				mkdir -p build/egl

				echo Building object files
				g++ -std=c++2a -Wall -g -I ./egl -c -o build/egl/main.cpp.obj egl/main.cpp
				g++ -std=c++2a -Wall -g -I ./egl -c -o build/egl/Wayland.cpp.obj egl/Wayland.cpp
				g++ -std=c++2a -Wall -g -I ./egl -c -o build/egl/format.cc.obj egl/fmt/format.cc
				gcc -Wall -g -I ./egl -c -o build/egl/xdg-shell-protocol.c.obj egl/xdg-shell-protocol.c 
				gcc -Wall -g -I ./egl -c -o build/egl/xdg-decoration-protocol.c.obj egl/xdg-decoration-protocol.c

				echo Linking exe
				g++ -std=c++2a -Wall -lwayland-client -lEGL -lwayland-egl -lGLESv2 -g -I ./ -o build/egl-test build/egl/*
			;;
			"run")
				echo Running
				./build/egl-test
			;;
			"debug")
				echo Debuging
				gdb ./build/egl-test
			;;
	    esac
	    shift
	done
elif [ "$1" == "Pointer" ]; then
	shift
	while [ "$1" != "" ]; do
		case $1 in
			"wayland")
				wayland-scanner client-header /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml pointer/xdg-shell-protocol.h
				wayland-scanner private-code /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml pointer/xdg-shell-protocol.c
				wayland-scanner client-header /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml pointer/xdg-decoration-protocol.h
				wayland-scanner private-code /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml pointer/xdg-decoration-protocol.c
			;;
			"build")
				rm -rf build/pointer
				mkdir -p build/pointer

				echo Building object files
				g++ -std=c++2a -Wall -g -I ./pointer -c -o build/pointer/main.cpp.obj pointer/main.cpp
				g++ -std=c++2a -Wall -g -I ./pointer -c -o build/pointer/Wayland.cpp.obj pointer/Wayland.cpp
				g++ -std=c++2a -Wall -g -I ./pointer -c -o build/pointer/format.cc.obj pointer/fmt/format.cc
				gcc -Wall -g -I ./pointer -c -o build/pointer/xdg-shell-protocol.c.obj pointer/xdg-shell-protocol.c 
				gcc -Wall -g -I ./pointer -c -o build/pointer/xdg-decoration-protocol.c.obj pointer/xdg-decoration-protocol.c

				echo Linking exe
				g++ -std=c++2a -Wall -lwayland-client -g -I ./ -o build/pointer-test build/pointer/*
			;;
			"run")
				echo Running
				./build/pointer-test
			;;
			"debug")
				echo Debuging
				gdb ./build/pointer-test
			;;
	    esac
	    shift
	done
fi
