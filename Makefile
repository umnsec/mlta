CUR_DIR = $(shell pwd)
LLVM_BUILD := ${CUR_DIR}/llvm-project/prefix
ANALYZER_DIR := ${CUR_DIR}/src
ANALYZER_BUILD := ${CUR_DIR}/build


UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
	NPROC := ${shell nproc}
else
	NPROC := ${shell sysctl -n hw.ncpu}
endif

build_analyzer_func = \
	(mkdir -p ${2} \
		&& cd ${2} \
		&& PATH=${LLVM_BUILD}/bin:${PATH} \
			LLVM_TOOLS_BINARY_DIR=${LLVM_BUILD}/bin \
			LLVM_LIBRARY_DIRS=${LLVM_BUILD}/lib \
			LLVM_INCLUDE_DIRS=${LLVM_BUILD}/include \
			CC=clang CXX=clang++ \
			cmake ${1}	\
				-DCMAKE_BUILD_TYPE=Build \
				-DLLVM_ENABLE_ASSERTIONS=ON \
				-DCMAKE_CXX_FLAGS_BUILD="-std=c++14 -fpic -fno-rtti -g" \
		&& make -j${NPROC})


all: kanalyzer

kanalyzer:
	$(call build_analyzer_func, ${ANALYZER_DIR}, ${ANALYZER_BUILD})

clean:
	rm -rf ${ANALYZER_BUILD}
