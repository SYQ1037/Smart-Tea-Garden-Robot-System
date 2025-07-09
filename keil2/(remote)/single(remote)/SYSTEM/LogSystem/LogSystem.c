#include "LogSystem.h"
#include "oled.h"

static uint32_t LogData = 0x00000000;
uint32_t LogHis[5] = {0};
uint8_t LogHisIndex = 0;

uint32_t LogRead(void)
{
    return LogData;
}

void LogWrite(ERROR_TYPE ErrorType)
{
    LogData |= ErrorType;
    LogHis[LogHisIndex++] = ErrorType;
    if (LogHisIndex >= 5)
    {
        LogHisIndex = 0;
    }
}

void LogClear(ERROR_TYPE ErrorType)
{
    LogData &= ~ErrorType;
}
