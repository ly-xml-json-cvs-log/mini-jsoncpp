/*
 * config.h
 *
 *  Created on: 2015-5-10
 *      Author: tiankonguse
 */

#ifndef CONFIG_H_INCLUDE_MINI_JSONCPP_
#define CONFIG_H_INCLUDE_MINI_JSONCPP_

#include <stdlib.h>
#include <sstream>
#include <ostream>
#include <string>
#include <vector>
#include <map>
#include <math.h>
#include <sstream>
#include <utility>
#include <cstring>
#include <cassert>
#include <cstddef> // size_t
#include <algorithm>
#include <functional>
using namespace std;

#if defined(__GNUC__) &&  (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5))
#define JSONCPP_DEPRECATED(message)  __attribute__ ((deprecated(message)))
#elif defined(__GNUC__) &&  (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
#define JSONCPP_DEPRECATED(message)  __attribute__((__deprecated__))
#else
#define JSONCPP_DEPRECATED(message)
#endif

/*
 *  It should not be possible for a maliciously designed file to  cause an abort() or seg-fault,
 *  so these macros are used only for pre-condition violations and internal logic errors.
 */
# define JSON_ASSERT(condition) assert(condition)

/*
 * The call to assert() will show the failure message in debug builds.
 * In release builds we abort, for a core-dump or debugger.
 */
# define JSON_FAIL_MESSAGE(message)  {  std::ostringstream oss; oss << message;   assert(false && oss.str().c_str()); abort();     }
#define JSON_ASSERT_MESSAGE(condition, message)    {  if (!(condition)) {  JSON_FAIL_MESSAGE(message);   } }

namespace Json {
typedef int Int;
typedef unsigned int UInt;
typedef long long int Int64;
typedef unsigned long long int UInt64;
typedef Int64 LargestInt;
typedef UInt64 LargestUInt;
#define JSON_HAS_INT64

// writer.h
class FastWriter;
class StyledWriter;

// reader.h
class Reader;

// value.h
typedef unsigned int ArrayIndex;
class Path;
class PathArgument;

class Value;
class ValueIteratorBase;
class ValueIterator;
class ValueConstIterator;

} // end namespace Json

#endif /* CONFIG_H_INCLUDE_MINI_JSONCPP_ */
