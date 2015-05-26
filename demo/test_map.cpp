/*************************************************************************
  > File Name: test_map.cpp
  > Author: tiankonguse(skyyuan)
  > Mail: i@tiankonguse.com 
  > Created Time: 2015年05月24日 星期日 23时32分44秒
***********************************************************************/

#include<iostream>
#include<cstdio>
#include<cstring>
#include<cstdlib>
#include<string>
#include<queue>
#include<map>
#include<cmath>
#include<stack>
#include<algorithm>
#include<functional>
#include<stdarg.h>
using namespace std;
#ifdef __int64
typedef __int64 LL;
#else
typedef long long LL;
#endif
int main() {
    typedef map<string, int> Map;
    Map* m = new Map();
    (*m)["test"]=2;
    printf("%d\n", (*m)["test"]);
    return 0;
}
