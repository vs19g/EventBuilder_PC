workspace "EVB"
	architecture "x64"
	configurations {
		"Release",
		"Debug"
	}

ROOTIncludeDir = "/usr/include/root/"
ROOTLibDir = "/usr/lib64/root/"

project "EVBDict"
	kind "SharedLib"
	language "C++"
	cppdialect "c++11"
	targetdir "./lib/"
	objdir "./objs/"

	prebuildcommands {
		"rootcint -f src/evbdict/evb_dict.cxx src/evbdict/DataStructs.h src/evbdict/LinkDef_evb.h",
		"{COPY} src/evbdict/*.pcm ./lib/"
	}

	postbuildcommands {
		"{COPY} src/evbdict/DataStructs.h ./include/"
	}

	files {
		"src/evbdict/DataStructs.h",
		"src/evbdict/*.cpp",
		"src/evbdict/*.cxx"
	}

	includedirs {
		"./",
		"src/evbdict",
	}

	sysincludedirs {
		ROOTIncludeDir
	}

	libdirs {
		ROOTLibDir
	}

	links {
		"Gui", "Core", "Imt", "RIO", "Net", "Hist", 
		"Graf", "Graf3d", "Gpad", "ROOTDataFrame", "ROOTVecOps",
		"Tree", "TreePlayer", "Rint", "Postscript", "Matrix",
		"Physics", "MathCore", "Thread", "MultiProc", "m", "dl"
	}

	filter "system:macosx or linux"
		linkoptions {
			"-pthread",
			"-rdynamic"
		}

	filter "configurations:Debug"
		symbols "On"

	filter "configurations:Release"
		optimize "On"

project "EventBuilderCore"
	kind "StaticLib"
	language "C++"
	cppdialect "c++11"
	targetdir "./lib/"
	objdir "./objs/"
	pchheader "EventBuilder.h"
	pchsource "./src/EventBuilder.cpp"

	files {
		"src/evbdict/DataStructs.h",
		"src/evb/*.cpp",
		"src/evb/*.h"
	}

	defines "ETC_DIR_PATH=\"./etc/\""

	includedirs {
		"./",
		"src/",
		"vendor/spdlog/include",
		"src/evb",
		"src/evbdict",
		"src/guidict"
	}

	sysincludedirs {
		ROOTIncludeDir
	}

	libdirs {
		ROOTLibDir,
	}

	links {
		"EVBDict", "Gui", "Core", "Imt", "RIO", "Net", "Hist", 
		"Graf", "Graf3d", "Gpad", "ROOTDataFrame", "ROOTVecOps",
		"Tree", "TreePlayer", "Rint", "Postscript", "Matrix",
		"Physics", "MathCore", "Thread", "MultiProc", "m", "dl"
	}

	filter "system:macosx or linux"
		linkoptions {
			"-pthread",
			"-rdynamic"
		}

	filter "configurations:Debug"
		symbols "On"

	filter "configurations:Release"
		optimize "On"

project "EventBuilderGui"
	kind "ConsoleApp"
	language "C++"
	cppdialect "c++11"
	targetdir "./bin/"
	objdir "./objs/"

	prebuildcommands {
		"rootcint -f src/guidict/gui_dict.cxx src/guidict/EVBMainFrame.h src/guidict/FileViewFrame.h src/guidict/LinkDef_Gui.h",
		"{COPY} src/guidict/*.pcm ./bin/"
	}

	files {
		"src/guidict/FileViewFrame.h",
		"src/guidict/EVBMainFrame.h",
		"src/guidict/*.cpp",
		"src/guidict/gui_dict.cxx",
		"src/gui_main.cpp"
	}

	includedirs {
		"./",
		"vendor/spdlog/include",
		"src/evb",
		"src/evbdict",
		"src/guidict"
	}

	sysincludedirs {
		ROOTIncludeDir
	}

	libdirs {
		ROOTLibDir,
	}

	links {
		"EventBuilderCore", "EVBDict", "Gui", "Core", "Imt", "RIO", "Net", "Hist", 
		"Graf", "Graf3d", "Gpad", "ROOTDataFrame", "ROOTVecOps",
		"Tree", "TreePlayer", "Rint", "Postscript", "Matrix",
		"Physics", "MathCore", "Thread", "MultiProc", "m", "dl"
	}

	filter "system:macosx or linux"
		linkoptions {
			"-pthread",
			"-rdynamic"
		}

	filter "configurations:Debug"
		symbols "On"

	filter "configurations:Release"
		optimize "On"

project "EventBuilder"
	kind "ConsoleApp"
	language "C++"
	cppdialect "c++11"
	targetdir "./bin/"
	objdir "./objs/"

	files {
		"src/main.cpp"
	}

	includedirs {
		"src/",
		"vendor/spdlog/include",
		"src/evb",
		"src/evbdict",
		"src/guidict"
	}

	sysincludedirs {
		ROOTIncludeDir
	}

	libdirs {
		ROOTLibDir,
	}

	links {
		"EventBuilderCore", "EVBDict", "Gui", "Core", "Imt", "RIO", "Net", "Hist", 
		"Graf", "Graf3d", "Gpad", "ROOTDataFrame", "ROOTVecOps",
		"Tree", "TreePlayer", "Rint", "Postscript", "Matrix",
		"Physics", "MathCore", "Thread", "MultiProc", "m", "dl"
	}

	filter "system:macosx or linux"
		linkoptions {
			"-pthread",
			"-rdynamic"
		}

	filter "configurations:Debug"
		symbols "On"

	filter "configurations:Release"
		optimize "On"
