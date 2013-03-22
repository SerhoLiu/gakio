#include <stdlib.h>
#include <string.h>
#include "tokenadt.h"
#include "gakio.h"

tokenadt *token_adt_new(size_t size)
{
    tokenadt *tokens;
    
    tokens = malloc(sizeof(tokenadt));
    if (tokens == NULL) {
        return NULL;
    }

    tokens->size = size;
    tokens->use = 0;
    tokens->items = (token *)malloc(tokens->size * sizeof(token));
    if (tokens->items == NULL) {
        free(tokens);
        return NULL;
    }
    memset(tokens->items, 0, tokens->size * sizeof(token)); 
    return tokens;
}

void token_adt_reset(tokenadt *tokens)
{
    if (tokens == NULL) {
        return;
    }

    tokens->use = 0;
}


void token_adt_delete(tokenadt *tokens)
{
    if (tokens == NULL) {
        return;
    }
    free(tokens->items);
    free(tokens);
}

int token_adt_append(tokenadt *tokens, token *item)
{
    int i = 0;
    token *t;

    if (tokens == NULL) {
        return 0;
    }
    if (tokens->size == tokens->use) {
        return 0;
    }

    i = tokens->use;
    t = &(tokens->items[i]);
    t->code = item->code;
    t->value = item->value;
    tokens->use++;
    return 1;
}