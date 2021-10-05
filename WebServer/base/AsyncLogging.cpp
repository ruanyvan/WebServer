#include "AsyncLogging.h"
#include "Condition.h"
#include "LogFile.h"
#include "LogStream.h"
#include "MutexLock.h"
#include <assert.h>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <functional>
#include <utility>

AsyncLogging::AsyncLogging(std::string logFileName, int flushInterval)
        : flushInterval_(flushInterval),
        running_(false),
        basename_(logFileName),
        thread_(std::bind(&AsyncLogging::threadFunc(), this), "Logging"),
        mutex_(),
        cond_(mutex_),
        currentBuffer_(new Buffer),
        nextBuffer_(new Buffer),
        buffers_(),
        latch_(1){
            assert(logFileName.size() > 1);
            currentBuffer_->bzero();
            nextBuffer_->bzero();
            buffers_.reserve(16);
}

void AsyncLogging::append(const char *logline, int len){
    MutexLockGuard lock(mutex_);
    if(currentBuffer_->avail() > len){
        currentBuffer_->append(logline, len);
    }else {
        buffers_.push_back(currentBuffer_);
        currentBuffer_->reset();
        if(nextBuffer_){
            currentBuffer_ = std::move(nextBuffer_);
        }else {
            currentBuffer_.reset(new Buffer);
        }
        currentBuffer_->append(logline, len);
        cond_.notify();
    }
}

void AsyncLogging::threadFunc(){
    assert(running_)
}