// Copyright 2012 Google Inc. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the COPYING file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS. All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.
// -----------------------------------------------------------------------------
//
//  Utility functions used by the example programs.
//

#include "./example_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -----------------------------------------------------------------------------
// File I/O

static const size_t kBlockSize = 16384;  // default initial size

int ExUtilReadFromStdin(const uint8_t** data, size_t* data_size) {
  size_t max_size = 0;
  size_t size = 0;
  uint8_t* input = NULL;

  if (data == NULL || data_size == NULL) return 0;
  *data = NULL;
  *data_size = 0;

  while (!feof(stdin)) {
    // We double the buffer size each time and read as much as possible.
    const size_t extra_size = (max_size == 0) ? kBlockSize : max_size;
    void* const new_data = realloc(input, max_size + extra_size);
    if (new_data == NULL) goto Error;
    input = (uint8_t*)new_data;
    max_size += extra_size;
    size += fread(input + size, 1, extra_size, stdin);
    if (size < max_size) break;
  }
  if (ferror(stdin)) goto Error;
  *data = input;
  *data_size = size;
  return 1;

 Error:
  free(input);
  fprintf(stderr, "Could not read from stdin\n");
  return 0;
}

int ExUtilReadFile(const char* const file_name,
                   const uint8_t** data, size_t* data_size) {
  int ok;
  void* file_data;
  size_t file_size;
  FILE* in;
  const int from_stdin = (file_name == NULL) || !strcmp(file_name, "-");

  if (from_stdin) return ExUtilReadFromStdin(data, data_size);

  if (data == NULL || data_size == NULL) return 0;
  *data = NULL;
  *data_size = 0;

  in = fopen(file_name, "rb");
  if (in == NULL) {
    fprintf(stderr, "cannot open input file '%s'\n", file_name);
    return 0;
  }
  fseek(in, 0, SEEK_END);
  file_size = ftell(in);
  fseek(in, 0, SEEK_SET);
  file_data = malloc(file_size);
  if (file_data == NULL) return 0;
  ok = (fread(file_data, file_size, 1, in) == 1);
  fclose(in);

  if (!ok) {
    fprintf(stderr, "Could not read %d bytes of data from file %s\n",
            (int)file_size, file_name);
    free(file_data);
    return 0;
  }
  *data = (uint8_t*)file_data;
  *data_size = file_size;
  return 1;
}

int ExUtilWriteFile(const char* const file_name,
                    const uint8_t* data, size_t data_size) {
  int ok;
  FILE* out;
  const int to_stdout = (file_name == NULL) || !strcmp(file_name, "-");

  if (data == NULL) {
    return 0;
  }
  out = to_stdout ? stdout : fopen(file_name, "wb");
  if (out == NULL) {
    fprintf(stderr, "Error! Cannot open output file '%s'\n", file_name);
    return 0;
  }
  ok = (fwrite(data, data_size, 1, out) == 1);
  if (out != stdout) fclose(out);
  return ok;
}

