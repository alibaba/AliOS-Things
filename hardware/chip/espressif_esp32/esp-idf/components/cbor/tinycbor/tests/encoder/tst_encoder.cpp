/****************************************************************************
**
** Copyright (C) 2016 Intel Corporation
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
** THE SOFTWARE.
**
****************************************************************************/

#include <QtTest>
#include "cbor.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
#include <qfloat16.h>
#endif

Q_DECLARE_METATYPE(CborError)
namespace QTest {
template<> char *toString<CborError>(const CborError &err)
{
    return qstrdup(cbor_error_string(err));
}
}

class tst_Encoder : public QObject
{
    Q_OBJECT
private slots:
    void fixed_data();
    void fixed();
    void strings_data();
    void strings() { fixed(); }
    void arraysAndMaps_data();
    void arraysAndMaps() { fixed(); }
    void tags_data();
    void tags();
    void arrays_data() { tags_data(); }
    void arrays();
    void maps_data() { tags_data(); }
    void maps();

    void shortBuffer_data() { tags_data(); }
    void shortBuffer();
    void tooShortArrays_data() { tags_data(); }
    void tooShortArrays();
    void tooShortMaps_data() { tags_data(); }
    void tooShortMaps();
    void tooBigArrays_data() { tags_data(); }
    void tooBigArrays();
    void tooBigMaps_data() { tags_data(); }
    void tooBigMaps();
    void illegalSimpleType_data();
    void illegalSimpleType();
};

#include "tst_encoder.moc"

static float myNaNf()
{
    uint32_t v = 0x7fc00000;
    float f;
    memcpy(&f, &v, sizeof(f));
    Q_ASSERT(qIsNaN(f));
    return f;
}

static float myInff()
{
    uint32_t v = 0x7f800000;
    float f;
    memcpy(&f, &v, sizeof(f));
    Q_ASSERT(qIsInf(f));
    return f;
}

static float myNInff()
{
    uint32_t v = 0xff800000;
    float f;
    memcpy(&f, &v, sizeof(f));
    Q_ASSERT(qIsInf(f));
    return f;
}

static double myNaN()
{
    uint64_t v = UINT64_C(0x7ff8000000000000);
    double f;
    memcpy(&f, &v, sizeof(f));
    Q_ASSERT(qIsNaN(f));
    return f;
}

static double myInf()
{
    uint64_t v = UINT64_C(0x7ff0000000000000);
    double f;
    memcpy(&f, &v, sizeof(f));
    Q_ASSERT(qIsInf(f));
    return f;
}

static double myNInf()
{
    uint64_t v = UINT64_C(0xfff0000000000000);
    double f;
    memcpy(&f, &v, sizeof(f));
    Q_ASSERT(qIsInf(f));
    return f;
}

template <size_t N> QByteArray raw(const char (&data)[N])
{
    return QByteArray::fromRawData(data, N - 1);
}

struct NegativeInteger { quint64 abs; };
Q_DECLARE_METATYPE(NegativeInteger)

struct SimpleType { uint8_t type; };
Q_DECLARE_METATYPE(SimpleType)

struct Float16Standin { uint16_t val; };
Q_DECLARE_METATYPE(Float16Standin)

struct Tag { CborTag tag; QVariant tagged; };
Q_DECLARE_METATYPE(Tag)

template <typename... Args>
QVariant make_list(const Args &... args)
{
    return QVariantList{args...};
}

typedef QVector<QPair<QVariant, QVariant>> Map;
Q_DECLARE_METATYPE(Map)
QVariant make_map(const std::initializer_list<QPair<QVariant, QVariant>> &list)
{
    return QVariant::fromValue(Map(list));
}

struct IndeterminateLengthArray : QVariantList { using QVariantList::QVariantList; };
struct IndeterminateLengthMap : Map { using Map::Map; };
Q_DECLARE_METATYPE(IndeterminateLengthArray)
Q_DECLARE_METATYPE(IndeterminateLengthMap)

QVariant make_ilarray(const std::initializer_list<QVariant> &list)
{
    return QVariant::fromValue(IndeterminateLengthArray(list));
}

QVariant make_ilmap(const std::initializer_list<QPair<QVariant, QVariant>> &list)
{
    return QVariant::fromValue(IndeterminateLengthMap(list));
}

static inline bool isOomError(CborError err)
{
    return err == CborErrorOutOfMemory;
}

CborError encodeVariant(CborEncoder *encoder, const QVariant &v)
{
    int type = v.userType();
    switch (type) {
    case QVariant::Int:
    case QVariant::LongLong:
        return cbor_encode_int(encoder, v.toLongLong());

    case QVariant::UInt:
    case QVariant::ULongLong:
        return cbor_encode_uint(encoder, v.toULongLong());

    case QVariant::Bool:
        return cbor_encode_boolean(encoder, v.toBool());

    case QVariant::Invalid:
        return cbor_encode_undefined(encoder);

    case QMetaType::VoidStar:
        return cbor_encode_null(encoder);

    case QVariant::Double:
        return cbor_encode_double(encoder, v.toDouble());

    case QMetaType::Float:
        return cbor_encode_float(encoder, v.toFloat());

    case QVariant::String: {
        QByteArray string = v.toString().toUtf8();
        return cbor_encode_text_string(encoder, string.constData(), string.length());
    }

    case QVariant::ByteArray: {
        QByteArray string = v.toByteArray();
        return cbor_encode_byte_string(encoder, reinterpret_cast<const quint8 *>(string.constData()), string.length());
    }

    default:
        if (type == qMetaTypeId<NegativeInteger>())
            return cbor_encode_negative_int(encoder, v.value<NegativeInteger>().abs);
        if (type == qMetaTypeId<SimpleType>())
            return cbor_encode_simple_value(encoder, v.value<SimpleType>().type);
#if QT_VERSION < QT_VERSION_CHECK(5, 9, 0)
        if (type == qMetaTypeId<Float16Standin>())
            return cbor_encode_half_float(encoder, v.constData());
#else
        if (type == qMetaTypeId<qfloat16>())
            return cbor_encode_half_float(encoder, v.constData());
#endif
        if (type == qMetaTypeId<Tag>()) {
            CborError err = cbor_encode_tag(encoder, v.value<Tag>().tag);
            if (err && !isOomError(err))
                return err;
            return static_cast<CborError>(err | encodeVariant(encoder, v.value<Tag>().tagged));
        }
        if (type == QVariant::List || type == qMetaTypeId<IndeterminateLengthArray>()) {
            CborEncoder sub;
            QVariantList list = v.toList();
            size_t len = list.length();
            if (type == qMetaTypeId<IndeterminateLengthArray>()) {
                len = CborIndefiniteLength;
                list = v.value<IndeterminateLengthArray>();
            }
            CborError err = cbor_encoder_create_array(encoder, &sub, len);
            if (err && !isOomError(err))
                return err;
            foreach (const QVariant &v2, list) {
                err = static_cast<CborError>(err | encodeVariant(&sub, v2));
                if (err && !isOomError(err))
                    return err;
            }
            return cbor_encoder_close_container_checked(encoder, &sub);
        }
        if (type == qMetaTypeId<Map>() || type == qMetaTypeId<IndeterminateLengthMap>()) {
            CborEncoder sub;
            Map map = v.value<Map>();
            size_t len = map.length();
            if (type == qMetaTypeId<IndeterminateLengthMap>()) {
                len = CborIndefiniteLength;
                map = v.value<IndeterminateLengthMap>();
            }
            CborError err = cbor_encoder_create_map(encoder, &sub, len);
            if (err && !isOomError(err))
                return err;
            for (auto pair : map) {
                err = static_cast<CborError>(err | encodeVariant(&sub, pair.first));
                if (err && !isOomError(err))
                    return err;
                err = static_cast<CborError>(err | encodeVariant(&sub, pair.second));
                if (err && !isOomError(err))
                    return err;
            }
            return cbor_encoder_close_container_checked(encoder, &sub);
        }
    }
    return CborErrorUnknownType;
}

void compare(const QVariant &input, const QByteArray &output)
{
    QByteArray buffer(output.length(), Qt::Uninitialized);
    uint8_t *bufptr = reinterpret_cast<quint8 *>(buffer.data());
    CborEncoder encoder;
    cbor_encoder_init(&encoder, bufptr, buffer.length(), 0);

    QCOMPARE(encodeVariant(&encoder, input), CborNoError);
    QCOMPARE(encoder.remaining, size_t(1));
    QCOMPARE(cbor_encoder_get_extra_bytes_needed(&encoder), size_t(0));

    buffer.resize(int(cbor_encoder_get_buffer_size(&encoder, bufptr)));
    QCOMPARE(buffer, output);
}

void addColumns()
{
    QTest::addColumn<QByteArray>("output");
    QTest::addColumn<QVariant>("input");
}

void addFixedData()
{
    // unsigned integers
    QTest::newRow("0U") << raw("\x00") << QVariant(0U);
    QTest::newRow("1U") << raw("\x01") << QVariant(1U);
    QTest::newRow("10U") << raw("\x0a") << QVariant(10U);
    QTest::newRow("23U") << raw("\x17") << QVariant(23U);
    QTest::newRow("24U") << raw("\x18\x18") << QVariant(24U);
    QTest::newRow("255U") << raw("\x18\xff") << QVariant(255U);
    QTest::newRow("256U") << raw("\x19\x01\x00") << QVariant(256U);
    QTest::newRow("65535U") << raw("\x19\xff\xff") << QVariant(65535U);
    QTest::newRow("65536U") << raw("\x1a\0\1\x00\x00") << QVariant(65536U);
    QTest::newRow("4294967295U") << raw("\x1a\xff\xff\xff\xff") << QVariant(4294967295U);
    QTest::newRow("4294967296U") << raw("\x1b\0\0\0\1\0\0\0\0") << QVariant(Q_UINT64_C(4294967296));
    QTest::newRow("UINT64_MAX") << raw("\x1b" "\xff\xff\xff\xff" "\xff\xff\xff\xff")
                                << QVariant(std::numeric_limits<quint64>::max());

    // signed integers containing non-negative numbers
    QTest::newRow("0") << raw("\x00") << QVariant(0);
    QTest::newRow("1") << raw("\x01") << QVariant(1);
    QTest::newRow("10") << raw("\x0a") << QVariant(10);
    QTest::newRow("23") << raw("\x17") << QVariant(23);
    QTest::newRow("24") << raw("\x18\x18") << QVariant(24);
    QTest::newRow("255") << raw("\x18\xff") << QVariant(255);
    QTest::newRow("256") << raw("\x19\x01\x00") << QVariant(256);
    QTest::newRow("65535") << raw("\x19\xff\xff") << QVariant(65535);
    QTest::newRow("65536") << raw("\x1a\0\1\x00\x00") << QVariant(65536);
    QTest::newRow("4294967295") << raw("\x1a\xff\xff\xff\xff") << QVariant(Q_INT64_C(4294967295));
    QTest::newRow("4294967296") << raw("\x1b\0\0\0\1\0\0\0\0") << QVariant(Q_INT64_C(4294967296));

    // signed integers containing negative numbers
    QTest::newRow("-1") << raw("\x20") << QVariant(-1);
    QTest::newRow("-2") << raw("\x21") << QVariant(-2);
    QTest::newRow("-24") << raw("\x37") << QVariant(-24);
    QTest::newRow("-25") << raw("\x38\x18") << QVariant(-25);
    QTest::newRow("-UINT8_MAX") << raw("\x38\xff") << QVariant(-256);
    QTest::newRow("-UINT8_MAX-1") << raw("\x39\x01\x00") << QVariant(-257);
    QTest::newRow("-UINT16_MAX") << raw("\x39\xff\xff") << QVariant(-65536);
    QTest::newRow("-UINT16_MAX-1") << raw("\x3a\0\1\x00\x00") << QVariant(-65537);
    QTest::newRow("-UINT32_MAX") << raw("\x3a\xff\xff\xff\xff") << QVariant(Q_INT64_C(-4294967296));
    QTest::newRow("-UINT32_MAX-1") << raw("\x3b\0\0\0\1\0\0\0\0") << QVariant(Q_INT64_C(-4294967297));

    // negative integers
    auto neg = [](quint64 v) { return QVariant::fromValue<NegativeInteger>({v}); };
    QTest::newRow("negative1") << raw("\x20") << neg(1);
    QTest::newRow("negative2") << raw("\x21") << neg(2);
    QTest::newRow("negative24") << raw("\x37") << neg(24);
    QTest::newRow("negative25") << raw("\x38\x18") << neg(25);
    QTest::newRow("negativeUINT8_MAX") << raw("\x38\xff") << neg(256);
    QTest::newRow("negativeUINT8_MAX-1") << raw("\x39\x01\x00") << neg(257);
    QTest::newRow("negativeUINT16_MAX") << raw("\x39\xff\xff") << neg(65536);
    QTest::newRow("negativeUINT16_MAX-1") << raw("\x3a\0\1\x00\x00") << neg(65537);
    QTest::newRow("negativeUINT32_MAX") << raw("\x3a\xff\xff\xff\xff") << neg(Q_UINT64_C(4294967296));
    QTest::newRow("negativeUINT32_MAX-1") << raw("\x3b\0\0\0\1\0\0\0\0") << neg(Q_UINT64_C(4294967297));
    QTest::newRow("negativeUINT64_MAX") << raw("\x3b" "\xff\xff\xff\xff" "\xff\xff\xff\xfe")
                                        << neg(std::numeric_limits<quint64>::max());
    QTest::newRow("negativeUINT64_MAX+1") << raw("\x3b" "\xff\xff\xff\xff" "\xff\xff\xff\xff") << neg(0);

    QTest::newRow("simple0") << raw("\xe0") << QVariant::fromValue(SimpleType{0});
    QTest::newRow("simple19") << raw("\xf3") << QVariant::fromValue(SimpleType{19});
    QTest::newRow("false") << raw("\xf4") << QVariant(false);
    QTest::newRow("true") << raw("\xf5") << QVariant(true);
    QTest::newRow("null") << raw("\xf6") << QVariant::fromValue<void *>(nullptr);
    QTest::newRow("undefined") << raw("\xf7") << QVariant();
    QTest::newRow("simple32") << raw("\xf8\x20") << QVariant::fromValue(SimpleType{32});
    QTest::newRow("simple255") << raw("\xf8\xff") << QVariant::fromValue(SimpleType{255});

    // floating point
#if QT_VERSION < QT_VERSION_CHECK(5, 9, 0)
    QTest::newRow("0.f16") << raw("\xf9\0\0") << QVariant::fromValue(Float16Standin{0x0000});
#else
    QTest::newRow("0.f16") << raw("\xf9\0\0") << QVariant::fromValue(qfloat16(0));
    QTest::newRow("-1.f16") << raw("\xf9\xbc\0") << QVariant::fromValue(qfloat16(-1));
    QTest::newRow("1.5f16") << raw("\xf9\x3e\0") << QVariant::fromValue(qfloat16(1.5));
    QTest::newRow("nan_f16") << raw("\xf9\x7e\0") << QVariant::fromValue<qfloat16>(myNaNf());
    QTest::newRow("-inf_f16") << raw("\xf9\xfc\0") << QVariant::fromValue<qfloat16>(myNInff());
    QTest::newRow("+inf_f16") << raw("\xf9\x7c\0") << QVariant::fromValue<qfloat16>(myInff());
#endif

    QTest::newRow("0.f") << raw("\xfa\0\0\0\0") << QVariant::fromValue(0.f);
    QTest::newRow("0.")  << raw("\xfb\0\0\0\0\0\0\0\0") << QVariant(0.);
    QTest::newRow("-1.f") << raw("\xfa\xbf\x80\0\0") << QVariant::fromValue(-1.f);
    QTest::newRow("-1.") << raw("\xfb\xbf\xf0\0\0\0\0\0\0") << QVariant(-1.);
    QTest::newRow("16777215.f") << raw("\xfa\x4b\x7f\xff\xff") << QVariant::fromValue(16777215.f);
    QTest::newRow("16777215.") << raw("\xfb\x41\x6f\xff\xff\xe0\0\0\0") << QVariant::fromValue(16777215.);
    QTest::newRow("-16777215.f") << raw("\xfa\xcb\x7f\xff\xff") << QVariant(-16777215.f);
    QTest::newRow("-16777215.") << raw("\xfb\xc1\x6f\xff\xff\xe0\0\0\0") << QVariant::fromValue(-16777215.);

    QTest::newRow("nan_f") << raw("\xfa\x7f\xc0\0\0") << QVariant::fromValue<float>(myNaNf());
    QTest::newRow("nan") << raw("\xfb\x7f\xf8\0\0\0\0\0\0") << QVariant(myNaN());
    QTest::newRow("-inf_f") << raw("\xfa\xff\x80\0\0") << QVariant::fromValue<float>(myNInff());
    QTest::newRow("-inf") << raw("\xfb\xff\xf0\0\0\0\0\0\0") << QVariant(myNInf());
    QTest::newRow("+inf_f") << raw("\xfa\x7f\x80\0\0") << QVariant::fromValue<float>(myInff());
    QTest::newRow("+inf") << raw("\xfb\x7f\xf0\0\0\0\0\0\0") << QVariant(myInf());
}

void addStringsData()
{
    // byte strings
    QTest::newRow("emptybytestring") << raw("\x40") << QVariant(QByteArray(""));
    QTest::newRow("bytestring1") << raw("\x41 ") << QVariant(QByteArray(" "));
    QTest::newRow("bytestring1-nul") << raw("\x41\0") << QVariant(QByteArray("", 1));
    QTest::newRow("bytestring5") << raw("\x45Hello") << QVariant(QByteArray("Hello"));
    QTest::newRow("bytestring24") << raw("\x58\x18""123456789012345678901234")
                                  << QVariant(QByteArray("123456789012345678901234"));
    QTest::newRow("bytestring256") << raw("\x59\1\0") + QByteArray(256, '3')
                                   << QVariant(QByteArray(256, '3'));

    // text strings
    QTest::newRow("emptytextstring") << raw("\x60") << QVariant("");
    QTest::newRow("textstring1") << raw("\x61 ") << QVariant(" ");
    QTest::newRow("textstring1-nul") << raw("\x61\0") << QVariant(QString::fromLatin1("", 1));
    QTest::newRow("textstring5") << raw("\x65Hello") << QVariant("Hello");
    QTest::newRow("textstring24") << raw("\x78\x18""123456789012345678901234")
                                  << QVariant("123456789012345678901234");
    QTest::newRow("textstring256") << raw("\x79\1\0") + QByteArray(256, '3')
                                   << QVariant(QString(256, '3'));
}

void addArraysAndMaps()
{
    QTest::newRow("emptyarray") << raw("\x80") << make_list();
    QTest::newRow("emptymap") << raw("\xa0") << make_map({});

    QTest::newRow("array-0") << raw("\x81\0") << make_list(0);
    QTest::newRow("array-{0-0}") << raw("\x82\0\0") << make_list(0, 0);
    QTest::newRow("array-Hello") << raw("\x81\x65Hello") << make_list("Hello");
    QTest::newRow("array-array-0") << raw("\x81\x81\0") << make_list(make_list(0));
    QTest::newRow("array-array-{0-0}") << raw("\x81\x82\0\0") << make_list(make_list(0, 0));
    QTest::newRow("array-array-0-0") << raw("\x82\x81\0\0") << make_list(make_list(0),0);
    QTest::newRow("array-array-Hello") << raw("\x81\x81\x65Hello") << make_list(make_list("Hello"));

    QTest::newRow("map-0:0") << raw("\xa1\0\0") << make_map({{0,0}});
    QTest::newRow("map-0:0-1:1") << raw("\xa2\0\0\1\1") << make_map({{0,0}, {1,1}});
    QTest::newRow("map-0:{map-0:0-1:1}") << raw("\xa1\0\xa2\0\0\1\1") << make_map({{0, make_map({{0,0}, {1,1}})}});

    QTest::newRow("array-map1") << raw("\x81\xa1\0\0") << make_list(make_map({{0,0}}));
    QTest::newRow("array-map2") << raw("\x82\xa1\0\0\xa1\1\1") << make_list(make_map({{0,0}}), make_map({{1,1}}));

    QTest::newRow("map-array1") << raw("\xa1\x62oc\x81\0") << make_map({{"oc", make_list(0)}});
    QTest::newRow("map-array2") << raw("\xa1\x62oc\x84\0\1\2\3") << make_map({{"oc", make_list(0, 1, 2, 3)}});
    QTest::newRow("map-array3") << raw("\xa2\x62oc\x82\0\1\2\3") << make_map({{"oc", make_list(0, 1)}, {2, 3}});

    // indeterminate length
    QTest::newRow("_emptyarray") << raw("\x9f\xff") << QVariant::fromValue(IndeterminateLengthArray{});
    QTest::newRow("_emptymap") << raw("\xbf\xff") << make_ilmap({});

    QTest::newRow("_array-0") << raw("\x9f\0\xff") << make_ilarray({0});
    QTest::newRow("_array-{0-0}") << raw("\x9f\0\0\xff") << make_ilarray({0, 0});
    QTest::newRow("_array-Hello") << raw("\x9f\x65Hello\xff") << make_ilarray({"Hello"});
    QTest::newRow("_array-array-0") << raw("\x9f\x81\0\xff") << make_ilarray({make_list(0)});
    QTest::newRow("_array-_array-0") << raw("\x9f\x9f\0\xff\xff") << make_ilarray({make_ilarray({0})});
    QTest::newRow("_array-_array-{0-0}") << raw("\x9f\x9f\0\0\xff\xff") << make_ilarray({make_ilarray({0, 0})});
    QTest::newRow("_array-_array-0-0") << raw("\x9f\x9f\0\xff\0\xff") << make_ilarray({make_ilarray({0}),0});
    QTest::newRow("_array-_array-Hello") << raw("\x9f\x9f\x65Hello\xff\xff") << make_ilarray({make_ilarray({"Hello"})});

    QTest::newRow("_map-0:0") << raw("\xbf\0\0\xff") << make_ilmap({{0,0}});
    QTest::newRow("_map-0:0-1:1") << raw("\xbf\0\0\1\1\xff") << make_ilmap({{0,0}, {1,1}});
    QTest::newRow("_map-0:{map-0:0-1:1}") << raw("\xbf\0\xa2\0\0\1\1\xff") << make_ilmap({{0, make_map({{0,0}, {1,1}})}});
    QTest::newRow("_map-0:{_map-0:0-1:1}") << raw("\xbf\0\xbf\0\0\1\1\xff\xff") << make_ilmap({{0, make_ilmap({{0,0}, {1,1}})}});

    QTest::newRow("_array-map1") << raw("\x9f\xa1\0\0\xff") << make_ilarray({make_map({{0,0}})});
    QTest::newRow("_array-_map1") << raw("\x9f\xbf\0\0\xff\xff") << make_ilarray({make_ilmap({{0,0}})});
    QTest::newRow("_array-map2") << raw("\x9f\xa1\0\0\xa1\1\1\xff") << make_ilarray({make_map({{0,0}}), make_map({{1,1}})});
    QTest::newRow("_array-_map2") << raw("\x9f\xbf\0\0\xff\xbf\1\1\xff\xff") << make_ilarray({make_ilmap({{0,0}}), make_ilmap({{1,1}})});

    QTest::newRow("_map-array1") << raw("\xbf\x62oc\x81\0\xff") << make_ilmap({{"oc", make_list(0)}});
    QTest::newRow("_map-_array1") << raw("\xbf\x62oc\x9f\0\xff\xff") << make_ilmap({{"oc", make_ilarray({0})}});
    QTest::newRow("_map-array2") << raw("\xbf\x62oc\x84\0\1\2\3\xff") << make_ilmap({{"oc", make_list(0, 1, 2, 3)}});
    QTest::newRow("_map-_array2") << raw("\xbf\x62oc\x9f\0\1\2\3\xff\xff") << make_ilmap({{"oc", make_ilarray({0, 1, 2, 3})}});
    QTest::newRow("_map-array3") << raw("\xbf\x62oc\x82\0\1\2\3\xff") << make_ilmap({{"oc", make_list(0, 1)}, {2, 3}});
    QTest::newRow("_map-_array3") << raw("\xbf\x62oc\x9f\0\1\xff\2\3\xff") << make_ilmap({{"oc", make_ilarray({0, 1})}, {2, 3}});

    // tagged
    QTest::newRow("array-1(0)") << raw("\x81\xc1\0") << make_list(QVariant::fromValue(Tag{1, 0}));
    QTest::newRow("array-1(map)") << raw("\x81\xc1\xa0") << make_list(QVariant::fromValue(Tag{1, make_map({})}));
    QTest::newRow("map-1(2):3(4)") << raw("\xa1\xc1\2\xc3\4") << make_map({{QVariant::fromValue(Tag{1, 2}), QVariant::fromValue(Tag{3, 4})}});
}

void tst_Encoder::fixed_data()
{
    addColumns();
    addFixedData();
}

void tst_Encoder::fixed()
{
    QFETCH(QVariant, input);
    QFETCH(QByteArray, output);
    compare(input, output);
}

void tst_Encoder::strings_data()
{
    addColumns();
    addStringsData();
}

void tst_Encoder::arraysAndMaps_data()
{
    addColumns();
    addArraysAndMaps();
}

void tst_Encoder::tags_data()
{
    addColumns();
    addFixedData();
    addStringsData();
    addArraysAndMaps();
}

void tst_Encoder::tags()
{
    QFETCH(QVariant, input);
    QFETCH(QByteArray, output);

    compare(QVariant::fromValue(Tag{1, input}), "\xc1" + output);
    if (QTest::currentTestFailed()) return;

    compare(QVariant::fromValue(Tag{24, input}), "\xd8\x18" + output);
    if (QTest::currentTestFailed()) return;

    compare(QVariant::fromValue(Tag{255, input}), "\xd8\xff" + output);
    if (QTest::currentTestFailed()) return;

    compare(QVariant::fromValue(Tag{256, input}), raw("\xd9\1\0") + output);
    if (QTest::currentTestFailed()) return;

    compare(QVariant::fromValue(Tag{CborSignatureTag, input}), raw("\xd9\xd9\xf7") + output);
    if (QTest::currentTestFailed()) return;

    compare(QVariant::fromValue(Tag{65535, input}), raw("\xd9\xff\xff") + output);
    if (QTest::currentTestFailed()) return;

    compare(QVariant::fromValue(Tag{65536, input}), raw("\xda\0\1\0\0") + output);
    if (QTest::currentTestFailed()) return;

    compare(QVariant::fromValue(Tag{UINT32_MAX, input}), raw("\xda\xff\xff\xff\xff") + output);
    if (QTest::currentTestFailed()) return;

    compare(QVariant::fromValue(Tag{UINT32_MAX + Q_UINT64_C(1), input}), raw("\xdb\0\0\0\1\0\0\0\0") + output);
    if (QTest::currentTestFailed()) return;

    compare(QVariant::fromValue(Tag{UINT64_MAX, input}), raw("\xdb\xff\xff\xff\xff\xff\xff\xff\xff") + output);
    if (QTest::currentTestFailed()) return;

    // nested tags
    compare(QVariant::fromValue(Tag{1, QVariant::fromValue(Tag{1, input})}), "\xc1\xc1" + output);
}

void tst_Encoder::arrays()
{
    QFETCH(QVariant, input);
    QFETCH(QByteArray, output);

    compare(make_list(input), "\x81" + output);
    if (QTest::currentTestFailed()) return;

    compare(make_list(input, input), "\x82" + output + output);
    if (QTest::currentTestFailed()) return;

    {
        QVariantList list{input};
        QByteArray longoutput = output;

        // make a list with 32 elements (1 << 5)
        for (int i = 0; i < 5; ++i) {
            list += list;
            longoutput += longoutput;
        }
        compare(list, "\x98\x20" + longoutput);
        if (QTest::currentTestFailed()) return;

        // now 256 elements (32 << 3)
        for (int i = 0; i < 3; ++i) {
            list += list;
            longoutput += longoutput;
        }
        compare(list, raw("\x99\1\0") + longoutput);
        if (QTest::currentTestFailed()) return;
    }

    // nested lists
    compare(make_list(make_list(input)), "\x81\x81" + output);
    if (QTest::currentTestFailed()) return;

    compare(make_list(make_list(input, input)), "\x81\x82" + output + output);
    if (QTest::currentTestFailed()) return;

    compare(make_list(make_list(input), input), "\x82\x81" + output + output);
    if (QTest::currentTestFailed()) return;

    compare(make_list(make_list(input), make_list(input)), "\x82\x81" + output + "\x81" + output);
}

void tst_Encoder::maps()
{
    QFETCH(QVariant, input);
    QFETCH(QByteArray, output);

    compare(make_map({{1, input}}), "\xa1\1" + output);
    if (QTest::currentTestFailed()) return;

    compare(make_map({{1, input}, {input, 24}}), "\xa2\1" + output + output + "\x18\x18");
    if (QTest::currentTestFailed()) return;

    compare(make_map({{input, input}}), "\xa1" + output + output);
    if (QTest::currentTestFailed()) return;

    {
        Map map{{1, input}};
        QByteArray longoutput = "\1" + output;

        // make a map with 32 elements (1 << 5)
        for (int i = 0; i < 5; ++i) {
            map += map;
            longoutput += longoutput;
        }
        compare(QVariant::fromValue(map), "\xb8\x20" + longoutput);
        if (QTest::currentTestFailed()) return;

        // now 256 elements (32 << 3)
        for (int i = 0; i < 3; ++i) {
            map += map;
            longoutput += longoutput;
        }
        compare(QVariant::fromValue(map), raw("\xb9\1\0") + longoutput);
        if (QTest::currentTestFailed()) return;
    }

    // nested maps
    compare(make_map({{1, make_map({{2, input}})}}), "\xa1\1\xa1\2" + output);
    if (QTest::currentTestFailed()) return;

    compare(make_map({{1, make_map({{2, input}, {input, false}})}}), "\xa1\1\xa2\2" + output + output + "\xf4");
    if (QTest::currentTestFailed()) return;

    compare(make_map({{1, make_map({{2, input}})}, {input, false}}), "\xa2\1\xa1\2" + output + output + "\xf4");
    if (QTest::currentTestFailed()) return;
}

void tst_Encoder::shortBuffer()
{
    QFETCH(QVariant, input);
    QFETCH(QByteArray, output);
    QByteArray buffer(output.length(), Qt::Uninitialized);

    for (int len = 0; len < output.length(); ++len) {
        CborEncoder encoder;
        cbor_encoder_init(&encoder, reinterpret_cast<quint8 *>(buffer.data()), len, 0);
        QCOMPARE(encodeVariant(&encoder, input), CborErrorOutOfMemory);
        QVERIFY(cbor_encoder_get_extra_bytes_needed(&encoder) != 0);
        QCOMPARE(len + cbor_encoder_get_extra_bytes_needed(&encoder), size_t(output.length()));
    }
}

void tst_Encoder::tooShortArrays()
{
    QFETCH(QVariant, input);
    QFETCH(QByteArray, output);
    QByteArray buffer(output.length() + 1, Qt::Uninitialized);

    CborEncoder encoder, container;
    cbor_encoder_init(&encoder, reinterpret_cast<quint8 *>(buffer.data()), buffer.length(), 0);
    QCOMPARE(cbor_encoder_create_array(&encoder, &container, 2), CborNoError);
    QCOMPARE(encodeVariant(&container, input), CborNoError);
    QCOMPARE(container.remaining, size_t(2));
    QCOMPARE(cbor_encoder_close_container_checked(&encoder, &container), CborErrorTooFewItems);
}

void tst_Encoder::tooShortMaps()
{
    QFETCH(QVariant, input);
    QFETCH(QByteArray, output);
    QByteArray buffer(output.length() + 1, Qt::Uninitialized);

    CborEncoder encoder, container;
    cbor_encoder_init(&encoder, reinterpret_cast<quint8 *>(buffer.data()), buffer.length(), 0);
    QCOMPARE(cbor_encoder_create_map(&encoder, &container, 2), CborNoError);
    QCOMPARE(encodeVariant(&container, input), CborNoError);
    QCOMPARE(container.remaining, size_t(4));
    QCOMPARE(cbor_encoder_close_container_checked(&encoder, &container), CborErrorTooFewItems);
}

void tst_Encoder::tooBigArrays()
{
    QFETCH(QVariant, input);
    QFETCH(QByteArray, output);
    QByteArray buffer(output.length() * 2 + 1, Qt::Uninitialized);

    CborEncoder encoder, container;
    cbor_encoder_init(&encoder, reinterpret_cast<quint8 *>(buffer.data()), buffer.length(), 0);
    QCOMPARE(cbor_encoder_create_array(&encoder, &container, 1), CborNoError);
    QCOMPARE(encodeVariant(&container, input), CborNoError);
    QCOMPARE(encodeVariant(&container, input), CborNoError);
    QCOMPARE(container.remaining, size_t(0));
    QCOMPARE(cbor_encoder_close_container_checked(&encoder, &container), CborErrorTooManyItems);
}

void tst_Encoder::tooBigMaps()
{
    QFETCH(QVariant, input);
    QFETCH(QByteArray, output);
    QByteArray buffer(output.length() * 3 + 1, Qt::Uninitialized);

    CborEncoder encoder, container;
    cbor_encoder_init(&encoder, reinterpret_cast<quint8 *>(buffer.data()), buffer.length(), 0);
    QCOMPARE(cbor_encoder_create_map(&encoder, &container, 1), CborNoError);
    QCOMPARE(encodeVariant(&container, input), CborNoError);
    QCOMPARE(encodeVariant(&container, input), CborNoError);
    QCOMPARE(encodeVariant(&container, input), CborNoError);
    QCOMPARE(container.remaining, size_t(0));
    QCOMPARE(cbor_encoder_close_container_checked(&encoder, &container), CborErrorTooManyItems);
}

void tst_Encoder::illegalSimpleType_data()
{
    QTest::addColumn<int>("type");
    QTest::newRow("half-float") << 25;
    QTest::newRow("float") << 26;
    QTest::newRow("double") << 27;
    QTest::newRow("28") << 28;
    QTest::newRow("29") << 29;
    QTest::newRow("30") << 30;
    QTest::newRow("31") << 31;
}

void tst_Encoder::illegalSimpleType()
{
    QFETCH(int, type);

    quint8 buf[2];
    CborEncoder encoder;
    cbor_encoder_init(&encoder, buf, sizeof(buf), 0);
    QCOMPARE(cbor_encode_simple_value(&encoder, type), CborErrorIllegalSimpleType);
}

QTEST_MAIN(tst_Encoder)
