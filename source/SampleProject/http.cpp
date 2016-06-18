#include "http.h"
#include <Energia.h>
#include <WiFi.h>

#define TIMEOUT (45)

long httpGetRequest(char* host, char* path, char* response) {
  String hostname = String(host);
  String head_post = "GET " + String(path) + " HTTP/1.1";
  String head_host = "Host: " + hostname;
  String request = head_post + "\n" + head_host + "\n\n";

  uint16_t socket_handle;
  uint32_t host_ip;
  long success;

  SlTimeval_t timeout { .tv_sec = TIMEOUT, .tv_usec = 0 };
  
  // retrieve IP from Hostname
  success = sl_NetAppDnsGetHostByName((signed char*)hostname.c_str(), hostname.length(), &host_ip, SL_AF_INET);

  if (success < 0) {
    return success;
  }

  // create socket
  SlSockAddrIn_t socket_address {
  	.sin_family = SL_AF_INET,
  	.sin_port = sl_Htons(80),
  	.sin_addr = {
  		.s_addr = sl_Htonl(host_ip)
  	}
  };

  // opens a secure socket
  socket_handle = sl_Socket(SL_AF_INET, SL_SOCK_STREAM, SL_IPPROTO_TCP);

  if (socket_handle < 0) {
    Serial.println("Failed to initialize handle");
    return socket_handle;
  }

  // configure the receive timeout
  success = sl_SetSockOpt(socket_handle, SL_SOL_SOCKET, SL_SO_RCVTIMEO, (const void*)&timeout, sizeof(timeout));

  if (success < 0) {
    Serial.println("Failed to set timeout");
    goto cleanup;
  }

  // connect to the server
  success = sl_Connect(socket_handle, (SlSockAddr_t*)&socket_address, sizeof(SlSockAddrIn_t));

  if (success < 0) {
    Serial.println("Failed to connect");
    goto cleanup;
  }

  // send the request to the server
  success = sl_Send(socket_handle, request.c_str(), request.length(), 0);

  if (success < 0) {
    Serial.println("Failed to send");
    goto cleanup;
  }

  // receive response packet from the server
  success = sl_Recv(socket_handle, response, 1024, 0);

  if (success < 0) {
    Serial.println("Failed to receive");
    goto cleanup;
  }

  cleanup:
  sl_Close(socket_handle);
  return success;
}

long httpsGetRequest(char* host, char* path, char* certpath, char* response) {
  String certificate = String(certpath);
  String hostname = String(host);
  String head_post = "GET " + String(path) + " HTTP/1.1";
  String head_host = "Host: " + hostname;
  String request = head_post + "\n" + head_host + "\n\n";

  uint16_t socket_handle;
  uint32_t host_ip;
  long success;

  SlSockSecureMask cipher { SL_SEC_MASK_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA };
  SlTimeval_t timeout { .tv_sec = TIMEOUT, .tv_usec = 0 };
  SlSockSecureMethod method { SL_SO_SEC_METHOD_SSLv3_TLSV1_2 };
  
  // retrieve IP from Hostname
  success = sl_NetAppDnsGetHostByName((signed char*)hostname.c_str(), hostname.length(), &host_ip, SL_AF_INET);

  if (success < 0) {
    return success;
  }

  // create socket
  SlSockAddrIn_t socket_address {
    .sin_family = SL_AF_INET,
    .sin_port = sl_Htons(443),
    .sin_addr = {
      .s_addr = sl_Htonl(host_ip)
    }
  };

  // opens a secure socket
  socket_handle = sl_Socket(SL_AF_INET, SL_SOCK_STREAM, SL_SEC_SOCKET);

  if (socket_handle < 0) {
    Serial.println("Failed to initialize handle");
    return socket_handle;
  }

  // configure the connection mode
  success = sl_SetSockOpt(socket_handle, SL_SOL_SOCKET, SL_SO_SECMETHOD, (const void*)&method, sizeof(method));

  if (success < 0) {
    Serial.println("Failed to setup secure mode");
    goto cleanup;
  }

  // configure the socket as secure
  success = sl_SetSockOpt(socket_handle, SL_SOL_SOCKET, SL_SO_SECURE_MASK, (const void*)&cipher, sizeof(cipher));

  if (success < 0) {
    Serial.println("Failed to setup encryption strength");
    goto cleanup;
  }

  // configure the socket with certificate - for server verification
  success = sl_SetSockOpt(socket_handle, SL_SOL_SOCKET, SL_SO_SECURE_FILES_CA_FILE_NAME, (const void*)certificate.c_str(), certificate.length());
  
  if (success < 0) {
    Serial.println("Failed to hand-over certificate");
    goto cleanup;
  }

  // configure the receive timeout
  success = sl_SetSockOpt(socket_handle, SL_SOL_SOCKET, SL_SO_RCVTIMEO, (const void*)&timeout, sizeof(timeout));

  if (success < 0) {
    Serial.println("Failed to set timeout");
    goto cleanup;
  }

  // connect to the server
  success = sl_Connect(socket_handle, (SlSockAddr_t*)&socket_address, sizeof(SlSockAddrIn_t));

  if (success < 0) {
    Serial.print("Failed to connect: ");
    Serial.println(success);
    goto cleanup;
  }

  // send the request to the server
  success = sl_Send(socket_handle, request.c_str(), request.length(), 0);

  if (success < 0) {
    Serial.println("Failed to send");
    goto cleanup;
  }

  // receive response packet from the server
  success = sl_Recv(socket_handle, response, 1024, 0);

  if (success < 0) {
    Serial.println("Failed to receive");
    goto cleanup;
  }

  cleanup:
  sl_Close(socket_handle);
  return success;
}
