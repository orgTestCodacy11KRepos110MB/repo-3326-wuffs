#ifndef PUFFS_FLATE_H
#define PUFFS_FLATE_H

// Code generated by puffs-c. DO NOT EDIT.

#ifndef PUFFS_BASE_HEADER_H
#define PUFFS_BASE_HEADER_H

// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// Puffs requires a word size of at least 32 bits because it assumes that
// converting a u32 to usize will never overflow. For example, the size of a
// decoded image is often represented, explicitly or implicitly in an image
// file, as a u32, and it is convenient to compare that to a buffer size.
//
// Similarly, the word size is at most 64 bits because it assumes that
// converting a usize to u64 will never overflow.
#if __WORDSIZE < 32
#error "Puffs requires a word size of at least 32 bits"
#elif __WORDSIZE > 64
#error "Puffs requires a word size of at most 64 bits"
#endif

// PUFFS_VERSION is the major.minor version number as a uint32. The major
// number is the high 16 bits. The minor number is the low 16 bits.
//
// The intention is to bump the version number at least on every API / ABI
// backwards incompatible change.
//
// For now, the API and ABI are simply unstable and can change at any time.
//
// TODO: don't hard code this in base-header.h.
#define PUFFS_VERSION (0x00001)

// puffs_base_slice_u8 is a 1-dimensional buffer (a pointer and length).
//
// A value with all fields NULL or zero is a valid, empty slice.
typedef struct {
  uint8_t* ptr;
  size_t len;
} puffs_base_slice_u8;

// puffs_base_buf1 is a 1-dimensional buffer (a pointer and length), plus
// additional indexes into that buffer, plus an opened / closed flag.
//
// A value with all fields NULL or zero is a valid, empty buffer.
typedef struct {
  uint8_t* ptr;  // Pointer.
  size_t len;    // Length.
  size_t wi;     // Write index. Invariant: wi <= len.
  size_t ri;     // Read  index. Invariant: ri <= wi.
  bool closed;   // No further writes are expected.
} puffs_base_buf1;

// puffs_base_buf1_mark marks a position (pointer + offset) in a
// puffs_base_buf1.
typedef struct {
  uint8_t* ptr;
} puffs_base_buf1_mark;

// puffs_base_limit1 provides a limited view of a 1-dimensional byte stream:
// its first N bytes. That N can be greater than a buffer's current read or
// write capacity. N decreases naturally over time as bytes are read from or
// written to the stream.
//
// A value with all fields NULL or zero is a valid, unlimited view.
typedef struct puffs_base_limit1 {
  uint64_t* ptr_to_len;            // Pointer to N.
  struct puffs_base_limit1* next;  // Linked list of limits.
} puffs_base_limit1;

typedef struct {
  puffs_base_buf1* buf;
  puffs_base_limit1 limit;
} puffs_base_reader1;

typedef struct {
  puffs_base_buf1* buf;
  puffs_base_limit1 limit;
} puffs_base_writer1;

#endif  // PUFFS_BASE_HEADER_H

#ifdef __cplusplus
extern "C" {
#endif

// ---------------- Status Codes

// Status codes are int32_t values:
//  - the sign bit indicates a non-recoverable status code: an error
//  - bits 10-30 hold the packageid: a namespace
//  - bits 8-9 are reserved
//  - bits 0-7 are a package-namespaced numeric code
//
// Do not manipulate these bits directly. Use the API functions such as
// puffs_flate_status_is_error instead.
typedef int32_t puffs_flate_status;

#define puffs_flate_packageid 967230  // 0x000ec23e

#define PUFFS_FLATE_STATUS_OK 0                               // 0x00000000
#define PUFFS_FLATE_ERROR_BAD_PUFFS_VERSION -2147483647       // 0x80000001
#define PUFFS_FLATE_ERROR_BAD_RECEIVER -2147483646            // 0x80000002
#define PUFFS_FLATE_ERROR_BAD_ARGUMENT -2147483645            // 0x80000003
#define PUFFS_FLATE_ERROR_CONSTRUCTOR_NOT_CALLED -2147483644  // 0x80000004
#define PUFFS_FLATE_ERROR_CLOSED_FOR_WRITES -2147483643       // 0x80000005
#define PUFFS_FLATE_ERROR_UNEXPECTED_EOF -2147483642          // 0x80000006
#define PUFFS_FLATE_SUSPENSION_SHORT_READ 7                   // 0x00000007
#define PUFFS_FLATE_SUSPENSION_SHORT_WRITE 8                  // 0x00000008
#define PUFFS_FLATE_SUSPENSION_LIMITED_READ 9                 // 0x00000009
#define PUFFS_FLATE_SUSPENSION_LIMITED_WRITE 10               // 0x0000000a

#define PUFFS_FLATE_ERROR_BAD_HUFFMAN_CODE_OVER_SUBSCRIBED \
  -1157040128  // 0xbb08f800
#define PUFFS_FLATE_ERROR_BAD_HUFFMAN_CODE_UNDER_SUBSCRIBED \
  -1157040127  // 0xbb08f801
#define PUFFS_FLATE_ERROR_BAD_HUFFMAN_CODE_LENGTH_COUNT \
  -1157040126  // 0xbb08f802
#define PUFFS_FLATE_ERROR_BAD_HUFFMAN_CODE_LENGTH_REPETITION \
  -1157040125                                           // 0xbb08f803
#define PUFFS_FLATE_ERROR_BAD_HUFFMAN_CODE -1157040124  // 0xbb08f804
#define PUFFS_FLATE_ERROR_BAD_HUFFMAN_MINIMUM_CODE_LENGTH \
  -1157040123                                                  // 0xbb08f805
#define PUFFS_FLATE_ERROR_BAD_DISTANCE_CODE_COUNT -1157040122  // 0xbb08f806
#define PUFFS_FLATE_ERROR_BAD_FLATE_BLOCK -1157040121          // 0xbb08f807
#define PUFFS_FLATE_ERROR_BAD_LITERAL_LENGTH_CODE_COUNT \
  -1157040120  // 0xbb08f808
#define PUFFS_FLATE_ERROR_INCONSISTENT_STORED_BLOCK_LENGTH \
  -1157040119  // 0xbb08f809
#define PUFFS_FLATE_ERROR_INTERNAL_ERROR_INCONSISTENT_HUFFMAN_DECODER_STATE \
  -1157040118  // 0xbb08f80a
#define PUFFS_FLATE_ERROR_INTERNAL_ERROR_INCONSISTENT_N_BITS \
  -1157040117                                                    // 0xbb08f80b
#define PUFFS_FLATE_ERROR_MISSING_END_OF_BLOCK_CODE -1157040116  // 0xbb08f80c
#define PUFFS_FLATE_ERROR_NO_HUFFMAN_CODES -1157040115           // 0xbb08f80d
#define PUFFS_FLATE_ERROR_INVALID_ZLIB_COMPRESSION_METHOD \
  -1157040114  // 0xbb08f80e
#define PUFFS_FLATE_ERROR_INVALID_ZLIB_COMPRESSION_WINDOW_SIZE \
  -1157040113                                                    // 0xbb08f80f
#define PUFFS_FLATE_ERROR_INVALID_ZLIB_PARITY_CHECK -1157040112  // 0xbb08f810
#define PUFFS_FLATE_ERROR_TODO_UNSUPPORTED_ZLIB_PRESET_DICTIONARY \
  -1157040111  // 0xbb08f811

bool puffs_flate_status_is_error(puffs_flate_status s);

const char* puffs_flate_status_string(puffs_flate_status s);

// ---------------- Public Consts

// ---------------- Structs

typedef struct {
  // Do not access the private_impl's fields directly. There is no API/ABI
  // compatibility or safety guarantee if you do so. Instead, use the
  // puffs_flate_decoder_etc functions.
  //
  // In C++, these fields would be "private", but C does not support that.
  //
  // It is a struct, not a struct*, so that it can be stack allocated.
  struct {
    puffs_flate_status status;
    uint32_t magic;

    uint32_t f_bits;
    uint32_t f_n_bits;
    uint32_t f_huffs[2][1234];
    uint32_t f_n_huffs_bits[2];
    uint8_t f_history[32768];
    uint32_t f_history_index;
    uint8_t f_code_lengths[320];

    struct {
      uint32_t coro_susp_point;
      puffs_base_buf1_mark v_m0;
      puffs_flate_status v_z;
      puffs_base_buf1_mark v_m1;
      puffs_base_slice_u8 v_written;
    } c_decode[1];
    struct {
      uint32_t coro_susp_point;
      uint32_t v_final;
      uint32_t v_type;
    } c_decode_blocks[1];
    struct {
      uint32_t coro_susp_point;
      uint32_t v_n;
      uint64_t scratch;
    } c_decode_uncompressed[1];
    struct {
      uint32_t coro_susp_point;
      uint32_t v_bits;
      uint32_t v_n_bits;
      uint32_t v_table_entry;
      uint32_t v_table_entry_n_bits;
      uint32_t v_lmask;
      uint32_t v_dmask;
      uint32_t v_redir_top;
      uint32_t v_redir_mask;
      uint32_t v_length;
      uint32_t v_distance;
      uint64_t scratch;
    } c_decode_huffman[1];
    struct {
      uint32_t coro_susp_point;
      uint32_t v_i;
    } c_init_fixed_huffman[1];
    struct {
      uint32_t coro_susp_point;
      uint32_t v_bits;
      uint32_t v_n_bits;
      uint32_t v_n_lit;
      uint32_t v_n_dist;
      uint32_t v_n_clen;
      uint32_t v_i;
      uint32_t v_mask;
      uint32_t v_table_entry;
      uint32_t v_table_entry_n_bits;
      uint32_t v_n_extra_bits;
      uint8_t v_rep_symbol;
      uint32_t v_rep_count;
    } c_init_dynamic_huffman[1];
  } private_impl;
} puffs_flate_decoder;

typedef struct {
  // Do not access the private_impl's fields directly. There is no API/ABI
  // compatibility or safety guarantee if you do so. Instead, use the
  // puffs_flate_zlib_decoder_etc functions.
  //
  // In C++, these fields would be "private", but C does not support that.
  //
  // It is a struct, not a struct*, so that it can be stack allocated.
  struct {
    puffs_flate_status status;
    uint32_t magic;

    puffs_flate_decoder f_dec;

    struct {
      uint32_t coro_susp_point;
      uint16_t v_x;
      uint32_t v_checksum;
      uint64_t scratch;
    } c_decode[1];
  } private_impl;
} puffs_flate_zlib_decoder;

// ---------------- Public Constructor and Destructor Prototypes

// puffs_flate_decoder_constructor is a constructor function.
//
// It should be called before any other puffs_flate_decoder_* function.
//
// Pass PUFFS_VERSION and 0 for puffs_version and for_internal_use_only.
void puffs_flate_decoder_constructor(puffs_flate_decoder* self,
                                     uint32_t puffs_version,
                                     uint32_t for_internal_use_only);

void puffs_flate_decoder_destructor(puffs_flate_decoder* self);

// puffs_flate_zlib_decoder_constructor is a constructor function.
//
// It should be called before any other puffs_flate_zlib_decoder_* function.
//
// Pass PUFFS_VERSION and 0 for puffs_version and for_internal_use_only.
void puffs_flate_zlib_decoder_constructor(puffs_flate_zlib_decoder* self,
                                          uint32_t puffs_version,
                                          uint32_t for_internal_use_only);

void puffs_flate_zlib_decoder_destructor(puffs_flate_zlib_decoder* self);

// ---------------- Public Function Prototypes

puffs_flate_status puffs_flate_decoder_decode(puffs_flate_decoder* self,
                                              puffs_base_writer1 a_dst,
                                              puffs_base_reader1 a_src);

puffs_flate_status puffs_flate_zlib_decoder_decode(
    puffs_flate_zlib_decoder* self,
    puffs_base_writer1 a_dst,
    puffs_base_reader1 a_src);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // PUFFS_FLATE_H
