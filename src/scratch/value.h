// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../libscratchcpp_global.h"
#include <string>
#include <variant>
#include <ctgmath>
#include <utf8.h>

namespace libscratchcpp
{

using ValueVariant = std::variant<long, double, bool, std::string>;

/*! \brief The Value class represents a Scratch value. */
class LIBSCRATCHCPP_EXPORT Value : public ValueVariant
{
    public:
        enum class SpecialValue
        {
            Infinity,
            NegativeInfinity,
            NaN
        };

        enum class Type
        {
            Number = 0,
            Bool = 1,
            String = 2,
            Infinity = -1,
            NegativeInfinity = -2,
            NaN = -3
        };

        Value(float numberValue);
        Value(double numberValue);
        Value(int numberValue = 0);
        Value(size_t numberValue);
        Value(long numberValue);
        Value(bool boolValue);
        Value(const std::string &stringValue);
        Value(const char *stringValue);
        Value(SpecialValue specialValue);

        Type type() const;

        bool isInfinity() const;
        bool isNegativeInfinity() const;
        bool isNaN() const;
        bool isNumber() const;
        bool isBool() const;
        bool isString() const;

        /*! Returns the int representation of the value. */
        inline int toInt() const { return toLong(); };

        /*! Returns the long representation of the value. */
        inline long toLong() const
        {
            if (static_cast<int>(m_type) < 0) {
                if (m_type == Type::Infinity)
                    return std::numeric_limits<long>::infinity();
                else if (m_type == Type::NegativeInfinity)
                    return -std::numeric_limits<long>::infinity();
                else
                    return 0;
            } else {
                if (auto p = std::get_if<long>(this))
                    return *p;
                else if (auto p = std::get_if<double>(this))
                    return *p;
                else if (auto p = std::get_if<bool>(this))
                    return *p;
                else if (auto p = std::get_if<std::string>(this))
                    return stringToLong(*p);
                else
                    return 0;
            }
        }

        /*! Returns the double representation of the value. */
        inline double toDouble() const
        {
            if (static_cast<int>(m_type) < 0) {
                if (m_type == Type::Infinity)
                    return std::numeric_limits<double>::infinity();
                if (m_type == Type::NegativeInfinity)
                    return -std::numeric_limits<double>::infinity();
                else
                    return 0;
            } else {
                if (std::holds_alternative<double>(*this))
                    return std::get<double>(*this);
                else if (std::holds_alternative<long>(*this))
                    return std::get<long>(*this);
                else if (std::holds_alternative<bool>(*this))
                    return std::get<bool>(*this);
                else if (std::holds_alternative<std::string>(*this))
                    return stringToDouble(std::get<std::string>(*this));
                else
                    return 0;
            }
        };

        /*! Returns the boolean representation of the value. */
        inline bool toBool() const
        {
            if (static_cast<int>(m_type) < 0) {
                return false;
            } else {
                if (std::holds_alternative<bool>(*this))
                    return std::get<bool>(*this);
                else if (std::holds_alternative<long>(*this))
                    return std::get<long>(*this) == 1 ? true : false;
                else if (std::holds_alternative<double>(*this))
                    return std::get<double>(*this) == 1 ? true : false;
                else if (std::holds_alternative<std::string>(*this)) {
                    const std::string &str = std::get<std::string>(*this);
                    return (stringsEqual(str, "true") || str == "1");
                } else
                    return false;
            }
        };

        /*! Returns the string representation of the value. */
        inline std::string toString() const
        {
            if (static_cast<int>(m_type) < 0) {
                if (m_type == Type::Infinity)
                    return "Infinity";
                else if (m_type == Type::NegativeInfinity)
                    return "-Infinity";
                else
                    return "NaN";
            } else {
                if (std::holds_alternative<std::string>(*this))
                    return std::get<std::string>(*this);
                else if (std::holds_alternative<long>(*this))
                    return std::to_string(std::get<long>(*this));
                else if (std::holds_alternative<double>(*this)) {
                    std::string s = std::to_string(std::get<double>(*this));
                    s.erase(s.find_last_not_of('0') + 1, std::string::npos);
                    if (s.back() == '.') {
                        s.pop_back();
                    }
                    return s;
                } else if (std::holds_alternative<bool>(*this))
                    return std::get<bool>(*this) ? "true" : "false";
                else
                    return "";
            }
        };

        /*! Returns the UTF-16 representation of the value. */
        std::u16string toUtf16() const { return utf8::utf8to16(toString()); };

        /*! Adds the given value to the value. */
        inline void add(const Value &v)
        {
            if ((static_cast<int>(m_type) < 0) || (static_cast<int>(v.m_type) < 0)) {
                if ((m_type == Type::Infinity && v.m_type == Type::NegativeInfinity) || (m_type == Type::NegativeInfinity && v.m_type == Type::Infinity))
                    m_type = Type::NaN;
                else if (m_type == Type::Infinity || v.m_type == Type::Infinity)
                    m_type = Type::Infinity;
                else if (m_type == Type::NegativeInfinity || v.m_type == Type::NegativeInfinity)
                    m_type = Type::NegativeInfinity;
            }
            auto p1 = std::get_if<long>(this);
            auto p2 = std::get_if<long>(&v);
            if (p1 && p2)
                *this = *p1 + *p2;
            else
                *this = toDouble() + v.toDouble();
        }

        inline const Value &operator=(float v)
        {
            m_type = Type::Number;
            ValueVariant::operator=(v);
            return *this;
        }

        inline const Value &operator=(double v)
        {
            m_type = Type::Number;
            ValueVariant::operator=(v);
            return *this;
        }

        inline const Value &operator=(int v)
        {
            m_type = Type::Number;
            ValueVariant::operator=(v);
            return *this;
        }

        inline const Value &operator=(long v)
        {
            m_type = Type::Number;
            ValueVariant::operator=(v);
            return *this;
        }

        inline const Value &operator=(bool v)
        {
            m_type = Type::Bool;
            ValueVariant::operator=(v);
            return *this;
        }

        inline const Value &operator=(const std::string &v)
        {
            m_type = Type::String;
            ValueVariant::operator=(v);
            return *this;
        }

        inline const Value &operator=(const char *v)
        {
            m_type = Type::String;
            ValueVariant::operator=(std::string(v));
            return *this;
        }

    private:
        static bool stringsEqual(std::u16string s1, std::u16string s2);
        static bool stringsEqual(std::string s1, std::string s2);
        static double stringToDouble(const std::string &s, bool *ok = nullptr);
        static long stringToLong(const std::string &s, bool *ok = nullptr);

        Type m_type;

        friend inline bool operator==(const Value &v1, const Value &v2)
        {
            if (v1.m_type == v2.m_type) {
                auto type = v1.m_type;
                switch (type) {
                    case Type::Number:
                        return v1.toDouble() == v2.toDouble();
                    case Type::Bool:
                        return v1.toBool() == v2.toBool();
                    case Type::String:
                        return stringsEqual(v1.toUtf16(), v2.toUtf16());
                    default:
                        if ((static_cast<int>(v1.m_type) < 0) && (static_cast<int>(v2.m_type) < 0)) {
                            if (v1.m_type == Type::NaN || v2.m_type == Type::NaN)
                                return false;
                            else
                                return ((v1.m_type == Type::Infinity && v2.m_type == Type::Infinity) || (v1.m_type == Type::NegativeInfinity && v2.m_type == Type::NegativeInfinity));
                        }
                }
            } else {
                if (v1.isNumber() || v2.isNumber())
                    return v1.toDouble() == v2.toDouble();
                else if (v1.isBool() || v2.isBool())
                    return ((v1.m_type != Type::NaN && v2.m_type != Type::NaN) && (v1.toBool() == v2.toBool()));
                else if (v1.isString() || v2.isString())
                    return stringsEqual(v1.toUtf16(), v2.toUtf16());
                else
                    return false;
            }
            return false;
        }

        friend inline bool operator>(const Value &v1, const Value &v2)
        {
            if ((static_cast<int>(v1.m_type) < 0) || (static_cast<int>(v2.m_type) < 0)) {
                if (v1.m_type == Type::Infinity) {
                    return v2.m_type != Type::Infinity;
                } else if (v1.m_type == Type::NegativeInfinity)
                    return false;
                else if (v2.m_type == Type::Infinity)
                    return false;
                else if (v2.m_type == Type::NegativeInfinity)
                    return true;
            }
            auto p1 = std::get_if<long>(&v1);
            auto p2 = std::get_if<long>(&v2);
            if (p1 && p2)
                return *p1 > *p2;
            else
                return v1.toDouble() > v2.toDouble();
        }

        friend inline bool operator<(const Value &v1, const Value &v2)
        {
            if ((static_cast<int>(v1.m_type) < 0) || (static_cast<int>(v2.m_type) < 0)) {
                if (v1.m_type == Type::Infinity) {
                    return false;
                } else if (v1.m_type == Type::NegativeInfinity)
                    return v2.m_type != Type::NegativeInfinity;
                else if (v2.m_type == Type::Infinity)
                    return v1.m_type != Type::Infinity;
                else if (v2.m_type == Type::NegativeInfinity)
                    return false;
            }
            auto p1 = std::get_if<long>(&v1);
            auto p2 = std::get_if<long>(&v2);
            if (p1 && p2)
                return *p1 < *p2;
            else
                return v1.toDouble() < v2.toDouble();
        }

        friend inline bool operator>=(const Value &v1, const Value &v2) { return v1 > v2 || v1 == v2; }

        friend inline bool operator<=(const Value &v1, const Value &v2) { return v1 < v2 || v1 == v2; }

        friend Value operator+(const Value &v1, const Value &v2)
        {
            if ((static_cast<int>(v1.m_type) < 0) || (static_cast<int>(v2.m_type) < 0)) {
                if ((v1.m_type == Type::Infinity && v2.m_type == Type::NegativeInfinity) || (v1.m_type == Type::NegativeInfinity && v2.m_type == Type::Infinity))
                    return Value(SpecialValue::NaN);
                else if (v1.m_type == Type::Infinity || v2.m_type == Type::Infinity)
                    return Value(SpecialValue::Infinity);
                else if (v1.m_type == Type::NegativeInfinity || v2.m_type == Type::NegativeInfinity)
                    return Value(SpecialValue::NegativeInfinity);
            }
            auto p1 = std::get_if<long>(&v1);
            auto p2 = std::get_if<long>(&v2);
            if (p1 && p2)
                return *p1 + *p2;
            else
                return v1.toDouble() + v2.toDouble();
        }

        friend inline Value operator-(const Value &v1, const Value &v2)
        {
            if ((static_cast<int>(v1.m_type) < 0) || (static_cast<int>(v2.m_type) < 0)) {
                if ((v1.m_type == Type::Infinity && v2.m_type == Type::Infinity) || (v1.m_type == Type::NegativeInfinity && v2.m_type == Type::NegativeInfinity))
                    return Value(SpecialValue::NaN);
                else if (v1.m_type == Type::Infinity || v2.m_type == Type::NegativeInfinity)
                    return Value(SpecialValue::Infinity);
                else if (v1.m_type == Type::NegativeInfinity || v2.m_type == Type::Infinity)
                    return Value(SpecialValue::NegativeInfinity);
            }
            auto p1 = std::get_if<long>(&v1);
            auto p2 = std::get_if<long>(&v2);
            if (p1 && p2)
                return *p1 - *p2;
            else
                return v1.toDouble() - v2.toDouble();
        }

        friend inline Value operator*(const Value &v1, const Value &v2)
        {
            if ((static_cast<int>(v1.m_type) < 0) || (static_cast<int>(v2.m_type) < 0)) {
                if (v1.m_type == Type::Infinity || v1.m_type == Type::NegativeInfinity || v2.m_type == Type::Infinity || v2.m_type == Type::NegativeInfinity) {
                    bool mode = (v1.m_type == Type::Infinity || v2.m_type == Type::Infinity);
                    const Value &value = (v1.m_type == Type::Infinity || v1.m_type == Type::NegativeInfinity) ? v2 : v1;
                    if (value > 0)
                        return Value(mode ? SpecialValue::Infinity : SpecialValue::NegativeInfinity);
                    else if (value < 0)
                        return Value(mode ? SpecialValue::NegativeInfinity : SpecialValue::Infinity);
                    else
                        return Value(SpecialValue::NaN);
                }
            }
            auto p1 = std::get_if<long>(&v1);
            auto p2 = std::get_if<long>(&v2);
            if (p1 && p2)
                return *p1 * *p2;
            else
                return v1.toDouble() * v2.toDouble();
        }

        friend inline Value operator/(const Value &v1, const Value &v2)
        {
            if ((v1 == 0) && (v2 == 0))
                return Value(SpecialValue::NaN);
            else if (v2 == 0) {
                if (v2.m_type == Type::Infinity || v2.m_type == Type::NegativeInfinity) {
                    if (v1.m_type == Type::Infinity || v1.m_type == Type::NegativeInfinity)
                        return Value(SpecialValue::NaN);
                    else
                        return 0;
                } else
                    return Value(v1 > 0 ? SpecialValue::Infinity : SpecialValue::NegativeInfinity);
            }
            return v1.toDouble() / v2.toDouble();
        }

        friend inline Value operator%(const Value &v1, const Value &v2)
        {

            if ((v2 == 0) || (v1.m_type == Type::Infinity || v1.m_type == Type::NegativeInfinity))
                return Value(SpecialValue::NaN);
            else if (v2.m_type == Type::Infinity || v2.m_type == Type::NegativeInfinity) {
                return v1.toDouble();
            }
            return fmod(v1.toDouble(), v2.toDouble());
        }
};

} // namespace libscratchcpp
