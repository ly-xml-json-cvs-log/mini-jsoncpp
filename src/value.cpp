/*
 * value.cpp
 *
 *  Created on: 2015-5-10
 *      Author: tiankonguse
 */

#include "value.h"
#include "writer.h"

namespace Json {

#define JSON_ASSERT_UNREACHABLE assert(false)

// This is a walkaround to avoid the static initialization of Value::null.
// kNull must be word-aligned to avoid crashing on ARM.  We use an alignment of
// 8 (instead of 4) as a bit of future-proofing.
#define ALIGNAS(byte_alignment)

static const unsigned char ALIGNAS(8) kNull[sizeof(Value)] = { 0 };
const unsigned char& kNullRef = kNull[0];
const Value& Value::null = reinterpret_cast<const Value&>(kNullRef);
const Value& Value::nullRef = null;

const Int Value::minInt = Int(~(UInt(-1) / 2));
const Int Value::maxInt = Int(UInt(-1) / 2);
const UInt Value::maxUInt = UInt(-1);
const Int64 Value::minInt64 = Int64(~(UInt64(-1) / 2));
const Int64 Value::maxInt64 = Int64(UInt64(-1) / 2);
const UInt64 Value::maxUInt64 = UInt64(-1);
// The constant is hard-coded because some compiler have trouble
// converting Value::maxUInt64 to a double correctly (AIX/xlC).
// Assumes that UInt64 is a 64 bits integer.
static const double maxUInt64AsDouble = 18446744073709551615.0;
const LargestInt Value::minLargestInt = LargestInt(~(LargestUInt(-1) / 2));
const LargestInt Value::maxLargestInt = LargestInt(LargestUInt(-1) / 2);
const LargestUInt Value::maxLargestUInt = LargestUInt(-1);

template<typename T, typename U>
static inline bool InRange(double d, T min, U max) {
	return d >= min && d <= max;
}

} // namespace Json

// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// ValueInternals...
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

namespace Json {

class Exception: public std::exception {
public:
	Exception(std::string const& msg);
	virtual ~Exception() throw ();
	virtual char const* what() const throw ();
protected:
	std::string const msg_;
};

class RuntimeError: public Exception {
public:
	RuntimeError(std::string const& msg);
};

class LogicError: public Exception {
public:
	LogicError(std::string const& msg);
};

Exception::Exception(std::string const& msg) :
		msg_(msg) {
}
Exception::~Exception() throw () {
}
char const* Exception::what() const throw () {
	return msg_.c_str();
}
RuntimeError::RuntimeError(std::string const& msg) :
		Exception(msg) {
}
LogicError::LogicError(std::string const& msg) :
		Exception(msg) {
}
void throwRuntimeError(std::string const& msg) {
	throw RuntimeError(msg);
}
void throwLogicError(std::string const& msg) {
	throw LogicError(msg);
}

// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// class Value::Value
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

void Value::initBasic(ValueType type) {
	type_ = type;

	switch (type) {
	case nullValue:
		break;
	case intValue:
	case uintValue:
		value_.int_ = 0;
		break;
	case realValue:
		value_.real_ = 0.0;
		break;
	case booleanValue:
		value_.bool_ = false;
		break;
	case stringValue:
		value_.string_ = new StringValues();
		break;
	case arrayValue:
		value_.array_ = new ArrayValues();
		break;
	case objectValue:
		value_.map_ = new ObjectValues();
		break;
	default:
		JSON_ASSERT_UNREACHABLE;
	}
}

Value::StringValues* Value::getStringVaule() {
	return (Value::StringValues*) (value_.string_);
}
Value::ObjectValues* Value::getObjectVaule() {
	return (Value::ObjectValues*) (value_.map_);
}
Value::ArrayValues* Value::getArrayVaule() {
	return (Value::ArrayValues*) (value_.array_);
}

void Value::transformType(ValueType newType) {
	if (type_ != newType) {
		destructor();
		initBasic(newType);
	}
}

Value::Value(ValueType type) {
	initBasic(type);
}

Value::Value(Int value) {
	initBasic(intValue);
	value_.int_ = value;
}

Value::Value(UInt value) {
	initBasic(uintValue);
	value_.uint_ = value;
}

Value::Value(Int64 value) {
	initBasic(intValue);
	value_.int_ = value;
}

Value::Value(UInt64 value) {
	initBasic(uintValue);
	value_.uint_ = value;
}

Value::Value(double value) {
	initBasic(realValue);
	value_.real_ = value;
}

Value::Value(bool value) {
	initBasic(booleanValue);
	value_.bool_ = value;
}

Value::Value(const char* value) {
	initBasic(stringValue);
	(*value_.string_) = value;
}

Value::Value(const std::string& value) {
	initBasic(stringValue);
	(*value_.string_) = value;
}

Value& Value::assignment(Value const& other) {
	switch (type_) {
	case nullValue:
	case intValue:
	case uintValue:
	case realValue:
	case booleanValue:
		value_ = other.value_;
		break;
	case stringValue:
		(*value_.string_) = (*other.value_.string_);
		break;
	case arrayValue:
		(*value_.array_) = (*other.value_.array_);
		break;
	case objectValue:
		(*value_.map_) = (*other.value_.map_);
		break;
	default:
		JSON_ASSERT_UNREACHABLE;
	}

	return *this;
}

Value::Value(Value const& other) {
	initBasic(other.type_);
	assignment(other);
}

void Value::destructor() {
	switch (type_) {
	case nullValue:
	case intValue:
	case uintValue:
	case realValue:
	case booleanValue:
		break;
	case stringValue:
		if (value_.string_ != NULL) {
			delete value_.string_;
			value_.string_ = NULL;
		}
		break;
	case arrayValue:
		if (value_.array_ != NULL) {
			delete value_.array_;
			value_.array_ = NULL;
		}
		break;
	case objectValue:
		if (value_.map_ != NULL) {
			delete value_.map_;
			value_.map_ = NULL;
		}
		break;
	default:
		JSON_ASSERT_UNREACHABLE;
	}
}

Value::~Value() {
	destructor();
}

void Value::swap(Value& other) {
	std::swap(type_, other.type_);
	std::swap(value_, other.value_);
}

Value& Value::operator=(const Value& other) {
	if (type_ != other.type_) {
		destructor();
		initBasic(other.type_);
	}
	assignment(other);
	return *this;
}

ValueType Value::type() const {
	return type_;
}

int Value::compare(const Value& other) const {
	if (*this < other) {
		return -1;
	}
	if (*this > other) {
		return 1;
	}
	return 0;
}

bool Value::operator<(const Value& other) const {
	if (type_ != other.type_) {
		int typeDelta = type_ - other.type_;
		return typeDelta < 0 ? true : false;
	}

	switch (type_) {
	case nullValue:
		return false;
	case intValue:
		return value_.int_ < other.value_.int_;
	case uintValue:
		return value_.uint_ < other.value_.uint_;
	case realValue:
		return value_.real_ < other.value_.real_;
	case booleanValue:
		return value_.bool_ < other.value_.bool_;
	case stringValue:
		return (*value_.string_) < (*other.value_.string_);
	case arrayValue:
		return (*value_.array_) < (*other.value_.array_);
	case objectValue:
		return (*value_.map_) < (*other.value_.map_);
	default:
		JSON_ASSERT_UNREACHABLE;
	}
	return false; // unreachable
}

bool Value::operator<=(const Value& other) const {
	return !(other < *this);
}

bool Value::operator>=(const Value& other) const {
	return !(*this < other);
}

bool Value::operator>(const Value& other) const {
	return other < *this;
}

bool Value::operator==(const Value& other) const {
	return !(*this < other || *this > other);
}

bool Value::operator!=(const Value& other) const {
	return !(*this == other);
}

static bool IsIntegral(double d) {
	double integral_part;
	return modf(d, &integral_part) == 0.0;
}

bool Value::isNull() const {
	return type_ == nullValue;
}

bool Value::isBool() const {
	return type_ == booleanValue;
}

bool Value::isInt() const {
	switch (type_) {
	case intValue:
		return value_.int_ >= minInt && value_.int_ <= maxInt;
	case uintValue:
		return value_.uint_ <= UInt(maxInt);
	case realValue:
		return value_.real_ >= minInt && value_.real_ <= maxInt
				&& IsIntegral(value_.real_);
	default:
		break;
	}
	return false;
}

bool Value::isUInt() const {
	switch (type_) {
	case intValue:
		return value_.int_ >= 0
				&& LargestUInt(value_.int_) <= LargestUInt(maxUInt);
	case uintValue:
		return value_.uint_ <= maxUInt;
	case realValue:
		return value_.real_ >= 0 && value_.real_ <= maxUInt
				&& IsIntegral(value_.real_);
	default:
		break;
	}
	return false;
}

bool Value::isInt64() const {
	switch (type_) {
	case intValue:
		return true;
	case uintValue:
		return value_.uint_ <= UInt64(maxInt64);
	case realValue:
		// Note that maxInt64 (= 2^63 - 1) is not exactly representable as a
		// double, so double(maxInt64) will be rounded up to 2^63. Therefore we
		// require the value to be strictly less than the limit.
		return value_.real_ >= double(minInt64)
				&& value_.real_ < double(maxInt64) && IsIntegral(value_.real_);
	default:
		break;
	}
	return false;
}

bool Value::isUInt64() const {
	switch (type_) {
	case intValue:
		return value_.int_ >= 0;
	case uintValue:
		return true;
	case realValue:
		// Note that maxUInt64 (= 2^64 - 1) is not exactly representable as a
		// double, so double(maxUInt64) will be rounded up to 2^64. Therefore we
		// require the value to be strictly less than the limit.
		return value_.real_ >= 0 && value_.real_ < maxUInt64AsDouble
				&& IsIntegral(value_.real_);
	default:
		break;
	}
	return false;
}

bool Value::isIntegral() const {
	return isInt64() || isUInt64();
}

bool Value::isDouble() const {
	return type_ == realValue || isIntegral();
}

bool Value::isNumeric() const {
	return isIntegral() || isDouble();
}

bool Value::isString() const {
	return type_ == stringValue;
}

bool Value::isArray() const {
	return type_ == arrayValue;
}

bool Value::isObject() const {
	return type_ == objectValue;
}

std::string Value::asString() const {
	switch (type_) {
	case nullValue:
		return "";
	case stringValue:
		return *value_.string_;
	case booleanValue:
		return value_.bool_ ? "true" : "false";
	case intValue:
		return valueToString(value_.int_);
	case uintValue:
		return valueToString(value_.uint_);
	case realValue:
		return valueToString(value_.real_);
	default:
		JSON_FAIL_MESSAGE("Type is not convertible to string");
	}
	return "";
}

Value::Int Value::asInt() const {
	switch (type_) {
	case intValue:
		JSON_ASSERT_MESSAGE(isInt(), "LargestInt out of Int range");
		return Int(value_.int_);
	case uintValue:
		JSON_ASSERT_MESSAGE(isInt(), "LargestUInt out of Int range");
		return Int(value_.uint_);
	case realValue:
		JSON_ASSERT_MESSAGE(InRange(value_.real_, minInt, maxInt),
				"double out of Int range");
		return Int(value_.real_);
	case nullValue:
		return 0;
	case booleanValue:
		return value_.bool_ ? 1 : 0;
	default:
		break;
	}
	JSON_FAIL_MESSAGE("Value is not convertible to Int.");
	return 0;
}

Value::UInt Value::asUInt() const {
	switch (type_) {
	case intValue:
		JSON_ASSERT_MESSAGE(isUInt(), "LargestInt out of UInt range");
		return UInt(value_.int_);
	case uintValue:
		JSON_ASSERT_MESSAGE(isUInt(), "LargestUInt out of UInt range");
		return UInt(value_.uint_);
	case realValue:
		JSON_ASSERT_MESSAGE(InRange(value_.real_, 0, maxUInt),
				"double out of UInt range");
		return UInt(value_.real_);
	case nullValue:
		return 0;
	case booleanValue:
		return value_.bool_ ? 1 : 0;
	default:
		break;
	}
	JSON_FAIL_MESSAGE("Value is not convertible to UInt.");
	return 0u;
}

Value::Int64 Value::asInt64() const {
	switch (type_) {
	case intValue:
		return Int64(value_.int_);
	case uintValue:
		JSON_ASSERT_MESSAGE(isInt64(), "LargestUInt out of Int64 range");
		return Int64(value_.uint_);
	case realValue:
		JSON_ASSERT_MESSAGE(InRange(value_.real_, minInt64, maxInt64),
				"double out of Int64 range");
		return Int64(value_.real_);
	case nullValue:
		return 0;
	case booleanValue:
		return value_.bool_ ? 1 : 0;
	default:
		break;
	}
	JSON_FAIL_MESSAGE("Value is not convertible to Int64.");
	return 0;
}

Value::UInt64 Value::asUInt64() const {
	switch (type_) {
	case intValue:
		JSON_ASSERT_MESSAGE(isUInt64(), "LargestInt out of UInt64 range");
		return UInt64(value_.int_);
	case uintValue:
		return UInt64(value_.uint_);
	case realValue:
		JSON_ASSERT_MESSAGE(InRange(value_.real_, 0, maxUInt64),
				"double out of UInt64 range");
		return UInt64(value_.real_);
	case nullValue:
		return 0;
	case booleanValue:
		return value_.bool_ ? 1 : 0;
	default:
		break;
	}
	JSON_FAIL_MESSAGE("Value is not convertible to UInt64.");
	return 0;
}

LargestInt Value::asLargestInt() const {
	return asInt64();
}

LargestUInt Value::asLargestUInt() const {
	return asUInt64();
}

double Value::asDouble() const {
	switch (type_) {
	case intValue:
		return static_cast<double>(value_.int_);
	case uintValue:
		return static_cast<double>(value_.uint_);
	case realValue:
		return value_.real_;
	case nullValue:
		return 0.0;
	case booleanValue:
		return value_.bool_ ? 1.0 : 0.0;
	default:
		break;
	}
	JSON_FAIL_MESSAGE("Value is not convertible to double.");
	return 0.0;
}

float Value::asFloat() const {
	switch (type_) {
	case intValue:
		return static_cast<float>(value_.int_);
	case uintValue:
		return static_cast<float>(value_.uint_);
	case realValue:
		return static_cast<float>(value_.real_);
	case nullValue:
		return 0.0;
	case booleanValue:
		return value_.bool_ ? 1.0f : 0.0f;
	default:
		break;
	}
	JSON_FAIL_MESSAGE("Value is not convertible to float.");
	return 0.0;
}

bool Value::asBool() const {
	switch (type_) {
	case booleanValue:
		return value_.bool_;
	case nullValue:
		return false;
	case intValue:
		return value_.int_ ? true : false;
	case uintValue:
		return value_.uint_ ? true : false;
	case realValue:
		return value_.real_ ? true : false;
	default:
		break;
	}
	JSON_FAIL_MESSAGE("Value is not convertible to bool.");
	return false;
}

bool Value::isConvertibleTo(ValueType other) const {
	switch (other) {
	case nullValue:
		if (isNumeric()) {
			return asDouble() == 0.0;
		} else if (type_ == booleanValue) {
			return value_.bool_ == false;
		} else if (type_ == stringValue) {
			return asString() == "";
		} else if (type_ == arrayValue) {
			return value_.map_->size() == 0;
		} else if (type_ == objectValue) {
			return value_.map_->size() == 0;
		} else {
			return type_ == nullValue;
		}

	case intValue:
		return isInt()
				|| (type_ == realValue && InRange(value_.real_, minInt, maxInt))
				|| type_ == booleanValue || type_ == nullValue;
	case uintValue:
		return isUInt()
				|| (type_ == realValue && InRange(value_.real_, 0, maxUInt))
				|| type_ == booleanValue || type_ == nullValue;
	case realValue:
		return isNumeric() || type_ == booleanValue || type_ == nullValue;
	case booleanValue:
		return isNumeric() || type_ == booleanValue || type_ == nullValue;
	case stringValue:
		return isNumeric() || type_ == booleanValue || type_ == stringValue
				|| type_ == nullValue;
	case arrayValue:
		return type_ == arrayValue || type_ == nullValue;
	case objectValue:
		return type_ == objectValue || type_ == nullValue;
	default:
		return false;
	}
	JSON_ASSERT_UNREACHABLE;
	return false;
}

/// Number of values in array or object
ArrayIndex Value::size() const {
	switch (type_) {
	case nullValue:
	case intValue:
	case uintValue:
	case realValue:
	case booleanValue:
	case stringValue:
		return ArrayIndex(value_.string_->length());
	case arrayValue:
		return ArrayIndex(value_.array_->size());
	case objectValue:
		return ArrayIndex(value_.map_->size());
	}
	JSON_ASSERT_UNREACHABLE;
	return 0; // unreachable;
}

bool Value::empty() const {
	if (isNull() || isArray() || isObject()) {
		return size() == 0u;
	} else {
		return false;
	}

}

bool Value::operator!() const {
	return isNull();
}

void Value::clear() {
	JSON_ASSERT_MESSAGE(
			type_ == nullValue || type_ == arrayValue || type_ == objectValue,
			"in Json::Value::clear(): requires complex value");

	switch (type_) {
	case arrayValue:
		value_.array_->clear();
		break;
	case objectValue:
		value_.map_->clear();
		break;
	default:
		break;
	}
}

void Value::resize(ArrayIndex newSize) {
	transformType(arrayValue);
	getArrayVaule()->resize(newSize);
}

Value& Value::operator[](ArrayIndex index) {
	transformType(arrayValue);
	if (index >= size()) {
		resize(index + 1);
	}
	return value_.array_->at(int(index));
}
Value& Value::operator[](int index) {
	return (*this)[ArrayIndex(index)];
}

const Value& Value::operator[](ArrayIndex index) const {
	return value_.array_->at(int(index));
}
const Value& Value::operator[](int index) const {
	return (*this)[ArrayIndex(index)];
}

void Value::append(const Value& value) {
	transformType(arrayValue);
	getArrayVaule()->push_back(value);
}

void Value::removeIndex(ArrayIndex index) {
	transformType(arrayValue);
	if (index >= size()) {
		return;
	}
	value_.array_->erase(value_.array_->begin() + index);
}

Value * Value::find(const char* key) {
	transformType(objectValue);
	ObjectValues::iterator it = value_.map_->find(key);
	if (it == value_.map_->end()) {
		return NULL;
	}
	return &(it->second);
}
Value * Value::find(const std::string& key) {
	return find(key.c_str());
}

const Value * Value::find(const char* key) const {
	if (objectValue != type_) {
		return NULL;
	}
	ObjectValues::iterator it = value_.map_->find(key);
	if (it == value_.map_->end()) {
		return NULL;
	}
	return &(it->second);
}

const Value * Value::find(const std::string& key) const {
	return find(key.c_str());

}

Value& Value::operator[](const char*key) {
	transformType(objectValue);
	if (find(key) == NULL) {
		(*value_.map_)[key] = Value();
	}
	return *find(key);
}
Value& Value::operator[](const std::string& key) {
	return (*this)[key.c_str()];
}

const Value& Value::operator[](const char* key) const {
	if (find(key) == NULL) {
		(*value_.map_)[key] = Value();
	}
	return *find(key);
}
const Value& Value::operator[](const string& key) const {
	return (*this)[key.c_str()];
}

void Value::removeMember(std::string const& key) {
	transformType(objectValue);
	ObjectValues::iterator it = value_.map_->find(key);
	if (it == value_.map_->end()) {
		return;
	}
	value_.map_->erase(it);
}

bool Value::isMember(string const& key) const {
	if (type_ != objectValue) {
		return false;
	}
	return NULL != find(key);
}

Value::Members Value::getMemberNames() const {
	Members members;
	if (type_ != objectValue) {
		return members;
	}
	int size = value_.map_->size();
	for (map<string, Value>::iterator it = value_.map_->begin();
			it != value_.map_->end() && size; it++, size--) {
		members.push_back(std::string(it->first));
	}

	return members;
}

std::string Value::toStyledString() const {
	FastWriter writer;
	return writer.write(*this);
}

} // namespace Json
