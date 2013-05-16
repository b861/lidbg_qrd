

# 这个变量用于给出当前文件的路径。
LOCAL_PATH:= $(call my-dir)
# HAL module implemenation stored in
# hw/<COPYPIX_HARDWARE_MODULE_ID>.<ro.board.platform>.so
include $(CLEAR_VARS)

# 这是要编译的源代码文件列表。
LOCAL_SRC_FILES := lidbg.c

# 指定最后的目标安装路径.
LOCAL_MODULE_PATH := $(LOCAL_PATH)/../../out

# 表示模块在运行时要依赖的共享库（动态库），在链接时就需要，以便在生成文件时嵌入其相应的信息。
LOCAL_SHARED_LIBRARIES := liblog

# 这是模块的名字，它必须是唯一的，而且不能包含空格。
LOCAL_MODULE := lidbg.default

# LOCAL_MODULE_TAGS ：=user eng tests optional
# user: 指该模块只在user版本下才编译
# eng: 指该模块只在eng版本下才编译
# tests: 指该模块只在tests版本下才编译
# optional:指该模块在所有版本下都编译
LOCAL_MODULE_TAGS := optional

# include $(BUILD_STATIC_LIBRARY)，编译出的是静态库
# include $(BUILD_SHARED_LIBRARY)，编译出的是动态库
include $(BUILD_SHARED_LIBRARY)


# LOCAL_C_INCLUDES 可选变量，表示头文件的搜索路径。 默认的头文件的搜索路径是LOCAL_PATH目录。
# LOCAL_STATIC_LIBRARIES: 表示该模块需要使用哪些静态库，以便在编译时进行链接。
