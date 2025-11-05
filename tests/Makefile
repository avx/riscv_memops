PATH := /opt/riscv-gcc/bin:$(PATH)

CROSS_COMPILE ?= $(GNUC_PATH)riscv64-unknown-elf-
CC = $(CROSS_COMPILE)gcc
CPP = $(CROSS_COMPILE)cpp
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
SIZE = $(CROSS_COMPILE)size

CFLAGS = -O3 -mrelax -mstrict-align -fno-tree-loop-distribute-patterns -mcmodel=medany -nostartfiles -nostdlib -I.
#CFLAGS += -g3 -ggdb

SRC_COMMON = 	entry.S \
	../memops.S \
	start.c \
	common.c \
	htif.c \
	memops.c \
	printf.c

SRC_TEST = $(SRC_COMMON) mem_tests.c
SRC_PERF = $(SRC_COMMON) mem_perf.c

LIBQUAD_SRC = libquad/divdi3.c  \
	      libquad/lshrdi3.c \
	      libquad/moddi3.c  \
	      libquad/qdivrem.c \
	      libquad/udivdi3.c \
	      libquad/umoddi3.c

LIBQUAD_INCLUDE = -Ilibquad/include -Ilibquad/include/sys

all: tests run-tests be_tests run-be_tests

# -DQUIET
define _test
test_$1$2_$3: $$(SRC_TEST)
	$$(CC) $$(CFLAGS) $$(ARGS) \
	 -march=$1$2 \
	 -mabi=$(if $(filter $1, rv32),ilp32,lp64d) \
	 $(if $(filter $3, be),-mbig-endian,) \
	 -Wl,-Tmain.ld $$(SRC_TEST) \
	 $(if $(filter $1, rv32), $$(LIBQUAD_SRC) $$(LIBQUAD_INCLUDE),) \
	 -o test_$1$2_$3

run-test_$1$2_$3: test_$1$2_$3
#	spike --isa=$1$2_zicntr --dc=8:8:32 --ic=8:8:32 --l2=8:8:128 -m$(if $(filter $1, rv32),32,64) $(if $(filter $3, be),--big-endian,) test_$1$2_$3
	stdbuf -oL spike --isa=$1$2_zicntr -m$(if $(filter $1, rv32),32,64) $(if $(filter $3, be),--big-endian,) test_$1$2_$3

perf_$1$2_$3: $$(SRC_PERF)
	$$(CC) $$(CFLAGS) $$(ARGS) \
	 -march=$1$2 \
	 -mabi=$(if $(filter $1, rv32),ilp32,lp64d) \
	 $(if $(filter $3, be),-mbig-endian,) \
	 -Wl,-Tmain.ld $$(SRC_PERF) \
	 $(if $(filter $1, rv32), $$(LIBQUAD_SRC) $$(LIBQUAD_INCLUDE),) \
	 -o perf_$1$2_$3

run-perf_$1$2_$3: perf_$1$2_$3
#	spike --isa=$1$2_zicntr --dc=8:8:32 --ic=8:8:32 --l2=8:8:128 -m$(if $(filter $1, rv32),32,64) $(if $(filter $3, be),--big-endian,) test_$1$2_$3
	spike --isa=$1$2_zicntr -m64 $(if $(filter $3, be),--big-endian,) perf_$1$2_$3

dump-test_$1$2_$3: test_$1$2_$3
	@readelf -a test_$1$2_$3 | grep _mem > test_$1$2_$3.func_sizes
	@$$(OBJDUMP) -M no-aliases -D test_$1$2_$3 > test_$1$2_$3.dump

dump-perf_$1$2_$3: perf_$1$2_$3
	@readelf -a perf_$1$2_$3 | grep _mem > perf_$1$2_$3.func_sizes
	@$$(OBJDUMP) -M no-aliases -D perf_$1$2_$3 > perf_$1$2_$3.dump
endef

$(eval $(call _test,rv64,g,le))
$(eval $(call _test,rv64,gc,le))
$(eval $(call _test,rv64,g,be))
$(eval $(call _test,rv64,gc,be))
$(eval $(call _test,rv32,g,le))
$(eval $(call _test,rv32,gc,le))
$(eval $(call _test,rv32,g,be))
$(eval $(call _test,rv32,gc,be))

le_tests: test_rv64gc_le test_rv64g_le test_rv32gc_le test_rv32g_le
be_tests: test_rv64gc_be test_rv64g_be test_rv32gc_be test_rv32g_be
tests: le_tests be_tests

run-le_tests: run-test_rv64gc_le run-test_rv64g_le run-test_rv32gc_le run-test_rv32g_le
run-be_tests: run-test_rv64gc_be run-test_rv64g_be run-test_rv32gc_be run-test_rv32g_be
run-tests: run-le_tests run-be_tests

run-le_perf: run-perf_rv64gc_le run-perf_rv64g_le run-perf_rv32gc_le run-perf_rv32g_le
run-be_perf: run-perf_rv64gc_be run-perf_rv64g_be run-perf_rv32gc_be run-perf_rv32g_be

run-perf: run-le_perf run-be_perf

clean:
	rm -rf test_rv* perf_rv*

.PHONY: clean run-tests run_le_tests run_be_tests run-tests le_tests be_tests tests run-le_perf run-be_perf run-perf
