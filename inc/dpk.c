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

#include "dpk.h"
#include "fastlz.h"

void
dpk_close(dpk_file* dpk)
{
    assert(dpk);
    if (!dpk->data)
        return;
    dpk_data_node* curr = dpk->data;
    dpk_data_node* last = dpk->data->left;
    while (1)
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

int
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

int
dpk_delete_data(dpk_file* dpk, const char* name)
{
    assert(name);
    assert(dpk);
    
    dpk_data_node* curr = dpk->data;
    dpk_data_node* last = dpk->data->left;
    while (1)
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

int
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

            int offset_offset = 0;
            dpk_data_header* uncomp_data_headers = 0;
            if (dpk->header.data_header_comp_size)
            {
                dpk_data_header* comp_data_headers = malloc(dpk->header.data_header_comp_size);
                fread(comp_data_headers, dpk->header.data_header_comp_size, 1, file);

                int uncomp_size = data_num * sizeof(dpk_data_header);
                uncomp_data_headers = malloc(uncomp_size);

                offset_offset = uncomp_size - dpk->header.data_header_comp_size;

                if (!fastlz_decompress(comp_data_headers, dpk->header.data_header_comp_size, uncomp_data_headers, uncomp_size))
                {
                    free(comp_data_headers);
                    free(uncomp_data_headers);
                    err = DPK_ER_HEADDECOMP;
                    goto end;
                }
                free(comp_data_headers);
            }

            dpk->header.data_num = 0; /*the value will be the same after dpk_add_data*/
            for (int i = 0; i < data_num; ++i)
            {
                dpk_data_header dataHd;

                if (uncomp_data_headers)
                {
                    dataHd = uncomp_data_headers[i];
                }
                else
                {
                    fread(&dataHd, sizeof(dpk_data_header), 1, file);
                }

                long tellPos = ftell(file); /*remember position*/

                fseek(file, dataHd.data_offset - offset_offset, SEEK_SET);
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
end:;
    fclose(file);
    return err;
}

int
dpk_save(dpk_file* dpk)
{
    dpk->header.magic = DPK_MAGIC;
    dpk->header.version = DPK_VERSION;
    dpk->header.data_num = dpk->header.data_num;

    FILE* file = fopen(dpk->file_name, "wb");
    if (!file)
        return DPK_ER_OPENSAVE;

    if (fwrite(&dpk->header, sizeof(dpk_header), 1, file) != 1)
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

    /*set data offset*/
    dpk_data_node* curr = dpk->data;
    dpk_data_node* last = dpk->data->left;
    while (1)
    {
        curr->header.data_offset = prev_data_position;
        prev_data_position = data_start_position + curr->header.comp_size;
        
     //   fwrite(&curr->header, sizeof(dpk_data_header), 1, file);

        if (curr == last)
            break;
        curr = curr->right;
    }

    /*compress headers*/
    unsigned char* dpk_data_array_compressed = 0;
    if (dpk->header.data_num > 10)
    {
        int data_headers_size = dpk->header.data_num * sizeof(dpk_data_header);
        unsigned char* dpk_data_array = malloc(data_headers_size);

        unsigned char* ptr = dpk_data_array;
        dpk_data_node* curr = dpk->data;
        dpk_data_node* last = dpk->data->left;
        while (1)
        {
            memcpy(ptr, (unsigned char*)curr, sizeof(dpk_data_header));
            ptr += sizeof(dpk_data_header);

            if (curr == last)
                break;
            curr = curr->right;
        }

        dpk_data_array_compressed = malloc(data_headers_size + (data_headers_size / 2));

        dpk->header.data_header_comp_size = fastlz_compress_level(1, dpk_data_array, data_headers_size, dpk_data_array_compressed);
        //printf("FastLZ: %i : %i\n", dpk->header.data_header_comp_size, data_headers_size);
        if (dpk->header.data_header_comp_size > data_headers_size)
        {
            dpk->header.data_header_comp_size = 0;
            free(dpk_data_array_compressed);
            dpk_data_array_compressed = 0;
        }
        free(dpk_data_array);
    }


    fseek(file, 0, SEEK_SET);
    fwrite(&dpk->header, sizeof(dpk_header), 1, file);
    
    /*write headers*/
    if (dpk_data_array_compressed)
    {
        fwrite(dpk_data_array_compressed, dpk->header.data_header_comp_size, 1, file);
        free(dpk_data_array_compressed);
    }
    else
    {
        curr = dpk->data;
        last = dpk->data->left;
        while (1)
        {
            fwrite(&curr->header, sizeof(dpk_data_header), 1, file);

            if (curr == last)
                break;
            curr = curr->right;
        }
    }


    /*write data*/
    curr = dpk->data;
    last = dpk->data->left;
    while (1)
    {
        fwrite(curr->data, curr->header.comp_size, 1, file);

        if (curr == last)
            break;
        curr = curr->right;
    }

    fclose(file);
    return DPK_ER_OK;
}

