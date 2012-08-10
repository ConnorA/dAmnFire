#include "auth.h"

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t write_callback( void *contents, size_t size, size_t nmemb, void *userp )
{
    size_t rsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    mem->memory = realloc (mem->memory, mem->size + rsize +1);

    memcpy(&(mem->memory[mem->size]), contents, rsize);
    mem->size += rsize;
    mem->memory[mem->size] = 0;

    return rsize;
}


char *get_authtoken (char *username, char *password)
{
    char *auth = NULL;

    struct MemoryStruct resp;

    resp.memory = malloc(1);
    resp.size   = 0;

    int content_len = strlen("&username=")+strlen(username)
                     +strlen("&password=")+strlen(password)
                     +strlen("&remember_me=1") + 1;

    char *post = malloc( content_len );
    sprintf( post, "&username=%s&password=%s&remember_me=1", username, password );

    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();
    if (curl) {
        struct curl_slist *chunk = NULL;

        chunk = curl_slist_append(chunk, "Accept: text/html");
        chunk = curl_slist_append(chunk, "Connection: close");

        curl_easy_setopt(curl, CURLOPT_URL, "https://www.deviantart.com/users/login");
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_REFERER, "https://www.deviantart.com/users/rockedout");
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "dAmnFire");
        curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL");
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(post));
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post);

        // Perform POST to deviantart.com/users/login
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "Curl perform failed: %s\n", curl_easy_strerror(res));
            return NULL;
        }

        struct curl_slist *cookies;
        res = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
        if (res != CURLE_OK) {
            fprintf(stderr, "Curl curl_easy_getinfo failed: %s\n", curl_easy_strerror(res));
            exit(1);
        }

        curl_easy_cleanup(curl);

        curl = curl_easy_init();
        if(curl){
            curl_easy_setopt(curl, CURLOPT_URL, "http://chat.deviantart.com/chat/devart");
            curl_easy_setopt(curl, CURLOPT_PORT, 80L);
            curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "dAmnFire");
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0L);

            curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL");
            while (cookies != NULL)
            {
                curl_easy_setopt(curl, CURLOPT_COOKIELIST, cookies->data);
                cookies = cookies->next;
            }

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&resp);

            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                fprintf(stderr, "Curl perform failed: %s\n", curl_easy_strerror(res));
                return NULL;
            }

            curl_slist_free_all(cookies);

            int offset = 17+strlen(username);

            char *ptr_auth = NULL;
            ptr_auth = strstr(resp.memory, "dAmn_Login");
            if(ptr_auth != NULL){
                auth = malloc(33); // The exact length of an authtoken is 32 + ending
                memcpy( auth, ptr_auth + offset, 32 );
                auth[32] = '\0';
                //debug("\n[authtoken]: %s\n", authtoken);
            }
        }
    }

    //debug("\n[page]: %s\n", resp.memory);
    if (resp.memory) free(resp.memory);

    curl_global_cleanup();


    return auth;
}
