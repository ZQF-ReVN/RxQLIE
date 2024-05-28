-- project
set_project("qlie-tools")
set_xmakever("2.8.9")

-- language
set_warnings("all")
set_languages("c++23")
set_encodings("utf-8")

-- allows
set_allowedplats("windows")
set_allowedarchs("x86", "x64")
set_allowedmodes("debug", "release", "releasedbg")

-- rules
add_rules("plugin.vsxmake.autoupdate")
add_rules("mode.debug", "mode.release", "mode.releasedbg")

-- lto
if is_mode("releasedbg") or is_mode("release") then
    set_policy("build.optimization.lto", true)
end

-- repo
add_repositories("Drepo https://github.com/Dir-A/Drepo.git")

-- packages
add_requires("zqf-zut 347d8cf510b493d713871ed429b728657240fd1d")

-- targets
target("qlie-tools-test")
    set_default(false)
    set_kind("binary")
    add_files("test/main.cpp")
    add_packages("zqf-zut")