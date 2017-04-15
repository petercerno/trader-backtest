def cc_proto(name, src):
  native.genrule(
      name = "%s_proto_gen" % name,
      outs = ["%s.pb.cc" % name, "%s.pb.h" % name],
      cmd = "$(location @protobuf//:protoc) --cpp_out=$(GENDIR) $<",
      srcs = [src],
      tools = ["@protobuf//:protoc"],
  )

  native.cc_library(
      name = "%s_proto" % name,
      srcs = ["%s.pb.cc" % name],
      hdrs = ["%s.pb.h" % name],
      deps = ["@protobuf//:protobuf"],
      copts = ["-Iprotobuf/src"],
  )
