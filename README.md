# 程序设计范式期末项目
## 部落冲突(Clash of Clans,COC)
|组员姓名|GitHub昵称|
|:-:|:-:|
|唐荣炜(组长)|[@nilesthump](https://github.com/nilesthump)|
|颜泽霖|[@CCAAttking7](https://github.com/CCAAttking7)|
|冯昊天|[@muMukefu](https://github.com/muMukefu)|
---
* 项目引擎
    * cocos2d-x-4.0
* 项目语言与编译器
    * C++
    * Microsoft Visual Studio 2022(IDE), MSVC编译器
* 项目功能
    * 登录
       * 游客登陆
          * 游客登录是本地登录，数据无法保存，无法进行战斗，仅可进行放置和升级建筑、收集资源等单机操作，每次进入时重置为初始状态
       * 账号密码登录
          * 通过联网功能将数据上传至服务器，本地操作可上传，可进行战斗等联网操作
    * 数据存储
       * 仅设置服务器数据库，通过服务器进行数据存储操作，无本地存储功能
    * 联网功能
       * [server](https://github.com/nilesthump/COC/tree/main/server)服务器连接客户端，进行联网交互操作
       * [SQLite](https://github.com/nilesthump/COC/tree/main/SqLiteTools)数据库记录服务器资源数据，客户端加载时从服务器数据库调用数据  数据库为开源C库SQLite
---
