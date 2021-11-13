/*
  Copyright (C) 2021 Basov Artyom
  The authors can be contacted at <artembasov@outlook.com>
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
  3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.
  THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _DPK_H_
#define _DPK_H_

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/*
	Debuggerfall Pack

    Modifying file: read all data in memory, rewrite all file.

    File structure:
    dpk_header
    dpk_data_header
    dpk_data_header
    dpk_data_header
    ...
    dpk_data
    dpk_data
    dpk_data
    ...
*/

#define DPK_MAGIC 4935748
#define DPK_VERSION 1
#define DPK_DATANAMESIZE 11

#define DPK_CMP_NOCOMPRESS 0
#define DPK_CMP_DEFLATE 1

typedef struct dpk_header_s {
    int magic;
    int version;
    int data_num;
} dpk_header;

typedef struct dpk_data_header_s {
    unsigned int comp_size;   /*It's data size, even if not compressed*/
    unsigned int uncomp_size; 
    unsigned int data_offset; /*offset from beginning*/
    unsigned char compression; /*DPK_CMP...*/
    char name[DPK_DATANAMESIZE]; /*can be without \0 at the end*/
} dpk_data_header;

typedef struct dpk_data_node_s {
    dpk_data_header header;
    void* data;
    struct dpk_data_node_s* left;
    struct dpk_data_node_s* right;
} dpk_data_node;

typedef struct dpk_file_s {
    dpk_header header;
    dpk_data_node* data;
    char file_name[256];
} dpk_file;

#define DPK_ER_OK 0
#define DPK_ER_BADARG 1 /*some argument is not valid*/
#define DPK_ER_NEWFILE 2 /*can't create new file*/
#define DPK_ER_WRITE 3 /*write error*/
#define DPK_ER_OPENSAVE 4 /*can't open file for save*/
#define DPK_ER_WRITENODATA 5 /*write error*/
#define DPK_ER_OPENREAD 6 /*can't open file for read*/

/* You must call this if you called dpk_open before. */
inline void
dpk_close(dpk_file* dpk)
{
    assert(dpk);
    if (!dpk->data)
        return;
    dpk_data_node* curr = dpk->data;
    dpk_data_node* last = dpk->data->left;
    while (true)
    {
        dpk_data_node* next = curr->right;
        if (curr->data)
            free(curr->data);
        free(curr);
        if (curr == last)
            break;
        curr = next;
    }
    dpk->data = 0;
}

/* Add data to dpk.
* dpk - dpk_file structure
* data - file data
* uncomp_size - uncompressed size
* comp_size - compressed size or uncompressed size. This us 'void* data' size.
* compression - compression type. Just for information, implement compression by yourself. 
*               See DPK_CMP_ for values.
* name - unique name with maximum DPK_DATANAMESIZE chars (can be without \0).
*        Check unique name by yourself.
* 
* If compression == DPK_CMP_NOCOMPRESS then comp_size must be == uncomp_size
* For saving you need to call dpk_save
*/
inline int
dpk_add_data(
    dpk_file* dpk, 
    void* data, 
    int uncomp_size, 
    int comp_size, 
    unsigned char compression, 
    const char * name)
{
    assert(name);
    assert(dpk);
    
    size_t str_len = strlen(name);
    if(!str_len)
        return DPK_ER_BADARG;

    if (str_len > DPK_DATANAMESIZE)
        str_len = DPK_DATANAMESIZE;

    dpk_data_header header;
    memset(&header, 0, sizeof(dpk_data_header));
    header.compression = compression;
    header.comp_size = comp_size;
    header.uncomp_size = uncomp_size;

    if (str_len)
        memcpy(header.name, name, str_len);

    dpk_data_node* new_node = (dpk_data_node*)malloc(sizeof(dpk_data_node));
    memset(new_node, 0, sizeof(dpk_data_node));
    new_node->header = header;
    new_node->data = malloc(comp_size);
    memcpy(new_node->data, data, comp_size);

    if (dpk->data)
    {
        dpk_data_node* last = dpk->data->left;
        last->right = new_node;
        new_node->left = last;
        new_node->right = dpk->data;
        dpk->data->left = new_node;
    }
    else
    {
        dpk->data = new_node;
        dpk->data->left = new_node;
        dpk->data->right = new_node;
    }

    dpk->header.data_num++;
    return DPK_ER_OK;
}

/* Delete data by name */
inline int
dpk_delete_data(dpk_file* dpk, const char* name)
{
    assert(name);
    assert(dpk);
    
    dpk_data_node* curr = dpk->data;
    dpk_data_node* last = dpk->data->left;
    while (true)
    {
        if (strcmp(curr->header.name, name) == 0)
        {
            if (curr->data)
                free(curr->data);

            dpk_data_node* del = curr;

            curr->left->right = curr->right;
            curr->right->left = curr->left;

            if (curr == dpk->data)
                dpk->data = curr->right;

            if (curr == dpk->data)
                dpk->data = 0;

            free(del);

            break;
        }

        if (curr == last)
            break;
        curr = curr->right;
    }


    return DPK_ER_OK;
}

/* Open .dpk or create new. Call dpc_close for free memory. */
inline int
dpk_open(const char* file_name, dpk_file* dpk)
{
    assert(file_name);
    assert(dpk);

    memset(dpk->file_name, 0, 256);
    dpk->data = 0;
    dpk->header.data_num = 0;

    int file_name_size = strlen(file_name);
    if (file_name_size > 0xff)
        return DPK_ER_BADARG;

    memcpy(&dpk->file_name[0], file_name, file_name_size);
    dpk->file_name[file_name_size] = 0;

    int err = DPK_ER_OK;

    FILE* file = fopen(file_name, "rb");
    if (file)
    {
        if (fread(&dpk->header, sizeof(dpk_header), 1, file) == 1)
        {
            int data_num = dpk->header.data_num; 
            dpk->header.data_num = 0; /*the value will be the same after dpk_add_data*/
            for (int i = 0; i < data_num; ++i)
            {
                dpk_data_header dataHd;
                fread(&dataHd, sizeof(dpk_data_header), 1, file);
                long tellPos = ftell(file); /*remember position*/

                fseek(file, dataHd.data_offset, SEEK_SET);
                if (dataHd.comp_size)
                {
                    void* data = malloc(dataHd.comp_size);
                    fread(data, dataHd.comp_size, 1, file);
                    dpk_add_data(dpk, data, dataHd.uncomp_size, dataHd.comp_size, dataHd.compression, dataHd.name);
                    free(data);
                }

                fseek(file, tellPos, SEEK_SET); 
            }
        }
        else
        {
            err = DPK_ER_OPENREAD;
        }
    }
    else
    {
        file = fopen(file_name, "wb");
        if (!file)
            return DPK_ER_NEWFILE;
    }
    fclose(file);
    return err;
}

/* Save everything to file. */
inline int
dpk_save(dpk_file* dpk)
{
    dpk_header file_header;
    file_header.magic = DPK_MAGIC;
    file_header.version = DPK_VERSION;
    file_header.data_num = dpk->header.data_num;

    FILE* file = fopen(dpk->file_name, "wb");
    if (!file)
        return DPK_ER_OPENSAVE;

    if (fwrite(&file_header, sizeof(dpk_header), 1, file) != 1)
    {
        fclose(file);
        return DPK_ER_WRITE;
    }

    if (!dpk->data || !dpk->header.data_num)
    {
        fclose(file);
        return DPK_ER_WRITENODATA;
    }

    /*first dpk->data*/
    size_t data_start_position = sizeof(dpk_header) + (dpk->header.data_num * sizeof(dpk_data_header));
    size_t prev_data_position = data_start_position;

    /*write data headers*/
    dpk_data_node* curr = dpk->data;
    dpk_data_node* last = dpk->data->left;
    while (true)
    {
        curr->header.data_offset = prev_data_position;
        prev_data_position = data_start_position + curr->header.comp_size;
        
        fwrite(&curr->header, sizeof(dpk_data_header), 1, file);

        if (curr == last)
            break;
        curr = curr->right;
    }

    /*write data*/
    curr = dpk->data;
    last = dpk->data->left;
    while (true)
    {
        fwrite(curr->data, curr->header.comp_size, 1, file);

        if (curr == last)
            break;
        curr = curr->right;
    }

    fclose(file);
    return DPK_ER_OK;
}

#endif