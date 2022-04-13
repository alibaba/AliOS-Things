/*
 * Compilation Instruction:
 * javac -cp bcprov-jdk15on-152.jar DumpPublicKey.java
 *
 * Run Instruction:
 * java -cp bcprov-jdk15on-152.jar:. DumpPublicKey PubKey.pem > source.c
 * java -cp bcprov-jdk15on-152.jar:. DumpPublicKey PubKey.pem OutKey.bin
 *
 */

/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//package com.android.dumpkey;

import org.bouncycastle.jce.provider.BouncyCastleProvider;

import java.io.FileInputStream;
import java.math.BigInteger;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;
import java.security.KeyStore;
import java.security.Key;
import java.security.PublicKey;
import java.security.Security;
import java.security.interfaces.ECPublicKey;
import java.security.interfaces.RSAPublicKey;
import java.security.spec.ECPoint;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.security.KeyFactory;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.PrivateKey;
import java.security.spec.InvalidKeySpecException;
import java.security.spec.PKCS8EncodedKeySpec;
import java.security.spec.X509EncodedKeySpec;

import java.io.InputStreamReader;
import org.bouncycastle.util.io.pem.PemObject;
import org.bouncycastle.util.io.pem.PemReader;

import java.io.FileOutputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.FileChannel;

class PemFile {

    private PemObject pemObject;

    public PemFile(String filename) throws FileNotFoundException, IOException {
        PemReader pemReader = new PemReader(new InputStreamReader(new FileInputStream(filename)));
        try {
            this.pemObject = pemReader.readPemObject();
        } finally {
            pemReader.close();
        }
    }

    public PemObject getPemObject() {
        return pemObject;
    }
}

class PemKey {
    protected KeyFactory factory;

    public PemKey() throws NoSuchAlgorithmException {
        factory = KeyFactory.getInstance("RSA");
    }

    public PrivateKey generatePrivateKey(String filename) throws InvalidKeySpecException, FileNotFoundException, IOException{
        PemFile pemFile = new PemFile(filename);
        byte[] content = pemFile.getPemObject().getContent();
        PKCS8EncodedKeySpec privKeySpec = new PKCS8EncodedKeySpec(content);
        return factory.generatePrivate(privKeySpec);
    }

    public PublicKey generatePublicKey(String filename) throws InvalidKeySpecException, FileNotFoundException, IOException {
        PemFile pemFile = new PemFile(filename);
        byte[] content = pemFile.getPemObject().getContent();
        X509EncodedKeySpec pubKeySpec = new X509EncodedKeySpec(content);
        return factory.generatePublic(pubKeySpec);
    }
}


/**
 * Command line tool to extract RSA public keys from X.509 certificates
 * and output source code with data initializers for the keys.
 * @hide
 */
class DumpPublicKey {
    /**
     * @param key to perform sanity checks on
     * @return version number of key.  Supported versions are:
     *     1: 2048-bit RSA key with e=3 and SHA-1 hash
     *     2: 2048-bit RSA key with e=65537 and SHA-1 hash
     *     3: 2048-bit RSA key with e=3 and SHA-256 hash
     *     4: 2048-bit RSA key with e=65537 and SHA-256 hash
     * @throws Exception if the key has the wrong size or public exponent
     */
    static int checkRSA(RSAPublicKey key, boolean useSHA256) throws Exception {
        BigInteger pubexp = key.getPublicExponent();
        BigInteger modulus = key.getModulus();
        int version;

        if (pubexp.equals(BigInteger.valueOf(65537))) {
            version = useSHA256 ? 4 : 2;
        } else {
            throw new Exception("Public exponent should be 65537 but is " +
                                pubexp.toString(10) + ".");
        }

        if (modulus.bitLength() != 2048) {
             throw new Exception("Modulus should be 2048 bits long but is " +
                        modulus.bitLength() + " bits.");
        }

        return version;
    }

    /**
     * @param key to perform sanity checks on
     * @return version number of key.  Supported versions are:
     *     5: 256-bit EC key with curve NIST P-256
     * @throws Exception if the key has the wrong size or public exponent
     */
    static int checkEC(ECPublicKey key) throws Exception {
        if (key.getParams().getCurve().getField().getFieldSize() != 256) {
            throw new Exception("Curve must be NIST P-256");
        }

        return 5;
    }

    /**
     * Perform sanity check on public key.
     */
    static int check(PublicKey key, boolean useSHA256) throws Exception {
        if (key instanceof RSAPublicKey) {
            return checkRSA((RSAPublicKey) key, useSHA256);
        } else if (key instanceof ECPublicKey) {
            if (!useSHA256) {
                throw new Exception("Must use SHA-256 with EC keys!");
            }
            return checkEC((ECPublicKey) key);
        } else {
            throw new Exception("Unsupported key class: " + key.getClass().getName());
        }
    }

    /**
     * @param key to output
     * @return a String representing this public key.  If the key is a
     *    version 1 key, the string will be a C initializer; this is
     *    not true for newer key versions.
     */
    static String printRSA(RSAPublicKey key, boolean useSHA256, String binfile) throws Exception {
        int version = check(key, useSHA256);
        final String tabpos = "    ";
        FileOutputStream os = null;
        ByteBuffer bbuf = null;

        if (binfile != null) {
            try {
                os = new FileOutputStream(binfile);
                bbuf = ByteBuffer.allocate(1024);
                bbuf.order(ByteOrder.LITTLE_ENDIAN);
            } catch (Exception e) {
                e.printStackTrace();
                System.exit(1);
            }
        }

        BigInteger N = key.getModulus();

        StringBuilder result = new StringBuilder();

        int nwords = N.bitLength() / 32;    // # of 32 bit integers in modulus

        BigInteger B = BigInteger.valueOf(0x100000000L);  // 2^32
        BigInteger N0inv = B.subtract(N.modInverse(B));   // -1 / N[0] mod 2^32

        result.append("{\n");
        result.append(tabpos);
        result.append("0x");
        result.append(N0inv.toString(16));
        if (bbuf != null) {
            bbuf.putInt(N0inv.intValue());
        }
        result.append(",\n");

        BigInteger R = BigInteger.valueOf(2).pow(N.bitLength());
        BigInteger RR = R.multiply(R).mod(N);    // 2^4096 mod N

        // Write out modulus as little endian array of integers.
        result.append(tabpos);
        result.append("{\n");
        result.append(tabpos);
        result.append(tabpos);
        for (int i = 0; i < nwords; ++i) {
            long n = N.mod(B).longValue();
            result.append(String.format("0x%08x, ", n));
            if (bbuf != null) {
                bbuf.putInt((int)n);
            }
            if ((i + 1) % 4 == 0) {
                result.append("\n");
                if (i + 1 != nwords) {
                    result.append(tabpos);
                    result.append(tabpos);
                }
            }
            N = N.divide(B);
        }
        result.append(tabpos);
        result.append("},\n");

        // Write R^2 as little endian array of integers.
        result.append(tabpos);
        result.append("{\n");
        result.append(tabpos);
        result.append(tabpos);
        for (int i = 0; i < nwords; ++i) {
            long rr = RR.mod(B).longValue();
            result.append(String.format("0x%08x, ", rr));
            if (bbuf != null) {
                bbuf.putInt((int)rr);
            }
            if ((i + 1) % 4 == 0) {
                result.append("\n");
                if (i + 1 != nwords) {
                    result.append(tabpos);
                    result.append(tabpos);
                }
            }
            RR = RR.divide(B);
        }

        if (os != null) {
            bbuf.flip();
            FileChannel out = os.getChannel();
            out.write(bbuf);
            out.close();
        }

        result.append(tabpos);
        result.append("},\n");

        result.append("},\n");
        return result.toString();
    }

    /**
     * @param key to output
     * @return a String representing this public key.  If the key is a
     *    version 1 key, the string will be a C initializer; this is
     *    not true for newer key versions.
     */
    static String printEC(ECPublicKey key) throws Exception {
        int version = checkEC(key);

        StringBuilder result = new StringBuilder();

        result.append("v");
        result.append(Integer.toString(version));
        result.append(" ");

        BigInteger X = key.getW().getAffineX();
        BigInteger Y = key.getW().getAffineY();
        int nbytes = key.getParams().getCurve().getField().getFieldSize() / 8;    // # of 32 bit integers in X coordinate

        result.append("{");
        result.append(nbytes);

        BigInteger B = BigInteger.valueOf(0x100L);  // 2^8

        // Write out Y coordinate as array of characters.
        result.append(",{");
        for (int i = 0; i < nbytes; ++i) {
            long n = X.mod(B).longValue();
            result.append(n);

            if (i != nbytes - 1) {
                result.append(",");
            }

            X = X.divide(B);
        }
        result.append("}");

        // Write out Y coordinate as array of characters.
        result.append(",{");
        for (int i = 0; i < nbytes; ++i) {
            long n = Y.mod(B).longValue();
            result.append(n);

            if (i != nbytes - 1) {
                result.append(",");
            }

            Y = Y.divide(B);
        }
        result.append("}");

        result.append("}");
        return result.toString();
    }

    static String print(PublicKey key, boolean useSHA256, String binfile) throws Exception {
        if (key instanceof RSAPublicKey) {
            return printRSA((RSAPublicKey) key, useSHA256, binfile);
        } else if (key instanceof ECPublicKey) {
            return printEC((ECPublicKey) key);
        } else {
            throw new Exception("Unsupported key class: " + key.getClass().getName());
        }
    }

    public static void main(String[] args) {
        if (args.length != 1 && args.length != 2) {
            System.err.println("Usage:");
            System.err.println("\tDumpPublicKey PubKey.pem [OutKey.bin] > source.c");
            System.exit(1);
        }
        Security.addProvider(new BouncyCastleProvider());
        try {
            PemKey pem = new PemKey();
            PublicKey key = pem.generatePublicKey(args[0]);
            check(key, true);
            if (args.length == 1) {
                System.out.print(print(key, true, null));
            } else {
                System.out.print(print(key, true, args[1]));
            }
        } catch (Exception e) {
            e.printStackTrace();
            System.exit(1);
        }
        System.exit(0);
    }
}

