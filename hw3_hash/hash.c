#include "hash.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>


#define INVALID_INDEX -1
#define M 1024 // максимальный размер ключа
#define MAX_CAP_INDEX 29
static int cap[MAX_CAP_INDEX] = {
	5, 11, 23, 47, 97, 193, 389, 769, 1543, 3079,
	12289, 24593, 49157, 98317, 196613, 393241,
	786433, 1572869, 3145739, 6291469, 12582917,
	25165843, 50331653, 100663319, 201326611,
	402653189, 805306457, 1610612711, 2147483629
};

static const int P1 = 53;
static const int P2 = 17;
static long long p1_pow[M];
static long long p2_pow[M];

struct _Node {
	bool flag;
	char* key;
	int value;
};

struct _Hash {
	int capIndex;
	size_t size;
	struct _Node* nodes;
};




static void initP()
{
	p1_pow[0] = 1;
	p2_pow[0] = 1;
	for (int i = 1; i < M; ++i) {
		p1_pow[i] = p1_pow[i - 1] * P1;
		p2_pow[i] = p2_pow[i - 1] * P2;
	}
}

static void getIndexAndInterval(struct _Hash* t_hash, const char* t_str, int* t_index, int* t_interval)
{
	uint64_t sh1 = 0;
	uint64_t sh2 = 0;
	for (int i = 0; t_str[i] != '\0'; ++i) {
		int s = abs(t_str[i]) - 'a' + 1;
		sh1 += s * p1_pow[i];
		sh2 += s * p2_pow[i];
	}
	*t_index = sh1 % cap[t_hash->capIndex];
	*t_interval = (sh2 % (cap[t_hash->capIndex] - 1)) + 1;
}

static bool resize(struct _Hash* t_hash)
{
	if (t_hash->capIndex == MAX_CAP_INDEX) {
		return false;
	}
	struct _Node* newNodes = (struct _Node*)calloc(cap[t_hash->capIndex + 1], sizeof(struct _Node));
	if (newNodes == NULL) {
		return false;
	}
	t_hash->capIndex++;
	int index = 0;
	int interval = 0;
	for (int i = 0; i < cap[t_hash->capIndex - 1]; ++i) {
		if (t_hash->nodes[i].flag) {
			getIndexAndInterval(t_hash, t_hash->nodes[i].key, &index, &interval);
			while (newNodes[index].flag) {
				index = (index + interval) % cap[t_hash->capIndex];
			}
			memmove(&newNodes[index], &t_hash->nodes[i], sizeof(struct _Node));
		}
	}
	free(t_hash->nodes);
	t_hash->nodes = newNodes;

	return true;
}

static int search(struct _Hash* t_hash, const char* t_key, int t_index, int t_interval)
{
	while (t_hash->nodes[t_index].key != NULL) {
		if (strcmp(t_hash->nodes[t_index].key, t_key) == 0) {
			return t_index;
		}
		t_index = (t_index + t_interval) % cap[t_hash->capIndex];
	}

	return INVALID_INDEX;
}




struct _Hash* createHash()
{
	static bool isInit = false;
	if (!isInit) {
		initP();
		isInit = true;
	}

	struct _Hash* h = (struct _Hash*) malloc(sizeof(struct _Hash));
	if (h == NULL) {
		return NULL;
	}
	h->capIndex = 0;
	h->size = 0;
	h->nodes = (struct _Node*) calloc(cap[h->capIndex], sizeof(struct _Node));
	if (h->nodes == NULL) {
		free(h);
		return NULL;
	}
	// у каждой _Node key равен NULL

	return h;
}

void removeHash(struct _Hash* t_hash)
{
	for (int i = 0; i < cap[t_hash->capIndex]; ++i) {
		free(t_hash->nodes[i].key);
	}
	free(t_hash->nodes);
	free(t_hash);
}


bool hashInsert(struct _Hash* t_hash, const char* t_key, int t_value)
{
	if (((double)t_hash->size) / cap[t_hash->capIndex] >= 0.75) {
		resize(t_hash);
	}
	if (fabs(((double)t_hash->size) / cap[t_hash->capIndex] - 1) < 1e-5) {
		return false;
	}

	int index = 0;
	int interval = 0;
	getIndexAndInterval(t_hash, t_key, &index, &interval);

	int resInd = search(t_hash, t_key, index, interval);
	if (resInd != INVALID_INDEX) {
		t_hash->nodes[resInd].value = t_value;
		return true;
	}

	while (t_hash->nodes[index].flag) {
		index = (index + interval) % cap[t_hash->capIndex];
	}

	struct _Node* node = &t_hash->nodes[index];
	node->key = (char*) malloc(strlen(t_key) + 1);
	if (node->key == NULL) {
		return false;
	}
	node->flag = true;
	strcpy(node->key, t_key);
	node->value = t_value;
	t_hash->size++;

	return true;
}

void hashDeleteElem(struct _Hash* t_hash, const char* t_key)
{
	int index = 0;
	int interval = 0;
	getIndexAndInterval(t_hash, t_key, &index, &interval);

	int resInd = search(t_hash, t_key, index, interval);

	if (resInd == INVALID_INDEX) {
		return;
	}

	struct _Node* node = &t_hash->nodes[resInd];
	free(node->key);
	node->flag = false;
	t_hash->size--;
	if (t_hash->nodes[(resInd + interval) % cap[t_hash->capIndex]].key == NULL) {
		node->key = NULL;
	}
}

int* hashValue(struct _Hash* t_hash, const char* t_key)
{
	int index = 0;
	int interval = 0;
	getIndexAndInterval(t_hash, t_key, &index, &interval);

	int resInd = search(t_hash, t_key, index, interval);

	if (resInd == INVALID_INDEX) {
		if (((double)t_hash->size) / cap[t_hash->capIndex] >= 0.75 && resize(t_hash)) {
			getIndexAndInterval(t_hash, t_key, &index, &interval);
		}
		if (fabs(((double)t_hash->size) / cap[t_hash->capIndex] - 1) < 1e-5) {
			return NULL;
		}

		while (t_hash->nodes[index].flag) {
			index = (index + interval) % cap[t_hash->capIndex];
		}

		struct _Node* node = &t_hash->nodes[index];
		node->key = (char*) malloc(strlen(t_key) + 1);
		if (node->key == NULL) {
			return NULL;
		}
		node->flag = true;
		strcpy(node->key, t_key);
		node->value = 0;
		t_hash->size++;

		resInd = index;
	}

	return &t_hash->nodes[resInd].value;
}

void hashPrint(struct _Hash* t_hash)
{
	for (int i = 0; i < cap[t_hash->capIndex]; ++i) {
		if (t_hash->nodes[i].flag) {
			printf("%s - %d\n", t_hash->nodes[i].key, t_hash->nodes[i].value);
		}
	}
}

