#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "utils.h"
#include "PublicDefine.h"
#include "SE_Interface.h"
#include "LOG_Interface.h"

static FILE *gResultFp = NULL;

int LOG_recordErr(int code,const char* msg) {

    char name[128];
    struct tm *t;
    time_t tt;

    time(&tt);
    t = localtime(&tt);

    sprintf(name, "/sdcard/wissen/err%4d%02d%02d%02d%02d%02d.txt", t->tm_year+1900,t->tm_mon+1,t->tm_mday, t->tm_hour,t->tm_min,t->tm_sec);
    printf("file name:%s\n", name);

    FILE* fp = fopen(name, "w");
    fprintf(fp, "err_code:%d\n err_msg:%s\n", code, msg);
    return 0;
}

int LOG_init(void)
{
    char name[128];
    struct tm *t;
    time_t tt;

    time(&tt);
    t = localtime(&tt);

    sprintf(name, "/sdcard/wissen/log%4d%02d%02d%02d%02d%02d.txt", t->tm_year+1900,t->tm_mon+1,t->tm_mday, t->tm_hour,t->tm_min,t->tm_sec);
    printf("file name:%s\n", name);

    gResultFp = fopen(name, "w");
    return 0;
}

int LOG_deinit(void)
{
    if(gResultFp)
        fclose(gResultFp);
    return 0;
}

int LOG_write(int frameNo, SE_UI_RESULT_s* result)
{
    if(!gResultFp)
    {
        printf("open file error\n");
        return -1;
    }

    struct tm *t;
    time_t tt;

    time(&tt);
    t = localtime(&tt);

    fprintf(gResultFp, "freameNo:%d, time:%4d-%02d-%02d %02d:%02d:%02d\n", frameNo, t->tm_year+1900,t->tm_mon+1,t->tm_mday, t->tm_hour,t->tm_min,t->tm_sec);
    if (NULL == result)
    {
        fprintf(gResultFp, "{\"LD\":{\"ldCount\":0,\"ldResult\":[]}}\n");
        fprintf(gResultFp, "{\"FC\":{\"fcCount\":0,\"fcResult\":[]}}\n");
    }
    else
    {
        int i = 0;
        fprintf(gResultFp, "{\"LD\":{\"ldCount\":%d,\"ldResult\":[",
                result->ldCount);
        for (i = 0; i < result->ldCount; i++)
        {
            LD_UI_RESULT_s* ld = result->ldResult + i;
            if (0 != i)
            {
                fprintf(gResultFp, ",");
            }
            fprintf(gResultFp, "{\"x\":%d,\"y\":%d,\"position\":%d,\"status\":%d}",
                    ld->point.x, ld->point.y, ld->position, ld->status);
        }
        fprintf(gResultFp, "]}}\n");
        fprintf(gResultFp, "{\"FC\":{\"fcCount\":%d,\"fcResult\":[",
                result->fcCount);
        for (i = 0; i < result->fcCount; i++)
        {
            FC_UI_RESULT_s* fc = result->fcResult + i;
            if (0 != i)
            {
                fprintf(gResultFp, ",");
            }
            fprintf(gResultFp, "{\"x\":%d,\"y\":%d,\"wid\":%d,\"hig\":%d,\"status\":%d}",
                    fc->point.x, fc->point.y, fc->wid, fc->hig, fc->status);
        }
        fprintf(gResultFp, "]}}\n");
    }

    return 0;
}

