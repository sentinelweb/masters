# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_LDLIBS += -llog   
LOCAL_MODULE    := automix
LOCAL_SRC_FILES := AutoMix.cpp Analysis.cpp  onset/FFT.cpp onset/Lookup.cpp onset/BlockProcessor.cpp onset/Onset.cpp onset/OnsetArray.cpp onset/PVBlockProcessor.cpp onset/IOIHistogram.cpp onset/Stats.cpp onset/QMFDecimate.cpp onset/VariableArray.cpp onset/CircularBuffer.cpp  onset/SongData.cpp onset/PVBlockProcessor2.cpp  mad/mad.h mad/bit.c mad/decoder.c mad/fixed.c mad/frame.c mad/huffman.c mad/layer12.c mad/layer3.c mad/stream.c mad/synth.c mad/timer.c mad/version.c
LOCAL_ARM_MODE := arm
LOCAL_C_INCLUDES := $(LOCAL_PATH)/android 
LOCAL_CFLAGS := -DHAVE_CONFIG_H -DFPM_ARM -ffast-math -O3 -Werror
    
include $(BUILD_SHARED_LIBRARY)

