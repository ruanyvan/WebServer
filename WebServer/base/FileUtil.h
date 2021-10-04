#pragma once
#include <bits/types/FILE.h>
#include <cstddef>
#include <string>
#include "noncopyable.h"

class AppendFile : noncopyable{
    public:
    explicit AppendFile(const std::string& filename);
    ~AppendFile();
    
    void append(const char *logline, const size_t len);
    void flush();

    private:
    size_t write(const char *logline, size_t len);
    FILE *fp_;
    char buffer_[64*1024];
};