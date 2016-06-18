#include "http.h"
#include <Energia.h>
#include <WiFi.h>

#define TIMEOUT (45)

long httpGetRequest(char* host, char* path) {
  String hostname = String(host);
  String head_post = "GET " + String(path) + " HTTP/1.1";
  String head_host = "Host: " + hostname;
  
  String request = head_post + "\n" + 
                   head_host + "\n\n";

  char receive_msg_buffer[1024];
  uint16_t socket_handle;
  uint32_t host_ip;
  long success;

  SlTimeval_t timeout { .tv_sec = TIMEOUT, .tv_usec = 0 };
  SlSockSecureMethod method { SL_SO_SEC_METHOD_TLSV1 };
  
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
  success = sl_Recv(socket_handle, receive_msg_buffer, sizeof(receive_msg_buffer), 0);

  if (success < 0) {
    Serial.println("Failed to receive");
    goto cleanup;
  }
  
  Serial.println(receive_msg_buffer);

  cleanup:
  sl_Close(socket_handle);
  return success;
}