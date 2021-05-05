package(default_visibility = ["//visibility:public"])

load("//thirdparty/protobuf:protobuf.bzl", "grpc_proto_library")

grpc_proto_library(
	name = "db_protos",
	include = "./",
	proots = [
		"server.proto",
		"master.proto"	
	],
	protoc = "//thirdparty/protobuf:protoc",
	deps = [
		"//thirdparty/protobuf:protobuf:protofbuf",
	],
)
