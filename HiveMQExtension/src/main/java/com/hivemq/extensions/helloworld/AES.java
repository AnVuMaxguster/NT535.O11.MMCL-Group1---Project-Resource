package com.hivemq.extensions.helloworld;

import javax.crypto.Cipher;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;
import java.nio.charset.StandardCharsets;
import java.util.Base64;

public class AES {
//        byte[] aes_key = { (byte)0x2B, (byte)0x7E, (byte)0x15, (byte)0x16, (byte)0x28, (byte)0xAE, (byte)0xD2, (byte)0xA6, (byte)0xAB, (byte)0xF7, (byte)0x15, (byte)0x88, (byte)0x09, (byte)0xCF, (byte)0x4F, (byte)0x3C };
//
//        byte[] aes_iv = { (byte)0, (byte)0, (byte)0, (byte)0, (byte)0, (byte)0, (byte)0, (byte)0, (byte)0, (byte)0, (byte)0, (byte)0, (byte)0, (byte)0, (byte)0, (byte)0 };

    private IvParameterSpec iv;
    private SecretKeySpec secretKey;
    public AES (byte[] aes_key,byte[] aes_iv)
    {
        this.iv=new IvParameterSpec(aes_iv);
        this.secretKey= new SecretKeySpec(aes_key, "AES");
    }


    public String AES_CBC_Encryption (String message) throws Exception
    {

        Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5PADDING");
        cipher.init(Cipher.ENCRYPT_MODE,this.secretKey,this.iv);

        byte[] encryptedMessage = cipher.doFinal(message.getBytes());

        String encodedMessage = Base64.getEncoder().encodeToString(encryptedMessage);
        return encodedMessage;
    }
    public String AES_CBC_Decryption (String ciphermsg) throws Exception
    {
        Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5PADDING");
        cipher.init(Cipher.DECRYPT_MODE, secretKey, iv);
        byte[] Decodedmsg=Base64.getDecoder().decode(ciphermsg);
        byte[] DecryptedMessage = cipher.doFinal(Decodedmsg);

        String originalMessage = new String(DecryptedMessage, StandardCharsets.UTF_8);
        return originalMessage;
    }
}