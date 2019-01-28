/* Copyright (c) 2017 - 2019 LiteSpeed Technologies Inc.  See LICENSE. */
/*
 * test_trapa.c -- Test transport parameters.
 */

#include <assert.h>

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "lsquic_types.h"
#include "lsquic_sizes.h"
#include "lsquic_trans_params.h"

#define ENC_BUF_SZ 0x1000

struct trapa_test
{
    int                         line;
    enum {
        TEST_ENCODE = 1 << 0,
        TEST_DECODE = 1 << 1,
    }                           flags;
    struct transport_params     params;
    size_t                      enc_len, dec_len;
    int                         expect_decode_err;
    unsigned char               encoded[ENC_BUF_SZ];
};

#define DRAFT_17_VERSION "\xFF\x00\x00\x11"

static const struct trapa_test tests[] =
{

    {
        .line   = __LINE__,
        .flags  = TEST_ENCODE,
        .params = {
            TP_DEFAULT_VALUES,
        },
        .enc_len = 6,
        .encoded =
            /* Version */   "\x00\x00\x00\x00"
     /* Overall length */   "\x00\x00"
    /* Trailer to make the end easily visible in gdb: */
    "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
    },

    {
        .line   = __LINE__,
        .flags  = TEST_ENCODE,
        .params = {
            .tp_flags   = 0,
            .tp_version_u.client.buf = DRAFT_17_VERSION,
            .tp_init_max_stream_data_bidi_local = 0x12348877,
            .tp_init_max_data = 0xAABB,
            .tp_idle_timeout = 10,
            .tp_max_ack_delay = TP_DEF_MAX_ACK_DELAY,
        },
        .enc_len = 37,
        .encoded =
            /* Version */   DRAFT_17_VERSION
     /* Overall length */   "\x00\x1F"
     /* Idle timeout */     "\x00\x01\x00\x01\x0A"
     /* Packet size */      "\x00\x03\x00\x01\x00"
     /* Max data */         "\x00\x04\x00\x04\x80\x00\xAA\xBB"
     /* Bidi local */       "\x00\x05\x00\x04\x92\x34\x88\x77"
     /* Ack delay exp */    "\x00\x0A\x00\x01\x00"
    /* Trailer to make the end easily visible in gdb: */
    "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
    },

    {
        .line   = __LINE__,
        .flags  = TEST_DECODE,
        .dec_len = 1,
        .expect_decode_err = 1,
    },

    {
        .line   = __LINE__,
        .flags  = TEST_DECODE,
        .dec_len = 3,
        .expect_decode_err = 1,
        .encoded = "\x00\x04",
    },


    {
        .line   = __LINE__,
        .flags  = TEST_DECODE,
        .params = {
            TP_DEFAULT_VALUES,
            .tp_flags = TRAPA_SERVER,
            .tp_init_max_data = 0x123456,
            .tp_init_max_stream_data_bidi_local = 0xABCDEF88,
            .tp_disable_migration = 1,
            .tp_max_packet_size = 0x333,
            .tp_version_u.server = {
                .negotiated = { .buf = DRAFT_17_VERSION, },
            },
        },
        .enc_len = 37,
        .encoded =
            /* Version */   DRAFT_17_VERSION
        /* N supported */   "\x00"
     /* Overall length */   "\x00\x1E"
     /* Packet size */      "\x00\x03\x00\x02\x43\x33"
     /* Max data */         "\x00\x04\x00\x04\x80\x12\x34\x56"
     /* Bidi local */       "\x00\x05\x00\x08\xC0\x00\x00\x00\xAB\xCD\xEF\x88"
     /* Migration */        "\x00\x0C\x00\x00"
    /* Trailer to make the end easily visible in gdb: */
    "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
    },

};


static int
params_are_equal (const struct transport_params *a,
                  const struct transport_params *b)
{
    return 0 == memcmp(a, b, sizeof(*a));
}


static void
run_test (const struct trapa_test *test)
{
    struct transport_params decoded_params;
    size_t dec_len;
    int s;
    unsigned char buf[ENC_BUF_SZ];

    if (test->flags & TEST_ENCODE)
    {
        s = lsquic_tp_encode(&test->params, buf, sizeof(buf));
        assert(s > 0);
        assert((size_t) s == test->enc_len);
        assert(0 == memcmp(test->encoded, buf, s));
    }

    if (test->flags & TEST_DECODE)
    {
        if (test->dec_len)
            dec_len = test->dec_len;
        else
            dec_len = sizeof(buf);
        s = lsquic_tp_decode(test->encoded, dec_len, &decoded_params);
        if (!test->expect_decode_err)
        {
            assert(s > 0);
            assert((size_t) s == test->enc_len);
            s = params_are_equal(&test->params, &decoded_params);
            assert(s);
        }
        else
            assert(s < 0);
    }
}


int
main (void)
{
    unsigned i;

    for (i = 0; i < sizeof(tests) / sizeof(tests[0]); ++i)
        run_test(&tests[i]);

    return 0;
}