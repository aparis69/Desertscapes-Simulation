solution "ImplicitTerrains"
	configurations { "Debug", "Release" }

	platforms { "x64" }
	
	includedirs { ".", "../Code/Include", "/usr/include/" }

	rootDir = path.getabsolute("../")
	
	configuration "Debug"
		targetdir "./Out/Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration "Release"
		targetdir "./Out/Release"
		flags { "OptimizeSpeed" }

	configuration "linux"
		buildoptions { "-mtune=native -march=native" }
		buildoptions { "-std=c++14" }
		buildoptions { "-w" }
		buildoptions { "-flto -g"}
		linkoptions { "-flto"}
		linkoptions { "-g"}

fileList = { rootDir .. "/Code/Source/*.cpp", rootDir .. "/Code/Include/*.h", rootDir .. "/Code/Source/TTree/*.cpp",
rootDir .. "/Code/Source/MC/*.cpp", rootDir .. "/Code/Source/GeoTree/*.cpp" }

project("ImplicitTerrains")
	language "C++"
	kind "ConsoleApp"
	targetdir "Out"
files ( fileList )
