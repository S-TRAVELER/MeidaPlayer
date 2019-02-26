﻿/*
 * MIT License
 *
 * Copyright (c) 2016 xiongziliang <771730766@qq.com>
 *
 * This file is part of ZLMediaKit(https://github.com/xiongziliang/ZLMediaKit).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include "HttpRequestSplitter.h"
#include "Util/logger.h"
#include "Util/util.h"
using namespace toolkit;

namespace mediakit {

void HttpRequestSplitter::input(const char *data,uint64_t len) {
    const char *ptr = data;
    if(!_remain_data.empty()){
        _remain_data.append(data,len);
        data = ptr = _remain_data.data();
        len = _remain_data.size();
    }

    splitPacket:

    /*确保ptr最后一个字节是0，防止strstr越界
     *由于ZLToolKit确保内存最后一个字节是保留未使用字节并置0，
     *所以此处可以不用再次置0
     *但是上层数据可能来自其他渠道，保险起见还是置0
     */

    char &tail_ref = ((char *) ptr)[len];
    char tail_tmp = tail_ref;
    tail_ref = 0;

    //数据按照请求头处理
    const char *index = nullptr;
    _remain_data_size = len;
    while (_content_len == 0 && _remain_data_size > 0 && (index = onSearchPacketTail(ptr,_remain_data_size)) != nullptr) {
        //_content_len == 0，这是请求头
        const char *header_ptr = ptr;
        int64_t header_size = index - ptr;

        ptr = index;
        _remain_data_size = len - (ptr - data);

        _content_len = onRecvHeader(header_ptr, header_size);
    }

    /*
     * 恢复末尾字节
     */
    tail_ref = tail_tmp;

    if(_remain_data_size <= 0){
        //没有剩余数据，清空缓存
        _remain_data.clear();
        return;
    }

    if(_content_len == 0){
        //尚未找到http头，缓存定位到剩余数据部分
        string str(ptr,_remain_data_size);
        _remain_data = str;
        return;
    }

    //已经找到http头了
    if(_content_len > 0){
        //数据按照固定长度content处理
        if(_remain_data_size < _content_len){
            //数据不够，缓存定位到剩余数据部分
            string str(ptr,_remain_data_size);
            _remain_data = str;
            return;
        }
        //收到content数据，并且接受content完毕
        onRecvContent(ptr,_content_len);

        _remain_data_size -= _content_len;
        ptr += _content_len;
        //content处理完毕,后面数据当做请求头处理
        _content_len = 0;

        if(_remain_data_size > 0){
            //还有数据没有处理完毕
            string str(ptr,_remain_data_size);
            _remain_data = str;

            data = ptr = (char *)_remain_data.data();
            len = _remain_data.size();
            goto splitPacket;
        }
        _remain_data.clear();
        return;
    }


    //_content_len < 0;数据按照不固定长度content处理
    onRecvContent(ptr,_remain_data_size);//消费掉所有剩余数据
    _remain_data.clear();
}

void HttpRequestSplitter::setContentLen(int64_t content_len) {
    _content_len = content_len;
}

void HttpRequestSplitter::reset() {
    _content_len = 0;
    _remain_data_size = 0;
    _remain_data.clear();
}

const char *HttpRequestSplitter::onSearchPacketTail(const char *data,int len) {
    auto pos = strstr(data,"\r\n\r\n");
    if(pos == nullptr){
        return nullptr;
    }
    return  pos + 4;
}

int64_t HttpRequestSplitter::remainDataSize() {
    return _remain_data_size;
}


} /* namespace mediakit */

