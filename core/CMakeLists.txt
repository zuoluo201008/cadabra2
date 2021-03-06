
cmake_minimum_required(VERSION 2.8)

project(Cadabra)


# Set path to additional cmake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../cmake/modules")

message("-- CMake version = ${CMAKE_VERSION}")

if (POLICY CMP0042)
  cmake_policy(SET CMP0042 NEW)
endif(POLICY CMP0042)


# Locate python. On Apple the standard CMake PythonLibs.cmake does not
# work correctly, because it will always give some paths which point
# to the Apple-supplied Python libraries. We need the Homebrew
# installed ones. In order to achieve this, we use CMake scripts which
# were taken from Nikolaus Dremmel's repo at github.com/NikolausDemmel/CMake.


if(USE_PYTHON_3)
  set(Python_ADDITIONAL_VERSIONS 3.4)
  set(PythonInterp_ADDITIONAL_VERSIONS 3.4)
  find_package(PythonInterp 3.4 REQUIRED)
  if(APPLE)
     find_package(PythonLibsOSX 3.4 REQUIRED)
  else()
     find_package(PythonLibs 3.4 REQUIRED)
  endif()
else()
  set(Python_ADDITIONAL_VERSIONS 2.7)
  set(PythonInterp_ADDITIONAL_VERSIONS 2.7)
  find_package(PythonInterp 2.7 REQUIRED)
  if(APPLE)
     find_package(PythonLibsOSX 2.7 REQUIRED)
  else()
     find_package(PythonLibs 2.7 REQUIRED)
  endif()
endif()
message("-- Python version = ${PYTHONLIBS_VERSION_STRING}")

# Locate Boost and Boost.Python.
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
if(USE_PYTHON_3)
  find_package(Boost 1.53.0 COMPONENTS python3)
  if(NOT Boost_PYTHON3_FOUND)
    if("${PYTHONLIBS_VERSION_STRING}" MATCHES "^3.4")
      find_package(Boost 1.53.0 COMPONENTS python-py34 REQUIRED)
    else()
      find_package(Boost 1.53.0 COMPONENTS python-py35 REQUIRED)
    endif()
  endif()
else()
  find_package(Boost 1.53.0 COMPONENTS python)
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.5)
        message(FATAL_ERROR "Clang version must be at least 3.5 to avoid known bugs.")
    endif()
#	 add_definitions("-Wno-potentially-evaluated-expression")
endif()



# Locate gmpxx
find_library(GMP_LIB gmp REQUIRED)
find_library(GMPXX_LIB gmpxx REQUIRED)
message("-- Found gmp ${GMP_LIB}")
message("-- Found gmpxx ${GMPXX_LIB}")
message("-- Found python ${PYTHON_LIBRARIES}")
include_directories("${Boost_INCLUDE_DIRS}" ${PYTHON_INCLUDE_DIRS})
message("-- Python include dirs: ${PYTHON_INCLUDE_DIRS}")

# Locate pcre
find_package(PCRE REQUIRED)

if(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions("-Wall -g -Wno-unused-but-set-variable")
endif()

set(LOCAL_SRC_FILES
  PythonCdb.cc
  Bridge.cc
  Algorithm.cc
  Cleanup.cc
  Combinatorics.cc
  Compare.cc
  DisplayBase.cc
  DisplayTeX.cc
  DisplaySympy.cc
  DisplayTerminal.cc
  Exceptions.cc
  Exchange.cc
  Functional.cc
  IndexIterator.cc
  Kernel.cc
  Parser.cc
  PreClean.cc
  PreProcessor.cc
  Props.cc
  PythonException.cc
  Stopwatch.cc
  Storage.cc
  Symbols.cc
  SympyCdb.cc
  YoungTab.cc
  properties/Accent.cc
  properties/AntiCommuting.cc
  properties/AntiSymmetric.cc
  properties/Commuting.cc
  properties/CommutingAsProduct.cc
  properties/CommutingAsSum.cc
  properties/CommutingBehaviour.cc
  properties/Coordinate.cc
  properties/DAntiSymmetric.cc
  properties/Depends.cc
  properties/DependsInherit.cc
  properties/Derivative.cc
  properties/Diagonal.cc
  properties/DiracBar.cc
  properties/Distributable.cc
  properties/EpsilonTensor.cc
  properties/FilledTableau.cc
  properties/GammaMatrix.cc
  properties/GammaTraceless.cc
  properties/ImplicitIndex.cc
  properties/Indices.cc
  properties/Integer.cc
  properties/InverseMetric.cc
  properties/KroneckerDelta.cc
  properties/LaTeXForm.cc
  properties/Matrix.cc
  properties/Metric.cc
  properties/NonCommuting.cc
  properties/NumericalFlat.cc
  properties/PartialDerivative.cc
  properties/RiemannTensor.cc
  properties/SatisfiesBianchi.cc
  properties/SelfAntiCommuting.cc
  properties/SelfCommuting.cc
  properties/SelfNonCommuting.cc
  properties/SortOrder.cc
  properties/Spinor.cc
  properties/Symbol.cc
  properties/Symmetric.cc
  properties/Tableau.cc
  properties/TableauBase.cc
  properties/TableauSymmetry.cc
  properties/Traceless.cc
  properties/Weight.cc
  properties/WeightInherit.cc
  properties/WeylTensor.cc
  modules/xperm_new.cc
  algorithms/canonicalise.cc
  algorithms/collect_components.cc
  algorithms/collect_factors.cc
  algorithms/collect_terms.cc
  algorithms/combine.cc
  algorithms/complete.cc
  algorithms/decompose_product.cc
  algorithms/distribute.cc
  algorithms/drop_weight.cc
  algorithms/einsteinify.cc
  algorithms/eliminate_kronecker.cc
  algorithms/eliminate_metric.cc
  algorithms/epsilon_to_delta.cc
  algorithms/evaluate.cc
  algorithms/expand.cc
  algorithms/expand_delta.cc
  algorithms/expand_diracbar.cc
  algorithms/expand_power.cc
  algorithms/factor_in.cc
  algorithms/factor_out.cc
  algorithms/fierz.cc
  algorithms/flatten_product.cc
  algorithms/flatten_sum.cc
  algorithms/indexsort.cc
  algorithms/integrate_by_parts.cc
  algorithms/join_gamma.cc
  algorithms/keep_terms.cc
  algorithms/lr_tensor.cc
  algorithms/order.cc
  algorithms/product_rule.cc
  algorithms/reduce_delta.cc
  algorithms/rename_dummies.cc
  algorithms/rewrite_indices.cc
  algorithms/sort_product.cc
  algorithms/split_gamma.cc
  algorithms/split_index.cc
  algorithms/substitute.cc
  algorithms/sym.cc
  algorithms/tab_basics.cc
  algorithms/take_match.cc
  algorithms/replace_match.cc
  algorithms/unwrap.cc
  algorithms/vary.cc
  algorithms/young_project.cc
  algorithms/young_project_product.cc
  algorithms/young_project_tensor.cc
)
set(PUBLIC_HEADER_FILES
  Parser.hh
  Storage.hh
)
set(IMAGES
  ../images/cadabra.png
)

if("${PYTHON_SITE_PATH}" STREQUAL "")
  execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import site; print (site.getsitepackages()[0]);"
                  OUTPUT_VARIABLE PYTHON_SITE_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

message("-- Python site path at ${PYTHON_SITE_PATH}")
install(FILES
  "${PROJECT_BINARY_DIR}/cadabra2.so"
  "${PROJECT_SOURCE_DIR}/cadabra2_defaults.py" DESTINATION "${PYTHON_SITE_PATH}")

# Create the 'variable-substituted' cadabra2 inside the source tree so that we can
# run the tests and pick up cadabra2_defaults.py from there.

configure_file("${PROJECT_SOURCE_DIR}/cadabra2.in"             "${PROJECT_SOURCE_DIR}/cadabra2")

install(PROGRAMS "${PROJECT_SOURCE_DIR}/cadabra2" DESTINATION bin)


# Create Python dynamic library 'cadabra' without lib prefix.
include_directories(".")
add_library(cadabra2 SHARED ${LOCAL_SRC_FILES})
set_target_properties(cadabra2 PROPERTIES PREFIX "" SUFFIX ".so" IMPORT_SUFFIX ".so")
set_target_properties(cadabra2 PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADER_FILES}")

target_link_libraries(cadabra2 ${PCRE_LIBRARIES} ${GMPXX_LIB} ${GMP_LIB} ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} )

install(DIRECTORY DESTINATION bin DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                                                        GROUP_READ GROUP_EXECUTE
                                                        WORLD_READ WORLD_EXECUTE)

install(DIRECTORY DESTINATION ${PYTHON_SITE_PATH} DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                                                        GROUP_READ GROUP_EXECUTE
                                                        WORLD_READ WORLD_EXECUTE)

# Install packages.
set(PYTHON_CDB_PACKAGE_PATH ${PYTHON_SITE_PATH}/cdb)
install(FILES
  "${PROJECT_SOURCE_DIR}/packages/cdb/gr.py" DESTINATION "${PYTHON_CDB_PACKAGE_PATH}" 
)

install(DIRECTORY DESTINATION ${PYTHON_CDB_PACKAGE_PATH} DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                                                        GROUP_READ GROUP_EXECUTE
                                                        WORLD_READ WORLD_EXECUTE)
