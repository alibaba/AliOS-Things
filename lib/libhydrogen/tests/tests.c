#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "hydrogen.h"

static const char *ctx = "libtests";

static int
streq(const char *expected, const char *found)
{
    if (strcmp(expected, found) != 0) {
        fprintf(stderr, "Found: [%s]\n", found);
        return 0;
    }
    return 1;
}
#define assert_streq(EXPECTED, FOUND) assert(streq((EXPECTED), (FOUND)))

static void
test_randombytes(void)
{
    uint8_t       dk[hydro_random_SEEDBYTES];
    uint8_t       tmp[10000];
    unsigned long b = 0U;
    unsigned long bp;
    uint32_t      x;
    size_t        i, j;

    for (i = 0; i < 10000; i++) {
        x = hydro_random_u32();
        for (j = 0; j < sizeof x; j++) {
            b += (x >> j) & 1;
        }
    }
    assert(b > 18000 && b < 22000);

    b = 0;
    hydro_random_buf(tmp, sizeof tmp);
    for (i = 0; i < 10000; i++) {
        for (j = 0; j < sizeof tmp[0]; j++) {
            b += (tmp[i] >> j) & 1;
        }
    }
    assert(b > 4500 && b < 5500);

    memcpy(dk, tmp, sizeof dk);
    b = 0;
    hydro_random_buf_deterministic(tmp, 10000, dk);
    for (i = 0; i < 10000; i++) {
        for (j = 0; j < sizeof tmp[0]; j++) {
            b += (tmp[i] >> j) & 1;
        }
    }
    assert(b > 4500 && b < 5500);
    bp = b;
    b  = 0;
    hydro_random_buf_deterministic(tmp, 10000, dk);
    for (i = 0; i < 10000; i++) {
        for (j = 0; j < sizeof tmp[0]; j++) {
            b += (tmp[i] >> j) & 1;
        }
    }
    assert(b == bp);

    for (i = 0; i < 1000; i++) {
        for (j = 1; j < 100; j++) {
            x = hydro_random_uniform((uint32_t) j);
            assert(x < j);
        }
    }
}

static void
test_hash(void)
{
    hydro_hash_state st;
    uint8_t          dk[hydro_random_SEEDBYTES];
    uint8_t          h[100];
    uint8_t          key[hydro_hash_KEYBYTES];
#ifdef  __TRUSTINSOFT_ANALYZER__
    uint8_t          msg[32];
#else
    uint8_t          msg[1000];
#endif
    char             hex[100 * 2 + 1];
    size_t           i;

    memset(dk, 0, sizeof dk);
    hydro_random_buf_deterministic(key, sizeof key, dk);
    hydro_increment(dk, sizeof dk);
    hydro_hash_init(&st, ctx, key);
    for (i = 0; i <= sizeof msg; i++) {
        hydro_random_buf_deterministic(msg, i, dk);
        hydro_increment(dk, sizeof dk);
        hydro_hash_update(&st, msg, i);
    }
    hydro_hash_final(&st, h, sizeof h);
    hydro_bin2hex(hex, sizeof hex, h, sizeof h);
#ifndef  __TRUSTINSOFT_ANALYZER__
    assert_streq(
        "e5d2beb77a039965850ee76327e06b2fa6cb5121db8038b11bce4641a9c4bd843658104bdf07342570bb5fd1d7"
        "2c0d31a8981b47c718fddaffbd4171605c873cbaf921bb57988dd814f3a3fbef9799ff7c762705c4bf37ab2981"
        "5981bf0d8833d60afe14",
        hex);
#endif
    hydro_hash_hash(h, sizeof h, msg, sizeof msg, ctx, key);
    hydro_bin2hex(hex, sizeof hex, h, sizeof h);
#ifndef  __TRUSTINSOFT_ANALYZER__
    assert_streq(
        "724bd8883df73320ffd70923cb997f9a99bc670c4d78887be4975add0099fbf489b266a85d1f56743062d60a05"
        "590cbce47e45108367879bf4641cbaefe584e8618cbeb8c230ae956da22c7c5c4f11a8804ca576ec20fa5da239"
        "dde3d03a6018383c21f5",
        hex);
#endif
    hydro_hash_hash(h, hydro_hash_BYTES, msg, sizeof msg, ctx, key);
    hydro_bin2hex(hex, sizeof hex, h, hydro_hash_BYTES);
#ifndef  __TRUSTINSOFT_ANALYZER__
    assert_streq("7dfa45ce18210e2422fd658bf7beccb6e534e44f99ae359f4af3ba41af8ca463", hex);
#endif
    /* total input length is a multiple of the rate */
    hydro_hash_hash(h, hydro_hash_BYTES, msg, 13, ctx, key);
    hydro_bin2hex(hex, sizeof hex, h, hydro_hash_BYTES);
#ifndef  __TRUSTINSOFT_ANALYZER__
    assert_streq("d57a9800549bb4bab6a06fa6e16e08aad68d7d4313fb69a81b9f5d5af375dbe7", hex);
#endif
}

static void
test_core(void)
{
    uint8_t     x[100];
    uint8_t     y[100];
    uint8_t     a[5] = { 1, 2, 3, 4, 5 };
    uint8_t     b[5] = { 1, 2, 3, 4, 5 };
    char        hex[201];
    const char *hexf;

    memset(x, 0xd0, sizeof x);
    hydro_memzero(x, sizeof x);
    assert(x[0] == 0);
    assert(x[sizeof x - 1] == 0);
    hydro_increment(x, sizeof x);
    assert(x[0] == 1);
    assert(x[sizeof x - 1] == 0);
    x[0] = 0xff;
    hydro_increment(x, sizeof x);
    assert(x[0] == 0);
    assert(x[1] == 1);
    assert(x[sizeof x - 1] == 0);
    assert(hydro_equal(a, b, sizeof a));
    assert(!hydro_equal(a, a, sizeof a));
    assert(hydro_compare(a, b, sizeof a) == 0);
    assert(hydro_compare(a, a, sizeof a) == 0);
    a[0]++;
    assert(hydro_compare(a, b, sizeof a) == 1);
    assert(hydro_compare(b, a, sizeof a) == -1);
    hydro_random_buf(x, sizeof x);
    assert(hydro_bin2hex(hex, sizeof hex, x, sizeof x) != NULL);
    assert(hydro_hex2bin(y, 1, hex, sizeof hex, NULL, NULL) == -1);
    assert(hydro_hex2bin(y, sizeof y, hex, sizeof hex, NULL, NULL) == -1);
    assert(hydro_hex2bin(y, sizeof y, hex, sizeof hex - 1, NULL, NULL) == sizeof x);
    assert(hydro_equal(x, y, sizeof x));
    assert(hydro_hex2bin(x, sizeof x, "452a", 4, NULL, NULL) == 2);
    assert(hydro_hex2bin(y, sizeof y, "#452a#", 6, "#", NULL) == 2);
    assert(hydro_equal(x, y, sizeof x));
    memcpy(hex, "#452a", sizeof "#452a");
    assert(hydro_hex2bin(x, sizeof x, hex, 0, NULL, &hexf) == 0);
    assert(hexf == hex);
    assert(hydro_hex2bin(x, sizeof x, hex, sizeof "#452a", NULL, &hexf) == 0);
    assert(hexf == hex);
    assert(hydro_hex2bin(x, sizeof x, hex, sizeof "#452a", "#", &hexf) == 2);
    assert(hexf == hex + 6);
}

static void
test_secretbox(void)
{
    uint8_t key[hydro_secretbox_KEYBYTES];
    uint8_t m[25];
    uint8_t m2[25];
    uint8_t c[hydro_secretbox_HEADERBYTES + 25];
    uint8_t dk[hydro_random_SEEDBYTES];
    uint8_t probe[hydro_secretbox_PROBEBYTES];

    memset(dk, 0, sizeof dk);
    hydro_random_buf_deterministic(m, sizeof m, dk);
    hydro_increment(dk, sizeof dk);
    hydro_random_buf_deterministic(key, sizeof key, dk);
    hydro_increment(dk, sizeof dk);
    hydro_secretbox_encrypt(c, m, sizeof m, 0, ctx, key);
    assert(hydro_secretbox_decrypt(m2, c, sizeof c, 0, ctx, key) == 0);
    assert(hydro_equal(m, m2, sizeof m));

    hydro_secretbox_probe_create(probe, c, sizeof c, ctx, key);
    assert(hydro_secretbox_probe_verify(probe, c, sizeof c, ctx, key) == 0);
    probe[0]++;
    assert(hydro_secretbox_probe_verify(probe, c, sizeof c, ctx, key) == -1);
    probe[0]--;
    key[0]++;
    assert(hydro_secretbox_probe_verify(probe, c, sizeof c, ctx, key) == -1);
    key[0]--;

    assert(hydro_secretbox_decrypt(m2, c, 0, 0, ctx, key) == -1);
    assert(hydro_secretbox_decrypt(m2, c, 1, 0, ctx, key) == -1);
    assert(hydro_secretbox_decrypt(m2, c, hydro_secretbox_HEADERBYTES, 0, ctx, key) == -1);
    assert(hydro_secretbox_decrypt(m2, c, sizeof c, 1, ctx, key) == -1);
    assert(!hydro_equal(m, m2, sizeof m));
    key[0]++;
    assert(hydro_secretbox_decrypt(m2, c, sizeof c, 0, ctx, key) == -1);
    key[0]--;
    c[hydro_random_uniform(sizeof c)]++;
    assert(hydro_secretbox_decrypt(m2, c, sizeof c, 0, ctx, key) == -1);
}

static void
test_kdf(void)
{
    uint8_t key[hydro_kdf_KEYBYTES];
    uint8_t dk[hydro_random_SEEDBYTES];
    uint8_t subkey1[16];
    uint8_t subkey2[16];
    uint8_t subkey3[32];
    uint8_t subkey4[50];
    char    subkey1_hex[16 * 2 + 1];
    char    subkey2_hex[16 * 2 + 1];
    char    subkey3_hex[32 * 2 + 1];
    char    subkey4_hex[50 * 2 + 1];

    memset(dk, 0, sizeof dk);
    hydro_random_buf_deterministic(key, sizeof key, dk);
    hydro_kdf_derive_from_key(subkey1, sizeof subkey1, 1, ctx, key);
    hydro_kdf_derive_from_key(subkey2, sizeof subkey2, 2, ctx, key);
    hydro_kdf_derive_from_key(subkey3, sizeof subkey3, 0, ctx, key);
    hydro_kdf_derive_from_key(subkey4, sizeof subkey4, 0, ctx, key);
    hydro_bin2hex(subkey1_hex, sizeof subkey1_hex, subkey1, sizeof subkey1);
    hydro_bin2hex(subkey2_hex, sizeof subkey2_hex, subkey2, sizeof subkey2);
    hydro_bin2hex(subkey3_hex, sizeof subkey3_hex, subkey3, sizeof subkey3);
    hydro_bin2hex(subkey4_hex, sizeof subkey4_hex, subkey4, sizeof subkey4);
    assert_streq("af8019d3516d4ba6c80a7ea5a87e4d77", subkey1_hex);
    assert_streq("af8c4cba4e1f36c293631cc7001717dd", subkey2_hex);
    assert_streq("ff9345489dea1e4fe59194cea8794c9b0af9380c2d18c3ab38eeef2af95c1e26", subkey3_hex);
    assert_streq(
        "a8dd79ca19d604d1487b82d76b8d4ad4138a29dfaeeb207b99b2e5904e7855555bb94a76070fa71871df6ed911"
        "661d99efec",
        subkey4_hex);
}

static void
test_sign(void)
{
#ifdef  __TRUSTINSOFT_ANALYZER__
    uint8_t            msg[32];
#else
    uint8_t            msg[500];
#endif
    uint8_t            sig[hydro_sign_BYTES];
    hydro_sign_state   st;
    hydro_sign_keypair kp;

    hydro_random_buf(msg, sizeof msg);
    hydro_sign_keygen(&kp);
    hydro_sign_create(sig, msg, sizeof msg, ctx, kp.sk);
    assert(hydro_sign_verify(sig, msg, sizeof msg, ctx, kp.pk) == 0);
    sig[0]++;
    assert(hydro_sign_verify(sig, msg, sizeof msg, ctx, kp.pk) == -1);
    sig[0]--;
    sig[hydro_sign_BYTES - 1]++;
    assert(hydro_sign_verify(sig, msg, sizeof msg, ctx, kp.pk) == -1);
    sig[hydro_sign_BYTES - 1]--;
    msg[0]++;
    assert(hydro_sign_verify(sig, msg, sizeof msg, ctx, kp.pk) == -1);
    msg[0]++;
    hydro_sign_create(sig, msg, sizeof msg, ctx, kp.sk);

    hydro_sign_init(&st, ctx);
    hydro_sign_update(&st, msg, (sizeof msg) / 3);
    hydro_sign_update(&st, msg + (sizeof msg) / 3, (sizeof msg) - (sizeof msg) / 3);
    assert(hydro_sign_final_verify(&st, sig, kp.pk) == 0);

    hydro_sign_init(&st, ctx);
    hydro_sign_update(&st, msg, (sizeof msg) / 3);
    hydro_sign_update(&st, msg + (sizeof msg) / 3, (sizeof msg) - (sizeof msg) / 3);
    hydro_sign_final_create(&st, sig, kp.sk);

    hydro_sign_init(&st, ctx);
    hydro_sign_update(&st, msg, (sizeof msg) / 3);
    hydro_sign_update(&st, msg + (sizeof msg) / 3, (sizeof msg) - (sizeof msg) / 3);
    assert(hydro_sign_final_verify(&st, sig, kp.pk) == 0);

    hydro_sign_init(&st, ctx);
    hydro_sign_update(&st, msg, (sizeof msg) / 3);
    hydro_sign_update(&st, msg + (sizeof msg) / 3, (sizeof msg) - (sizeof msg) / 3);
    sig[0]++;
    assert(hydro_sign_final_verify(&st, sig, kp.pk) == -1);

    hydro_sign_create(sig, msg, 0, ctx, kp.sk);
    assert(hydro_sign_verify(sig, msg, sizeof msg, ctx, kp.pk) == -1);
    assert(hydro_sign_verify(sig, msg, 0, ctx, kp.pk) == 0);
}

static void
test_kx_n(void)
{
    hydro_kx_keypair         server_static_kp;
    uint8_t                  psk[hydro_kx_PSKBYTES];
    uint8_t                  packet1[hydro_kx_N_PACKET1BYTES];
    hydro_kx_session_keypair kp_client;
    hydro_kx_session_keypair kp_server;

    hydro_kx_keygen(&server_static_kp);
    hydro_random_buf(psk, sizeof psk);

    hydro_kx_n_1(&kp_client, packet1, psk, server_static_kp.pk);
    hydro_kx_n_2(&kp_server, packet1, psk, &server_static_kp);

    assert(hydro_equal(kp_client.tx, kp_server.rx, hydro_kx_SESSIONKEYBYTES));
    assert(hydro_equal(kp_client.rx, kp_server.tx, hydro_kx_SESSIONKEYBYTES));
}

static void
test_kx_kk(void)
{
    hydro_kx_state           st_client;
    hydro_kx_keypair         client_static_kp;
    hydro_kx_keypair         server_static_kp;
    uint8_t                  packet1[hydro_kx_KK_PACKET1BYTES];
    uint8_t                  packet2[hydro_kx_KK_PACKET2BYTES];
    hydro_kx_session_keypair kp_client;
    hydro_kx_session_keypair kp_server;

    hydro_kx_keygen(&client_static_kp);
    hydro_kx_keygen(&server_static_kp);

    hydro_kx_kk_1(&st_client, packet1, server_static_kp.pk, &client_static_kp);
    hydro_kx_kk_2(&kp_server, packet2, packet1, client_static_kp.pk, &server_static_kp);
    hydro_kx_kk_3(&st_client, &kp_client, packet2, &client_static_kp);

    assert(hydro_equal(kp_client.tx, kp_server.rx, hydro_kx_SESSIONKEYBYTES));
    assert(hydro_equal(kp_client.rx, kp_server.tx, hydro_kx_SESSIONKEYBYTES));
}

static void
test_kx_xx(void)
{
    hydro_kx_state           st_client;
    hydro_kx_state           st_server;
    hydro_kx_keypair         client_static_kp;
    hydro_kx_keypair         server_static_kp;
    uint8_t                  psk[hydro_kx_PSKBYTES];
    uint8_t                  client_peer_pk[hydro_kx_PUBLICKEYBYTES];
    uint8_t                  server_peer_pk[hydro_kx_PUBLICKEYBYTES];
    uint8_t                  packet1[hydro_kx_XX_PACKET1BYTES];
    uint8_t                  packet2[hydro_kx_XX_PACKET2BYTES];
    uint8_t                  packet3[hydro_kx_XX_PACKET3BYTES];
    hydro_kx_session_keypair kp_client;
    hydro_kx_session_keypair kp_server;

    hydro_kx_keygen(&client_static_kp);
    hydro_kx_keygen(&server_static_kp);

    hydro_kx_xx_1(&st_client, packet1, NULL);
    hydro_kx_xx_2(&st_server, packet2, packet1, NULL, &server_static_kp);
    hydro_kx_xx_3(&st_client, &kp_client, packet3, NULL, packet2, NULL, &client_static_kp);
    hydro_kx_xx_4(&st_server, &kp_server, NULL, packet3, NULL);

    assert(hydro_equal(kp_client.tx, kp_server.rx, hydro_kx_SESSIONKEYBYTES));
    assert(hydro_equal(kp_client.rx, kp_server.tx, hydro_kx_SESSIONKEYBYTES));

    hydro_random_buf(psk, sizeof psk);
    hydro_kx_xx_1(&st_client, packet1, psk);
    hydro_kx_xx_2(&st_server, packet2, packet1, psk, &server_static_kp);
    hydro_kx_xx_3(&st_client, &kp_client, packet3, client_peer_pk, packet2, psk, &client_static_kp);
    hydro_kx_xx_4(&st_server, &kp_server, server_peer_pk, packet3, psk);

    assert(hydro_equal(kp_client.tx, kp_server.rx, hydro_kx_SESSIONKEYBYTES));
    assert(hydro_equal(kp_client.rx, kp_server.tx, hydro_kx_SESSIONKEYBYTES));
    assert(hydro_equal(client_peer_pk, server_static_kp.pk, hydro_kx_PUBLICKEYBYTES));
    assert(hydro_equal(server_peer_pk, client_static_kp.pk, hydro_kx_PUBLICKEYBYTES));
}

static void
test_kx_nk(void)
{
    hydro_kx_state           st_client;
    hydro_kx_keypair         server_static_kp;
    uint8_t                  psk[hydro_kx_PSKBYTES];
    uint8_t                  packet1[hydro_kx_NK_PACKET1BYTES];
    uint8_t                  packet2[hydro_kx_NK_PACKET2BYTES];
    hydro_kx_session_keypair kp_client;
    hydro_kx_session_keypair kp_server;

    hydro_kx_keygen(&server_static_kp);

    hydro_kx_nk_1(&st_client, packet1, NULL, server_static_kp.pk);
    hydro_kx_nk_2(&kp_server, packet2, packet1, NULL, &server_static_kp);
    hydro_kx_nk_3(&st_client, &kp_client, packet2);

    assert(hydro_equal(kp_client.tx, kp_server.rx, hydro_kx_SESSIONKEYBYTES));
    assert(hydro_equal(kp_client.rx, kp_server.tx, hydro_kx_SESSIONKEYBYTES));

    hydro_random_buf(psk, sizeof psk);

    hydro_kx_nk_1(&st_client, packet1, psk, server_static_kp.pk);
    hydro_kx_nk_2(&kp_server, packet2, packet1, psk, &server_static_kp);
    hydro_kx_nk_3(&st_client, &kp_client, packet2);

    assert(hydro_equal(kp_client.tx, kp_server.rx, hydro_kx_SESSIONKEYBYTES));
    assert(hydro_equal(kp_client.rx, kp_server.tx, hydro_kx_SESSIONKEYBYTES));
}

static void
test_pwhash(void)
{
    uint8_t            master_key[hydro_pwhash_MASTERKEYBYTES];
    uint8_t            new_master_key[hydro_pwhash_MASTERKEYBYTES];
    uint8_t            stored[hydro_pwhash_STOREDBYTES];
    uint8_t            h[64];
    uint8_t            static_key[64];
    char               h_hex[2 * 64 + 1];
    unsigned long long ops = 1000;

    memset(master_key, 'x', sizeof master_key);
    hydro_pwhash_deterministic(h, sizeof h, "test", sizeof "test" - 1, ctx, master_key, ops, 0, 1);
    hydro_bin2hex(h_hex, sizeof h_hex, h, sizeof h);
    if (ops == 1000) {
        assert_streq(
            "2f1a804a02f25066fd0688bf8b8e03dff3a3866958a9cf5883c459e602e232d38e3e488723f0b4a2bc61d2"
            "0cb36a04a4d2eb18be99bc61870d72d7de5d67f237",
            h_hex);
    }

    hydro_pwhash_keygen(master_key);
    assert(hydro_pwhash_create(stored, "test", sizeof "test" - 1, master_key, ops, 0, 1) == 0);
    assert(hydro_pwhash_verify(stored, "test", sizeof "test" - 1, master_key, ops, 0, 1) == 0);
    assert(hydro_pwhash_verify(stored, "test", sizeof "test" - 1, master_key, ops * 2, 10, 10) ==
           0);
    assert(hydro_pwhash_verify(stored, "test", sizeof "test" - 1, master_key, ops / 2, 10, 10) ==
           -1);
    assert(hydro_pwhash_verify(stored, "Test", sizeof "Test" - 1, master_key, ops, 0, 1) == -1);
    assert(hydro_pwhash_verify(stored, "test", sizeof "tes" - 1, master_key, ops, 0, 1) == -1);

    assert(hydro_pwhash_derive_static_key(static_key, sizeof static_key, stored, "test",
                                          sizeof "test" - 1, ctx, master_key, ops, 0, 1) == 0);
    assert(hydro_pwhash_derive_static_key(static_key, sizeof static_key, stored, "Test",
                                          sizeof "Test" - 1, ctx, master_key, ops, 0, 1) == -1);

    assert(hydro_pwhash_reencrypt(stored, master_key, master_key) == 0);
    assert(hydro_pwhash_verify(stored, "test", sizeof "test" - 1, master_key, ops, 0, 1) == 0);
    hydro_pwhash_keygen(new_master_key);
    assert(hydro_pwhash_reencrypt(stored, master_key, new_master_key) == 0);
    assert(hydro_pwhash_verify(stored, "test", sizeof "test" - 1, master_key, ops, 0, 1) == -1);
    assert(hydro_pwhash_verify(stored, "test", sizeof "test" - 1, new_master_key, ops, 0, 1) == 0);

    assert(hydro_pwhash_upgrade(stored, new_master_key, ops * 2, 0, 1) == 0);
    assert(hydro_pwhash_verify(stored, "test", sizeof "test" - 1, new_master_key, ops, 0, 1) == -1);
    assert(hydro_pwhash_verify(stored, "test", sizeof "test" - 1, new_master_key, ops * 2, 0, 1) ==
           0);
}

int
main(void)
{
#ifdef _MSC_VER
    /*
     * On Windows, disable the "Abort - Retry - Ignore" GUI dialog that otherwise pops up on
     * assertion failure.
     */
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
#endif

    int ret;

    ret = hydro_init();
    assert(ret == 0);

    test_core();
    test_hash();
    test_kdf();
    test_kx_n();
    test_kx_kk();
    test_kx_xx();
    test_kx_nk();
    test_pwhash();
    test_randombytes();
    test_secretbox();
    test_sign();

    return 0;
}
