# mprpc -  一个protobuf和moduo实现的rpc框架

## 概述

### 什么是RPC

RPC(remote Procedure Call) 远程过程调用可以说是现代分布式架构和微服务的基础。其主要目的是为了让构建分布式计算更加容易。而RPC框架的存在也是为了这个目的，通过RPC框架，用户可以通过简单的封装让本地业务轻松实现远程调用的功能。更详细的原理可以百度。

### 项目介绍

本项目基于Linux使用muduo、protobuf、Zookeeper实现了一个轻量级的RPC框架。项目有如下特点：

* 基于 muduo 网络库实现高并发网络通信模块，作为 RPC 远程调用的基础。
* 基于 Protobuf 实现 RPC 方法调用和参数的序列化和反序列化，并根据其提供得 RPC 接口编写 RPC 服务。
* 基于 ZooKeeper 分布式协调服务中间件提供服务注册和服务发现功能。
* 基于生产者消费者模型，实现了异步工作方式的日志模块。

## 环境部署

本项目在晚上也可以找到很多的分析和教程，所以我在这里不再赘述。我在这里只介绍一下我在实际开发中所遇到最头疼的问题——环境部署，来介绍一下如何部署项目环境。

项目使用编译器为GCC 9.4.0

### muduo安装

muduo库是C++中较为知名的网络库，其使用Reactor架构，通过Epoll+线程池+Asio提供了一个高性能的网络服务。其底层由一个线程接受连接请求，Epoll监听事件，线程池处理事件。原理上十分类似于非常著名的C++入门项目TinyWebServer（TinyWebServer也是muduo库作者陈硕老师书里的一个项目）。

我们可以在Github中直接搜索muduo，就能找到该项目。下载该项目的Release版本（尽量下载最新版本，旧版本源码编译可能触发C++ROV优化问题，导致代码无法正常编译，需要手动修改源码），解压后，在项目根目录执行

```shell
sudo apt install cmake make gcc g++ libboost-dev -y
./build.sh
./build.sh install
```

执行完上述命令后，在项目根目录上一级会出现一个新的文件夹叫做 `/build/`，这里面就存放着编译出的库文件和头文件，我们需要将其放到Linux系统指定位置。 `/build/release-install-cpp11/`

```shell
cd include/
mv muduo /usr/include/
cd ..
cd lib/
mv * /usr/local/lib/
sudo ldconfig
```


### protobuf安装

protobuf是什么我在这里就不再介绍了，本项目使用了非常多protobuf的内容，框架中服务的发布和调用本质就是protobuf的各种多态是实现。

源码的获取只需要前往[protobuf](https://github.com/protocolbuffers/protobuf)，下载Release版本源码即可。protobuf的版本推荐不高于3.21.1，因为3.21.1之后，项目的编译需要依赖一个外部的Abseil库，这个库的安装和后面protobuf的编译十分的麻烦，所以推荐使用3.21.1。

解压之后进入项目根目录执行以下命令

```shell
sudo apt install libtool autoconf automake -y
./autogen.sh
./configure
make -j$(nproc)
sudo make install
sud ldconfig
```


### Zookeeper安装

Zookeeper的分为两个部分，分别是可执行文件和开发API。两个我们都需要获取。这里我推荐直接前往[USTC镜像站地址](https://mirrors.ustc.edu.cn/apache/zookeeper/)来下载源代码

，直接下载stable目录中的稳定版本即可，我们需要获取 `apache-zookeeper-version-bin.tar.gz`(Zookeeper程序)，`apache-zookeeper-version-tar-gz`（项目源码）

Zookeeper的使用我在这里就不再介绍了，所以我们着重介绍一下API的编译。

#### CppUnit编译

CppUnit是可以直接通过apt安装的，但是会缺少一个.m4文件定义各种宏，所以还是需要我们自己编译

前往[SourceForge](https://sourceforge.net/projects/cppunit/files/cppunit/)，获取CppUnit最新源码（太老的版本会触发几个有关缓冲区的编译断言，导致编译失败）。获取到之后我们解压进入项目根目录，执行

```shell
./configure
make -j$(nproc)
sudo make install
sudo cp ./cppunit.m4 /usr/share/aclocal
```

#### 编译Zookeeper

##### 下载openjdk

直接通过apt下载openjdk-11，这个是用于运行Zookeeper程序的，如果你Linux换源了可能会找不到 `libcup2s`这个库，把源换回来就可以解决

```shell
sudo apt install openjdk-11-jdk -y
```

##### 下载maven

```shell
sudo apt install maven -y
```

##### 生成build.xml

进入Zookeeper源码的 `zookeeper-jute`目录，执行

```shell
mvn compile
```

##### 生成./configure

进入Zookeeper源码的 `zookeeper-client/zookeeper-client-c`目录

执行

```shell
autoreconf -if
./configure
make -j$(mproc)
sudo make install
```

至此，项目的全部环境部署完成。但是你要是真的用不需要安装这么多的东西，本项目提供了一个静态库，直接链接这个静态库就可以使用了
