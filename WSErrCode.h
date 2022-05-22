#ifndef __WISSEN_ERROR_CODE_H__
#define __WISSEN_ERROR_CODE_H__

typedef struct WS_FUN_STATUS {
    int status;
    char msg[256];
}WS_FUN_STATUS_s;

#define WS_OK                   (0)
#define WS_ERR_FILE_OPEN        (1)
#define WS_ERR_FILE_READ        (2)
#define WS_ERR_FILE_WRITE       (3)
#define WS_ERR_PARAM_INVALID    (4)
#define WS_ERR_MEM_ALLOC        (5)

char gErrorMsgs[][256] = {
    "ok\n",
    "open file fail\n",
    "read file fail\n",
    "write file fail\n",
    "input param invalid\n",
    "alloc mem fail\n"
};

#endif
