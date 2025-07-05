# B+树基本功能的实现（C语言版）

## 功能特性
- **插入操作**：支持向B+树中插入键值对，当节点达到最大容量时会自动进行分裂。
- **删除操作**：支持从B+树中删除指定键，当节点键数量过少时会进行节点合并。
- **等值查询**：根据给定的键查找对应的值，如果键不存在则返回 -1。
- **范围查询**：根据给定的键范围查找对应的键值对，并将结果存储在缓冲区中，返回结果的总数。
- **树高查询**：返回B+树的高度。
- **调试功能**：提供打印树结构的函数，方便调试和查看树的状态。

## 代码结构
```plaintext
BPlusTree/
├── CMakeLists.txt
├── src/
│   ├── main.c
│   ├── BPlusTree.h
│   └── BPlusTree.c
```
- `CMakeLists.txt`：CMake配置文件，用于编译项目。
- `src/main.c`：主程序文件，可用于测试B+树的功能。
- `src/BPlusTree.h`：B+树的头文件，定义了节点结构体、树结构体和各种操作函数的声明。
- `src/BPlusTree.c`：B+树的实现文件，包含了各种操作函数的具体实现。

## 编译
```bash
mkdir build
cd build
cmake ..
make
```

## 测试和使用
使用 `check.sh` 脚本利用IO重定向和 `diff` 命令检查程序输出结果是否正确：
```bash
chmod +x check.sh
# 单个
./check.sh 1
# 范围
./check.sh 3-6
# 全部
./check.sh
```
