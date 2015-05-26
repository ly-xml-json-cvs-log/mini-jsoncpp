/*************************************************************************
 > File Name: demo.cpp
 > Author: tiankonguse(skyyuan)
 > Mail: i@tiankonguse.com
 > Created Time: 2015年05月24日 星期日 22时14分27秒
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

#include "json.h"

#ifdef __int64
typedef __int64 LL;
#else
typedef long long LL;
#endif
int main(int argc, char** argv) {
	if (argc != 2) {
		printf("./demo json\n");
		exit(1);
	}
	string str = argv[1];
	printf("strJson =  %s\n", str.c_str());
	Json::Reader reader(1);
	Json::Value value;

	int ok = reader.parse(str, value);

	if (!ok) {
		printf("error=%s\n", reader.getFormattedErrorMessages().c_str());
		return 0;
	}
	printf("value =  %d, %d\n", (int) value.getOffsetStart(),
			(int) value.getOffsetLimit());

	vector<string> member = value.getMemberNames();
	printf("size =  %u\n", (int) member.size());
	int size = member.size();
	printf("size =  %d\n", size);
	for (int i = 0; i < size; i++) {
		printf("i=%d size=%d \n", i, size);
		printf("%d : %s\n", i, member[i].c_str());
	}

	return 0;
}
