// Copyright © 2020 Peter Cerno. All rights reserved.

#include "util/util_proto.h"

namespace trader {

using google::protobuf::Message;
using google::protobuf::io::CodedInputStream;
using google::protobuf::io::CodedOutputStream;
using google::protobuf::io::ZeroCopyInputStream;
using google::protobuf::io::ZeroCopyOutputStream;

bool ReadDelimitedFrom(ZeroCopyInputStream& raw_input, Message& message) {
  CodedInputStream input(&raw_input);
  // Read the size.
  uint32_t size;
  if (!input.ReadVarint32(&size)) return false;
  // Tell the stream not to read beyond that size.
  const auto limit = input.PushLimit(size);
  // Parse the message.
  if (!message.MergePartialFromCodedStream(&input) ||
      !input.ConsumedEntireMessage()) {
    // Cannot parse message.
    return false;
  }
  // Release the limit.
  input.PopLimit(limit);
  return true;
}

bool WriteDelimitedTo(const Message& message,
                      ZeroCopyOutputStream& raw_output) {
  CodedOutputStream output(&raw_output);
  // Write the size.
  const int size = message.ByteSize();
  output.WriteVarint32(size);
  uint8_t* buffer = output.GetDirectBufferForNBytesAndAdvance(size);
  if (buffer != NULL) {
    // Optimization: The message fits in one buffer, so use the faster
    // direct-to-array serialization path.
    message.SerializeWithCachedSizesToArray(buffer);
  } else {
    // Slightly-slower path when the message is multiple buffers.
    message.SerializeWithCachedSizes(&output);
    if (output.HadError()) {
      // Cannot serialize message.
      return false;
    }
  }
  return true;
}

}  // namespace trader
