#
# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright (c) 2021 Jessica Clarke <jrtc27@jrtc27.com>
#

OBJS += $(ROOT_DIR)/libquad/divdi3.o
OBJS += $(ROOT_DIR)/libquad/moddi3.o
OBJS += $(ROOT_DIR)/libquad/qdivrem.o
OBJS += $(ROOT_DIR)/libquad/udivdi3.o
OBJS += $(ROOT_DIR)/libquad/umoddi3.o
OBJS += $(ROOT_DIR)/libquad/lshrdi3.o

CFLAGS += -I$(ROOT_DIR)/libquad/include -I$(ROOT_DIR)/libquad/include/sys
