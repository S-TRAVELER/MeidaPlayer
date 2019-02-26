find_path(AVFORMAT_INCLUDE_DIR
  NAMES libavformat/avformat.h)

find_library(AVFORMAT_LIBRARY
  NAMES avformat)

set(AVFORMAT_LIBRARIES ${AVFORMAT_LIBRARY})
set(AVFORMAT_INCLUDE_DIRS ${AVFORMAT_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(AVFORMAT DEFAULT_MSG AVFORMAT_LIBRARY AVFORMAT_INCLUDE_DIR)
