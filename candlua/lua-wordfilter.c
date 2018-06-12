#include <lua.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>


#define WORD_NUM          256


//#pragma  pack(1)
struct trie_node {
	struct trie_node *node[WORD_NUM];
	int exist;
};
//#pragma  pack()

static int l_new(lua_State *L) {
	struct trie_node *root = calloc(1, sizeof(struct trie_node));
	lua_pushlightuserdata(L, (void*)root);
	//printf("l_new - node size:%ld\n", sizeof(struct trie_node)); 2056 = 8*256 +  8
	return 1;
}

static int l_addWord(lua_State *L) {
	struct trie_node *root = (struct trie_node *)lua_touserdata(L, 1);
	int size=0,i;
	unsigned char * word = (unsigned char *)luaL_checklstring(L,2,(size_t *)&size);
	struct trie_node *n = root;
	
	for(i=0;i<size;i++)
	{
		//printf("l_addWord - size:%d,word:%s,word:%d,node:%p\n", size, word,word[i], n->node[word[i]]);
		if (n->node[word[i]] == NULL) {
			n->node[word[i]] = calloc(1, sizeof(struct trie_node));
		}
		n = n->node[word[i]];
	}
	n->exist = 1;
	
	return 0;
}


static int l_hasMatch(lua_State *L)
{
	//printf("lua_islightuserdata(L, 1):%d,luaL_typename(L, 1):%s\n", lua_islightuserdata(L, 1), luaL_typename(L, 1));

	struct trie_node *root = (struct trie_node *)lua_touserdata(L, 1);
	char *word = (char *)luaL_checklstring(L, 2, NULL);
	struct trie_node *n = NULL;
	unsigned char *p = NULL;
	
	if (root == NULL) {
		lua_pushboolean(L, 0);
		return 1;
	}
	
	while (*word != 0) {
		p = (unsigned char *)word++;
		n = root;
		while (*p != 0) {
			n = n->node[*p];
			if (n == NULL) {
				break;
			}
			else if (n->exist == 1) {
				lua_pushboolean(L, 1);
				return 1;
			}
			p++;
		}
	}
		
	lua_pushboolean(L, 0);
	return 1;
}


static int l_doFiltering(lua_State *L)
{
	struct trie_node *root = (struct trie_node *)lua_touserdata(L, 1);
	int size;
	char *word = (char *)luaL_checklstring(L, 2, (size_t *)&size);
	struct trie_node *n = NULL;
	unsigned char *p = NULL;
	
	if (root == NULL) {
		lua_pushlstring(L, word, (size_t)size);
		return 1;
	}
	
	char *str = calloc(size+1,1);
	int newSize = 0;
	
	while (*word != 0) {
		p = (unsigned char *)word;
		n = root;
		while (*p != 0) {
			n = n->node[*p];
			if (n == NULL) {
				str[newSize] = *word;
				newSize++;
				break;
			}
			else if (n->exist == 1) {
				str[newSize] = '*';
				newSize++;
				word = (char *)p;
				break;
			}
			p++;
		}
		word++;
	}
		
	lua_pushlstring(L, str, (size_t)(newSize+1));
	free(str);
	return 1;
}


void copy_trie_tree(struct trie_node *root, struct trie_node **newRoot) 
{
	int i;
	if (root == NULL) {
		return;
	}
	(*newRoot) = calloc(1, sizeof(struct trie_node));
	(*newRoot)->exist = root->exist;
	for (i = 0; i < WORD_NUM; i++) {
		copy_trie_tree(root->node[i], &((*newRoot)->node[i]));
	}
}

static int l_copy(lua_State *L) {
	struct trie_node *root = (struct trie_node *)lua_touserdata(L, 1);
	
	struct trie_node *n;
	copy_trie_tree(root, &n);
	
	lua_pushlightuserdata(L, (void*)n);
	return 1;
}


void destroy_trie_tree(struct trie_node *root) 
{
	int i;
	if (root == NULL) {
		return;
	}
	for (i = 0; i < WORD_NUM; i++) {
		destroy_trie_tree(root->node[i]);
	}
	free(root);
}

static int l_destroy(lua_State *L) {
	struct trie_node *root = (struct trie_node *)lua_touserdata(L, 1);
	
	destroy_trie_tree(root);
	
	return 0;
}


int luaopen_wordfilter(lua_State *L) {
	luaL_checkversion(L);
	
	luaL_Reg l[] = {
		{ "addWord", l_addWord },
		{ "hasMatch", l_hasMatch },
		{ "doFiltering", l_doFiltering },
		{ "copy", l_copy },
		{ "destroy", l_destroy },
		{ "new", l_new },
		{ NULL, NULL },
	};
	luaL_newlib(L,l);

	return 1;
}
