#pragma once

long httpGetRequest(char* hostname, char* path, char* response);

long httpsGetRequest(char* hostname, char* path, char* certificate, char* response);