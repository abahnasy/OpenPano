

# find sources
file(GLOB SOURCES common/*.cc utils/*.cc, common/*.h utils/*.h)
add_executable(main main.cc ${SOURCES})