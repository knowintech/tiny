/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   WebSocketFrame.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __WEB_SOCKET_FRAME_H__
#define __WEB_SOCKET_FRAME_H__

#include <tiny_base.h>

TINY_BEGIN_DECLS


/**
 * 0                   1                   2                   3
 * 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-------+-+-------------+-------------------------------+
 * |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
 * |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
 * |N|V|V|V|       |S|             |   (if payload len==126/127)   |
 * | |1|2|3|       |K|             |                               |
 * +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
 * |     Extended payload length continued, if payload len == 127  |
 * + - - - - - - - - - - - - - - - +-------------------------------+
 * |                               |Masking-key, if MASK set to 1  |
 * +-------------------------------+-------------------------------+
 * | Masking-key (continued)       |          Payload Data         |
 * +-------------------------------- - - - - - - - - - - - - - - - +
 * :                     Payload Data continued ...                :
 * + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
 * |                     Payload Data continued ...                |
 * +---------------------------------------------------------------+
 */

typedef struct _WebSocketFrameHeader
{
    uint8_t        Opcode:4;
    uint8_t        RSV3:1;
    uint8_t        RSV2:1;
    uint8_t        RSV1:1;
    uint8_t        FIN:1;
    uint8_t        PayLoadLen:7;
    uint8_t        MASK:1;
} WebSocketFrameHeader;

typedef struct _WebSocketFrameMaxHeader1
{
    WebSocketFrameHeader    header;
    uint8_t                 maskingKey[4];
} WebSocketFrameMaxHeader1;

typedef struct _WebSocketFrameMaxHeader2
{
    WebSocketFrameHeader    header;
    uint16_t                payloadLength;
    uint8_t                 maskingKey[4];
} WebSocketFrameMaxHeader2;

typedef struct _WebSocketFrameMaxHeader3
{
    WebSocketFrameHeader    header;
    uint64_t                payloadLength;
    uint8_t                 maskingKey[4];
} WebSocketFrameMaxHeader3;

#define OPCODE_CONTINUATION_FRAME   0x0
#define OPCODE_TEXT_FRAME           0x1
#define OPCODE_BINARY_FRAME         0x2
#define OPCODE_CLOSE_FRAME          0x8
#define OPCODE_PING                 0x9
#define OPCODE_PONG                 0xA


typedef struct _WebSocketFrame
{
    bool        final;
    uint8_t     opcode;
    bool        mask;
    uint8_t     maskingKey[4];
    uint8_t   * data;
    uint64_t    length;
} WebSocketFrame;

TINY_API
TINY_LOR
WebSocketFrame * WebSocketFrame_New(void);

TINY_API
TINY_LOR
void WebSocketFrame_Delete(WebSocketFrame *thiz);


/**
 * 7.4.1.  Defined Status Codes
 */
#define WS_CLOSE_NORMAL                 1000
#define WS_CLOSE_GOING_AWAY             1001
#define WS_CLOSE_PROTOCOL_ERROR         1002
#define WS_CLOSE_UNSUPPORTED            1003
#define WS_CLOSE_NO_STATUS              1005
#define WS_CLOSE_ABNORMAL               1006
#define WS_UNSUPPORTED_DATA             1007
#define WS_POLICY_VIOLATION             1008
#define WS_CLOSE_TOO_LARGE              1009
#define WS_MISSING_EXTENSION            1010
#define WS_INTERNAL_ERROR               1011
#define WS_SERVICE_RESTART              1012
#define WS_TRY_AGAIN_LATER              1013
#define WS_TLS HANDSHAKE                1015


TINY_END_DECLS

#endif /* __WEB_SOCKET_FRAME_H__ */