#############################################################################
# AI Butler Board - Makefile
#
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.  The
# ASF licenses this file to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance with the
# License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations
# under the License.
#
#############################################################################

include $(TOPDIR)/Make.defs

# Board source files - minimal configuration

CSRCS = board_boot.c

ifeq ($(CONFIG_BOARDCTL),y)
CSRCS += board_appinit.c
endif

# AI Butler 源文件
CSRCS += ai_butler/ai_butler_main.c
CSRCS += ai_butler/env_monitor.c
CSRCS += ai_butler/timer_manager.c
CSRCS += ai_butler/scene_manager.c
CSRCS += ai_butler/cloud_connector.c

DEPPATH += --dep-path board
DEPPATH += --dep-path ai_butler
VPATH += :board
VPATH += :ai_butler

include $(TOPDIR)/boards/Board.mk
