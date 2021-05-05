#!/bin/bash

PROTOC_PATH=protoc

$PROTOC_PATH --cpp_out=./src/protos/ master.proto
$PROTOC_PATH --cpp_out=./src/protos/ server.proto