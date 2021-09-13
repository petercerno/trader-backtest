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

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"

namespace trader {
// Signal returned by the reader (see below).
enum class ReaderSignal { kContinue, kBreak };
using ReaderStatus = absl::StatusOr<ReaderSignal>;

// Reads delimited messages from the (compressed) input stream and applies the
// function "reader" on them.
template <typename T>
absl::Status ReadDelimitedMessagesFromIStream(
    std::istream& stream, std::function<ReaderStatus(const T&)> reader) {
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
        return absl::OkStatus();
      }
      return absl::InvalidArgumentError("Cannot parse the input stream");
    }
    ReaderStatus reader_status = reader(message);
    if (!reader_status.ok()) {
      return absl::AbortedError(reader_status.status().message());
    }
    switch (reader_status.value()) {
      case ReaderSignal::kContinue:
        break;
      case ReaderSignal::kBreak:
        return absl::OkStatus();
    }
  }
  return absl::OkStatus();
}

// Reads delimited messages from the (compressed) input stream.
template <typename T>
absl::Status ReadDelimitedMessagesFromIStream(std::istream& stream,
                                              std::vector<T>& messages) {
  return ReadDelimitedMessagesFromIStream<T>(
      stream, [&messages](const T& message) -> ReaderStatus {
        messages.push_back(message);
        return ReaderSignal::kContinue;
      });
}

// Reads delimited messages from the (compressed) input file and applies the
// function "reader" on them.
template <typename T>
absl::Status ReadDelimitedMessagesFromFile(
    const std::string& file_name,
    std::function<ReaderStatus(const T&)> reader) {
  std::fstream in_fstream(file_name, std::ios::in | std::ios::binary);
  if (!in_fstream) {
    return absl::InvalidArgumentError(
        absl::StrFormat("Cannot open the input file: %s", file_name));
  }
  return ReadDelimitedMessagesFromIStream(in_fstream, reader);
}

// Reads delimited messages from the (compressed) input file.
template <typename T>
absl::Status ReadDelimitedMessagesFromFile(const std::string& file_name,
                                           std::vector<T>& messages) {
  return ReadDelimitedMessagesFromFile<T>(
      file_name, [&messages](const T& message) -> ReaderStatus {
        messages.push_back(message);
        return ReaderSignal::kContinue;
      });
}

// Writes (and compresses) delimited messages to the output file.
template <class InputIterator>
absl::Status WriteDelimitedMessagesToOStream(InputIterator first,
                                             InputIterator last,
                                             std::ostream& stream,
                                             bool compress) {
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
      return absl::InvalidArgumentError(absl::StrFormat(
          "Cannot serialize the message:\n%s", first->DebugString()));
    }
    ++first;
  }
  return absl::OkStatus();
}

// Writes (and compresses) delimited messages to the output file.
template <class InputIterator>
absl::Status WriteDelimitedMessagesToFile(InputIterator first,
                                          InputIterator last,
                                          const std::string& file_name,
                                          bool compress) {
  std::fstream out_fstream(file_name,
                           std::ios::out | std::ios::trunc | std::ios::binary);
  if (!out_fstream) {
    return absl::InvalidArgumentError(
        absl::StrFormat("Cannot open the input file: %s", file_name));
  }
  return WriteDelimitedMessagesToOStream(first, last, out_fstream, compress);
}

}  // namespace trader

#endif  // UTIL_PROTO_H
