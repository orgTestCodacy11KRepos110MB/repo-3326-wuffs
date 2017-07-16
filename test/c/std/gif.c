// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

/*
This test program is typically run indirectly, by the "puffs test" or "puffs
bench" commands. These commands take an optional "-mimic" flag to check that
Puffs' output mimics (i.e. exactly matches) other libraries' output, such as
giflib for GIF, libpng for PNG, etc.

To manually run this test:

for cc in clang gcc; do
  $cc -std=c99 -Wall -Werror gif.c && ./a.out
  rm -f a.out
done

Each edition should print "PASS", amongst other information, and exit(0).

Add the "puffs mimic cflags" (everything after the colon below) to the C
compiler flags (after the .c file) to run the mimic tests.

To manually run the benchmarks, replace "-Wall -Werror" with "-O3" and replace
"./a.out" with "./a.out -bench". Combine these changes with the "puffs mimic
cflags" to run the mimic benchmarks.
*/

// !! puffs mimic cflags: -DPUFFS_MIMIC -lgif

#include "../../../gen/c/std/gif.c"
#include "../testlib/testlib.c"

const char* proc_filename = "std/gif.c";

// ---------------- Basic Tests

void test_basic_bad_argument_out_of_range() {
  proc_funcname = __func__;
  puffs_gif_lzw_decoder dec;
  puffs_gif_lzw_decoder_constructor(&dec, PUFFS_VERSION, 0);

  // Setting to 8 is in the [2..8] range.
  puffs_gif_lzw_decoder_set_literal_width(&dec, 8);
  if (dec.private_impl.status != PUFFS_GIF_STATUS_OK) {
    FAIL("status: got %d, want %d", dec.private_impl.status,
         PUFFS_GIF_STATUS_OK);
  }

  // Setting to 999 is out of range.
  puffs_gif_lzw_decoder_set_literal_width(&dec, 999);
  if (dec.private_impl.status != PUFFS_GIF_ERROR_BAD_ARGUMENT) {
    FAIL("status: got %d, want %d", dec.private_impl.status,
         PUFFS_GIF_ERROR_BAD_ARGUMENT);
  }

  // That error status code should be sticky.
  puffs_gif_lzw_decoder_set_literal_width(&dec, 8);
  if (dec.private_impl.status != PUFFS_GIF_ERROR_BAD_ARGUMENT) {
    FAIL("status: got %d, want %d", dec.private_impl.status,
         PUFFS_GIF_ERROR_BAD_ARGUMENT);
  }
}

void test_basic_bad_receiver() {
  proc_funcname = __func__;
  puffs_base_writer1 dst = {0};
  puffs_base_reader1 src = {0};
  puffs_gif_status status = puffs_gif_lzw_decoder_decode(NULL, dst, src);
  if (status != PUFFS_GIF_ERROR_BAD_RECEIVER) {
    FAIL("status: got %d, want %d", status, PUFFS_GIF_ERROR_BAD_RECEIVER);
  }
}

void test_basic_constructor_not_called() {
  proc_funcname = __func__;
  puffs_gif_lzw_decoder dec = {{0}};
  puffs_base_writer1 dst = {0};
  puffs_base_reader1 src = {0};
  puffs_gif_status status = puffs_gif_lzw_decoder_decode(&dec, dst, src);
  if (status != PUFFS_GIF_ERROR_CONSTRUCTOR_NOT_CALLED) {
    FAIL("status: got %d, want %d", status,
         PUFFS_GIF_ERROR_CONSTRUCTOR_NOT_CALLED);
  }
}

void test_basic_puffs_version_bad() {
  proc_funcname = __func__;
  puffs_gif_lzw_decoder dec;
  puffs_gif_lzw_decoder_constructor(&dec, 0, 0);  // 0 is not PUFFS_VERSION.
  if (dec.private_impl.status != PUFFS_GIF_ERROR_BAD_PUFFS_VERSION) {
    FAIL("status: got %d, want %d", dec.private_impl.status,
         PUFFS_GIF_ERROR_BAD_PUFFS_VERSION);
    goto cleanup0;
  }
cleanup0:
  puffs_gif_lzw_decoder_destructor(&dec);
}

void test_basic_puffs_version_good() {
  proc_funcname = __func__;
  puffs_gif_lzw_decoder dec;
  puffs_gif_lzw_decoder_constructor(&dec, PUFFS_VERSION, 0);
  if (dec.private_impl.magic != PUFFS_MAGIC) {
    FAIL("magic: got %u, want %u", dec.private_impl.magic, PUFFS_MAGIC);
    goto cleanup0;
  }
  if (dec.private_impl.f_literal_width != 8) {
    FAIL("f_literal_width: got %u, want %u", dec.private_impl.f_literal_width,
         8);
    goto cleanup0;
  }
cleanup0:
  puffs_gif_lzw_decoder_destructor(&dec);
}

void test_basic_status_is_error() {
  proc_funcname = __func__;
  if (puffs_gif_status_is_error(PUFFS_GIF_STATUS_OK)) {
    FAIL("is_error(OK) returned true");
    return;
  }
  if (!puffs_gif_status_is_error(PUFFS_GIF_ERROR_BAD_PUFFS_VERSION)) {
    FAIL("is_error(BAD_PUFFS_VERSION) returned false");
    return;
  }
  if (puffs_gif_status_is_error(PUFFS_GIF_SUSPENSION_SHORT_WRITE)) {
    FAIL("is_error(SHORT_WRITE) returned true");
    return;
  }
  if (!puffs_gif_status_is_error(PUFFS_GIF_ERROR_LZW_CODE_IS_OUT_OF_RANGE)) {
    FAIL("is_error(LZW_CODE_IS_OUT_OF_RANGE) returned false");
    return;
  }
}

void test_basic_status_strings() {
  proc_funcname = __func__;
  const char* s0 = puffs_gif_status_string(PUFFS_GIF_STATUS_OK);
  const char* t0 = "gif: ok";
  if (strcmp(s0, t0)) {
    FAIL("got \"%s\", want \"%s\"", s0, t0);
    return;
  }
  const char* s1 = puffs_gif_status_string(PUFFS_GIF_ERROR_BAD_PUFFS_VERSION);
  const char* t1 = "gif: bad puffs version";
  if (strcmp(s1, t1)) {
    FAIL("got \"%s\", want \"%s\"", s1, t1);
    return;
  }
  const char* s2 = puffs_gif_status_string(PUFFS_GIF_SUSPENSION_SHORT_WRITE);
  const char* t2 = "gif: short write";
  if (strcmp(s2, t2)) {
    FAIL("got \"%s\", want \"%s\"", s2, t2);
    return;
  }
  const char* s3 =
      puffs_gif_status_string(PUFFS_GIF_ERROR_LZW_CODE_IS_OUT_OF_RANGE);
  const char* t3 = "gif: LZW code is out of range";
  if (strcmp(s3, t3)) {
    FAIL("got \"%s\", want \"%s\"", s3, t3);
    return;
  }
  const char* s4 = puffs_gif_status_string(-254);
  const char* t4 = "gif: unknown status";
  if (strcmp(s4, t4)) {
    FAIL("got \"%s\", want \"%s\"", s4, t4);
    return;
  }
}

void test_basic_sub_struct_constructor() {
  proc_funcname = __func__;
  puffs_gif_decoder dec;
  puffs_gif_decoder_constructor(&dec, PUFFS_VERSION, 0);
  if (dec.private_impl.magic != PUFFS_MAGIC) {
    FAIL("outer magic: got %u, want %u", dec.private_impl.magic, PUFFS_MAGIC);
    goto cleanup0;
  }
  if (dec.private_impl.f_lzw.private_impl.magic != PUFFS_MAGIC) {
    FAIL("inner magic: got %u, want %u",
         dec.private_impl.f_lzw.private_impl.magic, PUFFS_MAGIC);
    goto cleanup0;
  }
cleanup0:
  puffs_gif_decoder_destructor(&dec);
}

// ---------------- LZW Tests

void test_puffs_gif_lzw_decode(const char* src_filename,
                               uint64_t src_size,
                               const char* want_filename,
                               uint64_t want_size,
                               uint64_t limit) {
  puffs_base_buf1 got = {.ptr = global_got_buffer, .len = BUFFER_SIZE};
  puffs_base_buf1 want = {.ptr = global_want_buffer, .len = BUFFER_SIZE};
  puffs_base_buf1 src = {.ptr = global_src_buffer, .len = BUFFER_SIZE};

  if (!read_file(&src, src_filename)) {
    goto cleanup0;
  }
  if (src.wi != src_size) {
    FAIL("src size: got %d, want %d", (int)(src.wi), (int)(src_size));
    goto cleanup0;
  }
  // The first byte in that file, the LZW literal width, should be 0x08.
  uint8_t literal_width = src.ptr[0];
  if (literal_width != 0x08) {
    FAIL("LZW literal width: got %d, want %d", (int)(src.ptr[0]), 0x08);
    goto cleanup0;
  }
  src.ri++;

  if (!read_file(&want, want_filename)) {
    goto cleanup0;
  }
  if (want.wi != want_size) {
    FAIL("want size: got %d, want %d", (int)(want.wi), (int)(want_size));
    goto cleanup0;
  }

  puffs_gif_lzw_decoder dec;
  puffs_gif_lzw_decoder_constructor(&dec, PUFFS_VERSION, 0);
  puffs_gif_lzw_decoder_set_literal_width(&dec, literal_width);
  puffs_base_writer1 got_writer = {.buf = &got};
  puffs_base_reader1 src_reader = {.buf = &src};
  int num_iters = 0;
  while (true) {
    num_iters++;
    uint64_t lim = limit;
    src_reader.limit.ptr_to_len = &lim;
    size_t old_ri = src.ri;
    puffs_gif_status status =
        puffs_gif_lzw_decoder_decode(&dec, got_writer, src_reader);
    if (src.ri == src.wi) {
      if (status != PUFFS_GIF_STATUS_OK) {
        FAIL("status: got %d, want %d", status, PUFFS_GIF_STATUS_OK);
        goto cleanup1;
      }
      break;
    }
    if (status != PUFFS_GIF_SUSPENSION_SHORT_READ) {
      FAIL("status: got %d, want %d", status, PUFFS_GIF_SUSPENSION_SHORT_READ);
      goto cleanup1;
    }
    if (src.ri < old_ri) {
      FAIL("read index src.ri went backwards");
      goto cleanup1;
    }
    if (src.ri == old_ri) {
      FAIL("no progress was made");
      goto cleanup1;
    }
  }

  if (limit < 1000000) {
    if (num_iters <= 1) {
      FAIL("num_iters: got %d, want > 1", num_iters);
      goto cleanup1;
    }
  } else {
    if (num_iters != 1) {
      FAIL("num_iters: got %d, want 1", num_iters);
      goto cleanup1;
    }
  }

  if (!buf1s_equal("", &got, &want)) {
    goto cleanup1;
  }

cleanup1:
  puffs_gif_lzw_decoder_destructor(&dec);
cleanup0:;
}

void test_puffs_gif_lzw_decode_few_big_reads() {
  proc_funcname = __func__;
  test_puffs_gif_lzw_decode("../../testdata/bricks-gray.indexes.giflzw", 14731,
                            "../../testdata/bricks-gray.indexes", 19200,
                            1 << 30);
}

void test_puffs_gif_lzw_decode_many_small_reads() {
  proc_funcname = __func__;
  test_puffs_gif_lzw_decode("../../testdata/bricks-gray.indexes.giflzw", 14731,
                            "../../testdata/bricks-gray.indexes", 19200, 100);
}

void test_puffs_gif_lzw_decode_bricks_dither() {
  proc_funcname = __func__;
  test_puffs_gif_lzw_decode("../../testdata/bricks-dither.indexes.giflzw",
                            14923, "../../testdata/bricks-dither.indexes",
                            19200, 1 << 30);
}

void test_puffs_gif_lzw_decode_bricks_nodither() {
  proc_funcname = __func__;
  test_puffs_gif_lzw_decode("../../testdata/bricks-nodither.indexes.giflzw",
                            13382, "../../testdata/bricks-nodither.indexes",
                            19200, 1 << 30);
}

void test_puffs_gif_lzw_decode_pi() {
  proc_funcname = __func__;
  test_puffs_gif_lzw_decode("../../testdata/pi.txt.giflzw", 50550,
                            "../../testdata/pi.txt", 100003, 1 << 30);
}

// ---------------- LZW Benches

void bench_puffs_gif_lzw_decode(const char* filename, uint64_t reps) {
  puffs_base_buf1 dst = {.ptr = global_got_buffer, .len = BUFFER_SIZE};
  puffs_base_buf1 src = {.ptr = global_src_buffer, .len = BUFFER_SIZE};
  puffs_base_writer1 dst_writer = {.buf = &dst};
  puffs_base_reader1 src_reader = {.buf = &src};

  if (!read_file(&src, filename)) {
    return;
  }
  if (src.wi <= 0) {
    FAIL("src size: got %d, want > 0", (int)(src.wi));
    return;
  }
  uint8_t literal_width = src.ptr[0];
  if (literal_width != 0x08) {
    FAIL("LZW literal width: got %d, want %d", (int)(src.ptr[0]), 0x08);
    return;
  }

  bench_start();
  uint64_t n_bytes = 0;
  uint64_t i;
  for (i = 0; i < reps; i++) {
    dst.wi = 0;
    src.ri = 1;  // Skip the literal width.
    puffs_gif_lzw_decoder dec;
    puffs_gif_lzw_decoder_constructor(&dec, PUFFS_VERSION, 0);
    puffs_gif_status s =
        puffs_gif_lzw_decoder_decode(&dec, dst_writer, src_reader);
    puffs_gif_lzw_decoder_destructor(&dec);
    if (s) {
      FAIL("decode: %s", puffs_gif_status_string(s));
      return;
    }
    n_bytes += dst.wi;
  }
  bench_finish(reps, n_bytes);
}

void bench_puffs_gif_lzw_decode_20k() {
  proc_funcname = __func__;
  bench_puffs_gif_lzw_decode("../../testdata/bricks-gray.indexes.giflzw", 5000);
}

void bench_puffs_gif_lzw_decode_100k() {
  proc_funcname = __func__;
  bench_puffs_gif_lzw_decode("../../testdata/pi.txt.giflzw", 1000);
}

// ---------------- GIF Tests

const char* puffs_gif_decode(puffs_base_buf1* dst, puffs_base_buf1* src) {
  puffs_gif_decoder dec;
  puffs_gif_decoder_constructor(&dec, PUFFS_VERSION, 0);
  puffs_base_writer1 dst_writer = {.buf = dst};
  puffs_base_reader1 src_reader = {.buf = src};
  puffs_gif_status s = puffs_gif_decoder_decode(&dec, dst_writer, src_reader);
  puffs_gif_decoder_destructor(&dec);
  if (s) {
    return puffs_gif_status_string(s);
  }
  return NULL;
}

void test_puffs_gif_decode(const char* filename,
                           const char* palette_filename,
                           const char* indexes_filename,
                           puffs_gif_status want,
                           uint64_t limit) {
  puffs_base_buf1 got = {.ptr = global_got_buffer, .len = BUFFER_SIZE};
  puffs_base_buf1 src = {.ptr = global_src_buffer, .len = BUFFER_SIZE};

  if (!read_file(&src, filename)) {
    goto cleanup0;
  }

  puffs_gif_decoder dec;
  puffs_gif_decoder_constructor(&dec, PUFFS_VERSION, 0);
  puffs_base_writer1 got_writer = {.buf = &got};
  puffs_base_reader1 src_reader = {.buf = &src};
  int num_iters = 0;
  while (true) {
    num_iters++;
    uint64_t lim = limit;
    src_reader.limit.ptr_to_len = &lim;
    size_t old_ri = src.ri;
    puffs_gif_status got =
        puffs_gif_decoder_decode(&dec, got_writer, src_reader);
    if ((src.ri == src.wi) || puffs_gif_status_is_error(got)) {
      if (got != want) {
        FAIL("status: got %d, want %d", got, want);
        goto cleanup1;
      }
      break;
    }
    if (got != PUFFS_GIF_SUSPENSION_SHORT_READ) {
      FAIL("status: got %d, want %d", got, PUFFS_GIF_SUSPENSION_SHORT_READ);
      goto cleanup1;
    }
    if (src.ri < old_ri) {
      FAIL("read index src.ri went backwards");
      goto cleanup1;
    }
    if (src.ri == old_ri) {
      FAIL("no progress was made");
      goto cleanup1;
    }
  }

  if (want != PUFFS_GIF_STATUS_OK) {
    goto cleanup1;
  }

  if (limit < 1000000) {
    if (num_iters <= 1) {
      FAIL("num_iters: got %d, want > 1", num_iters);
      goto cleanup1;
    }
  } else {
    if (num_iters != 1) {
      FAIL("num_iters: got %d, want 1", num_iters);
      goto cleanup1;
    }
  }

  // TODO: provide a public API for getting the width and height.
  if (dec.private_impl.f_width != 160) {
    FAIL("width: got %d, want %d", dec.private_impl.f_width, 160);
    goto cleanup1;
  }
  if (dec.private_impl.f_height != 120) {
    FAIL("height: got %d, want %d", dec.private_impl.f_height, 120);
    goto cleanup1;
  }

  // TODO: provide a public API for getting the palette.
  puffs_base_buf1 pal_got = {.ptr = dec.private_impl.f_gct, .len = 3 * 256};
  puffs_base_buf1 pal_want = {.ptr = global_palette_buffer, .len = 3 * 256};
  pal_got.wi = 3 * 256;
  if (!read_file(&pal_want, palette_filename)) {
    goto cleanup1;
  }
  if (!buf1s_equal("palette ", &pal_got, &pal_want)) {
    goto cleanup1;
  }

  puffs_base_buf1 ind_want = {.ptr = global_want_buffer, .len = BUFFER_SIZE};
  if (!read_file(&ind_want, indexes_filename)) {
    goto cleanup1;
  }
  if (!buf1s_equal("indexes ", &got, &ind_want)) {
    goto cleanup1;
  }

cleanup1:
  puffs_gif_decoder_destructor(&dec);
cleanup0:;
}

void test_puffs_gif_decode_input_is_a_gif_few_big_reads() {
  proc_funcname = __func__;
  test_puffs_gif_decode("../../testdata/bricks-dither.gif",
                        "../../testdata/bricks-dither.palette",
                        "../../testdata/bricks-dither.indexes",
                        PUFFS_GIF_STATUS_OK, 1 << 30);
}

void test_puffs_gif_decode_input_is_a_gif_many_small_reads() {
  proc_funcname = __func__;
  test_puffs_gif_decode("../../testdata/bricks-dither.gif",
                        "../../testdata/bricks-dither.palette",
                        "../../testdata/bricks-dither.indexes",
                        PUFFS_GIF_STATUS_OK, 1000);
}

void test_puffs_gif_decode_input_is_a_png() {
  proc_funcname = __func__;
  test_puffs_gif_decode("../../testdata/bricks-dither.png", "", "",
                        PUFFS_GIF_ERROR_BAD_GIF_HEADER, 1 << 30);
}

// ---------------- Mimic Tests

#ifdef PUFFS_MIMIC

#include "../mimiclib/gif.c"

void test_mimic_gif_decode(const char* filename) {
  puffs_base_buf1 src = {.ptr = global_src_buffer, .len = BUFFER_SIZE};
  if (!read_file(&src, filename)) {
    return;
  }

  src.ri = 0;
  puffs_base_buf1 got = {.ptr = global_got_buffer, .len = BUFFER_SIZE};
  const char* got_msg = puffs_gif_decode(&got, &src);
  if (got_msg) {
    FAIL("%s", got_msg);
    return;
  }

  src.ri = 0;
  puffs_base_buf1 want = {.ptr = global_want_buffer, .len = BUFFER_SIZE};
  const char* want_msg = mimic_gif_decode(&want, &src);
  if (want_msg) {
    FAIL("%s", want_msg);
    return;
  }

  if (!buf1s_equal("", &got, &want)) {
    return;
  }

  // TODO: check the palette RGB values, not just the palette indexes (pixels).
}

void test_mimic_gif_decode_bricks_dither() {
  proc_funcname = __func__;
  test_mimic_gif_decode("../../testdata/bricks-dither.gif");
}

void test_mimic_gif_decode_bricks_gray() {
  proc_funcname = __func__;
  test_mimic_gif_decode("../../testdata/bricks-gray.gif");
}

void test_mimic_gif_decode_bricks_nodither() {
  proc_funcname = __func__;
  test_mimic_gif_decode("../../testdata/bricks-nodither.gif");
}

void test_mimic_gif_decode_harvesters() {
  proc_funcname = __func__;
  test_mimic_gif_decode("../../testdata/harvesters.gif");
}

void test_mimic_gif_decode_hat() {
  proc_funcname = __func__;
  test_mimic_gif_decode("../../testdata/hat.gif");
}

void test_mimic_gif_decode_hibiscus() {
  proc_funcname = __func__;
  test_mimic_gif_decode("../../testdata/hibiscus.gif");
}

void test_mimic_gif_decode_pjw_thumbnail() {
  proc_funcname = __func__;
  test_mimic_gif_decode("../../testdata/pjw-thumbnail.gif");
}

#endif  // PUFFS_MIMIC

// ---------------- GIF Benches

void bench_gif_decode(const char* (*decode_func)(puffs_base_buf1*,
                                                 puffs_base_buf1*),
                      const char* filename,
                      uint64_t reps) {
  puffs_base_buf1 dst = {.ptr = global_got_buffer, .len = BUFFER_SIZE};
  puffs_base_buf1 src = {.ptr = global_src_buffer, .len = BUFFER_SIZE};

  if (!read_file(&src, filename)) {
    return;
  }

  bench_start();
  uint64_t n_bytes = 0;
  uint64_t i;
  for (i = 0; i < reps; i++) {
    dst.wi = 0;
    src.ri = 0;
    const char* error_msg = decode_func(&dst, &src);
    if (error_msg) {
      FAIL("%s", error_msg);
      return;
    }
    n_bytes += dst.wi;
  }
  bench_finish(reps, n_bytes);
}

void bench_puffs_gif_decode_1k() {
  proc_funcname = __func__;
  bench_gif_decode(puffs_gif_decode, "../../testdata/pjw-thumbnail.gif",
                   200000);
}

void bench_puffs_gif_decode_10k() {
  proc_funcname = __func__;
  bench_gif_decode(puffs_gif_decode, "../../testdata/hat.gif", 10000);
}

void bench_puffs_gif_decode_100k() {
  proc_funcname = __func__;
  bench_gif_decode(puffs_gif_decode, "../../testdata/hibiscus.gif", 1000);
}

void bench_puffs_gif_decode_1000k() {
  proc_funcname = __func__;
  bench_gif_decode(puffs_gif_decode, "../../testdata/harvesters.gif", 100);
}

// ---------------- Mimic Benches

#ifdef PUFFS_MIMIC

void bench_mimic_gif_decode_1k() {
  proc_funcname = __func__;
  bench_gif_decode(mimic_gif_decode, "../../testdata/pjw-thumbnail.gif",
                   200000);
}

void bench_mimic_gif_decode_10k() {
  proc_funcname = __func__;
  bench_gif_decode(mimic_gif_decode, "../../testdata/hat.gif", 10000);
}

void bench_mimic_gif_decode_100k() {
  proc_funcname = __func__;
  bench_gif_decode(mimic_gif_decode, "../../testdata/hibiscus.gif", 1000);
}

void bench_mimic_gif_decode_1000k() {
  proc_funcname = __func__;
  bench_gif_decode(mimic_gif_decode, "../../testdata/harvesters.gif", 100);
}

#endif  // PUFFS_MIMIC

// ---------------- Manifest

// The empty comments forces clang-format to place one element per line.
proc tests[] = {
    // Basic Tests
    test_basic_bad_argument_out_of_range,  //
    test_basic_bad_receiver,               //
    test_basic_constructor_not_called,     //
    test_basic_puffs_version_bad,          //
    test_basic_puffs_version_good,         //
    test_basic_status_is_error,            //
    test_basic_status_strings,             //
    test_basic_sub_struct_constructor,     //

    // LZW Tests
    test_puffs_gif_lzw_decode_few_big_reads,     //
    test_puffs_gif_lzw_decode_many_small_reads,  //
    test_puffs_gif_lzw_decode_bricks_dither,     //
    test_puffs_gif_lzw_decode_bricks_nodither,   //
    test_puffs_gif_lzw_decode_pi,                //

    // GIF Tests
    test_puffs_gif_decode_input_is_a_gif_few_big_reads,     //
    test_puffs_gif_decode_input_is_a_gif_many_small_reads,  //
    test_puffs_gif_decode_input_is_a_png,                   //

#ifdef PUFFS_MIMIC

    // Mimic Tests
    test_mimic_gif_decode_bricks_dither,    //
    test_mimic_gif_decode_bricks_gray,      //
    test_mimic_gif_decode_bricks_nodither,  //
    test_mimic_gif_decode_harvesters,       //
    test_mimic_gif_decode_hat,              //
    test_mimic_gif_decode_hibiscus,         //
    test_mimic_gif_decode_pjw_thumbnail,    //

#endif  // PUFFS_MIMIC

    NULL,
};

// The empty comments forces clang-format to place one element per line.
proc benches[] = {
    // LZW Benches
    bench_puffs_gif_lzw_decode_20k,   //
    bench_puffs_gif_lzw_decode_100k,  //

    // GIF Benches
    bench_puffs_gif_decode_1k,     //
    bench_puffs_gif_decode_10k,    //
    bench_puffs_gif_decode_100k,   //
    bench_puffs_gif_decode_1000k,  //

#ifdef PUFFS_MIMIC

    // Mimic Benches
    bench_mimic_gif_decode_1k,     //
    bench_mimic_gif_decode_10k,    //
    bench_mimic_gif_decode_100k,   //
    bench_mimic_gif_decode_1000k,  //

#endif  // PUFFS_MIMIC

    NULL,
};
