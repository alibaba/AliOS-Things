"""
This code was Ported from CPython's sha512module.c
"""

SHA_BLOCKSIZE = 128
SHA_DIGESTSIZE = 64


def new_shaobject():
    return {
        "digest": [0] * 8,
        "count_lo": 0,
        "count_hi": 0,
        "data": [0] * SHA_BLOCKSIZE,
        "local": 0,
        "digestsize": 0,
    }


ROR64 = (
    lambda x, y: (((x & 0xFFFFFFFFFFFFFFFF) >> (y & 63)) | (x << (64 - (y & 63))))
    & 0xFFFFFFFFFFFFFFFF
)
Ch = lambda x, y, z: (z ^ (x & (y ^ z)))
Maj = lambda x, y, z: (((x | y) & z) | (x & y))
S = lambda x, n: ROR64(x, n)
R = lambda x, n: (x & 0xFFFFFFFFFFFFFFFF) >> n
Sigma0 = lambda x: (S(x, 28) ^ S(x, 34) ^ S(x, 39))
Sigma1 = lambda x: (S(x, 14) ^ S(x, 18) ^ S(x, 41))
Gamma0 = lambda x: (S(x, 1) ^ S(x, 8) ^ R(x, 7))
Gamma1 = lambda x: (S(x, 19) ^ S(x, 61) ^ R(x, 6))


def sha_transform(sha_info):
    W = []

    d = sha_info["data"]
    for i in range(0, 16):
        W.append(
            (d[8 * i] << 56)
            + (d[8 * i + 1] << 48)
            + (d[8 * i + 2] << 40)
            + (d[8 * i + 3] << 32)
            + (d[8 * i + 4] << 24)
            + (d[8 * i + 5] << 16)
            + (d[8 * i + 6] << 8)
            + d[8 * i + 7]
        )

    for i in range(16, 80):
        W.append(
            (Gamma1(W[i - 2]) + W[i - 7] + Gamma0(W[i - 15]) + W[i - 16]) & 0xFFFFFFFFFFFFFFFF
        )

    ss = sha_info["digest"][:]

    def RND(a, b, c, d, e, f, g, h, i, ki):
        t0 = (h + Sigma1(e) + Ch(e, f, g) + ki + W[i]) & 0xFFFFFFFFFFFFFFFF
        t1 = (Sigma0(a) + Maj(a, b, c)) & 0xFFFFFFFFFFFFFFFF
        d = (d + t0) & 0xFFFFFFFFFFFFFFFF
        h = (t0 + t1) & 0xFFFFFFFFFFFFFFFF
        return d & 0xFFFFFFFFFFFFFFFF, h & 0xFFFFFFFFFFFFFFFF

    ss[3], ss[7] = RND(
        ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], 0, 0x428A2F98D728AE22
    )
    ss[2], ss[6] = RND(
        ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], 1, 0x7137449123EF65CD
    )
    ss[1], ss[5] = RND(
        ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], 2, 0xB5C0FBCFEC4D3B2F
    )
    ss[0], ss[4] = RND(
        ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], 3, 0xE9B5DBA58189DBBC
    )
    ss[7], ss[3] = RND(
        ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], 4, 0x3956C25BF348B538
    )
    ss[6], ss[2] = RND(
        ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], 5, 0x59F111F1B605D019
    )
    ss[5], ss[1] = RND(
        ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], 6, 0x923F82A4AF194F9B
    )
    ss[4], ss[0] = RND(
        ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], 7, 0xAB1C5ED5DA6D8118
    )
    ss[3], ss[7] = RND(
        ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], 8, 0xD807AA98A3030242
    )
    ss[2], ss[6] = RND(
        ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], 9, 0x12835B0145706FBE
    )
    ss[1], ss[5] = RND(
        ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], 10, 0x243185BE4EE4B28C
    )
    ss[0], ss[4] = RND(
        ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], 11, 0x550C7DC3D5FFB4E2
    )
    ss[7], ss[3] = RND(
        ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], 12, 0x72BE5D74F27B896F
    )
    ss[6], ss[2] = RND(
        ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], 13, 0x80DEB1FE3B1696B1
    )
    ss[5], ss[1] = RND(
        ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], 14, 0x9BDC06A725C71235
    )
    ss[4], ss[0] = RND(
        ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], 15, 0xC19BF174CF692694
    )
    ss[3], ss[7] = RND(
        ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], 16, 0xE49B69C19EF14AD2
    )
    ss[2], ss[6] = RND(
        ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], 17, 0xEFBE4786384F25E3
    )
    ss[1], ss[5] = RND(
        ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], 18, 0x0FC19DC68B8CD5B5
    )
    ss[0], ss[4] = RND(
        ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], 19, 0x240CA1CC77AC9C65
    )
    ss[7], ss[3] = RND(
        ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], 20, 0x2DE92C6F592B0275
    )
    ss[6], ss[2] = RND(
        ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], 21, 0x4A7484AA6EA6E483
    )
    ss[5], ss[1] = RND(
        ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], 22, 0x5CB0A9DCBD41FBD4
    )
    ss[4], ss[0] = RND(
        ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], 23, 0x76F988DA831153B5
    )
    ss[3], ss[7] = RND(
        ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], 24, 0x983E5152EE66DFAB
    )
    ss[2], ss[6] = RND(
        ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], 25, 0xA831C66D2DB43210
    )
    ss[1], ss[5] = RND(
        ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], 26, 0xB00327C898FB213F
    )
    ss[0], ss[4] = RND(
        ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], 27, 0xBF597FC7BEEF0EE4
    )
    ss[7], ss[3] = RND(
        ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], 28, 0xC6E00BF33DA88FC2
    )
    ss[6], ss[2] = RND(
        ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], 29, 0xD5A79147930AA725
    )
    ss[5], ss[1] = RND(
        ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], 30, 0x06CA6351E003826F
    )
    ss[4], ss[0] = RND(
        ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], 31, 0x142929670A0E6E70
    )
    ss[3], ss[7] = RND(
        ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], 32, 0x27B70A8546D22FFC
    )
    ss[2], ss[6] = RND(
        ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], 33, 0x2E1B21385C26C926
    )
    ss[1], ss[5] = RND(
        ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], 34, 0x4D2C6DFC5AC42AED
    )
    ss[0], ss[4] = RND(
        ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], 35, 0x53380D139D95B3DF
    )
    ss[7], ss[3] = RND(
        ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], 36, 0x650A73548BAF63DE
    )
    ss[6], ss[2] = RND(
        ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], 37, 0x766A0ABB3C77B2A8
    )
    ss[5], ss[1] = RND(
        ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], 38, 0x81C2C92E47EDAEE6
    )
    ss[4], ss[0] = RND(
        ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], 39, 0x92722C851482353B
    )
    ss[3], ss[7] = RND(
        ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], 40, 0xA2BFE8A14CF10364
    )
    ss[2], ss[6] = RND(
        ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], 41, 0xA81A664BBC423001
    )
    ss[1], ss[5] = RND(
        ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], 42, 0xC24B8B70D0F89791
    )
    ss[0], ss[4] = RND(
        ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], 43, 0xC76C51A30654BE30
    )
    ss[7], ss[3] = RND(
        ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], 44, 0xD192E819D6EF5218
    )
    ss[6], ss[2] = RND(
        ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], 45, 0xD69906245565A910
    )
    ss[5], ss[1] = RND(
        ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], 46, 0xF40E35855771202A
    )
    ss[4], ss[0] = RND(
        ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], 47, 0x106AA07032BBD1B8
    )
    ss[3], ss[7] = RND(
        ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], 48, 0x19A4C116B8D2D0C8
    )
    ss[2], ss[6] = RND(
        ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], 49, 0x1E376C085141AB53
    )
    ss[1], ss[5] = RND(
        ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], 50, 0x2748774CDF8EEB99
    )
    ss[0], ss[4] = RND(
        ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], 51, 0x34B0BCB5E19B48A8
    )
    ss[7], ss[3] = RND(
        ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], 52, 0x391C0CB3C5C95A63
    )
    ss[6], ss[2] = RND(
        ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], 53, 0x4ED8AA4AE3418ACB
    )
    ss[5], ss[1] = RND(
        ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], 54, 0x5B9CCA4F7763E373
    )
    ss[4], ss[0] = RND(
        ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], 55, 0x682E6FF3D6B2B8A3
    )
    ss[3], ss[7] = RND(
        ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], 56, 0x748F82EE5DEFB2FC
    )
    ss[2], ss[6] = RND(
        ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], 57, 0x78A5636F43172F60
    )
    ss[1], ss[5] = RND(
        ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], 58, 0x84C87814A1F0AB72
    )
    ss[0], ss[4] = RND(
        ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], 59, 0x8CC702081A6439EC
    )
    ss[7], ss[3] = RND(
        ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], 60, 0x90BEFFFA23631E28
    )
    ss[6], ss[2] = RND(
        ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], 61, 0xA4506CEBDE82BDE9
    )
    ss[5], ss[1] = RND(
        ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], 62, 0xBEF9A3F7B2C67915
    )
    ss[4], ss[0] = RND(
        ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], 63, 0xC67178F2E372532B
    )
    ss[3], ss[7] = RND(
        ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], 64, 0xCA273ECEEA26619C
    )
    ss[2], ss[6] = RND(
        ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], 65, 0xD186B8C721C0C207
    )
    ss[1], ss[5] = RND(
        ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], 66, 0xEADA7DD6CDE0EB1E
    )
    ss[0], ss[4] = RND(
        ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], 67, 0xF57D4F7FEE6ED178
    )
    ss[7], ss[3] = RND(
        ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], 68, 0x06F067AA72176FBA
    )
    ss[6], ss[2] = RND(
        ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], 69, 0x0A637DC5A2C898A6
    )
    ss[5], ss[1] = RND(
        ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], 70, 0x113F9804BEF90DAE
    )
    ss[4], ss[0] = RND(
        ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], 71, 0x1B710B35131C471B
    )
    ss[3], ss[7] = RND(
        ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], 72, 0x28DB77F523047D84
    )
    ss[2], ss[6] = RND(
        ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], 73, 0x32CAAB7B40C72493
    )
    ss[1], ss[5] = RND(
        ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], 74, 0x3C9EBE0A15C9BEBC
    )
    ss[0], ss[4] = RND(
        ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], ss[4], 75, 0x431D67C49C100D4C
    )
    ss[7], ss[3] = RND(
        ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], ss[3], 76, 0x4CC5D4BECB3E42B6
    )
    ss[6], ss[2] = RND(
        ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], ss[2], 77, 0x597F299CFC657E2A
    )
    ss[5], ss[1] = RND(
        ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], ss[1], 78, 0x5FCB6FAB3AD6FAEC
    )
    ss[4], ss[0] = RND(
        ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], ss[0], 79, 0x6C44198C4A475817
    )

    dig = []
    for i, x in enumerate(sha_info["digest"]):
        dig.append((x + ss[i]) & 0xFFFFFFFFFFFFFFFF)
    sha_info["digest"] = dig


def sha_init():
    sha_info = new_shaobject()
    sha_info["digest"] = [
        0x6A09E667F3BCC908,
        0xBB67AE8584CAA73B,
        0x3C6EF372FE94F82B,
        0xA54FF53A5F1D36F1,
        0x510E527FADE682D1,
        0x9B05688C2B3E6C1F,
        0x1F83D9ABFB41BD6B,
        0x5BE0CD19137E2179,
    ]
    sha_info["count_lo"] = 0
    sha_info["count_hi"] = 0
    sha_info["local"] = 0
    sha_info["digestsize"] = 64
    return sha_info


def sha384_init():
    sha_info = new_shaobject()
    sha_info["digest"] = [
        0xCBBB9D5DC1059ED8,
        0x629A292A367CD507,
        0x9159015A3070DD17,
        0x152FECD8F70E5939,
        0x67332667FFC00B31,
        0x8EB44A8768581511,
        0xDB0C2E0D64F98FA7,
        0x47B5481DBEFA4FA4,
    ]
    sha_info["count_lo"] = 0
    sha_info["count_hi"] = 0
    sha_info["local"] = 0
    sha_info["digestsize"] = 48
    return sha_info


def getbuf(s):
    if isinstance(s, str):
        return s.encode("ascii")
    else:
        return bytes(s)


def sha_update(sha_info, buffer):
    if isinstance(buffer, str):
        raise TypeError("Unicode strings must be encoded before hashing")
    count = len(buffer)
    buffer_idx = 0
    clo = (sha_info["count_lo"] + (count << 3)) & 0xFFFFFFFF
    if clo < sha_info["count_lo"]:
        sha_info["count_hi"] += 1
    sha_info["count_lo"] = clo

    sha_info["count_hi"] += count >> 29

    if sha_info["local"]:
        i = SHA_BLOCKSIZE - sha_info["local"]
        if i > count:
            i = count

        # copy buffer
        for x in enumerate(buffer[buffer_idx : buffer_idx + i]):
            sha_info["data"][sha_info["local"] + x[0]] = x[1]

        count -= i
        buffer_idx += i

        sha_info["local"] += i
        if sha_info["local"] == SHA_BLOCKSIZE:
            sha_transform(sha_info)
            sha_info["local"] = 0
        else:
            return

    while count >= SHA_BLOCKSIZE:
        # copy buffer
        sha_info["data"] = list(buffer[buffer_idx : buffer_idx + SHA_BLOCKSIZE])
        count -= SHA_BLOCKSIZE
        buffer_idx += SHA_BLOCKSIZE
        sha_transform(sha_info)

    # copy buffer
    pos = sha_info["local"]
    sha_info["data"][pos : pos + count] = list(buffer[buffer_idx : buffer_idx + count])
    sha_info["local"] = count


def sha_final(sha_info):
    lo_bit_count = sha_info["count_lo"]
    hi_bit_count = sha_info["count_hi"]
    count = (lo_bit_count >> 3) & 0x7F
    sha_info["data"][count] = 0x80
    count += 1
    if count > SHA_BLOCKSIZE - 16:
        # zero the bytes in data after the count
        sha_info["data"] = sha_info["data"][:count] + ([0] * (SHA_BLOCKSIZE - count))
        sha_transform(sha_info)
        # zero bytes in data
        sha_info["data"] = [0] * SHA_BLOCKSIZE
    else:
        sha_info["data"] = sha_info["data"][:count] + ([0] * (SHA_BLOCKSIZE - count))

    sha_info["data"][112] = 0
    sha_info["data"][113] = 0
    sha_info["data"][114] = 0
    sha_info["data"][115] = 0
    sha_info["data"][116] = 0
    sha_info["data"][117] = 0
    sha_info["data"][118] = 0
    sha_info["data"][119] = 0

    sha_info["data"][120] = (hi_bit_count >> 24) & 0xFF
    sha_info["data"][121] = (hi_bit_count >> 16) & 0xFF
    sha_info["data"][122] = (hi_bit_count >> 8) & 0xFF
    sha_info["data"][123] = (hi_bit_count >> 0) & 0xFF
    sha_info["data"][124] = (lo_bit_count >> 24) & 0xFF
    sha_info["data"][125] = (lo_bit_count >> 16) & 0xFF
    sha_info["data"][126] = (lo_bit_count >> 8) & 0xFF
    sha_info["data"][127] = (lo_bit_count >> 0) & 0xFF

    sha_transform(sha_info)

    dig = []
    for i in sha_info["digest"]:
        dig.extend(
            [
                ((i >> 56) & 0xFF),
                ((i >> 48) & 0xFF),
                ((i >> 40) & 0xFF),
                ((i >> 32) & 0xFF),
                ((i >> 24) & 0xFF),
                ((i >> 16) & 0xFF),
                ((i >> 8) & 0xFF),
                (i & 0xFF),
            ]
        )
    return bytes(dig)


class sha512(object):
    digest_size = digestsize = SHA_DIGESTSIZE
    block_size = SHA_BLOCKSIZE

    def __init__(self, s=None):
        self._sha = sha_init()
        if s:
            sha_update(self._sha, getbuf(s))

    def update(self, s):
        sha_update(self._sha, getbuf(s))

    def digest(self):
        return sha_final(self._sha.copy())[: self._sha["digestsize"]]

    def hexdigest(self):
        return "".join(["%.2x" % i for i in self.digest()])

    def copy(self):
        new = sha512()
        new._sha = self._sha.copy()
        return new


class sha384(sha512):
    digest_size = digestsize = 48

    def __init__(self, s=None):
        self._sha = sha384_init()
        if s:
            sha_update(self._sha, getbuf(s))

    def copy(self):
        new = sha384()
        new._sha = self._sha.copy()
        return new


def test():
    a_str = "just a test string"

    assert (
        sha512().digest()
        == b"\xcf\x83\xe15~\xef\xb8\xbd\xf1T(P\xd6m\x80\x07\xd6 \xe4\x05\x0bW\x15\xdc\x83\xf4\xa9!\xd3l\xe9\xceG\xd0\xd1<]\x85\xf2\xb0\xff\x83\x18\xd2\x87~\xec/c\xb91\xbdGAz\x81\xa582z\xf9'\xda>"
    )
    assert (
        sha512().hexdigest()
        == "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e"
    )
    assert (
        sha512(a_str).hexdigest()
        == "68be4c6664af867dd1d01c8d77e963d87d77b702400c8fabae355a41b8927a5a5533a7f1c28509bbd65c5f3ac716f33be271fbda0ca018b71a84708c9fae8a53"
    )
    assert (
        sha512(a_str * 7).hexdigest()
        == "3233acdbfcfff9bff9fc72401d31dbffa62bd24e9ec846f0578d647da73258d9f0879f7fde01fe2cc6516af3f343807fdef79e23d696c923d79931db46bf1819"
    )

    s = sha512(a_str)
    s.update(a_str)
    assert (
        s.hexdigest()
        == "341aeb668730bbb48127d5531115f3c39d12cb9586a6ca770898398aff2411087cfe0b570689adf328cddeb1f00803acce6737a19f310b53bbdb0320828f75bb"
    )


if __name__ == "__main__":
    test()
