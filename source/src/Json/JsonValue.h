/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   JsonValue.h
 *
 * @remark
 *
 */

#ifndef __JSON_VALUE_H__
#define __JSON_VALUE_H__

#include <tiny_base.h>

TINY_BEGIN_DECLS

struct _JsonBoolean;
typedef struct _JsonBoolean JsonBoolean;

struct _JsonString;
typedef struct _JsonString JsonString;

struct _JsonNumber;
typedef struct _JsonNumber JsonNumber;

struct _JsonObject;
typedef struct _JsonObject JsonObject;

struct _JsonArray;
typedef struct _JsonArray JsonArray;

typedef enum _JsonValueType
{
    JSON_UNDEFINED      = 0,
    JSON_STRING         = 1,
    JSON_NUMBER         = 2,
    JSON_OBJECT         = 3,
    JSON_ARRAY          = 4,
    JSON_BOOLEAN        = 5,
    JSON_NULL           = 6,
} JsonValueType;

typedef union _JsonValueData
{
    JsonString    * string;
    JsonNumber    * number;
    JsonObject    * object;
    JsonArray     * array;
    JsonBoolean   * boolean;
} JsonValueData;

typedef struct _JsonValue
{
    JsonValueType   type;
    JsonValueData   data;
} JsonValue;

TINY_LOR
TINY_API
JsonValue * JsonValue_New(void);

TINY_LOR
TINY_API
JsonValue * JsonValue_NewString(const char *value);

TINY_LOR
TINY_API
JsonValue * JsonValue_NewInteger(long value);

TINY_LOR
TINY_API
JsonValue * JsonValue_NewFloat(float value);

TINY_LOR
TINY_API
JsonValue * JsonValue_NewBoolean(bool value);

TINY_LOR
TINY_API
JsonValue * JsonValue_NewNull(void);

TINY_LOR
TINY_API
JsonValue * JsonValue_NewValue(JsonValueType type, void *value);

TINY_LOR
TINY_API
JsonValue * JsonValue_Copy(JsonValue *other);

TINY_LOR
TINY_API
void JsonValue_Delete(JsonValue *thiz);


TINY_END_DECLS

#endif /* __JSON_VALUE_H__ */