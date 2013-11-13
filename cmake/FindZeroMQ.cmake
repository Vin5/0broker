include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(ZeroMQ_PKGCONF libzmq)

# Include dir
find_path(ZeroMQ_INCLUDE_DIR
  NAMES zmq.hpp
  PATHS ${ZEROMQ_ROOT}/include ${ZeroMQ_PKGCONF_INCLUDE_DIRS}
)
# Finally the library itself
find_library(ZeroMQ_LIBRARY
  NAMES zmq
  PATHS ${ZEROMQ_ROOT}/lib ${ZeroMQ_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(ZeroMQ_PROCESS_INCLUDES ZeroMQ_INCLUDE_DIR ZeroMQ_INCLUDE_DIRS)
set(ZeroMQ_PROCESS_LIBS ZeroMQ_LIBRARY ZeroMQ_LIBRARIES)
libfind_process(ZeroMQ)
