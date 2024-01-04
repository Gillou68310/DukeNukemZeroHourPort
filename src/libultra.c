#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <PR/os.h>
#include "PR/sched.h"
#include "macros.h"

/*TODO: make sure all mixer commands are from mixer.h and not PR/abi.h*/

s32 osPiStartDma(UNUSED OSIoMesg *mb, UNUSED s32 priority, UNUSED s32 direction,
                 uintptr_t devAddr, void *vAddr, u32 nbytes,
                 UNUSED OSMesgQueue *mq)
{
    memcpy(vAddr, (const void *)devAddr, nbytes);
    return 0;
}

void osCreateMesgQueue(OSMesgQueue *mq, OSMesg *msgBuf, s32 count)
{
    mq->validCount = 0;
    mq->first = 0;
    mq->msgCount = count;
    mq->msg = msgBuf;
    return;
}

void osSetEventMesg(UNUSED OSEvent e, UNUSED OSMesgQueue *mq, UNUSED OSMesg msg)
{
}

s32 osJamMesg(UNUSED OSMesgQueue *mq, UNUSED OSMesg msg, UNUSED s32 flag)
{
    return 0;
}

s32 osSendMesg(UNUSED OSMesgQueue *mq, UNUSED OSMesg msg, UNUSED s32 flag)
{
    /*s32 index;
    if (mq->validCount >= mq->msgCount) {
        return -1;
    }
    index = (mq->first + mq->validCount) % mq->msgCount;
    mq->msg[index] = msg;
    mq->validCount++;*/
    return 0;
}

s32 osRecvMesg(UNUSED OSMesgQueue *mq, UNUSED OSMesg *msg, UNUSED s32 flag)
{
    /*if (mq->validCount == 0) {
        return -1;
    }
    if (msg != NULL) {
        *msg = *(mq->first + mq->msg);
    }
    mq->first = (mq->first + 1) % mq->msgCount;
    mq->validCount--;*/
    return 0;
}

uintptr_t osVirtualToPhysical(void *addr)
{
    return (uintptr_t)addr;
}

void osViBlack(UNUSED u8 active)
{
}

OSTime osGetTime(void)
{
    return 0;
}

void osWritebackDCacheAll(void)
{
}

void osWritebackDCache(UNUSED void *a, UNUSED s32 b)
{
}

void osInvalDCache(UNUSED void *a, UNUSED s32 b)
{
}

u32 osGetCount(void)
{
    static u32 counter;
    return counter++;
}

s32 osEepromProbe(UNUSED OSMesgQueue *mq)
{
    return 1;
}

s32 osEepromLongRead(UNUSED OSMesgQueue *mq, u8 address, u8 *buffer, int nbytes)
{
    u8 content[512];
    s32 ret = -1;

    FILE *fp = fopen("save.bin", "rb");
    if (fp == NULL)
    {
        return -1;
    }
    if (fread(content, 1, 512, fp) == 512)
    {
        memcpy(buffer, content + address * 8, nbytes);
        ret = 0;
    }
    fclose(fp);
    return ret;
}

s32 osEepromLongWrite(UNUSED OSMesgQueue *mq, u8 address, u8 *buffer, int nbytes)
{
    u8 content[512] = {0};
    if (address != 0 || nbytes != 512)
    {
        osEepromLongRead(mq, 0, content, 512);
    }
    memcpy(content + address * 8, buffer, nbytes);

    FILE *fp = fopen("save.bin", "wb");
    if (fp == NULL)
    {
        return -1;
    }
    s32 ret = fwrite(content, 1, 512, fp) == 512 ? 0 : -1;
    fclose(fp);
    return ret;
}

s32 osMotorInit(UNUSED OSMesgQueue *mq, UNUSED OSPfs *pfs, UNUSED int channel)
{
    return 0;
}

s32 __osMotorAccess(OSPfs *pfs, s32 cmd)
{
    return 0;
}

OSPiHandle *osCartRomInit(void)
{
    static OSPiHandle handle;
    return &handle;
}

s32 osPfsNumFiles(OSPfs *pfs, s32 *max_files, s32 *files_used)
{
    return 0;
}

s32 osPfsFreeBlocks(OSPfs *pfs, s32 *leftoverBytes)
{
    return 0;
}

s32 osPfsDeleteFile(OSPfs *pfs, u16 companyCode, u32 gameCode, u8 *gameName, u8 *extName)
{
    return 0;
}

s32 osPfsFileState(OSPfs *pfs, s32 fileNo, OSPfsState *state)
{
    return 0;
}

s32 osPfsInitPak(OSMesgQueue *mq, OSPfs *pfs, s32 channel)
{
    return 0;
}

s32 osPfsRepairId(OSPfs *pfs)
{
    return 0;
}

s32 osPfsReadWriteFile(OSPfs *pfs, s32 file_no, u8 flag, int offset, int size_in_bytes, u8 *data_buffer)
{
    return 0;
}

s32 osPfsAllocateFile(OSPfs *pfs, u16 company_code, u32 game_code, u8 *game_name, u8 *ext_name, int file_size_in_bytes, s32 *file_no)
{
    return 0;
}

s32 osPfsFindFile(OSPfs *pfs, u16 companyCode, u32 gameCode, u8 *gameName, u8 *extName, s32 *fileNo)
{
    return 0;
}

s32 osContStartQuery(OSMesgQueue *mq)
{
    return 0;
}

void osContGetQuery(OSContStatus *data)
{
}

void osCreateThread(OSThread *thread, OSId id, void (*entry)(void *), void *arg, void *sp, OSPri pri)
{
}

void osStartThread(OSThread *thread)
{
}

void osSetThreadPri(OSThread *thread, OSPri pri)
{
}

s32 osAfterPreNMI(void)
{
    return 0;
}

u32 osAiGetLength(void)
{
    return 0;
}

s32 osAiSetFrequency(u32 freq)
{
    /*TODO*/
    return 0;
}

s32 osAiSetNextBuffer(void *buf, size_t size)
{
    /*TODO*/
    return 0;
}

s32 osEPiStartDma(UNUSED OSPiHandle *pihandle, OSIoMesg *mb, UNUSED s32 direction)
{
    memcpy(mb->dramAddr, (const void *) mb->devAddr, mb->size);
    osSendMesg(mb->hdr.retQueue, mb, OS_MESG_NOBLOCK);
}

OSIntMask osGetIntMask(void)
{
}

OSIntMask osSetIntMask(OSIntMask mask)
{
}

u32 osAiGetStatus()
{
    return 0;
}

OSMesgQueue *osScGetCmdQ(OSSched *sc)
{
    return 0;
}

void osScAddClient(OSSched *sc, OSScClient *c, OSMesgQueue *msgQ)
{
}

void osInitialize()
{
}

void osCreatePiManager(OSPri pri, OSMesgQueue *cmdQ, OSMesg *cmdBuf, s32 cmdMsgCnt)
{
}

void osCreateScheduler(OSSched *sc, void *stack, OSPri priority, u8 mode, u8 numFields)
{
}

void osCreateViManager(UNUSED OSPri pri)
{
}

void osViSetMode(UNUSED OSViMode *mode)
{
}

void osViSetEvent(UNUSED OSMesgQueue *mq, UNUSED OSMesg msg, UNUSED u32 retraceCount)
{
}

void osViSetSpecialFeatures(UNUSED u32 func)
{
}

void osViSwapBuffer(UNUSED void *vaddr)
{
}