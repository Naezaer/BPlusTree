#define _CRT_SECURE_NO_WARNINGS 1
#include <stdlib.h>
#include <stdio.h>
#include "BPlusTree.h"

/*
	以下首先实现一些内部使用的辅助函数
 */

 // 找到node的键数组中最后一个小于或等于target的键，返回其所在的*位置*
 // 若所有键均大于target，则应返回-1
 // 例如key=[2,4,6]，那么target<=1时应返回-1，target=2或者3时应返回0，target>=6都应返回2
long searchKeyInNode(Node* node, long target) {
	long* arr = node->key;
	long low = 0;
	long high = node->n;

	while (low < high) {
		long mid = low + ((high - low) >> 1);
		if (target >= arr[mid]) {
			low = mid + 1;
		} else {
			high = mid;
		}
	}

	return low - 1;
}

// 对parent的第i个子节点进行分裂
void splitNode(Node* parent, long i) {
	Node* child = parent->child[i];
	Node* sibling = (Node*)malloc(sizeof(Node));  // sibling就是分裂出的节点
	sibling->right = NULL;
	sibling->isLeaf = child->isLeaf;
	sibling->parent = parent;

	// 将key、data、child数组中的数据，重新分配给新旧节点
	if (child->isLeaf) {
		sibling->right = child->right;  // 原来的右邻居会成为新节点的右邻居
		child->right = sibling;
		// 对于叶子节点，分裂后两个节点的key的数量总和 = 原节点key的数量
		// 给原节点留一半的数据
		sibling->n = child->n / 2;
		child->n -= sibling->n;
		sibling->data = (long*)malloc(sizeof(long) * (MAX_KEY + 1));

		//TODO STEP 3：将原来child节点key和data数组中的数据按照上述比例分给sibling节点
		// 叶子节点的话，把child节点的后半部分key和data分给sibling节点
		for (long j = 0; j < sibling->n; j++) {
			sibling->key[j] = child->key[j + child->n];
			sibling->data[j] = child->data[j + child->n];
		}
		//TODO END
	} else {
		// 对于非叶子节点，分裂后两个节点的key的数量总和 + 1 = 原节点key的数量
		sibling->n = child->n / 2;  // 给新节点分配原来一半数量的键（向下取整）
		child->n -= sibling->n + 1;
		sibling->child = (Node**)malloc(sizeof(Node*) * (MAX_CHILD + 1));

		//TODO STEP 3：将原来child节点key和child数组中的数据按照上述比例分给sibling节点
		// 注意把siblin节点分到的子节点的parent指针指向sibling
		for (long j = 0; j < sibling->n; j++) {
			sibling->key[j] = child->key[j + child->n + 1];
			sibling->child[j] = child->child[j + child->n + 1];
			sibling->child[j]->parent = sibling;
		}

		// 由于child指针比key要多一个，所以最后一个单独处理
		sibling->child[sibling->n] = child->child[sibling->n + child->n + 1];
		//TODO END
	}

	//TODO STEP 3：在parent节点的key数组中，第i位插入一个合适的key，用来分隔child和sibling
	//TODO STEP 3：然后在parent节点的child数组中插入sibling节点，最后注意维护parent->n的值
	// 先把parent节点后半部分数据往后挪腾出空位置
	for (long j = parent->n; j > i; j--) {
		parent->key[j] = parent->key[j - 1];
		parent->child[j + 1] = parent->child[j];
	}

	// 再把合适的key（大于child节点的key小于等于sibling节点的key）
	parent->key[i] = child->key[child->n];
	parent->child[i + 1] = sibling;     // 把parent节点新插入的key右边的指针指向sibling
	parent->n++;    // 注意parent->n的更新，插入+1
	//TODO END
}

// 向node中加入(key,value)对
long insertToNode(Node* node, long key, long value) {
	if (node->n > MAX_KEY) {
		return -1; // 已经超过MAX_KEY了，说明split功能没有写好，要检查splitNode函数
	}

	long current_idx = searchKeyInNode(node, key);
	if (node->isLeaf) {
		//TODO STEP 1：如果已存在相同的键，返回-1
		//TODO STEP 1：否则在key和data数组中，第i位插入对应的数据
		// 如果已存在相同的键无法插入，返回-1
		if (current_idx != -1 && node->key[current_idx] == key) {
			return -1;
		}

		// 否则插入到第i个的位置
		// 把node节点后半部分数据往后挪腾出空间
		for (long i = node->n; i > current_idx + 1; i--) {
			node->key[i] = node->key[i - 1];
			node->data[i] = node->data[i - 1];
		}

		// 再把新的key和data填充进去，并更新node->n
		node->key[current_idx + 1] = key;
		node->data[current_idx + 1] = value;
		node->n++;
		//TODO END

		return 0;
	}

	Node* child = node->child[current_idx + 1];
	// TODO STEP 3：对于非叶子节点，应该递归地调用本函数，将数据插入child节点中
	// TODO STEP 3：并注意根据返回结果判断是否应该返回-1、是否应该调用splitNode函数分裂child节点
	// 如果node不是node节点，则尝试给node节点的合适的孩子节点尝试插入(递归)
	if (insertToNode(child, key, value) == -1) {
		return -1;
	}
	// 如果插入成功后node节点的key超过了(在定义节点结构体的时候预留了一个位置)，则需要拆分节点
	if (child->n > MAX_KEY) {
		splitNode(node, current_idx + 1);
	}
	//TODO END

	return 0;
}

// 对parent的第k个子节点进行合并
void mergeNode(Node* parent, long k) {
	// 默认是向右合并（k与k+1合并）；如果k是parent的最后一个子节点了，就向左合并（k-1与k合并）
	if (k == parent->n) {
		k--;
	}
	if (k < 0) {
		return; // 如果k<0，说明parent的子节点数量小于2，一般是因为delFromTree或者mergeNode函数没有写好
	}

	Node* lchild = parent->child[k];
	Node* rchild = parent->child[k + 1];

	// 如果两个节点的key数量足够多，就不必合并，而是在两个节点间重新分配key
	long total = lchild->n + rchild->n;
	if (total >= MIN_KEY + MIN_KEY) {
		// 计算两个节点分完各应该有多少个key
		long left_size = total / 2;
		long right_size = total - left_size;

		// 情况1：左边节点的数量比计算出来的少，把右边节点的数据往左挪
		long offset = left_size - lchild->n;
		if (offset > 0) {
			// 先把数据按照计算好的下标从右节点拷贝到左节点
			for (long i = lchild->n; i < left_size; i++) {
				lchild->key[i] = parent->key[k];
				if (lchild->isLeaf) {
					parent->key[k] = rchild->key[i - lchild->n + 1];
					lchild->data[i] = rchild->data[i - lchild->n];
				} else {
					parent->key[k] = rchild->key[i - lchild->n];
					lchild->child[i + 1] = rchild->child[i - lchild->n];
				}
			}

			// 挪完后，把右节点未拷贝的数据往前移，放到正确的位置上
			if (lchild->isLeaf) {
				for (long i = 0; i < right_size; i++) {
					rchild->key[i] = rchild->key[i + offset];
					rchild->data[i] = rchild->data[i + offset];
				}
			} else {
				for (long i = 0; i < right_size; i++) {
					rchild->key[i] = rchild->key[i + offset];
					rchild->child[i] = rchild->child[i + offset];
				}
				rchild->child[right_size] = rchild->child[right_size + offset];
			}
		}

		//情况2：右边节点的数量比计算出来的少，把左边节点的数据往右挪
		offset = right_size - rchild->n;
		if (offset > 0) {
			// 与上面的步骤相反，需要先把右节点的已有数据往后移，为拷贝数据腾出空间
			if (lchild->isLeaf) {
				//TODO STEP 5：参考上面的代码，完成叶子结点的数据挪动
				// 就跟上面的完全相反，先把rchild已有数据往后挪
				for (long j = rchild->n - 1; j >= 0; j--) {
					rchild->key[j + offset] = rchild->key[j];
					rchild->data[j + offset] = rchild->data[j];
				}
				// TODO END
			} else {
				//TODO STEP 5：参考上面的代码，完成非叶子结点的数据挪动
				// 把rchild节点的key和child指针往后挪
				for (long j = rchild->n - 1; j >= 0; j--) {
					rchild->key[j + offset] = rchild->key[j];
					rchild->child[j + offset + 1] = rchild->child[j + 1];
				}

				// 由于child指针比key多一个，所以最后一个单独处理
				rchild->child[offset] = rchild->child[0];
				//TODO END
			}
			//TODO STEP 5：参考上面的代码，将数据从左节点拷贝到右节点的正确位置
			//TODO STEP 5：注意parent的key数组应该如何更新
			for (long j = offset - 1; j >= 0; j--) {
				if (lchild->isLeaf) {
					rchild->key[j] = lchild->key[left_size + j];
					rchild->data[j] = lchild->data[left_size + j];
				} else {
					rchild->key[j] = parent->key[k];
					rchild->child[j] = lchild->child[left_size + j + 1];
				}
				parent->key[k] = lchild->key[left_size + j];
			}
			//TODO END
		}

		lchild->n = left_size;
		rchild->n = right_size;
	} else { // 两个节点不够分，必须要合并节点，这里选择把右节点合并到左节点中
		if (lchild->isLeaf) {
			//TODO STEP 5：完成叶子结点的数据挪动
			for (long j = 0; j < rchild->n; j++) {
				lchild->key[j + lchild->n] = rchild->key[j];
				lchild->data[j + lchild->n] = rchild->data[j];
			}
			//TODO END

			lchild->n = lchild->n + rchild->n;
			free(rchild->data);
		} else {
			//TODO STEP 5：完成非叶子结点的数据挪动，注意parent的key数组应该如何使用
			lchild->key[lchild->n] = parent->key[k];
			for (long j = 0; j < rchild->n; j++) {
				lchild->key[lchild->n + j + 1] = rchild->key[j];
				lchild->child[lchild->n + j + 1] = rchild->child[j];
			}
			lchild->child[lchild->n + rchild->n + 1] = rchild->child[rchild->n];
			//TODO END

			lchild->n = lchild->n + rchild->n + 1;
			free(rchild->child);
		}

		//TODO STEP 5：在parent节点的key和child数组中删除相应位置的值
		for (long j = k; j < parent->n - 1; j++) {
			parent->key[j] = parent->key[j + 1];
			parent->child[j + 1] = parent->child[j + 2];
		}
		//TODO END

		parent->n--;
		lchild->right = rchild->right;
		free(rchild);
	}
}

// 从node中删除key
long delFromNode(Node* node, long key) {
	long current_idx = searchKeyInNode(node, key);

	if (node->isLeaf) {
		//TODO STEP 4：如果已存在相同的键，返回-1
		//TODO STEP 4：否则在key和data数组中，删除第i位的数据，更新node->n的值
		if (current_idx != -1 && node->key[current_idx] == key) {
			if (!(current_idx == node->n - 1)) {
				for (long j = current_idx; j < node->n - 1; j++) {
					node->key[j] = node->key[j + 1];
					node->data[j] = node->data[j + 1];
				}
			}
			node->n--;
			return 0;
		} else {
			return -1;
		}
		//TODO END
	}

	Node* child = node->child[current_idx + 1];
	// 对非叶子节点，递归地在child节点中进行删除
	if (delFromNode(child, key) == -1) {
		return -1;
	} else {
		// delFromNode返回0，说明删除成功，如果当前节点也存了这个key，需要替换为child子树中最小的key
		if (current_idx > -1 && node->key[current_idx] == key) {
			while (!child->isLeaf) {
				child = child->child[0];
			}
			node->key[current_idx] = child->key[0];
		}
		//TODO STEP 5：当子节点的key过少时，需要调用mergeNode进行节点的合并
		if (child->n < MIN_KEY) {
			mergeNode(node, current_idx + 1);
		}
		//TODO END
	}
	return 0;
}

/*
	以下是提供给外部使用的函数
 */

 // 创建B+树,并进行相应的初始化。
Tree* createTree() {
	Node* root = (Node*)malloc(sizeof(Node));
	root->data = (long*)malloc(sizeof(long) * (MAX_KEY + 1));
	root->n = 0;
	root->isLeaf = 1;
	root->right = NULL;
	root->parent = NULL;

	Tree* tree = (Tree*)malloc(sizeof(Tree));
	tree->root = root;
	tree->size = 0;

	return tree;
}

// 等值查询，给出key，查找对应的value，并返回。如果不存在该key，返回-1
long equalSearch(Tree* tree, long key) {
	Node* root = tree->root;
	long current_idx, current_key = -1, result = -1;
	while (1) {
		if (root->isLeaf) {
			//TODO STEP 2：对于叶子节点，判断key是否存在，相应返回-1或者对应的值
			for (long i = 0; i < root->n; i++) {
				if (root->key[i] == key) {
					result = root->data[i];
					break;
				}
			}
			//TODO END
			return result;
		} else {
			// 对于非叶子节点，找到key可能存在的唯一子节点，并赋值给r，以进行下一轮循环
			current_idx = searchKeyInNode(root, key);
			root = root->child[current_idx + 1];
		}
	}
}

// 范围查询，给出key的范围[start,end]，查找对应的键值对，并返回总数。
// buffer要有足够的空间
long rangeSearch(Tree* tree, long start, long end, KV* buffer, long buffer_length) {
	Node* root = tree->root;
	long current_idx, current_key;
	while (1) {
		current_idx = searchKeyInNode(root, start);
		if (root->isLeaf) {
			current_key = current_idx == -1 ? -1 : root->key[current_idx];
			break;
		}
		root = root->child[current_idx + 1];
	}

	// 如果所有key都大于start，返回的currentKey = -1，可不做特殊处理
	if (current_key < start) {
		current_idx++;
		if (current_idx == root->n) {
			root = root->right;
			current_idx = 0;
		}
	}
	long count = 0;
	//TODO STEP 7：上面已经找到了起始节点r和起始位置current_idx，现在把数据按顺序放入buffer中，
	//TODO STEP 7：虽然输入保证了数据量不会超过buffer_length，但是最好应该判断一下
	//TODO STEP 7：提示：利用好right指针，记得统计总数count
	// 
	int flag = 0;    // 完成的标志
	// 从范围查询最小值对应的节点开始逐个遍历每一个叶子节点，直到范围查询最大值大于节点的值
	while (root && !flag) {
		for (; current_idx < root->n; current_idx++) {
			if (root->key[current_idx] > end) {
				flag = 1;
				break;
			}
			// 把key和data追加到结果集中
			buffer[count].key = root->key[current_idx];
			buffer[count].value = root->data[current_idx];
			count++;

            if (count >= buffer_length) {
                // printf("Buffer is full!\n");
                return count;
            }
		}
		if (flag == 1) {    // 已找到则结束循环
			break;
		}
		// 如果这个叶子节点遍历完了，则遍历它右边的叶子节点
		if (current_idx == root->n) {
			root = root->right;
			current_idx = 0;
		}
	}
	// 
	//TODO END
	return count;
}

// 返回树的高度
long treeHeight(Tree* tree) {
	long height = 1;
	Node* root = tree->root;
	while (!root->isLeaf) {
		height++;
		root = root->child[0];
	}
	return height;
}

long insertToTree(Tree* tree, long key, long value) {
	if (insertToNode(tree->root, key, value) == -1) {
		return -1;
	}

	// 检查是否需要分裂根节点
	if (tree->root->n > MAX_KEY) {
		Node* old_root = tree->root;
		Node* new_root = (Node*)malloc(sizeof(Node));
		new_root->isLeaf = 0;
		new_root->n = 0;
		new_root->parent = NULL;
		new_root->right = NULL;
		new_root->child = (Node**)malloc(sizeof(Node*) * (MAX_CHILD + 1));
		new_root->child[0] = old_root;
		old_root->parent = new_root;
		splitNode(new_root, 0);
		tree->root = new_root;
	}

	tree->size++;

	return 0;
}

long delFromTree(Tree* tree, long key) {
	if (delFromNode(tree->root, key) == -1) {
		return -1;
	}

	// 检查是否需要替换根节点
	if (tree->root->n == 0 && !tree->root->isLeaf) {
		//TODO STEP 6：将根节点的唯一子节点作为新的根节点，然后删除原根节点的数据
		// 
		Node* old_root = tree->root;    // 为例防止内存泄漏，先把根指针复制一下
		tree->root = tree->root->child[0];
		tree->root->parent = NULL;

		free(old_root->child);
		old_root->child = NULL;
		free(old_root);
		old_root = NULL;
		// 
		//TODO END
	}

	tree->size--;

	return 0;
}

/*
	供调试代码使用的函数
*/

// printTree的递归辅助函数。
void printTreeRec(Node* node, long level) {
	printf("level %ld\n", level);
	if (node->isLeaf) {
		for (long i = 0; i < node->n; i++) {
			printf("(%ld, %ld) ", node->key[i], node->data[i]);
		}
		printf("\n");
	} else {
		for (long i = 0; i < node->n; i++) {
			printf("%ld ", node->key[i]);
		}
		printf("\n");
		for (long i = 0; i <= node->n; i++) {
			printTreeRec(node->child[i], level * 10 + i);
		}
	}
}

// 打印树的结构，打印出来的level是一个整数，第一位固定是1，其余每一位表示当前node在对应层的第几棵子树里。
// 例如level=132，表示当前node是根节点的第3个子节点的第2个子节点。
void printTree(Tree* tree) {
	printf("----------start of tree----------\n");
	printTreeRec(tree->root, 1);
	printf("----------end of tree----------\n");
}

