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
#define DPK_DATANAMESIZE 15

#define DPK_CMP_NOCOMPRESS 0
#define DPK_CMP_DEFLATE 1

typedef struct dpk_header_s {
    int magic;
    int version;
    int data_num;
    int data_header_comp_size; /*if 0 then not compressed*/
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
#define DPK_ER_HEADDECOMP 7 /*headers decompress*/

#if defined(__cplusplus)
extern "C" {
#endif

/* You must call this if you called dpk_open before. */
void dpk_close(dpk_file* dpk);

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
int dpk_add_data(dpk_file* dpk, void* data, int uncomp_size, int comp_size, unsigned char compression, const char* name);

/* Delete data by name */
int dpk_delete_data(dpk_file* dpk, const char* name);

/* Open .dpk or create new. Call dpc_close for free memory. */
int dpk_open(const char* file_name, dpk_file* dpk);

/* Save everything to file. */
int dpk_save(dpk_file* dpk);

#if defined(__cplusplus)
}
#endif

#endif