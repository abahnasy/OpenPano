

# find sources
file(GLOB SOURCES common/*.cc utils/*.cc)
add_executable(main main.cc ${SOURCES})