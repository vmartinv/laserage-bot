cmake_minimum_required (VERSION 2.6)
project (laserage-bot)
add_executable(Bot bot.cpp)
target_link_libraries(Bot gdi32)
