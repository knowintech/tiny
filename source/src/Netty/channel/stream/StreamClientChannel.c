/**
 * Copyright (C) 2013-2015
 *
 * @author wenzhenwei@xiaomi.com
 * @date   2013-11-19
 *
 * @file   StreamClientChannel.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include <tiny_socket.h>
#include "StreamClientChannel.h"
#include "StreamClientChannelContext.h"
#include "channel/SocketChannel.h"

#define TAG     "StreamClientChannel"

TINY_LOR
static int64_t StreamClientChannel_GetConnectingTimeout(Channel *thiz, void *ctx)
{
    return ((StreamClientChannelContext *) thiz->ctx)->connectingTimeout * 1000000;
}

TINY_LOR
static void StreamClientChannel_Dispose(Channel *thiz)
{
    if (thiz->ctx != NULL)
    {
        StreamClientChannelContext_Delete((StreamClientChannelContext *) thiz->ctx);
        thiz->ctx = NULL;
    }

    if (StreamClientChannel_isConnected(thiz))
    {
        StreamClientChannel_Close(thiz);
    }
}

TINY_LOR
void StreamClientChannel_Delete(Channel *thiz)
{
    StreamClientChannel_Dispose(thiz);
    tiny_free(thiz);
}

TINY_LOR
static void StreamClientChannel_OnRegister(Channel *thiz, Selector *selector)
{
    if (Channel_IsActive(thiz))
    {
        Selector_Register(selector, thiz->fd, SELECTOR_OP_ALL);
        LOG_D(TAG, "StreamClientChannel_OnRegister: %d", thiz->fd);
    }
}

TINY_LOR
static void StreamClientChannel_OnRemove(Channel *thiz)
{
    LOG_D(TAG, "StreamClientChannel_OnRemove");

    StreamClientChannel_Delete(thiz);
}

TINY_LOR
static TinyRet StreamClientChannel_OnRead(Channel *thiz, Selector *selector)
{
    LOG_D(TAG, "StreamClientChannel_OnRead");

    if (Selector_IsReadable(selector, thiz->fd))
    {
        int error = 0;
        socklen_t len = sizeof(error);

        if (getsockopt(thiz->fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
        {
            LOG_E(TAG, "connect error: %d", error);
            return TINY_RET_E_INTERNAL;
        }

        thiz->onActive(thiz);
    }

    return TINY_RET_OK;
}

TINY_LOR
static TinyRet StreamClientChannel_OnWrite(Channel *thiz, Selector *selector)
{
    LOG_D(TAG, "StreamClientChannel_OnWrite");

    if (Selector_IsWriteable(selector, thiz->fd))
    {
        int error = 0;
        socklen_t len = sizeof(error);

        if (getsockopt(thiz->fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
        {
            LOG_E(TAG, "connect error: %d", error);
            return TINY_RET_E_INTERNAL;
        }

        thiz->onActive(thiz);
    }

    return TINY_RET_OK;
}

TINY_LOR
static void StreamClientChannel_OnConnectingTimeout(Channel *thiz, void *event)
{
    RETURN_IF_FAIL(thiz);

    Channel_Close(thiz);
}

TINY_LOR
static void StreamClientChannel_OnActive(Channel *thiz)
{
    RETURN_IF_FAIL(thiz);

    LOG_D(TAG, "connected");

    if (thiz->ctx != NULL)
    {
        ((StreamClientChannelContext *) thiz->ctx)->initializer(thiz, ((StreamClientChannelContext *) thiz->ctx)->initializerContext);
    }

    thiz->onActive = SocketChannel_OnActive;
    thiz->onRead = SocketChannel_OnRead;
    thiz->onRegister = SocketChannel_OnRegister;
    thiz->onEventTriggered = SocketChannel_OnEventTriggered;
    thiz->getNextTimeout = SocketChannel_GetNextTimeout;
    thiz->onActive(thiz);
}

TINY_LOR
static TinyRet StreamClientChannel_Construct(Channel *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        thiz->onRegister = StreamClientChannel_OnRegister;
        thiz->onRemove = StreamClientChannel_OnRemove;
        thiz->onActive = StreamClientChannel_OnActive;
        thiz->onRead = StreamClientChannel_OnRead;
        thiz->onWrite = StreamClientChannel_OnWrite;
        thiz->onEventTriggered = StreamClientChannel_OnConnectingTimeout;
        thiz->getNextTimeout = StreamClientChannel_GetConnectingTimeout;

        thiz->ctx = StreamClientChannelContext_New();
        if (thiz->ctx == NULL)
        {
            ret = TINY_RET_E_OUT_OF_MEMORY;
            break;
        }
    } while (0);

    return ret;
}

TINY_LOR
Channel *StreamClientChannel_New()
{
    Channel *thiz = NULL;

    do
    {
        thiz = SocketChannel_New();
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(StreamClientChannel_Construct(thiz)))
        {
            StreamClientChannel_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TINY_LOR
TinyRet StreamClientChannel_Initialize(Channel *thiz, ChannelInitializer initializer, void *ctx)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(initializer, TINY_RET_E_ARG_NULL);

    ((StreamClientChannelContext *) thiz->ctx)->initializer = initializer;
    ((StreamClientChannelContext *) thiz->ctx)->initializerContext = ctx;

    return TINY_RET_OK;
}

TINY_LOR
TinyRet StreamClientChannel_Connect(Channel *thiz, const char *ip, uint16_t port, uint32_t timeout)
{
    TinyRet ret = TINY_RET_OK;

    LOG_D(TAG, "StreamClientChannel_Connect: %s:%d", ip, port);

    do
    {
        ret = SocketChannel_Open(thiz, TYPE_TCP_CONNECTION);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "SocketChannel_Open failed");
            break;
        }

        ret = SocketChannel_SetBlock(thiz, false);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "SocketChannel_SetBlock failed");
            break;
        }

        ((StreamClientChannelContext *) thiz->ctx)->connectingTimeout = timeout;

        SocketChannel_SetRemoteInfo(thiz, ip, port);

        ret = tiny_async_connect(thiz->fd, ip, port);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "tiny_async_connect failed");
            break;
        }

        if (TINY_RET_CODE(ret) == CODE_PENDING)
        {
            LOG_D(TAG, "connecting is pending");
            break;
        }

        thiz->onActive(thiz);
    } while (0);

    return ret;
}

TINY_LOR
TinyRet StreamClientChannel_Close(Channel *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_INVALID);
    Channel_Close(thiz);
    return TINY_RET_OK;
}

TINY_LOR
bool StreamClientChannel_isConnected(Channel *thiz)
{
    return (thiz->fd >= 0);
}