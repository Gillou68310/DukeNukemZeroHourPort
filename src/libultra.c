#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <dirent.h>
#include <assert.h> 
#include <PR/os.h>
#include "PR/sched.h"
#include "macros.h"

#define MAXSAVES 16
#define SAVEFOLDER "save"

typedef struct
{
    s32 size;
    u32 game_code;
    u16 company_code;
    u8 ext_name[PFS_FILE_EXT_LEN];
    u8 game_name[PFS_FILE_NAME_LEN];
} Save;

static Save _save[MAXSAVES];
static s32 _saveCount;

void init_save(void)
{
    FILE * f;
    DIR *d;
    struct dirent *dir;
    s32 i, j;
    char buf[32];

    _saveCount = 0;
    memset(_save, 0, sizeof(_save));
    mkdir(SAVEFOLDER);
    d = opendir(SAVEFOLDER);
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (!memcmp(dir->d_name, "save", 4))
            {
                i = atoi(&dir->d_name[4]);
                if((i >= 0) && (i < MAXSAVES))
                {
                    j = strlen(dir->d_name)-4;
                    if (!memcmp(&dir->d_name[j], ".dat", 4))
                    {
                        sprintf(buf, "%s/%s", SAVEFOLDER, dir->d_name);
                        f = fopen(buf, "rb");
                        fread(&_save[i].game_code, 1, sizeof(u32), f);
                        fread(&_save[i].company_code, 1, sizeof(u16), f);
                        fread(_save[i].game_name, 1, PFS_FILE_NAME_LEN, f);
                        fread(_save[i].ext_name, 1, PFS_FILE_EXT_LEN, f);
                        fclose(f);
                    }
                    else if (!memcmp(&dir->d_name[j], ".bin", 4))
                    {
                        sprintf(buf, "%s/%s", SAVEFOLDER, dir->d_name);
                        f = fopen(buf, "rb");
                        fseek(f, 0L, SEEK_END);
                        _save[i].size = ftell(f);
                        fclose(f);
                    }
                }
            }
        }
        closedir(d);
    }
}

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
    return 0;
}

s32 osRecvMesg(UNUSED OSMesgQueue *mq, UNUSED OSMesg *msg, UNUSED s32 flag)
{
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
    assert(0);
}

s32 osEepromLongWrite(UNUSED OSMesgQueue *mq, u8 address, u8 *buffer, int nbytes)
{
    assert(0);
}

s32 osMotorInit(UNUSED OSMesgQueue *mq, UNUSED OSPfs *pfs, UNUSED int channel)
{
    return PFS_ERR_DEVICE;
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
    *files_used = _saveCount;
    *max_files = MAXSAVES;
    return 0;
}

s32 osPfsFreeBlocks(OSPfs *pfs, s32 *leftoverBytes)
{
    *leftoverBytes = 131072;
    return 0;
}

s32 osPfsDeleteFile(OSPfs *pfs, u16 companyCode, u32 gameCode, u8 *gameName, u8 *extName)
{
    s32 file_no;
    char buf[32];

    osPfsFindFile(pfs, companyCode, gameCode, gameName, extName, &file_no);
    if (file_no == -1)
        return PFS_ERR_INVALID;

    sprintf(buf, "%s/save%d.bin", SAVEFOLDER, file_no);
    remove(buf);
    sprintf(buf, "%s/save%d.dat", SAVEFOLDER, file_no);
    remove(buf);
    memset(&_save[file_no], 0, sizeof(Save));
    _saveCount--;
    return 0;
}

s32 osPfsFileState(OSPfs *pfs, s32 fileNo, OSPfsState *state)
{
    char buf[32];

    assert((state != NULL) && (fileNo >= 0) && (fileNo < MAXSAVES));
    if(_save[fileNo].size == 0)
        return PFS_ERR_INVALID;

    state->file_size = _save[fileNo].size;
    state->game_code = _save[fileNo].game_code;
    state->company_code = _save[fileNo].company_code;

    memcpy(state->game_name, _save[fileNo].game_name, PFS_FILE_NAME_LEN);
    memcpy(state->ext_name, _save[fileNo].ext_name, PFS_FILE_EXT_LEN);
    return 0;
}

s32 osPfsInitPak(OSMesgQueue *mq, OSPfs *pfs, s32 channel)
{
    return 0;
}

s32 osPfsRepairId(OSPfs *pfs)
{
    assert(0);
}

s32 osPfsReadWriteFile(OSPfs *pfs, s32 file_no, u8 flag, int offset, int size_in_bytes, u8 *data_buffer)
{
    FILE* f;
    char buf[32];

    assert((file_no>=0)&&(file_no<MAXSAVES));
    assert(offset == 0);
    assert(_save[file_no].size == size_in_bytes);

    sprintf(buf, "%s/save%d.bin", SAVEFOLDER, file_no);
    if(flag == OS_READ)
    {
        f = fopen(buf, "rb");
        fread(data_buffer, 1, size_in_bytes, f);
        fclose(f);
    }
    else
    {
        f = fopen(buf, "wb");
        fwrite(data_buffer, 1, size_in_bytes, f);
        fclose(f);
    } 

    return 0;
}

s32 osPfsAllocateFile(OSPfs *pfs, u16 company_code, u32 game_code, u8 *game_name, u8 *ext_name, int file_size_in_bytes, s32 *file_no)
{
    FILE* f;
    s32 i;
    char buf[32];

    osPfsFindFile(pfs, company_code, game_code, game_name, ext_name, file_no);
    if (*file_no != -1)
        return PFS_ERR_EXIST;

    for(i = 0; i<MAXSAVES; i++)
    {
        if(_save[i].size == 0)
        {
            _save[i].size = file_size_in_bytes;
            _save[i].game_code = game_code;
            _save[i].company_code = company_code;

            memcpy(_save[i].game_name, game_name, PFS_FILE_NAME_LEN);
            memcpy(_save[i].ext_name, ext_name, PFS_FILE_EXT_LEN);

            sprintf(buf, "%s/save%d.dat", SAVEFOLDER, i);
            f = fopen(buf, "wb");
            fwrite(&_save[i].game_code, 1, sizeof(u32), f);
            fwrite(&_save[i].company_code, 1, sizeof(u16), f);
            fwrite(_save[i].game_name, 1, PFS_FILE_NAME_LEN, f);
            fwrite(_save[i].ext_name, 1, PFS_FILE_EXT_LEN, f);
            fclose(f);
            _saveCount++;
            *file_no = i;
            return 0;
        }
    }

    *file_no = -1;
    return PFS_DIR_FULL;
}

s32 osPfsFindFile(OSPfs *pfs, u16 companyCode, u32 gameCode, u8 *gameName, u8 *extName, s32 *fileNo)
{
    s32 i;

    for(i = 0; i<MAXSAVES; i++)
    {
        if((_save[i].size != 0) &&
           (_save[i].game_code == gameCode) &&
           (_save[i].company_code == companyCode) &&
           (!memcmp(_save[i].game_name, gameName, PFS_FILE_NAME_LEN)) && 
           (!memcmp(_save[i].ext_name, extName, PFS_FILE_EXT_LEN)))
        {
            *fileNo = i;
            return 0;
        }
    }

    *fileNo = -1;
    return PFS_ERR_INVALID;
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
    u32 a1;
    s32 a2;
    u32 viClock;

    viClock = 0x02E6D354;

    a1 = viClock / (float) freq + .5f;

    if (a1 < 0x84) {
        return -1;
    }

    a2 = (a1 / 66) & 0xff;
    if (a2 > 16) {
        a2 = 16;
    }

    return viClock / (s32) a1;
}

s32 osAiSetNextBuffer(void *buf, size_t size)
{
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