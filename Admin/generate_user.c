#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <winhttp.h>
#include "../network.h"
#include "../crypto_obfuscate.h"
#pragma comment(lib, "winhttp.lib")

#define CREDS_FILE "users.dat"
#define WEBHOOK_USERS "https://discord.com/api/webhooks/1534587699464503459/b-9PzGBqdH5kBu8f-P78je3UNVnqC-IkcdsjjGe5wxRNZmdgHwlwfnqjrRRVuyZ5ypSh"

// --- Send to Discord users channel ---
static void send_creds_to_discord(const char* user, const char* pass) {
    char msg[512];
    snprintf(msg, sizeof(msg),
        "🔑 **New user created by admin!**\n```\nUsername : %s\nPassword : %s\n```",
        user, pass);
    HttpResponse resp = webhook_post(WEBHOOK_USERS, msg);
    free_response(&resp);
}

// --- Save to local users.dat (password stored as SHA-256 hash) ---
static void save_credential(const char* user, const char* pass) {
    char hash[SHA256_HEX_SIZE];
    sha256_hash(pass, hash);

    FILE* f = fopen(CREDS_FILE, "a");
    if (f) {
        fprintf(f, "%s %s\n", user, hash);
        fclose(f);
    }
}

int main(int argc, char* argv[]) {
    // Seed with more entropy still needed for username generation fallback
    srand((unsigned int)time(NULL));

    char username[128];
    char password[64];

    if (argc == 3) {
        // Command-line mode: generate_user.exe <username> <password>
        strncpy(username, argv[1], sizeof(username) - 1);
        username[sizeof(username) - 1] = '\0';
        strncpy(password, argv[2], sizeof(password) - 1);
        password[sizeof(password) - 1] = '\0';
    } else if (argc == 2) {
        // generate_user.exe <username>  (auto-generate password)
        strncpy(username, argv[1], sizeof(username) - 1);
        username[sizeof(username) - 1] = '\0';
        generate_secure_password(password, sizeof(password));
    } else {
        // Interactive mode
        printf("=== 🔐 ShopLock Admin: Generate User ===\n\n");
        printf("Enter username: ");
        scanf("%127s", username);
        printf("Enter password (or press Enter to auto-generate): ");
        getchar(); // consume newline
        if (fgets(password, sizeof(password), stdin)) {
            // Strip newline
            password[strcspn(password, "\n")] = '\0';
        }
        if (password[0] == '\0') {
            generate_secure_password(password, sizeof(password));
        }
    }

    // Save locally (password will be SHA-256 hashed)
    save_credential(username, password);
    printf("[+] Saved to %s: %s (password hashed with SHA-256)\n", CREDS_FILE, username);
    printf("[+] Plaintext password: %s\n", password);

    // Send to Discord (plaintext for the admin)
    send_creds_to_discord(username, password);
    printf("[+] Sent to Discord users channel.\n");

    return 0;
}