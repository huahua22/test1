cmake_minimum_required(VERSION 3.4.1)




# 添加C\C++文件路径
SET(DIR_SRC src/main/cpp)

FOREACH(next_ITEM ${DIR_SRC})
  aux_source_directory (${next_ITEM} SRC_INCLUDE)
  list(APPEND ALL_SRC ${SRC_INCLUDE})      #TODO
ENDFOREACH(next_ITEM ${DIR_SRC})
aux_source_directory(./src/main/cpp/include INCLUDE_SRC)
list(APPEND ALL_SRC ${INCLUDE_SRC})
aux_source_directory(./src/main/cpp/speex SPEEX_SRC)
list(APPEND ALL_SRC ${SPEEX_SRC})


# 添加NDK的库文件
find_library(log-lib
              log )


# 生成库文件
add_library( speex
             SHARED
             ${ALL_SRC})

# 库文件的链接
target_link_libraries( speex
                       ${log-lib})
