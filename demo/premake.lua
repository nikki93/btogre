---------------------------------------------------------------------------------------------
----------------------------- NGFExample 'Premake.lua' file ---------------------------------
---------------------------------------------------------------------------------------------

-- Project ----------------------------------------------------------------------------------

project.name = "BtOgreTest"
project.bindir = "bin"

-- Package ----------------------------------------------------------------------------------

package = newpackage()

package.name = "BtOgreTest"
package.kind = "exe"
package.language = "c++"
package.configs = { "Debug", "Release" }

if (windows) then
   table.insert(package.defines, "WIN32") -- Needed to fix something on Windows.
end

-- Include and library search paths, system dependent (I don't assume a directory structure)

package.includepaths = {
-- Edit include directories here. Add Ogre and OIS include directories if you don't use
-- pkg-config
"./exampleapp/",
"../include/",
}

package.libpaths = {
-- Edit library directories here. Add Ogre and OIS library directories if you don't use
-- pkg-config
}

-- Libraries to link to ---------------------------------------------------------------------

package.links = {
-- Add Ogre and OIS libraries here, if you don't use pkg-config.
}

-- pkg-configable stuff ---------------------------------------------------------------------

if (linux) then
package.buildoptions = {
"`pkg-config OGRE --cflags`" ..
"`pkg-config OIS --cflags`" ..
"`pkg-config bullet --cflags`"
}

package.linkoptions = {
"`pkg-config OGRE --libs`" ..
"`pkg-config OIS --libs`" ..
"`pkg-config bullet --libs`"
}
end

-- Files ------------------------------------------------------------------------------------

package.files = {
matchrecursive("*.h", "*.cpp"),
"../BtOgre.cpp"
}

-- Debug configuration ----------------------------------------------------------------------

debug = package.config["Debug"]
debug.defines = { "DEBUG", "_DEBUG" }
debug.objdir = "obj/debug"
debug.target = "debug/" .. package.name .. "_d"

debug.buildoptions = { "-g" }

-- Release configuration --------------------------------------------------------------------

release = package.config["Release"]
release.objdir = "obj/release"
release.target = "release/" .. package.name
