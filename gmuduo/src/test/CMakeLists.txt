file(GLOB TestFiles "./*.cc")
foreach(testFile ${TestFiles})
    get_filename_component(fname ${testFile} NAME)
    add_executable(${fname} ${testFile})
    target_link_libraries(${fname} gmuduo pthread)
endforeach(testFile)


# add_executable(test001 test001.cc)
# target_link_libraries(test001 gmuduo pthread)

# add_executable(test002 test002.cc)
# target_link_libraries(test002 gmuduo pthread)

# add_executable(test003 test003.cc)
# target_link_libraries(test003 gmuduo pthread)

# add_executable(test004 test004.cc)
# target_link_libraries(test004 gmuduo pthread)

# add_executable(test005 test005.cc)
# target_link_libraries(test005 gmuduo pthread)

# add_executable(test006 test006.cc)
# target_link_libraries(test006 gmuduo pthread)