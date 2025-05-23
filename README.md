# C++ 网络聊天室（基于 Winsock）

本项目是一个使用 C++ 编写的多用户网络聊天室，基于 `Windows` 平台的 `Winsock API` 实现。支持用户管理、聊天室管理、用户特征提取与匹配等功能，适用于学习网络编程、并发编程、文本处理与用户建模等相关知识。

## 🔧 项目功能

- ✅ 用户注册与登录
- ✅ 多聊天室支持（创建、加入、列出成员）
- ✅ 消息广播与聊天记录持久化
- ✅ 用户语言风格特征提取
- ✅ 用户相似度推荐
- ✅ 聊天室匹配推荐
- ✅ 控制台命令管理

## 📁 项目结构

```text
CMD_online_chatroom
│
├─ CMDoc_server
│   │
│   ├─ main.cpp
│   │   └─ 启动服务器、注册命令、主控循环
│   │
│   ├─ header_file
│   │   ├─ ChatServer.h
│   │   │   └─ 服务器主控、用户管理、消息分发
│   │   ├─ Chatroom.h
│   │   │   └─ 聊天房间管理、房间成员、消息广播
│   │   ├─ ChatHistoryManager.h
│   │   │   └─ 聊天记录保存与加载
│   │   ├─ MatchEngine.h
│   │   │   └─ 用户特征提取、相似度计算
│   │   ├─ UserDataManager.h
│   │   │   └─ 用户注册、登录、数据持久化
│   │   ├─ MessagePacket.h
│   │   │   └─ 消息数据结构
│   │   ├─ CommandManager.h
│   │   │   └─ 命令注册与执行
│   │   ├─ ServerCommand.h
│   │   │   └─ 具体命令实现（如 help、room、stop）
│   │   ├─ PrintLog.h
│   │   │   └─ 日志与错误输出
│   │   └─ ...（其他头文件）
│   │
│   ├─ source_file
│   │   ├─ ChatServer.cpp
│   │   ├─ ChatRoom.cpp
│   │   ├─ ChatHistoryManager.cpp
│   │   ├─ MatchEngine.cpp
│   │   ├─ ...（其他实现文件）
│   │
│   ├─ room_list.txt
│   │   └─ 房间列表持久化
│   ├─ room/
│   │   └─ <room_name>.dat（各房间聊天记录）
│   ├─ user/
│   │   └─ 用户数据文件
│   └─ xiandaihanyuchangyongcibiao.txt（词典文件）
│
└─ ...（其他目录或文件）
```
## 🖥️ 使用说明
### 编译方式
环境要求：

+ Windows 系统

+ Visual Studio / g++（支持 Winsock API）

+ C++17 标准或以上

### 启动服务
```bash
./chat_server
```

服务启动后默认监听端口 8088（可修改main.cpp文件）。

### 控制台命令（服务器端）

+ help：显示可用命令
+ room：进入房间管理命令模式
+ stop：停止服务器

更多命令功能请在命令行中输入 help 查看。

## 🧑‍💻 客户端指令（用户端）
用户连接后可发送如下指令：

+ /register <username> <password>：注册新用户
+ /login <username> <password>：登录已注册用户
+ /help：查看用户指令帮助
+ /usrname：查看当前用户名
+ /room create <roomname>：创建聊天室
+ /room join <roomname>：加入聊天室
+ /room list：列出所有聊天室及成员
+ /features：展示与其他用户的语言风格相似度
+ /best-room：推荐最适合的聊天室

## 💡 特征匹配原理简介
采用简易分词（基于常用词表）与特征提取：

交互频率、词汇多样度、句法结构、标点使用等

使用余弦相似度进行用户之间/用户与聊天室之间的匹配

## 📌 注意事项
聊天室信息与用户信息均支持持久化存储

特征提取线程每隔 10 秒自动更新用户与聊天室特征

所有消息与指令以 MessagePacket 结构封装并发送

## 📜 许可证
该项目为学习与研究用途，暂无特定许可证。如需商用请联系作者。

## 🙋‍♂️ 作者
由 bl333 和 ParseeLemonade 开发完成，欢迎交流反馈与优化建议！

> 本README文件由AI生成，可能有误，请甄别
