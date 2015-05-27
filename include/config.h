/*
 * config.h
 *
 *  Created on: 2015-5-10
 *      Author: tiankonguse
 */

#ifndef CONFIG_H_INCLUDE_MINI_JSONCPP_
#define CONFIG_H_INCLUDE_MINI_JSONCPP_

#include <stdlib.h>
#include <cstdio>
#include <iosfwd>
#include <istream>
#include <sstream>
#include <ostream>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <stack>
#include <set>
#include <deque>
#include <math.h>
#include <utility>
#include <cassert>
#include <iomanip>
#include <memory>
#include <stddef.h> // size_t
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

#ifndef LOG
#define LOG(X, Y...) do  { printf("%s:%s:%d" X "\n",__FILE__, __FUNCTION__, __LINE__, ##Y); } while(0)
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
# define JSON_FAIL_MESSAGE(message)  do { std::ostringstream oss; oss << message;   assert(true && oss.str().c_str()); abort();} while (0)
#define JSON_ASSERT_MESSAGE(condition, message)     do {  if (!(condition)) {  JSON_FAIL_MESSAGE(message);   }} while (0)

namespace Json {
typedef int Int;
typedef unsigned int UInt;
typedef long long int Int64;
typedef unsigned long long int UInt64;
typedef Int64 LargestInt;
typedef UInt64 LargestUInt;

// writer.h
class FastWriter;
class StyledWriter;

// reader.h
class Reader;

// value.h
typedef unsigned int ArrayIndex;
class Value;

} // end namespace Json

#endif /* CONFIG_H_INCLUDE_MINI_JSONCPP_ */
