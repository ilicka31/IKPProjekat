#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    void* key;
    void* value;
} PairValue;
typedef struct
{
    PairValue** data;
    size_t length;
    size_t size;
} HashMap;

// Metoda inicijalizacije HashMap
void HashInit(HashMap* array)
{
    array->length = 0;
    array->size = 5;      // promeniti kasnije po potrebi
    array->data =(PairValue**) malloc(sizeof(PairValue*) * array->size);
}

// Dodavanje novog para u HashMapu, takodje povecavanje HashMap-e po potrebi
void HashAdd(HashMap* array, PairValue* value)
{
    array->data[array->length] = value;
    array->length++;
    if (array->length == array->size)
    {
        array->size += 5;
        array->data =(PairValue**) realloc(array->data, sizeof(PairValue) * array->size);
    }
}

// Oslobadjanje prosledjenog indeksa
void HashFreeIndex(HashMap* array, size_t index)
{
    if (index <= array->length)
    {
        free(array->data[index]->key);
        free(array->data[index]->value);
        free(array->data[index]);
    }
}

// Oslobadjanje cele mape
void HashFree(HashMap* array)
{
    for (size_t index = 0; index < array->length; index++)
    {
        // HashFreeIndex(array,index);
        free(array->data[index]->key);
        free(array->data[index]->value);
        free(array->data[index]);
    }
}

// Logicko brisanje prosledjenog indeksa
int HashDeleteIndex(HashMap* array, size_t index)
{
    if (index <= array->length)
    {
        array->data[index] = NULL;
        return 1;
    }
    return 0;
}

// Logicko brisanje cele mape
void HashDelete(HashMap* array)
{
    for (size_t index = 0; index < array->length; index++)
    {
        array->data[index] = NULL;
    }
    array->length = 0;
}

// Trazenje odredjenog para po indeksu
PairValue* HashGetIndex(HashMap* array, size_t index)
{
    if (index <= array->length)
    {
        return array->data[index];
    }
    return NULL;
}

// Trazenje odredjenog para po kljucu iz objekta PairValue
PairValue* HashGetWithKey(HashMap* array, void* key)
{
    for (size_t index = 0; index < array->length; index++)
    {
        PairValue* item = array->data[index];
        if (item != NULL && item->key == key)
        {
            return item;
        }
    }
    return NULL;
}

// Vracanje value parametra objekta PairValue po key-u
void* HashGetValue(HashMap* array, void* key)
{
    for (size_t index = 0; index < array->length; index++)
    {
        PairValue* item = array->data[index];
        if (item != NULL && item->key == key)
        {
            return item->value;
        }
    }
    return NULL;
}

