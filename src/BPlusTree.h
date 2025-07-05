#ifndef BPlusTree_H
#define BPlusTree_H

// 定义树的节点大小，存储2-5个key；对应非叶节点包含3-6个指针，叶节点包含2-5个值。
// 注意会多留一个位置，用于节点分裂前的临时存储
#define MAX_KEY 5
#define MIN_KEY 2
#define MAX_CHILD (MAX_KEY + 1)
#define MIN_CHILD (MIN_KEY + 1)

// 定义节点结构体
typedef struct Node {
    long key[MAX_KEY + 1];  // 键（搜索码）数组
    long n;    // 记录这个节点存了多少个键，注意对于非叶子节点来说，子节点的数量就是n+1
    long isLeaf;          // 判断是否为叶节点
    long *data; // 叶节点存储数据的数组
    struct Node **child;  // 子节点指针数组
    struct Node *parent, *right;  // 指向parent和右邻居
} Node;

typedef struct Tree {
    Node *root;
    long size;
} Tree;

/*
    以下是一些内部使用的辅助函数
 */

// 找到node的键数组中最后一个小于或等于target的键，返回其所在位置
// 若所有键均大于target，则应返回-1
// 例如key=[2,4,6]，那么target<=1时应返回-1，target=2或者3时应返回0，target>=6都应返回2
long searchKeyInNode(Node *node, long target);

// 对parent的第i个子节点进行分裂
void splitNode(Node *parent, long i);

// 向node中加入(key,value)对
long insertToNode(Node *node, long key, long value);

// 对parent的第k个子节点进行合并
void mergeNode(Node *parent, long k);

// 从node中删除key
long delFromNode(Node *node, long key);

/*
    以下是提供给外部使用的函数
 */

// 创建B+树,并进行相应的初始化。
Tree* createTree();

// 等值查询，给出key，查找对应的value，并返回。如果不存在该key，返回-1
long equalSearch(Tree *tree, long key);

// 用于存储范围查询结果的结构体
typedef struct {
    long key, value;
} KV;

// 范围查询，给出key的范围[start,end]，查找对应的键值对，并返回总数。
// buffer要有足够的空间
long rangeSearch(Tree *tree, long start, long end, KV *buffer, long buffer_length);

// 返回树的高度
long treeHeight(Tree *tree);

long insertToTree(Tree *tree, long key, long value);

long delFromTree(Tree *tree, long key);

/*
    供调试代码使用的函数
*/

// printTree的递归辅助函数。
void printTreeRec(Node *node, long level);

// 打印树的结构，打印出来的level是一个整数，第一位固定是1，其余每一位表示当前node在对应层的第几棵子树里。
// 例如level=132，表示当前node是根节点的第3个子节点的第2个子节点。
void printTree(Tree *tree);

#endif
