#ifndef LOGGER_H_
#define LOGGER_H_

#include "Types.h"

namespace slnp {

#ifndef __FILENAME__
#define __FILENAME__ (__FILE__ + SOURCE_PATH_SIZE)
#endif

typedef enum {
  LOG_ERROR,
  LOG_WARN,
  LOG_INFO,
  LOG_DEBUG,
  LOG_VERBOSE
} LogLevel;

void Log(const char* file, int line, const char* fn, LogLevel level, const char* fmt, ...);

#if SLNP_ENABLE_LOGGER
#define LOGE(...) Log(__FILENAME__, __LINE__, __func__, slnp::LOG_ERROR, __VA_ARGS__)
#define LOGW(...) Log(__FILENAME__, __LINE__, __func__, slnp::LOG_WARN, __VA_ARGS__)
#define LOGI(...) Log(__FILENAME__, __LINE__, __func__, slnp::LOG_INFO, __VA_ARGS__)
#define LOGD(...) Log(__FILENAME__, __LINE__, __func__, slnp::LOG_DEBUG, __VA_ARGS__)
#define LOGV(...) Log(__FILENAME__, __LINE__, __func__, slnp::LOG_VERBOSE, __VA_ARGS__)
#define LOG_ENTER LOGV("enter")
#define LOG_LEAVE LOGV("leave")
#else
#define LOGE(...) ;
#define LOGW(...) ;
#define LOGI(...) ;
#define LOGD(...) ;
#define LOGV(...) ;
#define LOG_ENTER ;
#define LOG_LEAVE ;
#endif // SLNP_ENABLE_LOGGER

} /* namespace slnp */

#endif /* LOGGER_H_ */
