// Copyright © 2019 Peter Cerno. All rights reserved.

#ifndef UTIL_PROTO_H
#define UTIL_PROTO_H

#include <iostream>
#include <string>
#include <vector>

#include <fcntl.h>
#include <unistd.h>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/gzip_stream.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/message.h>

namespace trader {

// Reads delimited message from the input stream.
bool ReadDelimitedFrom(google::protobuf::io::ZeroCopyInputStream* raw_input,
                       google::protobuf::Message* message);

// Writes delimited message to the output stream.
bool WriteDelimitedTo(const google::protobuf::Message& message,
                      google::protobuf::io::ZeroCopyOutputStream* raw_output);

// Reads delimited messages from the (compressed) input file.
template <typename T>
bool ReadDelimitedMessagesFromFile(const std::string& file_name,
                                   std::vector<T>* messages) {
  if (messages == nullptr) {
    std::cerr << "Undefined output vector" << std::endl;
    return false;
  }
  int fd = open(file_name.c_str(), O_RDONLY);
  if (fd == -1) {
    std::cerr << "Cannot open file: " << file_name << std::endl;
    return false;
  }
  {
    google::protobuf::io::FileInputStream file_stream(fd);
    google::protobuf::io::GzipInputStream gzip_stream(&file_stream);
    T message;
    while (ReadDelimitedFrom(&gzip_stream, &message)) {
      messages->emplace_back(message);
    }
  }
  close(fd);
  return true;
}

// Writes (and compresses) delimited messages to the output file.
template <class InputIterator>
bool WriteDelimitedMessagesToFile(InputIterator first, InputIterator last,
                                  const std::string& file_name, bool compress) {
  int fd = open(file_name.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd == -1) {
    std::cerr << "Cannot open file: " << file_name << std::endl;
    return false;
  }
  bool success = true;
  {
    google::protobuf::io::FileOutputStream file_stream(fd);
    google::protobuf::io::GzipOutputStream::Options options;
    options.format = google::protobuf::io::GzipOutputStream::GZIP;
    options.compression_level = compress ? Z_DEFAULT_COMPRESSION : 0;
    google::protobuf::io::GzipOutputStream gzip_stream(&file_stream, options);
    while (first != last) {
      if (!WriteDelimitedTo(*first, &gzip_stream)) {
        std::cerr << "Cannot write message: " << first->DebugString()
                  << std::endl;
        success = false;
        break;
      }
      ++first;
    }
  }
  close(fd);
  return success;
}

}  // namespace trader

#endif  // UTIL_PROTO_H
