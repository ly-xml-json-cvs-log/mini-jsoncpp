/*************************************************************************
 > File Name: demo.cpp
 > Author: tiankonguse(skyyuan)
 > Mail: i@tiankonguse.com
 > Created Time: 2015骞�05鏈�24鏃� 鏄熸湡鏃� 22鏃�14鍒�27绉�
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
using namespace Json;

#ifdef __int64
typedef __int64 LL;
#else
typedef long long LL;
#endif

void outputMember(Json::Value& value) {
	vector<string> member = value.getMemberNames();
	int size = member.size();
	printf("size =  %d\n", size);
	for (int i = 0; i < size; i++) {
		printf("%d : key = %s\n", i, member[i].c_str());
	}
}

void outputObjectValue(Json::Value& value) {
	vector<string> member = value.getMemberNames();
	int size = member.size();
	printf("size =  %d\n", size);
	for (int i = 0; i < size; i++) {
		string key = member[i];
		printf("%d : key = %s ", i, member[i].c_str());
		if (value[key].isBool()) {
			printf("value=%d\n", value[key].asBool());
		} else if (value[key].isArray()) {
			printf("value=%s\n", "array");
		} else if (value[key].isDouble()) {
			printf("value=%lf\n", value[key].asDouble());
		} else if (value[key].isInt()) {
			printf("value=%d\n", value[key].asInt());
		} else if (value[key].isString()) {
			printf("value=%s\n", value[key].asString().c_str());
		} else {
			printf("value=%s\n", "unknow");
		}
	}
}

void outputArrayValue(Json::Value& value) {
	int size = value.size();
	printf("size =  %d\n", size);
	for (int i = 0; i < size; i++) {
		printf("%d :  ", i);
		int key = i;
		if (value[key].isBool()) {
			printf("value=%d\n", value[key].asBool());
		} else if (value[key].isArray()) {
			printf("value=%s\n", "array");
		} else if (value[key].isDouble()) {
			printf("value=%lf\n", value[key].asDouble());
		} else if (value[key].isInt()) {
			printf("value=%d\n", value[key].asInt());
		} else if (value[key].isString()) {
			printf("value=%s\n", value[key].asString().c_str());
		} else {
			printf("value=%s\n", "unknow");
		}
	}
}

string testWriter(Json::Value& value) {
	Json::FastWriter writer;
	string outPut;
	outPut = writer.write(value);
	printf("writer = %s\n", outPut.c_str());
	return outPut;
}

void testValue() {
	Json::Value value(Json::objectValue);
	printf("isObject = %d\n", value.isObject());
	outputMember(value);

	value["first"] = 1;

	value["second"] = 1.2;

	value["child"] = "test";
	outputMember(value);

	outputObjectValue(value);

	value = Json::arrayValue;
	printf("isArray = %d\n", value.isArray());

	value.append(1);
	value.append(1.2);
	value.append("abcd");
	outputArrayValue(value);

	value[1] = "hjsha";
	outputArrayValue(value);

}

void testReader(char* c_str) {
	Json::Value value;
	Json::Reader reader(1);
	Json::FastWriter writer;
	string outPut;
	int ret;

	string str = c_str;
	printf("strJson =  %s\n", str.c_str());

	ret = reader.parse(str, value);

	if (!ret) {
		printf("error=%s\n", reader.getFormattedErrorMessages().c_str());
		return;
	}
	outPut = writer.write(value);
	printf("writer = %s\n", outPut.c_str());

}

void testWriter() {
	Json::FastWriter writer;
	string outPut;

	Json::Value value(Json::objectValue);
	printf("isObject = %d\n", value.isObject());
	outputMember(value);

	outPut = writer.write(value);
	printf("writer = %s\n", outPut.c_str());

	value["first"] = 1;

	outPut = writer.write(value);
	printf("writer = %s\n", outPut.c_str());

	value["second"] = 1.2;

	value["child"] = "test";
	outputMember(value);
	outputObjectValue(value);

	outPut = writer.write(value);
	printf("writer = %s\n", outPut.c_str());

	value = Json::arrayValue;
	printf("isArray = %d\n", value.isArray());

	outPut = writer.write(value);
	printf("writer = %s\n", outPut.c_str());

	value.append(1);

	outPut = writer.write(value);
	printf("writer = %s\n", outPut.c_str());

	value.append(1.2);
	value.append("abcd");
	outputArrayValue(value);

	outPut = writer.write(value);
	printf("writer = %s\n", outPut.c_str());

	value[1] = "hjsha";
	outputArrayValue(value);

	outPut = writer.write(value);
	printf("writer = %s\n", outPut.c_str());
}

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("./demo json\n");
		exit(1);
	}

	int op = atoi(argv[1]);

	if (op == 1) {
		testValue();
	} else if (op == 2) {
		testReader(argv[2]);
	} else if (op == 3) {
		testWriter();
	}

	return 0;
}
