/* Copyright (c) 2017 - 2018 LiteSpeed Technologies Inc.  See LICENSE. */
/*
 * lsquic_trans_params.h -- Transport parameters types and functions.
 */

#ifndef LSQUIC_TRANS_PARAMS_H
#define LSQUIC_TRANS_PARAMS_H 1

/* [draft-ietf-quic-transport-17], Section 18 */
enum transport_param_id
{
    TPI_ORIGINAL_CONNECTION_ID            =  0,
    TPI_IDLE_TIMEOUT                      =  1,
    TPI_STATELESS_RESET_TOKEN             =  2,
    TPI_MAX_PACKET_SIZE                   =  3,
    TPI_INIT_MAX_DATA                     =  4,
    TPI_INIT_MAX_STREAM_DATA_BIDI_LOCAL   =  5,
    TPI_INIT_MAX_STREAM_DATA_BIDI_REMOTE  =  6,
    TPI_INIT_MAX_STREAM_DATA_UNI          =  7,
    TPI_INIT_MAX_STREAMS_BIDI             =  8,
    TPI_INIT_MAX_STREAMS_UNI              =  9,
    TPI_ACK_DELAY_EXPONENT                =  10,
    TPI_MAX_ACK_DELAY                     =  11,
    TPI_DISABLE_MIGRATION                 =  12,
    TPI_PREFERRED_ADDRESS                 =  13,
#define MAX_TPI TPI_PREFERRED_ADDRESS
};

#define NUMERIC_TRANS_PARAMS (\
     (1 << TPI_MAX_PACKET_SIZE) \
    |(1 << TPI_INIT_MAX_STREAMS_UNI) \
    |(1 << TPI_INIT_MAX_STREAMS_UNI) \
    |(1 << TPI_INIT_MAX_STREAMS_BIDI) \
    |(1 << TPI_INIT_MAX_DATA) \
    |(1 << TPI_INIT_MAX_STREAM_DATA_BIDI_LOCAL) \
    |(1 << TPI_INIT_MAX_STREAM_DATA_BIDI_REMOTE) \
    |(1 << TPI_INIT_MAX_STREAM_DATA_UNI) \
    |(1 << TPI_IDLE_TIMEOUT) \
    |(1 << TPI_MAX_ACK_DELAY) \
    |(1 << TPI_ACK_DELAY_EXPONENT) \
    )

#define IQUIC_MAX_SUPP_VERS ((2<<7) - 4)/sizeof(uint32_t)

enum trapa_flags
{
    TRAPA_RESET_TOKEN   = 1 << 0,   /* Reset token is set */
    TRAPA_SERVER        = 1 << 1,   /* Server transport parameters */
    TRAPA_PREFERRED_ADDR= 1 << 2,   /* Preferred address is set */
    TRAPA_ORIGINAL_CID  = 1 << 3,   /* Original CID is set */
};

struct transport_params
{
    enum trapa_flags        tp_flags;
    union
    {
        union {
            uint32_t    initial;
            unsigned char   buf[4];
        }   client;
        struct
        {
            union
            {
                uint32_t      tag;
#ifndef DEBUG
                unsigned char buf[4];
#endif
            }           negotiated;
            uint32_t    supported[IQUIC_MAX_SUPP_VERS];
            /* Number of elements in `supported' array: */
            unsigned    n_supported;
        }   server;
    }           tp_version_u;

    union {
        struct {
            uint64_t init_max_stream_data_bidi_local;
            uint64_t init_max_stream_data_bidi_remote;
            uint64_t init_max_stream_data_uni;
            uint64_t init_max_data;
            uint64_t idle_timeout;
            uint64_t init_max_streams_bidi;
            uint64_t init_max_streams_uni;
            uint64_t max_packet_size;
            uint64_t ack_delay_exponent;
            uint64_t max_ack_delay;
        }               s;
        uint64_t        a[10];
    }           tp_numerics_u;
#define tp_init_max_stream_data_bidi_local tp_numerics_u.s.init_max_stream_data_bidi_local
#define tp_init_max_stream_data_bidi_remote tp_numerics_u.s.init_max_stream_data_bidi_remote
#define tp_init_max_stream_data_uni tp_numerics_u.s.init_max_stream_data_uni
#define tp_init_max_data tp_numerics_u.s.init_max_data
#define tp_idle_timeout tp_numerics_u.s.idle_timeout
#define tp_init_max_streams_bidi tp_numerics_u.s.init_max_streams_bidi
#define tp_init_max_streams_uni tp_numerics_u.s.init_max_streams_uni
#define tp_max_packet_size tp_numerics_u.s.max_packet_size
#define tp_ack_delay_exponent tp_numerics_u.s.ack_delay_exponent
#define tp_max_ack_delay tp_numerics_u.s.max_ack_delay

    signed char tp_disable_migration;
    uint8_t     tp_stateless_reset_token[IQUIC_SRESET_TOKEN_SZ];
    struct {
        unsigned        ip_ver;         /* 4 or 6 */
        uint8_t         ip_addr[16];    /* 4 or 16 bytes */
        uint16_t        port;
        lsquic_cid_t    cid;
        uint8_t         srt[16];
    }           tp_preferred_address;
    lsquic_cid_t    tp_original_cid;
};

#define TP_DEF_MAX_PACKET_SIZE 65527
#define TP_DEF_ACK_DELAY_EXP 3
#define TP_DEF_INIT_MAX_STREAMS_UNI 0
#define TP_DEF_INIT_MAX_STREAMS_BIDI 0
#define TP_DEF_INIT_MAX_DATA 0
#define TP_DEF_DISABLE_MIGRATION 0
#define TP_DEF_INIT_MAX_STREAM_DATA_BIDI_LOCAL 0
#define TP_DEF_INIT_MAX_STREAM_DATA_BIDI_REMOTE 0
#define TP_DEF_INIT_MAX_STREAM_DATA_UNI 0
#define TP_DEF_IDLE_TIMEOUT 0
#define TP_DEF_MAX_ACK_DELAY 25

#define TP_DEFAULT_VALUES                                                             \
    .tp_idle_timeout                      =  TP_DEF_IDLE_TIMEOUT,                     \
    .tp_max_ack_delay                     =  TP_DEF_MAX_ACK_DELAY,                    \
    .tp_max_packet_size                   =  TP_DEF_MAX_PACKET_SIZE,                  \
    .tp_ack_delay_exponent                =  TP_DEF_ACK_DELAY_EXP,                    \
    .tp_init_max_streams_bidi             =  TP_DEF_INIT_MAX_STREAMS_BIDI,            \
    .tp_init_max_streams_uni              =  TP_DEF_INIT_MAX_STREAMS_UNI,             \
    .tp_init_max_data                     =  TP_DEF_INIT_MAX_DATA,                    \
    .tp_disable_migration                 =  TP_DEF_DISABLE_MIGRATION,                \
    .tp_init_max_stream_data_bidi_local   =  TP_DEF_INIT_MAX_STREAM_DATA_BIDI_LOCAL,  \
    .tp_init_max_stream_data_bidi_remote  =  TP_DEF_INIT_MAX_STREAM_DATA_BIDI_REMOTE, \
    .tp_init_max_stream_data_uni          =  TP_DEF_INIT_MAX_STREAM_DATA_UNI

#define TP_INITIALIZER() (struct transport_params) { TP_DEFAULT_VALUES }

int
lsquic_tp_encode (const struct transport_params *,
                  unsigned char *buf, size_t bufsz);

int
lsquic_tp_decode (const unsigned char *buf, size_t bufsz,
                  struct transport_params *);

void
lsquic_tp_to_str (const struct transport_params *params, char *buf, size_t sz);

#endif