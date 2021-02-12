package(default_visibility = ["//visibility:public"])

grpc_proto_library(
	name = "db_protos",
	include = "./",
	proots = [
		"server.proto",		
	],
	protoc = "//thirdparty/protobuf:protoc",
	deps = [
		"//thirdparty/protobuf:protobuf:protofbuf",
	],
)
