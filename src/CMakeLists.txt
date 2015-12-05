cmake_minimum_required(VERSION 2.8)
set(CMAKE_INSTALL_PREFIX $ENV{HOME})
project(FbyNetCPP)
add_definitions(-std=c++11 -Wall -Wextra -Weffc++)

# -Weffc++


# enable_testing()
# add_test( testname Exename arg1 arg2 ... )
#
# Or, in its longer form:
#
#   add_test(NAME <name> [CONFIGURATIONS [Debug|Release|...]]
#            [WORKING_DIRECTORY dir]
#            COMMAND <command> [arg1 [arg2 ...]])
#
# http://www.cmake.org/Wiki/CMake/Testing_With_CTest






set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)

LINK_DIRECTORIES( ${LINK_DIRECTORIES} $ENV{HOME}/lib)
include_directories($ENV{HOME}/include)


add_library(fby STATIC fbystd.cpp fbystring.cpp fbyparsetree.cpp fbyregex.cpp fby_core_unix_helpers.cpp fbydb.cpp fbydb_sqlite3.cpp fbydb_postgresql.cpp fbyregexparser.cpp fbytreebuilder.cpp fbywikistrings.cpp fbynetserver.cpp fbynetsocket.cpp fbynethttp.cpp fbyfileutils.cpp fbyimageutils.cpp fbyfilefind.cpp textparse.yy.cpp parsertokens.cpp)

target_link_libraries(fby LINK_PUBLIC sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams )



#FIND_LIBRARY(GTEST_LIBRARY gtest /home/danlyke/lib)
#TARGET_LINK_LIBRARIES(testOVAS ${GTEST_LIBRARY})
#LINK_DIRECTORIES( ${LINK_DIRECTORIES} /home/danyke/lib)
include_directories($ENV{HOME}/include)


add_custom_command( OUTPUT ../var/fby.sqlite3
  COMMAND sqlite3 ARGS  ../var/fby.sqlite3 -init sqldefinitions.sql
  MAIN_DEPENDENCY sqldefinitions.sql )



add_executable(fixedstringextractor fixedstringextractor.cpp)
target_link_libraries (fixedstringextractor LINK_PUBLIC fby boost_filesystem boost_system pcre boost_iostreams boost_program_options cgicc )

add_executable(sqlextractor sqlextractor.cpp)
target_link_libraries (sqlextractor LINK_PUBLIC fby boost_filesystem boost_system pcre boost_iostreams boost_program_options )


add_executable(test_image_name_decomposition test_image_name_decomposition.cpp)
target_link_libraries (test_image_name_decomposition LINK_PUBLIC fby boost_program_options cgicc /home/danlyke/lib/libgtest.a pthread)

add_executable(test_time test_time.cpp)
target_link_libraries (test_time LINK_PUBLIC fby boost_program_options cgicc /home/danlyke/lib/libgtest.a pthread)

add_executable( test_subst  test_subst.cpp)
target_link_libraries ( test_subst LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)

add_executable(test_zstring test_zstring.cpp)
target_link_libraries (test_zstring LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)

add_executable(test_removecrs test_removecrs.cpp)
target_link_libraries (test_removecrs LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)

add_executable(test_wikistring test_wikistring.cpp)
target_link_libraries (test_wikistring LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)

add_executable(example_inverse example_inverse.cpp)
target_link_libraries (example_inverse LINK_PUBLIC Geographic)

add_executable(example_filesystempathstem example_filesystempathstem.cpp)
target_link_libraries (example_filesystempathstem LINK_PUBLIC boost_filesystem boost_system)

add_executable(example_simplexml2 example_simplexml2.cpp)
target_link_libraries (example_simplexml2 LINK_PUBLIC tinyxml2)

add_executable(example_parsegpx example_parsegpx.cpp)
target_link_libraries (example_parsegpx LINK_PUBLIC tinyxml2 Geographic fby)

add_executable(example_file example_file.cpp)
target_link_libraries (example_file LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)

add_executable(example_lambda example_lambda.cpp)
target_link_libraries (example_lambda LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)

add_executable(example_flex example_flex.cpp)
target_link_libraries (example_flex LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)

add_executable(example_tester example_tester.cpp)
target_link_libraries (example_tester LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)

add_executable(example_reset example_reset.cpp)
target_link_libraries (example_reset LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)

add_executable(example_regex example_regex.cpp)
target_link_libraries (example_regex LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)

add_executable(example_gdbm example_gdbm.cpp)
target_link_libraries (example_gdbm LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)

add_executable( example_readstdout  example_readstdout.cpp)
target_link_libraries ( example_readstdout LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)

add_executable(example_sqlite example_sqlite.cpp)
target_link_libraries (example_sqlite LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)

add_executable( example_strftime  example_strftime.cpp)
target_link_libraries ( example_strftime LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)

add_executable(example_findimageinfo example_findimageinfo.cpp)
target_link_libraries (example_findimageinfo LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)

add_executable(example_findjpeginfo example_findjpeginfo.cpp)
target_link_libraries (example_findjpeginfo LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)

add_executable( example_iter  example_iter.cpp)
target_link_libraries ( example_iter LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)

add_executable(example_boost_options example_boost_options.cpp)
target_link_libraries (example_boost_options LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)

add_executable(example_netserver example_netserver.cpp)
target_link_libraries (example_netserver LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)

add_executable(example_httpservertimer example_httpservertimer.cpp)
target_link_libraries (example_httpservertimer LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)

add_executable(example_httpserver example_httpserver.cpp)
target_link_libraries (example_httpserver LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)


add_executable(test_nettimers test_nettimers.cpp)
target_link_libraries (test_nettimers LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)



add_executable(example_inifiles example_inifiles.cpp)
target_link_libraries (example_inifiles LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)


add_executable(test_regexparse test_regexparse.cpp)
target_link_libraries (test_regexparse LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)

add_executable(test_fbyregex test_fbyregex.cpp)
target_link_libraries (test_fbyregex LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)

add_executable(test_fbydb test_fbydb.cpp)
target_link_libraries (test_fbydb LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)

add_executable(test_endswith test_endswith.cpp)
target_link_libraries (test_endswith LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)

add_executable(test_parsehtml test_parsehtml.cpp)
target_link_libraries (test_parsehtml LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)

add_executable(test_nethttpheaders test_nethttpheaders.cpp)
target_link_libraries (test_nethttpheaders LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)

add_executable(test_bodyparse test_bodyparse.cpp)
target_link_libraries (test_bodyparse LINK_PUBLIC fby sqlite3 pq boost_filesystem boost_system gdbm pcre boost_iostreams boost_program_options cgicc /home/danlyke/lib/libgtest.a /home/danlyke/lib/libgtest_main.a pthread)


install (TARGETS fixedstringextractor sqlextractor DESTINATION bin)
install (TARGETS fby DESTINATION lib)
install (FILES
  fbyfileutils.h   fbyparsetree.h    fbytreebuilder.h
  fby.h            fbyregex.h        fbywikistrings.h
  fbyimageutils.h  fbyregexparser.h fby_core_unix_propertyhelper.h
  fbydb.h                         fbystd.h
  fbyfilefind.h                   fbynet.h         fbystring.h
  DESTINATION include)
