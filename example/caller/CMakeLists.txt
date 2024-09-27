#set(SRC_LIST calluserservice.cc ../user.pb.cc)
set(SRC_LIST callfriendservice.cc ../friend.pb.cc) 

add_executable(comsumer ${SRC_LIST})
target_link_libraries(comsumer mprpc protobuf)