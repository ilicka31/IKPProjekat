#pragma once

#include <stdlib.h>
#include <string.h>

#define EMPTY	2
#define FULL	1
#define OK	    0
#define ERROR	-1

/*
** Ring buffer struktura
** Sadrzi:
** - pokazivac na pravi buffer u memoriji
** - pokazivac na kraj buffer-a u memoriji
** - pokazivac na pocetak podataka
** - pokazivac na kraj podataka
** - broj podataka
*/
typedef struct	s_ringbuffer
{
    char* buffer;
    unsigned int	start;
    unsigned int	end;
    size_t	count;
    size_t	max;
    size_t	cap;
}		t_ringbuffer;

/*
** Inicijalizacija ring buffer-a
** size_t max broj slotova koji ce se stvoriti
** size_t size velicina jednog slota
*/
t_ringbuffer* init_buffer(size_t max, size_t size)
{
    t_ringbuffer* buffer;

    if ((buffer =(t_ringbuffer*) malloc(sizeof(t_ringbuffer))) == NULL)
        return (NULL);
    if ((buffer->buffer =(char*) malloc(sizeof(char) * (max * size))) == NULL)
        return (NULL);
    buffer->max = max;
    buffer->cap = size;
    buffer->count = 0;
    buffer->end = 0;
    buffer->start = 0;
    return (buffer);
}

// Dodavanje stringa (niza karaktera) u slot buffer-a ako je moguce
void add_item(t_ringbuffer* buffer, char* item)
{
    unsigned int	i;
    unsigned int	next;

    if (!buffer)
        return;
    if (buffer->count >= buffer->max)
        return;
    for (i = 0; i < strlen(item); ++i)
    {
        if (item[i] == '\n')
            buffer->count += 1;
        next = (unsigned int)(buffer->start + 1) % (buffer->cap * buffer->max);
        if (next != buffer->end)
        {
            buffer->buffer[buffer->start] = item[i];
            buffer->start += 1;
        }
    }
}

/*
** Vraca string do prosledjenog delimitera
*/
char* get_item(t_ringbuffer* buf, char delim)
{
    unsigned int	idx;
    unsigned int	size;
    char* tmp;

    if (buffer_status(buf) == EMPTY)
        return (NULL);
    size = 0;
    for (idx = buf->end; buf->buffer[idx] != delim
        && idx < buf->max * buf->cap; ++idx)
        size++;
    if (size == buf->max * buf->cap)
        return (NULL);
    if ((tmp =(char*) malloc(sizeof(char) * (size + 1))) == NULL)
        return (NULL);
    for (idx = 0; idx < size; ++idx)
    {
        tmp[idx] = buf->buffer[buf->end];
        buf->end = (unsigned int)(buf->end + 1 % (buf->max * buf->cap));
    }
    tmp[idx] = '\0';
    if (buf->buffer[buf->end] == delim)
        buf->end = (unsigned int)(buf->end + 1 % (buf->max * buf->cap));
    buf->count -= 1;
    return (tmp);
}

void free_buffer(t_ringbuffer* buf)
{
    if (buf->buffer)
        free(buf->buffer);
    free(buf);
}

// Proverava status buffer-a
int buffer_status(t_ringbuffer* buf)
{
    if (!buf)
        return (ERROR);
    if (buf->count == 0)
        return (EMPTY);
    else if (buf->start + 1 == buf->end)
        return (FULL);
    else
        return (OK);
    return (OK);
}

