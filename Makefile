BAZEL = bazel
BAZEL_OPT = \
						-s -c opt --ignore_unsupported_sandboxing --verbose_failures --experimental_sandbox_base=/dev/shm --jobs 10

ALL_TARGET = proxy kvdb dbserver

.PHONY: all clean prepare ${ALL_TARGET}

all: ${ALL_TARGET}

clean: ${BAZEL} clean

prepare:
				git submodule sync --recursive
				git submodule update --init --recursive
				
proxy: prepare
			 ${BAZEL} build ${BUILD_OPT} proxy:all

kvdb: prepare
			${BAZEL} build ${BUILD_OPT} kvdb:all

dbserver: prepare
					${BAZEL} build ${BAZEL_OPT} dbserver:all
