package com.hivemq.extensions.helloworld;

import com.hivemq.extension.sdk.api.annotations.NotNull;
import com.hivemq.extension.sdk.api.auth.SimpleAuthenticator;
import com.hivemq.extension.sdk.api.auth.parameter.SimpleAuthInput;
import com.hivemq.extension.sdk.api.auth.parameter.SimpleAuthOutput;
import com.hivemq.extension.sdk.api.auth.parameter.TopicPermission;
import com.hivemq.extension.sdk.api.packets.auth.DefaultAuthorizationBehaviour;
import com.hivemq.extension.sdk.api.packets.connect.ConnectPacket;
import com.hivemq.extension.sdk.api.services.builder.Builders;

import java.nio.charset.Charset;

public class MyAuthenticator implements SimpleAuthenticator {
    @Override
    public void onConnect(@NotNull SimpleAuthInput simpleAuthInput, @NotNull SimpleAuthOutput simpleAuthOutput) {
        ConnectPacket connect = simpleAuthInput.getConnectPacket();

        //check if the client set username and password
        if (!connect.getUserName().isPresent() || !connect.getPassword().isPresent()) {
            simpleAuthOutput.failAuthentication();
        }

        String username = connect.getUserName().get();
        String password = Charset.forName("UTF-8").decode(connect.getPassword().get()).toString();

        if (username.equals("user1") && password.equals("11111")) {
            //create a topic permission for all topics starting with the client identifier
            final TopicPermission permission = Builders.topicPermission()
                    .topicFilter("/TopicA")
                    .activity(TopicPermission.MqttActivity.PUBLISH)
                    .type(TopicPermission.PermissionType.ALLOW)
                    .build();

            simpleAuthOutput.getDefaultPermissions().add(permission);
            simpleAuthOutput.authenticateSuccessfully();
        }
        else if (username.equals("user2") && password.equals("22222"))
        {
            final TopicPermission permission = Builders.topicPermission()
                    .topicFilter("/TopicA")
                    .activity(TopicPermission.MqttActivity.SUBSCRIBE)
                    .type(TopicPermission.PermissionType.ALLOW)
                    .build();

            simpleAuthOutput.getDefaultPermissions().add(permission);
            simpleAuthOutput.authenticateSuccessfully();
        }
        else if (username.equals("user3") && password.equals("33333"))
        {
            final TopicPermission permission = Builders.topicPermission()
                    .topicFilter("/TopicA")
                    .type(TopicPermission.PermissionType.ALLOW)
                    .build();

            simpleAuthOutput.getDefaultPermissions().add(permission);
            simpleAuthOutput.authenticateSuccessfully();
        }
        else
        {
            simpleAuthOutput.failAuthentication();
        }

//        //get username and password from the connect packet
//        String encr_username = connect.getUserName().get();
//        String encr_password = Charset.forName("UTF-8").decode(connect.getPassword().get()).toString();
//
//        byte[] aes_key = {(byte) 0x2B, (byte) 0x7E, (byte) 0x15, (byte) 0x16, (byte) 0x28, (byte) 0xAE, (byte) 0xD2, (byte) 0xA6, (byte) 0xAB, (byte) 0xF7, (byte) 0x15, (byte) 0x88, (byte) 0x09, (byte) 0xCF, (byte) 0x4F, (byte) 0x3C};
//        byte[] aes_iv = {(byte) 0, (byte) 0, (byte) 0, (byte) 0, (byte) 0, (byte) 0, (byte) 0, (byte) 0, (byte) 0, (byte) 0, (byte) 0, (byte) 0, (byte) 0, (byte) 0, (byte) 0, (byte) 0};
//
//        AES aes = new AES(aes_key, aes_iv);
//
//        try {
//            String decr_username = aes.AES_CBC_Decryption(encr_username);
//            String decr_password = aes.AES_CBC_Decryption(encr_password);
//
//            if (decr_username.equals("user1") && decr_password.equals("11111")) {
//                //create a topic permission for all topics starting with the client identifier
//                final TopicPermission permission = Builders.topicPermission()
//                        .topicFilter("/TopicA")
//                        .activity(TopicPermission.MqttActivity.PUBLISH)
//                        .type(TopicPermission.PermissionType.ALLOW)
//                        .build();
//
//                simpleAuthOutput.getDefaultPermissions().add(permission);
//                simpleAuthOutput.authenticateSuccessfully();
//            }
//            else if (decr_username.equals("user2") && decr_password.equals("22222"))
//            {
//                final TopicPermission permission = Builders.topicPermission()
//                        .topicFilter("/TopicA")
//                        .activity(TopicPermission.MqttActivity.SUBSCRIBE)
//                        .type(TopicPermission.PermissionType.ALLOW)
//                        .build();
//
//                simpleAuthOutput.getDefaultPermissions().add(permission);
//                simpleAuthOutput.authenticateSuccessfully();
//            }
//            else if (decr_username.equals("user3") && decr_password.equals("33333"))
//            {
//                final TopicPermission permission = Builders.topicPermission()
//                        .topicFilter("/TopicA")
//                        .type(TopicPermission.PermissionType.ALLOW)
//                        .build();
//
//                simpleAuthOutput.getDefaultPermissions().add(permission);
//                simpleAuthOutput.authenticateSuccessfully();
//            }
//            else
//            {
//                simpleAuthOutput.failAuthentication();
//            }
//        } catch (Exception e) {
//            throw new RuntimeException(e);
//        }
    }
}
