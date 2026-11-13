#ifndef AES_H
#define AES_H

#include "mbedtls/aes.h"

class aes {
  private:
    const char* key = "12345678901234561234567890123456";
    const char* iv_static = "1234567890123456";

  public:
    String encrypt(String plaintext) {
      int plainLen = plaintext.length();
      int paddedLen = ((plainLen / 16) + 1) * 16;
      unsigned char input[paddedLen];
      unsigned char output[paddedLen];
      unsigned char iv[16];

      memcpy(iv, iv_static, 16);
      memset(input, 0, paddedLen);
      memcpy(input, plaintext.c_str(), plainLen);
      
      uint8_t paddingVal = paddedLen - plainLen;
      for(int i = plainLen; i < paddedLen; i++) input[i] = paddingVal;

      mbedtls_aes_context aes;
      mbedtls_aes_init(&aes);
      mbedtls_aes_setkey_enc(&aes, (const unsigned char*)key, 256);
      mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, paddedLen, iv, input, output);
      mbedtls_aes_free(&aes);

      String hexResult = "";
      for (int i = 0; i < paddedLen; i++) {
        if (output[i] < 0x10) hexResult += "0";
        hexResult += String(output[i], HEX);
      }
      return hexResult;
    }

    String decrypt(String hexCiphertext) {
      int len = hexCiphertext.length() / 2;
      if (len % 16 != 0) return "ERROR: Panjang Data Salah";

      unsigned char input[len];
      unsigned char output[len];
      unsigned char iv[16];

      for (int i = 0; i < len; i++) {
        input[i] = (uint8_t) strtol(hexCiphertext.substring(i*2, i*2+2).c_str(), NULL, 16);
      }

      memcpy(iv, iv_static, 16);
      mbedtls_aes_context aes;
      mbedtls_aes_init(&aes);
      mbedtls_aes_setkey_dec(&aes, (const unsigned char*)key, 256);
      mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, len, iv, input, output);
      mbedtls_aes_free(&aes);

      int paddingVal = output[len - 1];
      if (paddingVal <= 0 || paddingVal > 16) paddingVal = 0;
      
      String result = "";
      for (int i = 0; i < (len - paddingVal); i++) result += (char)output[i];
      return result;
    }
};

#endif AES_H