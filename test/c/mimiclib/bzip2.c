// Copyright 2022 The Wuffs Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "bzlib.h"

const char*  //
mimic_bzip2_decode(wuffs_base__io_buffer* dst,
                   wuffs_base__io_buffer* src,
                   uint32_t wuffs_initialize_flags,
                   uint64_t wlimit,
                   uint64_t rlimit) {
  if (wuffs_base__io_buffer__writer_length(dst) > 0x7FFFFFFF) {
    return "dst length is too large";
  } else if (wuffs_base__io_buffer__reader_length(src) > 0x7FFFFFFF) {
    return "src length is too large";
  } else if ((wlimit < UINT64_MAX) || (rlimit < UINT64_MAX)) {
    // Supporting this would probably mean using BZ2_bzDecompress instead of
    // the simpler BZ2_bzBuffToBuffDecompress function.
    return "unsupported I/O limit";
  }
  unsigned int dlen = wuffs_base__io_buffer__writer_length(dst);
  unsigned int slen = wuffs_base__io_buffer__reader_length(src);
  int err = BZ2_bzBuffToBuffDecompress(
      ((char*)wuffs_base__io_buffer__writer_pointer(dst)), &dlen,
      ((char*)wuffs_base__io_buffer__reader_pointer(src)), slen, 0, 0);
  if (err != BZ_OK) {
    return "libbz2: an error occurred";
  } else if (dlen > wuffs_base__io_buffer__writer_length(dst)) {
    return "libbz2: dst buffer overflow";
  }
  dst->meta.wi += dlen;
  return NULL;
}
