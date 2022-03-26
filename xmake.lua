add_rules("mode.debug", "mode.release")


target("QNekoPlayer")
    add_rules("qt.shared")
    add_frameworks("QtAV","QtAVWidgets","QtGui","QtWidgets")
    add_includedirs("include")
    add_headerfiles("include/*.hpp")
    add_files("src/*.cpp")
    add_files("src/nekoplayer.ui")
    -- add files with Q_OBJECT meta (only for qt.moc)
    add_files("include/qnekoplayer.hpp")

target("hello")
    add_includedirs("include")
    add_rules("qt.widgetapp")
    add_files("tests/hello.cpp")
    add_deps("QNekoPlayer")