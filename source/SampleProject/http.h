#pragma once

long httpGetRequest(char* hostname, char* path);

long httpsGetRequest(char* hostname, char* path, char* certificate);