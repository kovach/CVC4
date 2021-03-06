set(ANTLR_HOME ${ANTLR_DIR})
find_package(ANTLR REQUIRED)

if(NOT HAVE_ANTLR3_FILE_STREAM_NEW)
  add_definitions(-DCVC4_ANTLR3_OLD_INPUT_STREAM)
endif()

# Java runtime is required for ANTLR
find_package(Java COMPONENTS Runtime REQUIRED)

#-----------------------------------------------------------------------------#
# libcvc4parser source files

set(libcvc4parser_src_files
  antlr_input.cpp
  antlr_input.h
  antlr_input_imports.cpp
  antlr_line_buffered_input.cpp
  antlr_line_buffered_input.h
  antlr_tracing.h
  bounded_token_buffer.cpp
  bounded_token_buffer.h
  bounded_token_factory.cpp
  bounded_token_factory.h
  cvc/cvc.cpp
  cvc/cvc.h
  cvc/cvc_input.cpp
  cvc/cvc_input.h
  input.cpp
  input.h
  line_buffer.cpp
  line_buffer.h
  memory_mapped_input_buffer.cpp
  memory_mapped_input_buffer.h
  parser.cpp
  parser.h
  parser_builder.cpp
  parser_builder.h
  parser_exception.h
  smt2/parse_op.h
  smt2/smt2.cpp
  smt2/smt2.h
  smt2/smt2_input.cpp
  smt2/smt2_input.h
  smt2/sygus_input.cpp
  smt2/sygus_input.h
  tptp/TptpLexer.c
  tptp/TptpParser.c
  tptp/tptp.cpp
  tptp/tptp.h
  tptp/tptp_input.cpp
  tptp/tptp_input.h
)

#-----------------------------------------------------------------------------#
# Generate parsers for all supported languages

foreach(lang Cvc Smt2 Tptp)
  string(TOLOWER ${lang} lang_dir)
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${lang_dir})
  add_custom_command(
    OUTPUT
      ${CMAKE_CURRENT_BINARY_DIR}/${lang_dir}/${lang}Lexer.c
      ${CMAKE_CURRENT_BINARY_DIR}/${lang_dir}/${lang}Lexer.h
      ${CMAKE_CURRENT_BINARY_DIR}/${lang_dir}/${lang}Parser.c
      ${CMAKE_CURRENT_BINARY_DIR}/${lang_dir}/${lang}Parser.h
      ${CMAKE_CURRENT_BINARY_DIR}/${lang_dir}/${lang}.tokens
    COMMAND
      ${ANTLR_BINARY}
        ${CMAKE_CURRENT_SOURCE_DIR}/${lang_dir}/${lang}.g
        -fo ${CMAKE_CURRENT_BINARY_DIR}/${lang_dir}
    DEPENDS
      ${lang_dir}/${lang}.g
  )
  set(gen_src_files
    ${CMAKE_CURRENT_BINARY_DIR}/${lang_dir}/${lang}Lexer.c
    ${CMAKE_CURRENT_BINARY_DIR}/${lang_dir}/${lang}Parser.c)

  # Tell cmake that generated source files are actually c++ files
  set_source_files_properties(${gen_src_files} PROPERTIES LANGUAGE CXX)
  set_source_files_properties(${gen_src_files} PROPERTIES GENERATED TRUE)

  # We don't want to enable -Wall for code generated by ANTLR.
  set_source_files_properties(
    ${gen_src_files} PROPERTIES COMPILE_FLAGS -Wno-all)

  # Add generated source files to the parser source files
  list(APPEND libcvc4parser_src_files ${gen_src_files})
endforeach()

#-----------------------------------------------------------------------------#
# libcvc4parser configuration

add_library(cvc4parser ${libcvc4parser_src_files})
set_target_properties(cvc4parser PROPERTIES SOVERSION ${CVC4_SOVERSION})
target_compile_definitions(cvc4parser PRIVATE -D__BUILDING_CVC4PARSERLIB)
target_link_libraries(cvc4parser cvc4 ${ANTLR_LIBRARIES})
target_include_directories(cvc4parser PRIVATE ${ANTLR_INCLUDE_DIR})
install(TARGETS cvc4parser
  EXPORT cvc4-targets
  DESTINATION ${LIBRARY_INSTALL_DIR})

# The generated lexer/parser files define some functions as
# __declspec(dllexport) via the ANTLR3_API macro, which leads to lots of
# unresolved symbols when linking against libcvc4parser.
# -Wl,--export-all-symbols makes sure that all symbols are exported when
# building a DLL.
if(CVC4_WINDOWS_BUILD)
  set_target_properties(cvc4parser
    PROPERTIES LINK_FLAGS "-Wl,--export-all-symbols")
endif()
