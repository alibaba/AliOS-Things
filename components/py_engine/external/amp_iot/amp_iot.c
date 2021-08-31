#include "amp_iot.h"
#include "amp_encrypt.h"
#include "amp_string.h"

#define LOG() aos_printf("zzz %s line  %d\n", __FUNCTION__, __LINE__)

// define static value for test
// -----BEGIN PUBLIC KEY-----
// MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCLGcVZcBZmsusYf4cjkjSgCkiV
// 2d2bfFyeMbBvFI58enXX3ZX9my7VjzeAVfv7u4mIbnCcTeYGYW/v8cvxty2jcDVC
// SLvuOMuPimMBjjU+EhayFOzyzfQamXc0fHtvXIHMgN5RQg2Gjjs684LaG8SaFNQx
// ZjubD00zykFtxkjD0wIDAQAB
// -----END PUBLIC KEY-----
static char public_key[] = "-----BEGIN PUBLIC KEY-----\n"
                            "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCLGcVZcBZmsusYf4cjkjSgCkiV\n"
                            "2d2bfFyeMbBvFI58enXX3ZX9my7VjzeAVfv7u4mIbnCcTeYGYW/v8cvxty2jcDVC\n"
                            "SLvuOMuPimMBjjU+EhayFOzyzfQamXc0fHtvXIHMgN5RQg2Gjjs684LaG8SaFNQx\n"
                            "ZjubD00zykFtxkjD0wIDAQAB\n"
                            "-----END PUBLIC KEY-----\n";


// -----BEGIN RSA PRIVATE KEY-----
// MIICWwIBAAKBgQCLGcVZcBZmsusYf4cjkjSgCkiV2d2bfFyeMbBvFI58enXX3ZX9
// my7VjzeAVfv7u4mIbnCcTeYGYW/v8cvxty2jcDVCSLvuOMuPimMBjjU+EhayFOzy
// zfQamXc0fHtvXIHMgN5RQg2Gjjs684LaG8SaFNQxZjubD00zykFtxkjD0wIDAQAB
// AoGAZppCxJ72FxjAatiegPF2WdpcAhSq7ubDlniGcNkT5OObivfKjD9sQ2CZitNh
// xCRn5Te5XsHi+y/iFnYffhMk2MFrJwVhCvEbp6TCcFHinKhLuviNjt8LNRjDTw+Q
// keZ+xWItEIuOpdZdW5BHA7x60Tv9vwKrC3xmLulC/QL+CwECQQDZFBYH4aSd0Q8d
// yUeVSoviCgYDnRfqb0sHmrbjPVZ0JxJwQGgqdRZeRCtobHVHf90IcqreNrVJSyPH
// 65QB44FTAkEApAp/5XLH+Io9OuOjZnmOOvTkmmeOb8kwNMFXJwQZ+3lB2j+qCcMV
// H0sLuMPlrotSPCCQIVh35qcPM6paVijjgQJAf2NNJmThEIfNgiJLMq3jdsuBCf/X
// OAkFk7nUF3g4UsNXC66QMlIxxTHl80qqbEH5uOppoSujqRZCMiDp403Z5wJACwxh
// /NQ0P7sf0J2QXdCGEdZ4pyWeZE0KoCL9ebU/Ar7x/n8XNr0a6Ram3XTkPVNI4J/Y
// 1UYeHTzXO0WX/AGZAQJAG5uQm7gB4fjkVuW3WcndMBJEDGT32YIWbApmLRKCdYot
// 8TzwcCI+Wc/a4iEQdEdYQ/5lMu6fQOWJ3wZ8bdagMQ==
// -----END RSA PRIVATE KEY-----
static char private_key[] = "-----BEGIN RSA PRIVATE KEY-----\n"
                            "MIICWwIBAAKBgQCLGcVZcBZmsusYf4cjkjSgCkiV2d2bfFyeMbBvFI58enXX3ZX9\n"
                            "my7VjzeAVfv7u4mIbnCcTeYGYW/v8cvxty2jcDVCSLvuOMuPimMBjjU+EhayFOzy\n"
                            "zfQamXc0fHtvXIHMgN5RQg2Gjjs684LaG8SaFNQxZjubD00zykFtxkjD0wIDAQAB\n"
                            "AoGAZppCxJ72FxjAatiegPF2WdpcAhSq7ubDlniGcNkT5OObivfKjD9sQ2CZitNh\n"
                            "xCRn5Te5XsHi+y/iFnYffhMk2MFrJwVhCvEbp6TCcFHinKhLuviNjt8LNRjDTw+Q\n"
                            "keZ+xWItEIuOpdZdW5BHA7x60Tv9vwKrC3xmLulC/QL+CwECQQDZFBYH4aSd0Q8d\n"
                            "yUeVSoviCgYDnRfqb0sHmrbjPVZ0JxJwQGgqdRZeRCtobHVHf90IcqreNrVJSyPH\n"
                            "65QB44FTAkEApAp/5XLH+Io9OuOjZnmOOvTkmmeOb8kwNMFXJwQZ+3lB2j+qCcMV\n"
                            "H0sLuMPlrotSPCCQIVh35qcPM6paVijjgQJAf2NNJmThEIfNgiJLMq3jdsuBCf/X\n"
                            "OAkFk7nUF3g4UsNXC66QMlIxxTHl80qqbEH5uOppoSujqRZCMiDp403Z5wJACwxh\n"
                            "/NQ0P7sf0J2QXdCGEdZ4pyWeZE0KoCL9ebU/Ar7x/n8XNr0a6Ram3XTkPVNI4J/Y\n"
                            "1UYeHTzXO0WX/AGZAQJAG5uQm7gB4fjkVuW3WcndMBJEDGT32YIWbApmLRKCdYot\n"
                            "8TzwcCI+Wc/a4iEQdEdYQ/5lMu6fQOWJ3wZ8bdagMQ==\n"
                            "-----END RSA PRIVATE KEY-----\n";


static char signature_output[] = "ZbUUmlmmENOqFQm3EiWPl6wwTyllt/90eVgYwsheKvizdJFenulePQTiDBeOFSAWlcMQArT+5wK1e1e3eyhP4bBPFMgRGp7hN4rSdM3orj/obBr+pcB7DyNmeci/4wQgMSrIzk4kI11HvHacSC1WajZKBE/107raobfgrE0+Gxg=";

static char input_text[] = "1234567890";


int haas_sig_verify(void)
{
    return amp_verify_rsa_sign(input_text, signature_output, public_key);
}

int haas_sig_encrypt(void)
{
    amp_encrypt_rsa(input_text, public_key);
}

int haas_string_test(void)
{
    int ret = 0;
    uint8_t expected_digest[16] = {0};

    char str_input[]="000102030405060708090a0b0c0d0e0f";

    aos_printf("amp str len sizeof len=%d\n", sizeof(str_input));
    aos_printf("amp str len strlen len=%d\n", strlen(str_input));
    ret = amp_str2hex(str_input, strlen(str_input), expected_digest);
    if(ret != 0)
    {
        aos_printf("amp str to hex error !!!\n");
    }

    for(int ii = 0; ii < 16; ii++) {
        aos_printf("%02x", expected_digest[ii]);
    }
    aos_printf("\n\n");
}

// use pri key sign test
int sing_test(void)
{
    FILE *f;
    int ret = 1;
    int exit_code = MBEDTLS_EXIT_FAILURE;
    size_t i;
    mbedtls_rsa_context rsa;
    unsigned char hash[32];
    unsigned char buf[MBEDTLS_MPI_MAX_SIZE];
    char filename[512];
    mbedtls_mpi N, P, Q, D, E, DP, DQ, QP;

    char prikey_n[]="A1D46FBA2318F8DCEF16C280948B1CF27966B9B47225ED2989F8D74B45BD36049C0AAB5AD0FF003553BA843C8E12782FC5873BB89A3DC84B883D25666CD22BF3ACD5B675969F8BEBFBCAC93FDD927C7442B178B10D1DFF9398E52316AAE0AF74E594650BDC3C670241D418684593CDA1A7B9DC4F20D2FDC6F66344074003E211";
    char prikey_e[]="010001";
    char prikey_d[]="589552BB4F2F023ADDDD5586D0C8FD857512D82080436678D07F984A29D892D31F1F7000FC5A39A0F73E27D885E47249A4148C8A5653EF69F91F8F736BA9F84841C2D99CD8C24DE8B72B5C9BE0EDBE23F93D731749FEA9CFB4A48DD2B7F35A2703E74AA2D4DB7DE9CEEA7D763AF0ADA7AC176C4E9A22C4CDA65CEC0C65964401";
    char prikey_p[]="CD083568D2D46C44C40C1FA0101AF2155E59C70B08423112AF0C1202514BBA5210765E29FF13036F56C7495894D80CF8C3BAEE2839BACBB0B86F6A2965F60DB1";
    char prikey_q[]="CA0EEEA5E710E8E9811A6B846399420E3AE4A4C16647E426DDF8BBBCB11CD3F35CE2E4B6BCAD07AE2C0EC2ECBFCC601B207CDD77B5673E16382B1130BF465261";
    char prikey_dp[]="0D0E21C07BF434B4A83B116472C2147A11D8EB98A33CFBBCF1D275EF19D815941622435AAF3839B6C432CA53CE9E772CFBE1923A937A766FD93E96E6EDEC1DF1";
    char prikey_dq[]="269CEBE6305DFEE4809377F078C814E37B45AE6677114DFC4F76F5097E1F3031D592567AC55B9B98213B40ECD54A4D2361F5FAACA1B1F51F71E4690893C4F081";
    char prikey_qp[]="97AC5BB885ABCA314375E9E4DB1BA4B2218C90619F61BD474F5785075ECA81750A735199A8C191FE2D3355E7CF601A70E5CABDE0E02C2538BB9FB4871540B3C1";


    mbedtls_rsa_init( &rsa, MBEDTLS_RSA_PKCS_V15, 0 );

    mbedtls_mpi_init( &N ); mbedtls_mpi_init( &P ); mbedtls_mpi_init( &Q );
    mbedtls_mpi_init( &D ); mbedtls_mpi_init( &E ); mbedtls_mpi_init( &DP );
    mbedtls_mpi_init( &DQ ); mbedtls_mpi_init( &QP );

    aos_printf( "\n  . Reading private key from rsa_priv.txt" );

    if( ( ret = mbedtls_mpi_read_string( &N , 16, prikey_n ) ) != 0 ||
        ( ret = mbedtls_mpi_read_string( &E , 16, prikey_e ) ) != 0 ||
        ( ret = mbedtls_mpi_read_string( &D , 16, prikey_d ) ) != 0 ||
        ( ret = mbedtls_mpi_read_string( &P , 16, prikey_p ) ) != 0 ||
        ( ret = mbedtls_mpi_read_string( &Q , 16, prikey_q ) ) != 0 ||
        ( ret = mbedtls_mpi_read_string( &DP , 16, prikey_dp ) ) != 0 ||
        ( ret = mbedtls_mpi_read_string( &DQ , 16, prikey_dq ) ) != 0 ||
        ( ret = mbedtls_mpi_read_string( &QP , 16, prikey_qp ) ) != 0 )
    {
        aos_printf( " failed\n  ! mbedtls_mpi_read_file returned %d\n\n", ret );
        goto exit;
    }

    if( ( ret = mbedtls_rsa_import( &rsa, &N, &P, &Q, &D, &E ) ) != 0 )
    {
        aos_printf( " failed\n  ! mbedtls_rsa_import returned %d\n\n", ret );
        goto exit;
    }

    if( ( ret = mbedtls_rsa_complete( &rsa ) ) != 0 )
    {
        aos_printf( " failed\n  ! mbedtls_rsa_complete returned %d\n\n", ret );
        goto exit;
    }

    aos_printf( "\n  . Checking the private key" );

    if( ( ret = mbedtls_rsa_check_privkey( &rsa ) ) != 0 )
    {
        aos_printf( " failed\n  ! mbedtls_rsa_check_privkey failed with -0x%0x\n", -ret );
        goto exit;
    }

    /*
     * Compute the SHA-256 hash of the input file,
     * then calculate the RSA signature of the hash.
     */
    aos_printf( "\n  . Generating the RSA/SHA-256 signature" );


    int                 dig_len = 0;
    unsigned char       *dig    = NULL;

    dig_len = 32;
    dig = aos_malloc(dig_len);

    if (NULL == dig) {
        ret = -1;
        aos_printf("222 rsa verify err:%d\n", ret);
        goto exit;
    }
    // input_text hash change
    ret = amp_verify_rsa_hash(input_text, strlen(input_text), dig, AMP_HASH_SHA256);

    if (ret != 0) {
        ret = -1;
        aos_printf("333 rsa verify err:%d\n", ret);
        goto exit;
    }
    // aos_printf("zzadd hash hash=%s\n", hash);
    // 1234567890
    // hash :  c775e7b757ede630cd0aa1113bd102661ab38829ca52a6422ab782862f268646
    aos_printf("zzadd hash finish\n");
    for(int ii=0; ii<32; ii++) {
        aos_printf("%02x", dig[ii]);
    }
    aos_printf("\n");
    
    if( ( ret = mbedtls_rsa_pkcs1_sign( &rsa, NULL, NULL, MBEDTLS_RSA_PRIVATE, MBEDTLS_MD_SHA256,
                                20, dig, buf ) ) != 0 )
    {
        aos_printf( " failed\n  ! mbedtls_rsa_pkcs1_sign returned -0x%0x\n\n", -ret );
        goto exit;
    }

    // make signe
    // 92 EC 7E 4B AA DA A7 C4 5B F7 65 03 79 B5 33 26
    // 7B 66 84 86 D1 50 78 B7 E2 B0 FA 36 7E 79 C2 DC
    // BC F2 66 06 8F 44 81 CD FF 33 C3 27 3A 47 B9 87
    // E0 B8 AF B8 13 CE AC B2 23 4E 7C 4E B9 E3 24 8C
    // F5 33 07 01 89 EC C7 2B C8 D5 65 4E 60 50 02 5D
    // B3 A8 ED 1E CD 21 A9 58 64 68 35 66 8D 97 64 38
    // CE 04 5E 2E 00 04 E8 E3 F9 28 9C 35 41 3E B3 D9
    // 83 D3 3D FD 55 6A 55 7D 28 9A 5D 78 DD 16 45 3B
    // 
    // kux+S6rap8Rb92UDebUzJntmhIbRUHi34rD6Nn55wty88mYGj0SBzf8zwyc6R7mH4LivuBPOrLIjTnxOueMkjPUzBwGJ7McryNVlTmBQAl2zqO0ezSGpWGRoNWaNl2Q4zgReLgAE6OP5KJw1QT6z2YPTPf1ValV9KJpdeN0WRTs=
    char sig[1024] = {0};
    size_t sig_len;

    mbedtls_base64_encode(sig, sizeof(sig), &sig_len, buf, 128);
    aos_printf("sig=%s\n", sig);
    aos_printf("sig_len=%d\n", sig_len);

    exit_code = MBEDTLS_EXIT_SUCCESS;

exit:

    mbedtls_rsa_free( &rsa );
    mbedtls_mpi_free( &N ); mbedtls_mpi_free( &P ); mbedtls_mpi_free( &Q );
    mbedtls_mpi_free( &D ); mbedtls_mpi_free( &E ); mbedtls_mpi_free( &DP );
    mbedtls_mpi_free( &DQ ); mbedtls_mpi_free( &QP );

    return( exit_code );
}


int verify_sing_test(void)
{
    FILE *f;
    int ret = 1, c;
    int exit_code = MBEDTLS_EXIT_FAILURE;
    size_t i;
    mbedtls_rsa_context rsa;
    unsigned char hash[32];
    unsigned char buf[MBEDTLS_MPI_MAX_SIZE];
    char filename[512];

    char prikey_n[]="A1D46FBA2318F8DCEF16C280948B1CF27966B9B47225ED2989F8D74B45BD36049C0AAB5AD0FF003553BA843C8E12782FC5873BB89A3DC84B883D25666CD22BF3ACD5B675969F8BEBFBCAC93FDD927C7442B178B10D1DFF9398E52316AAE0AF74E594650BDC3C670241D418684593CDA1A7B9DC4F20D2FDC6F66344074003E211";
    char prikey_e[]="010001";

    char sign_file[]="kux+S6rap8Rb92UDebUzJntmhIbRUHi34rD6Nn55wty88mYGj0SBzf8zwyc6R7mH4LivuBPOrLIjTnxOueMkjPUzBwGJ7McryNVlTmBQAl2zqO0ezSGpWGRoNWaNl2Q4zgReLgAE6OP5KJw1QT6z2YPTPf1ValV9KJpdeN0WRTs=";

    mbedtls_rsa_init( &rsa, MBEDTLS_RSA_PKCS_V15, 0 );

    aos_printf( "\n  . Reading public key from rsa_pub.txt\n" );

    if( ( ret = mbedtls_mpi_read_string( &rsa.N, 16, prikey_n ) ) != 0 ||
        ( ret = mbedtls_mpi_read_string( &rsa.E, 16, prikey_e ) ) != 0 )
    {
        aos_printf( " failed\n  ! mbedtls_mpi_read_file returned %d\n\n", ret );
        goto exit;
    }

    rsa.len = ( mbedtls_mpi_bitlen( &rsa.N ) + 7 ) >> 3;

    /*
     * Extract the RSA signature from the text file
     */
    char sig[256] = {0};
    size_t sig_len;

    mbedtls_base64_decode(sig, sizeof(sig), &sig_len, sign_file, strlen(sign_file));

    aos_printf("zzzzzzzzzzz sig_len= %d\n", sig_len);
    for(int ii=0; ii<sig_len; ii++)
    {
        aos_printf("%02x", sig[ii]);
    }
    aos_printf("\n");

    /*
     * Compute the SHA-256 hash of the input file and
     * verify the signature
     */
    aos_printf( "\n  . Verifying the RSA/SHA-256 signature" );

    int                 dig_len = 0;
    unsigned char       *dig    = NULL;

    dig_len = 32;
    dig = aos_malloc(dig_len);

    if (NULL == dig) {
        ret = -1;
        aos_printf("222 rsa verify err:%d\n", ret);
        goto exit;
    }
    // input_text hash change
    ret = amp_verify_rsa_hash(input_text, strlen(input_text), dig, AMP_HASH_SHA256);

    if (ret != 0) {
        ret = -1;
        aos_printf("333 rsa verify err:%d\n", ret);
        goto exit;
    }
    // aos_printf("zzadd hash hash=%s\n", hash);
    // 1234567890
    // hash :  c775e7b757ede630cd0aa1113bd102661ab38829ca52a6422ab782862f268646
    aos_printf("zzadd hash finish\n");
    for(int ii=0; ii<32; ii++) {
        aos_printf("%02x", dig[ii]);
    }
    aos_printf("\n");


    if( ( ret = mbedtls_rsa_pkcs1_verify( &rsa, NULL, NULL, MBEDTLS_RSA_PUBLIC,
                                  MBEDTLS_MD_SHA256, 20, dig, sig ) ) != 0 )
    {
        aos_printf( " failed\n  ! mbedtls_rsa_pkcs1_verify returned -0x%0x\n\n", -ret );
        goto exit;
    }

    aos_printf( "\n  . OK (the signature is valid)\n\n" );

    exit_code = MBEDTLS_EXIT_SUCCESS;

exit:

    mbedtls_rsa_free( &rsa );

    return( exit_code );
}

int rsa_encrypt_test(void)
{
    FILE *f;
    int ret = 1;
    int exit_code = MBEDTLS_EXIT_FAILURE;
    size_t i, olen = 0;
    mbedtls_pk_context pk;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    unsigned char input[1024];
    unsigned char buf[512];
    const char *pers = "mbedtls_pk_encrypt";

    mbedtls_ctr_drbg_init( &ctr_drbg );
    mbedtls_entropy_init( &entropy );
    mbedtls_pk_init( &pk );

    aos_printf( "\n  . Seeding the random number generator...\n" );

    if( ( ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func,
                                       &entropy, (const unsigned char *) pers,
                                       strlen( pers ) ) ) != 0 )
    {
        aos_printf( " failed\n  ! mbedtls_ctr_drbg_seed returned -0x%04x\n", -ret );
        goto exit;
    }

    //======== step 1 : get public key n & e param from pub_key =======//

    ret = mbedtls_pk_parse_public_key(&pk, public_key, (strlen(public_key)+1));
    // 

    /*
     * Calculate the RSA encryption of the hash.
     */
    aos_printf( "\n  . Generating the encrypted value\n" );

    if( ( ret = mbedtls_pk_encrypt( &pk, input_text, strlen(input_text),
                            buf, &olen, sizeof(buf),
                            mbedtls_ctr_drbg_random, &ctr_drbg ) ) != 0 )
    {
        aos_printf( " failed\n  ! mbedtls_pk_encrypt returned -0x%04x\n", -ret );
        goto exit;
    }

    /*
     * Write the signature into result-enc.txt
     */
    aos_printf("==zzzzzz encrypt data ==\n\n");
    for( i = 0; i < olen; i++ )
    {
        aos_printf("%02X", buf[i]);
    }
    aos_printf("\n");

    char sig[1024] = {0};
    size_t sig_len;

    mbedtls_base64_encode(sig, sizeof(sig), &sig_len, buf, 128);
    aos_printf("sig=%s\n", sig);
    aos_printf("sig_len=%d\n", sig_len);

    // fclose( f );
    aos_printf( "\n  . Done (created \"%s\")\n\n");
    // 
    // CdYgOedtTZG0aC9NYkZcbl4RfRmLsb5NB5IBwWr7Df3j5Jd2yQwjzX3AkiCkfWLfqXY+Ft95stPQb0GIbH1KdsIVgIG/gKGByKPayWMW7RVitfgJ311tJOMRK4BQkWmfH66kV5Eu1e66QeX78VuURx5g5AKqgQsjl2JweRuVWfs=
    // 
    exit_code = MBEDTLS_EXIT_SUCCESS;

exit:

    mbedtls_pk_free( &pk );
    mbedtls_entropy_free( &entropy );
    mbedtls_ctr_drbg_free( &ctr_drbg );

    return( exit_code );
}

// 
// test data :
//  Block Cipher : algorithm : AES-256
//  MODE: CBC (Cipher Block Chaining)
//  test KEY :  c775e7b757ede630cd0aa1113bd102661ab38829ca52a6422ab782862f268646   (use string "1234567890" do sha256 change)
//  IV : 000102030405060708090A0B0C0D0E0F
//  input test : "1234567890"
//  result Bytes: 9e f5 00 f5 97 09 2e 89 f1 a7 a0 f0 a4 2a b1 6b  (change to string by base64: nvUA9ZcJLonxp6DwpCqxaw== )
// 
#define MODE_ENCRYPT    0
#define MODE_DECRYPT    1

int aes_crypt_test(void)
{
    int i;
    int ret = 0;
	mbedtls_aes_context aes_ctx;
	
	//密钥数值
	unsigned char key[32] = {0xc7,0x75,0xe7,0xb7,0x57,0xed,0xe6,0x30,0xcd,0x0a,0xa1,0x11,0x3b,0xd1,0x02,0x66,0x1a,0xb3,0x88,0x29,0xca,0x52,0xa6,0x42,0x2a,0xb7,0x82,0x86,0x2f,0x26,0x86,0x46};
	//iv
	unsigned char iv[16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};
	
	//明文空间
	unsigned char plain[16] = {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x30,0x30,0x30,0x30,0x30,0x30};
	//解密后明文的空间
	unsigned char dec_plain[32]={0};
	//密文空间
	unsigned char cipher[32]={0};

	mbedtls_aes_init( &aes_ctx );
	
	// encode
	aos_printf("plain:%s\r\n", plain);
	mbedtls_aes_setkey_enc( &aes_ctx, key, 256);

	mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_ENCRYPT, 32, iv, plain, cipher);

    for(int ii=0; ii<32; ii++)
    {
        aos_printf("%02x", cipher[ii]);
    }
	aos_printf("\n");

	// decode
	// mbedtls_aes_setkey_dec(&aes_ctx, key, 256);
	
	// mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_DECRYPT, 64, iv, cipher, dec_plain);
	// aos_printf("dec_plain:%s\r\n", dec_plain);
	// aos_printf("\r\n");

	mbedtls_aes_free( &aes_ctx );

    return ret;
}
