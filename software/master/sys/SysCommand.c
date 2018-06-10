#include "SysCommand.h"
#include "APP.h"
static uint8_t g_buff[64];


//warning=5,shutdown=10
void SysCommandDataRecv(uint8_t *data, uint16_t len)
{
    uint16_t i;
    char *p;
    uint8_t warning, shutdown;
    static uint8_t dcount = 0;

    for(i = 0; i < len; i++)
    {
        g_buff[dcount++] = data[i];

        if(dcount >= sizeof(g_buff))
        {
            dcount = 0;
        }

        if(data[i] == '\n')
        {
            g_buff[dcount] = '\0';
            SysPrintf("%s", g_buff);

            if(strstr((char *)g_buff, "warning"))
            {
                p = strchr((char *)g_buff, '=');
                if(p)
                {
                    warning = strtol(p + 1, NULL, 10);
                }

                p = strstr((char *)g_buff, "shutdown");

                if(p)
                {
                    shutdown = strtol(p + strlen("shutdown="), NULL, 10);
                }
                SysDegreeArgSave(warning, shutdown);
                APPDegreeUpdate();
            }
            else if(strstr((char *)g_buff, "selfAjust"))
            {
                APPSlaveAjust();
            }

            dcount = 0;
        }
    }
}

void SysCommandInitialize(void)
{
}

void SysCommandPoll(void)
{
}

