// Copyright © 2021 Peter Cerno. All rights reserved.

#ifndef UTIL_PROTO_H
#define UTIL_PROTO_H

#include <google/protobuf/io/coded_stream.h>
#ifdef HAVE_ZLIB
#include <google/protobuf/io/gzip_stream.h>
#endif
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/message.h>
#include <google/protobuf/util/delimited_message_util.h>

#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace trader {
// Status returned by the reader (see below).
enum class ReaderStatus { kContinue, kBreak, kFailure };

// Reads delimited messages from the (compressed) input stream and applies the
// function "reader" on them.
template <typename T>
bool ReadDelimitedMessagesFromIStream(
    std::istream& stream, std::function<ReaderStatus(const T&)> reader) {
  bool success = true;
  google::protobuf::io::IstreamInputStream in_stream(&stream);
#ifdef HAVE_ZLIB
  google::protobuf::io::GzipInputStream gzip_stream(&in_stream);
#else
  google::protobuf::io::ZeroCopyInputStream& gzip_stream = in_stream;
#endif
  while (true) {
    T message;
    bool clean_eof;
    bool parsed = google::protobuf::util::ParseDelimitedFromZeroCopyStream(
        &message, &gzip_stream, &clean_eof);
    if (!parsed) {
      if (clean_eof) {
        break;
      }
      std::cerr << "Cannot parse the input stream" << std::endl;
      success = false;
      break;
    }
    const ReaderStatus reader_status = reader(message);
    if (reader_status == ReaderStatus::kContinue) {
      continue;
    } else if (reader_status == ReaderStatus::kBreak) {
      break;
    } else {
      assert(reader_status == ReaderStatus::kFailure);
      success = false;
      break;
    }
  }
  return success;
}

// Reads delimited messages from the (compressed) input stream.
template <typename T>
bool ReadDelimitedMessagesFromIStream(std::istream& stream,
                                      std::vector<T>& messages) {
  return ReadDelimitedMessagesFromIStream<T>(
      stream, [&messages](const T& message) -> ReaderStatus {
        messages.push_back(message);
        return ReaderStatus::kContinue;
      });
}

// Reads delimited messages from the (compressed) input file and applies the
// function "reader" on them.
template <typename T>
bool ReadDelimitedMessagesFromFile(
    const std::string& file_name,
    std::function<ReaderStatus(const T&)> reader) {
  std::fstream in_fstream(file_name, std::ios::in | std::ios::binary);
  if (!in_fstream) {
    std::cerr << "Cannot open file: " << file_name << std::endl;
    return false;
  }
  return ReadDelimitedMessagesFromIStream(in_fstream, reader);
}

// Reads delimited messages from the (compressed) input file.
template <typename T>
bool ReadDelimitedMessagesFromFile(const std::string& file_name,
                                   std::vector<T>& messages) {
  return ReadDelimitedMessagesFromFile<T>(
      file_name, [&messages](const T& message) -> ReaderStatus {
        messages.push_back(message);
        return ReaderStatus::kContinue;
      });
}

// Writes (and compresses) delimited messages to the output file.
template <class InputIterator>
bool WriteDelimitedMessagesToOStream(InputIterator first, InputIterator last,
                                     std::ostream& stream, bool compress) {
  bool success = true;
  google::protobuf::io::OstreamOutputStream out_stream(&stream);
#ifdef HAVE_ZLIB
  google::protobuf::io::GzipOutputStream::Options options;
  options.format = google::protobuf::io::GzipOutputStream::GZIP;
  options.compression_level = compress ? Z_DEFAULT_COMPRESSION : 0;
  google::protobuf::io::GzipOutputStream gzip_stream(&out_stream, options);
#else
  google::protobuf::io::ZeroCopyOutputStream& gzip_stream = out_stream;
#endif
  while (first != last) {
    bool serialized =
        google::protobuf::util::SerializeDelimitedToZeroCopyStream(
            *first, &gzip_stream);
    if (!serialized) {
      std::cerr << "Cannot serialize message: " << first->DebugString()
                << std::endl;
      success = false;
      break;
    }
    ++first;
  }
  return success;
}

// Writes (and compresses) delimited messages to the output file.
template <class InputIterator>
bool WriteDelimitedMessagesToFile(InputIterator first, InputIterator last,
                                  const std::string& file_name, bool compress) {
  std::fstream out_fstream(file_name,
                           std::ios::out | std::ios::trunc | std::ios::binary);
  if (!out_fstream) {
    std::cerr << "Cannot open file: " << file_name << std::endl;
    return false;
  }
  return WriteDelimitedMessagesToOStream(first, last, out_fstream, compress);
}

}  // namespace trader

#endif  // UTIL_PROTO_H
