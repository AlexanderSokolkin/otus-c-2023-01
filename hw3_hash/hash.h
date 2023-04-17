/*
* структура данных hash - хэш-таблица в которой
* ключ - строка
* значение - целое число
*/
#pragma once
#include <stdbool.h>



struct _Hash;
typedef struct _Hash* hash;


hash createHash();
void removeHash(hash t_hash);

bool hashInsert		(hash t_hash, const char* t_key, int t_value);
void hashDeleteElem	(hash t_hash, const char* t_key);
int* hashValue		(hash t_hash, const char* t_key);
void hashPrint		(hash t_hash);
