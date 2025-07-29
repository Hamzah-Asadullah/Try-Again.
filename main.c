#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <stdbool.h>
#include <string.h>

//  Personal stuff
/*
    Code licensed MIT @ 2025 Hamzah Asadullah
    Inspired by / Conversation text from https://vm.tiktok.com/ZNdHMe5n6/

    :> signals notice / warning
    !> signals error
    >> signals normal output
    >_ signals input

    Long live the Islamic Republic of Pakistan / Pakistan Zindabad / پاکِستان زِندہ‌باد
    Long live the People's Republic of China / Zhōnghuá Rénmín Gònghéguó wànsuì / 中华人民共和国万岁
    Long live the Federal Republic of Germany / Lang lebe Deutschland
*/

//  Stuff related to the program
/*
    .cnv filetype description:
    1. uint8_t: The amount of notices for this conversation
    2. uint8_t containing the amount of characters needed for the notice, followed by the notice, repeat until all notices are read
    3. uint8_t describing the amount of characters needed for the name, followed by the name, repeat until two names are read
    4. uint16_t: the amount of messages in the entire conversation (by all characters)
    5. The same as 3. but for the actual messages, and after each message the uint16_t timing of the message is stored

    Notes for the filetype:
    - Max amount of messages in the entire conversation limited to 2^16 - 1 (65536)
    - Max amount of characters for each name is 2^8 - 1 (255)
    - There have to be exactly two names, and each second message applies to the second name
    - Max amount of characters pre message is 2^8 - 1 (255)
*/

typedef uint16_t timing;

#ifdef _WIN32
#include <Windows.h>
void mysleep(timing ms) { Sleep((DWORD)ms); }
#elif defined(__linux__)
#include <time.h>
void mysleep(timing ms)
{
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = (uint64_t)ms * 1000000UL;
    nanosleep(&ts, NULL);
}
#else
void mysleep(timing ms) { /* Feel free to push request */ }
#endif

struct data
{
    uint16_t n_msg;
    uint8_t n_notice;
    char** msg;
    char** notice;
    char* name[2];
    timing* msg_timing;
};
typedef struct data data;

void data_init(data* d)
{
    d->n_msg = 0;
    d->n_notice = 0;
    d->msg = NULL;
    d->notice = NULL;
    d->name[0] = NULL, d->name[1] = NULL;
    d->msg_timing = NULL;
}

void data_set_null(data* d)
{
    if (d->msg)
    {
        for (uint8_t i = 0; i < d->n_msg; ++i)
            free(d->msg[i]);
        free(d->msg);
    }
    if (d->notice)
    {
        for (uint8_t i = 0; i < d->n_notice; ++i)
            free(d->notice[i]);
        free(d->notice);
    }
    if (d->name[0]) free(d->name[0]); // d->name can never be NULL; it's set with [2]
    if (d->name[1]) free(d->name[1]);
    if (d->msg_timing) free(d->msg_timing);

    d->n_msg = 0;
    d->n_notice = 0;
    d->msg = NULL;
    d->notice = NULL;
    d->msg_timing = NULL;
}

bool read_conversation(data* d, char* filepath)
{
    uint8_t tmp = 0;
    FILE* r = fopen(filepath, "rb");
    if (r == NULL) return false;
    data_init(d);

    // Notices (possibly for custom metadata)
    if (!fread(&(d->n_notice), sizeof(uint8_t), 1, r)) return false;
    d->notice = (char**)malloc(sizeof(char*) * d->n_notice);
    for (uint8_t i = 0; i < d->n_notice; ++i)
    {
        if (!fread(&tmp, sizeof(tmp), 1, r)) return false;
        d->notice[i] = (char*)malloc(sizeof(char) * tmp + 1);
        if (fread(d->notice[i], sizeof(char), tmp, r) != tmp) return false;
        d->notice[i][tmp] = '\0';
    }

    // Character names
    for (uint8_t i = 0; i < 2; ++i)
    {
        if (!fread(&tmp, sizeof(tmp), 1, r)) return false;
        d->name[i] = (char*)malloc(sizeof(char) * tmp + 1);
        if (fread(d->name[i], sizeof(char), tmp, r) != tmp) return false;
        d->name[i][tmp] = '\0';
    }

    // Actual conversation
    fread(&(d->n_msg), sizeof(uint16_t), 1, r);
    d->msg = (char**)malloc(sizeof(char*) * d->n_msg);
    d->msg_timing = (timing*)malloc(sizeof(timing) * d->n_msg);
    for (uint16_t i = 0; i < d->n_msg; ++i)
    {
        if (!fread(&tmp, sizeof(tmp), 1, r)) return false;
        d->msg[i] = (char*)malloc(sizeof(char) * tmp + 1);
        if (fread(d->msg[i], sizeof(char), tmp, r) != tmp) return false;
        if (!fread(&(d->msg_timing[i]), sizeof(timing), 1, r)) return false;
        d->msg[i][tmp] = '\0';
    }

    fclose(r);
    return true;
}

bool write_conversation(data* d, char* filepath)
{
    uint8_t tmp = 0;
    FILE* w = fopen(filepath, "wb");
    if (w == NULL) return false;

    // Notices
    fwrite(&(d->n_notice), sizeof(d->n_notice), 1, w);
    for (uint8_t i = 0; i < d->n_notice; ++i)
    {
        tmp = strlen(d->notice[i]);
        fwrite(&tmp, sizeof(tmp), 1, w);
        fwrite(d->notice[i], sizeof(char), tmp, w);
    }

    // Names
    for (uint8_t i = 0; i < 2; ++i)
    {
        tmp = strlen(d->name[i]);
        fwrite(&tmp, sizeof(tmp), 1, w);
        fwrite(d->name[i], sizeof(char), tmp, w);
    }

    // Actual conversation
    fwrite(&(d->n_msg), sizeof(d->n_msg), 1, w);
    for (uint16_t i = 0; i < d->n_msg; ++i)
    {
        tmp = strlen(d->msg[i]);
        fwrite(&tmp, sizeof(tmp), 1, w);
        fwrite(d->msg[i], sizeof(char), tmp, w);
        fwrite(&(d->msg_timing[i]), sizeof(d->msg_timing[0]), 1, w);
    }

    fclose(w);
    return true;
}

char buffer[256];
void print_drama(char* name, char* msg, timing sleep_for)
{
    for (uint8_t c = 0; msg[c] != '\0'; ++c)
    {
        buffer[c] = msg[c];
        buffer[c + 1] = '\0';
        if (msg[c] == ' ')
        {
            printf("\r>> \033[38;2;220;150;250m%s\033[0m: %s", name, buffer);
            mysleep(sleep_for);
        }
    }
    printf("\r>> %s: %s", name, msg);
}

int main(int argc, char* argv[])
{
    data d;
    if (argc == 1)
    {
        if (!read_conversation(&d, "./conversation/try-again.cnv"))
        {
            printf("!> Failed to read standard conversation \"Try Again\", exiting.\n");
            return 1;
        }
    }
    else
    {
        if (!read_conversation(&d, argv[1]))
        {
            printf("!> Failed to read \"%s\", exiting.\n", argv[1]);
            printf(":> Just for notice; this parameter is supposed to be the path to a supported .cnv\n");
            return 1;
        }
    }

    for (uint8_t i = 0; i < d.n_notice; ++i)
        printf("\033[38;2;90;90;90m:> %s \033[0m\n", d.notice[i]);

    while (1)
    {
        char tmp;
        for (unsigned char m = 0; m < d.n_msg; ++m)
        {
            mysleep((timing)1000);
            print_drama(((m % 2) - 1) ? d.name[0] : d.name[1], d.msg[m], d.msg_timing[m]);
            scanf("%c", &tmp);
        }
        printf(">>\n");
    }
    data_set_null(&d);

    return 0;
}