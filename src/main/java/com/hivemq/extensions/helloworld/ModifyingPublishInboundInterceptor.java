package com.hivemq.extensions.helloworld;

import com.hivemq.extension.sdk.api.annotations.NotNull;
import com.hivemq.extension.sdk.api.interceptor.publish.PublishInboundInterceptor;
import com.hivemq.extension.sdk.api.interceptor.publish.parameter.PublishInboundInput;
import com.hivemq.extension.sdk.api.interceptor.publish.parameter.PublishInboundOutput;
import com.hivemq.extension.sdk.api.packets.publish.ModifiablePublishPacket;

import java.nio.ByteBuffer;
import java.util.Optional;

public class ModifyingPublishInboundInterceptor implements PublishInboundInterceptor {

    @Override
    public void onInboundPublish(@NotNull PublishInboundInput publishInboundInput, @NotNull PublishInboundOutput publishInboundOutput) {
        final ModifiablePublishPacket publish = publishInboundOutput.getPublishPacket();

        final Optional<ByteBuffer> payload = publishInboundInput.getPublishPacket().getPayload();
        if(payload.isPresent()) {
            String encr_payload = new String(getBytesFromBuffer(payload.get()));

            byte[] aes_key = {(byte) 0x2B, (byte) 0x7E, (byte) 0x15, (byte) 0x16, (byte) 0x28, (byte) 0xAE, (byte) 0xD2, (byte) 0xA6, (byte) 0xAB, (byte) 0xF7, (byte) 0x15, (byte) 0x88, (byte) 0x09, (byte) 0xCF, (byte) 0x4F, (byte) 0x3C};
            byte[] aes_iv = {(byte) 0, (byte) 0, (byte) 0, (byte) 0, (byte) 0, (byte) 0, (byte) 0, (byte) 0, (byte) 0, (byte) 0, (byte) 0, (byte) 0, (byte) 0, (byte) 0, (byte) 0, (byte) 0};

            AES aes = new AES(aes_key, aes_iv);
            try {
                String decr_payload = aes.AES_CBC_Decryption(encr_payload);
                publish.setPayload(ByteBuffer.wrap(decr_payload.getBytes()));
            } catch (Exception e) {
                throw new RuntimeException(e);
            }
        }
    }

    @NotNull
    private byte[] getBytesFromBuffer(final @NotNull ByteBuffer byteBuffer){
        final ByteBuffer rewind = byteBuffer.asReadOnlyBuffer().rewind();
        final byte[] array = new byte[rewind.remaining()];
        rewind.get(array);
        return array;
    }
}
