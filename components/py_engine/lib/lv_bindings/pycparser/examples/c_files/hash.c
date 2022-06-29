/*
** C implementation of a hash table ADT
*/
typedef enum tagReturnCode {SUCCESS, FAIL} ReturnCode;


typedef struct tagEntry
{
    char* key;
    char* value;
} Entry;



typedef struct tagNode
{
    Entry* entry;

    struct tagNode* next;
} Node;


typedef struct tagHash
{
    unsigned int table_size;

    Node** heads; 

} Hash;


static unsigned int hash_func(const char* str, unsigned int table_size)
{
    unsigned int hash_value;
    unsigned int a = 127;

    for (hash_value = 0; *str != 0; ++str)
        hash_value = (a*hash_value + *str) % table_size;

    return hash_value;
}


ReturnCode HashCreate(Hash** hash, unsigned int table_size)
{
    unsigned int i;

    if (table_size < 1)
        return FAIL;

    //
    // Allocate space for the Hash
    //
    if (((*hash) = malloc(sizeof(**hash))) == NULL)
        return FAIL;

    //
    // Allocate space for the array of list heads
    //
    if (((*hash)->heads = malloc(table_size*sizeof(*((*hash)->heads)))) == NULL)
        return FAIL;

    //
    // Initialize Hash info
    //
    for (i = 0; i < table_size; ++i)
    {
        (*hash)->heads[i] = NULL;
    }

    (*hash)->table_size = table_size;

    return SUCCESS;
}


ReturnCode HashInsert(Hash* hash, const Entry* entry)
{
    unsigned int index = hash_func(entry->key, hash->table_size);
    Node* temp = hash->heads[index];

    HashRemove(hash, entry->key);

    if ((hash->heads[index] = malloc(sizeof(Node))) == NULL)
        return FAIL;

    hash->heads[index]->entry = malloc(sizeof(Entry));
    hash->heads[index]->entry->key = malloc(strlen(entry->key)+1);
    hash->heads[index]->entry->value = malloc(strlen(entry->value)+1);
    strcpy(hash->heads[index]->entry->key, entry->key);
    strcpy(hash->heads[index]->entry->value, entry->value);

    hash->heads[index]->next = temp;

    return SUCCESS;
}



const Entry* HashFind(const Hash* hash, const char* key)
{
    unsigned int index = hash_func(key, hash->table_size);
    Node* temp = hash->heads[index];

    while (temp != NULL)
    {
        if (!strcmp(key, temp->entry->key))
            return temp->entry;

        temp = temp->next;
    }

    return NULL;
}


ReturnCode HashRemove(Hash* hash, const char* key)
{
    unsigned int index = hash_func(key, hash->table_size);
    Node* temp1 = hash->heads[index];
    Node* temp2 = temp1;

    while (temp1 != NULL)
    {
        if (!strcmp(key, temp1->entry->key))
        {
            if (temp1 == hash->heads[index])
                hash->heads[index] = hash->heads[index]->next;
            else
                temp2->next = temp1->next;

            free(temp1->entry->key);
            free(temp1->entry->value);
            free(temp1->entry);
            free(temp1);
            temp1 = NULL;

            return SUCCESS;
        }
        
        temp2 = temp1;
        temp1 = temp1->next;
    }

    return FAIL;
}


void HashPrint(Hash* hash, void (*PrintFunc)(char*, char*))
{
    unsigned int i;

    if (hash == NULL || hash->heads == NULL)
        return;

    for (i = 0; i < hash->table_size; ++i)
    {
        Node* temp = hash->heads[i];

        while (temp != NULL)
        {
            PrintFunc(temp->entry->key, temp->entry->value);
            temp = temp->next;
        }
    }
}



void HashDestroy(Hash* hash)
{
    unsigned int i;

    if (hash == NULL)
        return;

    for (i = 0; i < hash->table_size; ++i)
    {
        Node* temp = hash->heads[i];

        while (temp != NULL)
        {
            Node* temp2 = temp;

            free(temp->entry->key);
            free(temp->entry->value);
            free(temp->entry);

            temp = temp->next;
            
            free(temp2);
        }
    }    

    free(hash->heads);
    hash->heads = NULL;

    free(hash);
}

