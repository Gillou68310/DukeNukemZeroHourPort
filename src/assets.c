#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>
#include "common.h"
#include "code0/code0.h"
#include "code0/audio.h"
#include "code0/edl.h"
#include "code0/engine.h"
#include "code0/9410.h"
#include "code0/data/modelinfo.h"
#include "code0/data/E0640.h"
#include "code0/data/D8D20.h"
#include "code1/EB300.h"
#include "static/mapinfo.h"
#include "static/tileinfo.h"
#include "code0/data/D8D20.h"
#include "libmus_data.h"
#include "player.h"

/*TODO: swap all palettes and remove the swap from fast3d*/

#ifdef _WIN32
#define SWAP_S16(A) ((s16)_byteswap_ushort(A))
#define SWAP_S32(A) ((s32)_byteswap_ulong(A))
#define SWAP_U16(A) ((u16)_byteswap_ushort(A))
#define SWAP_U32(A) ((u32)_byteswap_ulong(A))
#else
#define SWAP_S16(A) ((s16)__builtin_bswap16(A))
#define SWAP_S32(A) ((s32)__builtin_bswap32(A))
#define SWAP_U16(A) ((u16)__builtin_bswap16(A))
#define SWAP_U32(A) ((u32)__builtin_bswap32(A))
#endif

#define READ_S32(A) SWAP_S32(*((s32*)A)); A+=sizeof(s32);
#define READ_U32(A) SWAP_U32(*((u32*)A)); A+=sizeof(u32);
#define READ_S16(A) SWAP_S16(*((s16*)A)); A+=sizeof(s16);
#define READ_U16(A) SWAP_U16(*((u16*)A)); A+=sizeof(u16);

#define ALIGN16(A) A = (void*)ALIGN(A, 16)
#define MEMBER_SIZE(type, member) sizeof(((type *)0)->member)
#define BASE_OFFSET(A, B) A->B = (void*)((intptr_t)A->B - (intptr_t)A)

typedef struct
{
    edlUnkStruct1 *edl;
    s32 offset_count;
    _E0640UnkStruct *info;
    u8 *data;
} Art;

static u8 _buffer[0x4000000];
static u8 *_pBuffer;

static SectorType _sectors[MAXSECTORS];

u8 *sounds_bank0_ptr_ROM_START;
u8 *sounds_bank0_ptr_ROM_END;

u8 *sounds_bank0_wbk_ROM_START;
u8 *sounds_bank0_wbk_ROM_END;

u8 *sounds_sfx_bfx_ROM_START;
u8 *sounds_sfx_bfx_ROM_END;

static Art _arts[] = {
    {&D_800E0D18[0],  -1, D_800DFA40, NULL},
    {&D_800E0D18[1],  -1, D_800DFA40, NULL},
    {&D_800E0D18[2],  -1, D_800DFA90, NULL},
    {&D_800E0D18[5],  -1, D_800E0778, NULL},
    {&D_800E0D18[3],  -1, D_800DFAB8, NULL},
    {&D_800E0D18[4],  -1, D_800DFAE0, NULL},
    {&D_800E0D18[6],  -1, D_800E0454, NULL},
    {&D_800E0D18[7],  -1, D_800E0454, NULL},
    {&D_800E0D18[8],  -1, D_800E0454, NULL},
    {&D_800E0D18[9],  -1, D_800E0454, NULL},
    {&D_800E0D18[10], -1, D_800E0454, NULL},
    {&D_800E0D18[11], -1, D_800DFB08, NULL},
    {&D_800E0D18[12], -1, NULL,       NULL},
    {&D_800E0D18[13], -1, D_800E0404, NULL},
    {&D_800E0D18[14], -1, D_800E047C, NULL},
    {&D_800E0D18[15], -1, D_800E064C, NULL},
    {&D_800E0D18[16], 13, NULL,       NULL}, /*D_800E0BE4*/
    {&D_800E0D18[17], -1, NULL,       NULL},
    {&D_800E0D18[18], -1, NULL,       NULL},
    {&D_800E0D18[19], -1, NULL,       NULL},
    {&D_800E0D18[24],  1, NULL,       NULL}, /*D_800E0BE0*/
    {&D_800E0D18[25], -1, NULL,       NULL},
    {&D_800E0D18[26], -1, NULL,       NULL},
    {&D_800E0D18[27], -1, NULL,       NULL},
    {&D_800E0D18[20], 64, NULL,       NULL}, /*D_800E0C18*/
    {&D_800E0D18[21], -1, NULL,       NULL},
    {&D_800E0D18[22], -1, NULL,       NULL},
    {&D_800E0D18[23], -1, NULL,       NULL},
    {&D_800E0D18[28], -1, D_800E07DC, NULL},
    {&D_800E0D18[29], -1, NULL,       NULL},
};

static ModelInfo *_models[] = {
    &D_800BE2D0, &D_800BE42C, &D_800BE768, &D_800C0070, &D_800C00A0, &D_800C08C8, &D_800C14EC, &D_800C2908,
    &D_800C2938, &D_800C2968, &D_800C2998, &D_800C29C8, &D_800C29F8, &D_800C2A28, &D_800C2A58, &D_800C2A88,
    &D_800CD0E4, &D_800CD114, &D_800BD850, &D_800BD79C, &D_800BD7D8, &D_800BD814, &D_800BD8C8, &D_800BD88C,
    &D_800BD904, &D_800BD940, &D_800BD97C, &D_800BD9B8, &D_800BD9F4, &D_800BDA30, &D_800BDA6C, &D_800BDAA8,
    &D_800BDAE4, &D_800BDB20, &D_800BDB5C, &D_800BDB98, &D_800BDBD4, &D_800BDC10, &D_800BDC4C, &D_800BDC88,
    &D_800BDCC4, &D_800BDD00, &D_800BDD3C, &D_800BDD78, &D_800BDDB4, &D_800BDDF0, &D_800BDE2C, &D_800BDE68,
    &D_800BDEA4, &D_800BDEE0, &D_800BDF1C, &D_800BDF58, &D_800BDF94, &D_800BDFD0, &D_800BE00C, &D_800BE048,
    &D_800BE084, &D_800BE0C0, &D_800BE0FC, &D_800BE138, &D_800BE174, &D_800BE1B0, &D_800BE1EC, &D_800BE228,
    &D_800BE264, &D_800BE2A0, &D_800BE3C0, &D_800BE30C, &D_800BE348, &D_800BE384, &D_800BE3FC, &D_800BE468,
    &D_800BE4A4, &D_800BE4E0, &D_800BE51C, &D_800BE558, &D_800BE594, &D_800BE5D0, &D_800BE60C, &D_800BE648,
    &D_800BE684, &D_800BE6C0, &D_800BE6FC, &D_800BE738, &D_800BE7A4, &D_800BE7E0, &D_800BE81C, &D_800BE858,
    &D_800BE894, &D_800BE8D0, &D_800BE90C, &D_800BE948, &D_800BE984, &D_800BE9C0, &D_800BE9FC, &D_800BEA38,
    &D_800BEA74, &D_800BEAB0, &D_800BEAEC, &D_800BEB28, &D_800BEB64, &D_800BEBA0, &D_800BEBDC, &D_800BEC18,
    &D_800BEC54, &D_800BEC90, &D_800BECCC, &D_800BED08, &D_800BED44, &D_800BED80, &D_800BEDBC, &D_800BEDF8,
    &D_800BEE34, &D_800BEE70, &D_800BEEAC, &D_800BEEE8, &D_800BEF24, &D_800BEF60, &D_800BEF9C, &D_800BEFD8,
    &D_800BF014, &D_800BF050, &D_800BF08C, &D_800BF0C8, &D_800BF104, &D_800BF140, &D_800BF17C, &D_800BF1B8,
    &D_800BF1F4, &D_800BF35C, &D_800BF398, &D_800BF3D4, &D_800BF410, &D_800BF44C, &D_800BF488, &D_800BF230,
    &D_800BF26C, &D_800BF2A8, &D_800BF2E4, &D_800BF320, &D_800BF4C4, &D_800BF500, &D_800BF53C, &D_800BF578,
    &D_800BF5B4, &D_800BF5F0, &D_800BF62C, &D_800BF668, &D_800BF6A4, &D_800BF6E0, &D_800BF71C, &D_800BF758,
    &D_800BF794, &D_800BF7D0, &D_800BF80C, &D_800BF848, &D_800BF884, &D_800BF8C0, &D_800BF8FC, &D_800BF938,
    &D_800BF974, &D_800BF9B0, &D_800BF9EC, &D_800BFA28, &D_800BFA64, &D_800BFAA0, &D_800BFADC, &D_800BFB18,
    &D_800BFB54, &D_800BFB90, &D_800BFBCC, &D_800BFC08, &D_800BFC44, &D_800BFC80, &D_800BFCBC, &D_800BFCF8,
    &D_800BFD34, &D_800BFD70, &D_800BFDAC, &D_800BFDE8, &D_800BFE24, &D_800BFE60, &D_800BFE9C, &D_800BFED8,
    &D_800BFF14, &D_800BFF50, &D_800BFF8C, &D_800BFFC8, &D_800C0004, &D_800C0040, &D_800C00DC, &D_800C0118,
    &D_800C0154, &D_800C0190, &D_800C01CC, &D_800C0208, &D_800C0244, &D_800C0280, &D_800C02BC, &D_800C02F8,
    &D_800C0334, &D_800C0370, &D_800C03AC, &D_800C03E8, &D_800C0424, &D_800C0460, &D_800C049C, &D_800C04D8,
    &D_800C0514, &D_800C0550, &D_800C058C, &D_800C05C8, &D_800C0604, &D_800C0640, &D_800C067C, &D_800C06B8,
    &D_800C06F4, &D_800C0730, &D_800C076C, &D_800C07A8, &D_800C07E4, &D_800C0820, &D_800C085C, &D_800C0898,
    &D_800C0904, &D_800C0940, &D_800C097C, &D_800C09B8, &D_800C09F4, &D_800C0A30, &D_800C0A6C, &D_800C0AA8,
    &D_800C0AE4, &D_800C0B20, &D_800C0B5C, &D_800C0B98, &D_800C0BD4, &D_800C0C10, &D_800C0C4C, &D_800C0C88,
    &D_800C0CC4, &D_800C0D00, &D_800C0D3C, &D_800C0D78, &D_800C0DB4, &D_800C0DF0, &D_800C0E2C, &D_800C0E68,
    &D_800C0EA4, &D_800C0EE0, &D_800C0F1C, &D_800C0F58, &D_800C0F94, &D_800C0FD0, &D_800C100C, &D_800C1048,
    &D_800C1084, &D_800C10C0, &D_800C10FC, &D_800C1138, &D_800C1174, &D_800C11B0, &D_800C11EC, &D_800C1228,
    &D_800C1264, &D_800C12A0, &D_800C12DC, &D_800C1318, &D_800C1354, &D_800C1390, &D_800C13CC, &D_800C1408,
    &D_800C1A50, &D_800C1A8C, &D_800C1AC8, &D_800C1B04, &D_800C1B40, &D_800C1B7C, &D_800C1BB8, &D_800C1BF4,
    &D_800C1C30, &D_800C1C6C, &D_800C1CA8, &D_800C1CE4, &D_800C1D20, &D_800C1D5C, &D_800C1D98, &D_800C1DD4,
    &D_800C1444, &D_800C1480, &D_800C14BC, &D_800C1528, &D_800C15A0, &D_800C1564, &D_800C1E10, &D_800C15DC,
    &D_800C1618, &D_800C1654, &D_800C1690, &D_800C16CC, &D_800C1708, &D_800C1744, &D_800C1780, &D_800C17BC,
    &D_800C17F8, &D_800C1834, &D_800C1870, &D_800C18AC, &D_800C18E8, &D_800C1924, &D_800C1960, &D_800C199C,
    &D_800C19D8, &D_800C1A14, &D_800C1E4C, &D_800C1E88, &D_800C1EC4, &D_800C1F00, &D_800C1F3C, &D_800C1F78,
    &D_800C1FB4, &D_800C1FF0, &D_800C202C, &D_800C2068, &D_800C20A4, &D_800C20E0, &D_800C211C, &D_800C2158,
    &D_800C2194, &D_800C21D0, &D_800C220C, &D_800C2248, &D_800C2284, &D_800C22C0, &D_800C22FC, &D_800C2338,
    &D_800C2374, &D_800C23B0, &D_800C23EC, &D_800C2428, &D_800C2464, &D_800C24A0, &D_800C24DC, &D_800C2518,
    &D_800C2554, &D_800C2590, &D_800C25CC, &D_800C2608, &D_800C2644, &D_800C2680, &D_800C26BC, &D_800C26F8,
    &D_800C2734, &D_800C2770, &D_800C27AC, &D_800C27E8, &D_800C2824, &D_800C2860, &D_800C289C, &D_800C28D8,
    &D_800C2AC4, &D_800C2B00, &D_800C2B3C, &D_800C2B78, &D_800C2BB4, &D_800C2BF0, &D_800C2C2C, &D_800C2C68,
    &D_800C2CA4, &D_800C2CE0, &D_800C2D1C, &D_800C2D58, &D_800C2D94, &D_800CCB50, &D_800CCB8C, &D_800C475C,
    &D_800C4798, &D_800C47D4, &D_800C4810, &D_800C484C, &D_800C4888, &D_800C48C4, &D_800C4900, &D_800C493C,
    &D_800C4978, &D_800C49B4, &D_800C49F0, &D_800C4A2C, &D_800CCBC8, &D_800CCC04, &D_800CCC40, &D_800C5878,
    &D_800C58B4, &D_800C58F0, &D_800C592C, &D_800C5968, &D_800C59A4, &D_800C59E0, &D_800C5A1C, &D_800C5A58,
    &D_800C5A94, &D_800C5AD0, &D_800C5B0C, &D_800C5B48, &D_800C5B84, &D_800CCC7C, &D_800CCCB8, &D_800CCCF4,
    &D_800CCD30, &D_800CCD6C, &D_800CCDA8, &D_800CCDE4, &D_800CCE20, &D_800CCE5C, &D_800CCE98, &D_800CCED4,
    &D_800CCF10, &D_800CCF4C, &D_800CCF88, &D_800CCFC4, &D_800CD000, &D_800CD03C, &D_800CD078, &D_800CD0B4,
    &D_800C5BC0, &D_800C5BFC, &D_800C5C38, &D_800C5C74, &D_800C5CB0, &D_800C5CEC, &D_800C5D28, &D_800C5D64,
    &D_800C5DA0, &D_800C5DDC, &D_800C5E18, &D_800C5E54, &D_800C5E90, &D_800C77A4, &D_800C77E0, &D_800C781C,
    &D_800C7858, &D_800C7894, &D_800C78D0, &D_800C790C, &D_800C7948, &D_800C7984, &D_800C79C0, &D_800C79FC,
    &D_800C7A38, &D_800C7A74, &D_800C4A68, &D_800C4AA4, &D_800C4AE0, &D_800C4B1C, &D_800C4B58, &D_800C4B94,
    &D_800C4BD0, &D_800C4C0C, &D_800C4C48, &D_800C4C84, &D_800C4CC0, &D_800C4CFC, &D_800C4D38, &D_800C4D74,
    &D_800C4DB0, &D_800C4DEC, &D_800C4E28, &D_800C4E64, &D_800C4EA0, &D_800C4EDC, &D_800C4F18, &D_800C4F54,
    &D_800C4F90, &D_800C4FCC, &D_800C5008, &D_800C5044, &D_800CD150, &D_800CD18C, &D_800CD1C8, &D_800CD204,
    &D_800CD240, &D_800CD27C, &D_800CD2B8, &D_800CD2F4, &D_800CD330, &D_800CD36C, &D_800C2DD0, &D_800C2E0C,
    &D_800C2E48, &D_800C2E84, &D_800C2EC0, &D_800C2EFC, &D_800C2F38, &D_800C2F74, &D_800C2FB0, &D_800C2FEC,
    &D_800C3028, &D_800C3064, &D_800C30A0, &D_800C30DC, &D_800C3118, &D_800C3154, &D_800C3190, &D_800C31CC,
    &D_800C3208, &D_800C3244, &D_800C3280, &D_800C32BC, &D_800C32F8, &D_800C3334, &D_800C3370, &D_800C33AC,
    &D_800C3730, &D_800C376C, &D_800C37A8, &D_800C37E4, &D_800C3820, &D_800C385C, &D_800C3898, &D_800C38D4,
    &D_800C3910, &D_800C394C, &D_800C3988, &D_800C39C4, &D_800C3A00, &D_800C7DBC, &D_800C7DF8, &D_800C7E34,
    &D_800C7E70, &D_800C7EAC, &D_800C7EE8, &D_800C7F24, &D_800CD3A8, &D_800CD3E4, &D_800CD420, &D_800CD45C,
    &D_800CD498, &D_800CD4D4, &D_800CD510, &D_800CD54C, &D_800CD588, &D_800CD5C4, &D_800CD600, &D_800CD63C,
    &D_800CD678, &D_800CD6B4, &D_800CD6F0, &D_800CD72C, &D_800CD768, &D_800CD7A4, &D_800CD7E0, &D_800CD81C,
    &D_800CD858, &D_800CD894, &D_800CD8D0, &D_800CD90C, &D_800CD948, &D_800CD984, &D_800CD9C0, &D_800CD9FC,
    &D_800CDA38, &D_800CDA74, &D_800CDAB0, &D_800CDAEC, &D_800C6868, &D_800C68A4, &D_800C68E0, &D_800C691C,
    &D_800C6958, &D_800C6994, &D_800C69D0, &D_800C6A0C, &D_800C6A48, &D_800C6A84, &D_800C6AC0, &D_800C6AFC,
    &D_800C6B38, &D_800C6B74, &D_800C6BB0, &D_800C6BEC, &D_800C6C28, &D_800C6C64, &D_800C6CA0, &D_800C6CDC,
    &D_800C6D18, &D_800C6D54, &D_800C6D90, &D_800C6DCC, &D_800C6E08, &D_800C6E44, &D_800C6E80, &D_800C6EBC,
    &D_800C6EF8, &D_800C6F34, &D_800C6F70, &D_800C6FAC, &D_800C6FE8, &D_800C7024, &D_800C7060, &D_800C709C,
    &D_800C70D8, &D_800C7114, &D_800C7150, &D_800CDB28, &D_800CDB64, &D_800CDBA0, &D_800CDBDC, &D_800CDC18,
    &D_800CDC54, &D_800CDC90, &D_800C7F60, &D_800C7F9C, &D_800C7FD8, &D_800C8014, &D_800C8050, &D_800C808C,
    &D_800C80C8, &D_800C8104, &D_800C8140, &D_800C817C, &D_800C81B8, &D_800C81F4, &D_800C8230, &D_800C826C,
    &D_800C82A8, &D_800C718C, &D_800C71C8, &D_800C7204, &D_800C7240, &D_800C727C, &D_800C72B8, &D_800C72F4,
    &D_800C7330, &D_800C736C, &D_800C73A8, &D_800C73E4, &D_800C7420, &D_800C745C, &D_800C7498, &D_800C74D4,
    &D_800C7510, &D_800C754C, &D_800C7588, &D_800C75C4, &D_800C7600, &D_800C763C, &D_800C7678, &D_800C76B4,
    &D_800C76F0, &D_800C772C, &D_800C7768, &D_800C7AB0, &D_800C7AEC, &D_800C7B28, &D_800C7B64, &D_800C7BA0,
    &D_800C7BDC, &D_800C7C18, &D_800C7C54, &D_800C7C90, &D_800C7CCC, &D_800C7D08, &D_800C7D44, &D_800C7D80,
    &D_800C3A3C, &D_800C3A78, &D_800C3AB4, &D_800C3AF0, &D_800C3B2C, &D_800C3B68, &D_800C3BA4, &D_800C3BE0,
    &D_800C3C1C, &D_800C3C58, &D_800C3C94, &D_800C3CD0, &D_800C3D0C, &D_800C4414, &D_800C4450, &D_800C448C,
    &D_800C44C8, &D_800C4504, &D_800C4540, &D_800C457C, &D_800C45B8, &D_800C45F4, &D_800C4630, &D_800C466C,
    &D_800C46A8, &D_800C46E4, &D_800C3D48, &D_800C3D84, &D_800C3DC0, &D_800C3DFC, &D_800C3E38, &D_800C3E74,
    &D_800C3EB0, &D_800C3EEC, &D_800C3F28, &D_800C3F64, &D_800C3FA0, &D_800C3FDC, &D_800C4018, &D_800CDCCC,
    &D_800CDD08, &D_800CDD44, &D_800CDD80, &D_800CDDBC, &D_800CDDF8, &D_800CDE34, &D_800CDE70, &D_800CDEAC,
    &D_800CDEE8, &D_800CDF24, &D_800CDF60, &D_800CDF9C, &D_800CDFD8, &D_800CE014, &D_800CE050, &D_800CE08C,
    &D_800CE0C8, &D_800CE104, &D_800CE140, &D_800CE17C, &D_800CE1B8, &D_800CE1F4, &D_800CE230, &D_800CE26C,
    &D_800CE2A8, &D_800CE2E4, &D_800CE320, &D_800CE35C, &D_800CE398, &D_800CE3D4, &D_800CE410, &D_800CE44C,
    &D_800CE488, &D_800CE4C4, &D_800CE500, &D_800CE53C, &D_800CE578, &D_800CE5B4, &D_800CE5F0, &D_800CE62C,
    &D_800CE668, &D_800CE6A4, &D_800CE6E0, &D_800CE71C, &D_800CE758, &D_800CEBCC, &D_800CEC08, &D_800CEC44,
    &D_800CEC80, &D_800CECBC, &D_800CECF8, &D_800CED34, &D_800CED70, &D_800CEDAC, &D_800CEDE8, &D_800CEE24,
    &D_800CE794, &D_800CE7D0, &D_800CE80C, &D_800CE848, &D_800CE884, &D_800CE8C0, &D_800CE8FC, &D_800CE938,
    &D_800CE974, &D_800CE9B0, &D_800CE9EC, &D_800CEA28, &D_800CEA64, &D_800CEAA0, &D_800CEADC, &D_800CEB18,
    &D_800CEB54, &D_800CEB90, &D_800CEE60, &D_800CEE9C, &D_800CEED8, &D_800CEF14, &D_800C9694, &D_800C96D0,
    &D_800C970C, &D_800C9748, &D_800CEF50, &D_800CEF8C, &D_800CEFC8, &D_800CF004, &D_800C8C80, &D_800C8CBC,
    &D_800C8CF8, &D_800C8D34, &D_800C8D70, &D_800C8DAC, &D_800C8DE8, &D_800C8E24, &D_800C8E60, &D_800C8E9C,
    &D_800C8ED8, &D_800C8F14, &D_800C8F50, &D_800C8F8C, &D_800C8FC8, &D_800C9004, &D_800C9040, &D_800C907C,
    &D_800C90B8, &D_800C90F4, &D_800C9130, &D_800C916C, &D_800C91A8, &D_800C91E4, &D_800C9220, &D_800C925C,
    &D_800C9298, &D_800C92D4, &D_800C9784, &D_800C97C0, &D_800C97FC, &D_800C9838, &D_800CF040, &D_800CF07C,
    &D_800CF0B8, &D_800CF0F4, &D_800CF130, &D_800C9874, &D_800C98B0, &D_800C98EC, &D_800C9928, &D_800C9964,
    &D_800C99A0, &D_800C99DC, &D_800C9A18, &D_800C9A54, &D_800C9A90, &D_800C9ACC, &D_800C9B08, &D_800C9B44,
    &D_800C9B80, &D_800C9BBC, &D_800C9BF8, &D_800C9C34, &D_800C9C70, &D_800C9CAC, &D_800C9CE8, &D_800C9D24,
    &D_800C9D60, &D_800C9D9C, &D_800C9DD8, &D_800C9E14, &D_800C9E50, &D_800C9E8C, &D_800C9EC8, &D_800C9F04,
    &D_800C9F40, &D_800C9F7C, &D_800C9FB8, &D_800C9FF4, &D_800C85F0, &D_800C862C, &D_800C8668, &D_800C86A4,
    &D_800C86E0, &D_800C871C, &D_800C8758, &D_800C8794, &D_800C87D0, &D_800C880C, &D_800C8848, &D_800C8884,
    &D_800C88C0, &D_800C9310, &D_800C9388, &D_800C93C4, &D_800C9400, &D_800C943C, &D_800C9478, &D_800C94B4,
    &D_800C94F0, &D_800C952C, &D_800C9568, &D_800C95A4, &D_800C95E0, &D_800C961C, &D_800C9658, &D_800C934C,
    &D_800CF16C, &D_800CF1A8, &D_800CF1E4, &D_800CF25C, &D_800CF220, &D_800C54B8, &D_800C54F4, &D_800C5530,
    &D_800C556C, &D_800C55A8, &D_800C55E4, &D_800C5620, &D_800C565C, &D_800C5698, &D_800C56D4, &D_800C5710,
    &D_800C574C, &D_800C5788, &D_800C57C4, &D_800C5800, &D_800C583C, &D_800CF298, &D_800CF2D4, &D_800CF310,
    &D_800CF34C, &D_800CF388, &D_800C33E8, &D_800C3424, &D_800C3460, &D_800C349C, &D_800C34D8, &D_800C3514,
    &D_800C3550, &D_800C358C, &D_800C35C8, &D_800C3604, &D_800C3640, &D_800C367C, &D_800C36B8, &D_800CF4F0,
    &D_800CA15C, &D_800CA198, &D_800CA1D4, &D_800CA210, &D_800CF3C4, &D_800CF400, &D_800CF43C, &D_800CF478,
    &D_800CF4B4, &D_800CF52C, &D_800CF568, &D_800CF5A4, &D_800CF5E0, &D_800CF61C, &D_800CF658, &D_800CF694,
    &D_800CF6D0, &D_800CF70C, &D_800CF748, &D_800CF784, &D_800CF7C0, &D_800C82E4, &D_800C835C, &D_800C8398,
    &D_800C83D4, &D_800C8410, &D_800C844C, &D_800C8488, &D_800C84C4, &D_800C8500, &D_800C853C, &D_800C8578,
    &D_800C85B4, &D_800C8320, &D_800CF7FC, &D_800CF838, &D_800CF874, &D_800CF8B0, &D_800CF8EC, &D_800CF928,
    &D_800CF964, &D_800CF9A0, &D_800CF9DC, &D_800CFA18, &D_800CFA54, &D_800CFA90, &D_800CFACC, &D_800CFB08,
    &D_800CFB44, &D_800CA24C, &D_800CA288, &D_800CA2C4, &D_800CA300, &D_800CA33C, &D_800CA378, &D_800CA3B4,
    &D_800CA3F0, &D_800CA42C, &D_800CA468, &D_800CA4A4, &D_800CA4E0, &D_800CA51C, &D_800CA558, &D_800CA594,
    &D_800CA5D0, &D_800CA60C, &D_800CA648, &D_800CA684, &D_800CA6C0, &D_800CA6FC, &D_800CA738, &D_800CA774,
    &D_800CA7B0, &D_800CA7EC, &D_800CFB80, &D_800CFBBC, &D_800CFBF8, &D_800C5080, &D_800C50BC, &D_800C50F8,
    &D_800CA828, &D_800CA864, &D_800CA8A0, &D_800CA8DC, &D_800CA918, &D_800CFC34, &D_800CFC70, &D_800CFCAC,
    &D_800CFCE8, &D_800CFD24, &D_800CFD60, &D_800CFD9C, &D_800CFDD8, &D_800CFE14, &D_800CFE50, &D_800CFE8C,
    &D_800CFEC8, &D_800CFF04, &D_800CFF40, &D_800CFF7C, &D_800CFFB8, &D_800CFFF4, &D_800D0030, &D_800D006C,
    &D_800D00A8, &D_800D00E4, &D_800CA954, &D_800CA990, &D_800CA9CC, &D_800CAA08, &D_800CAA44, &D_800CAA80,
    &D_800CAABC, &D_800CAAF8, &D_800CAB34, &D_800CAB70, &D_800CABAC, &D_800CABE8, &D_800CAC24, &D_800CAC60,
    &D_800CAC9C, &D_800CACD8, &D_800CAD14, &D_800CAD50, &D_800CAD8C, &D_800CADC8, &D_800CAE04, &D_800CAE40,
    &D_800CAE7C, &D_800CAEB8, &D_800CAEF4, &D_800D0120, &D_800D015C, &D_800C538C, &D_800C53C8, &D_800C5404,
    &D_800C547C, &D_800C5440, &D_800D0198, &D_800D01D4, &D_800D0210, &D_800D024C, &D_800D0288, &D_800D02C4,
    &D_800D0300, &D_800D033C, &D_800D0378, &D_800D03B4, &D_800D03F0, &D_800D042C, &D_800D0468, &D_800D04A4,
    &D_800D04E0, &D_800D051C, &D_800D0558, &D_800D0594, &D_800CAF30, &D_800CAF6C, &D_800CAFA8, &D_800CAFE4,
    &D_800CB020, &D_800CB05C, &D_800CB098, &D_800CB0D4, &D_800CB110, &D_800CB14C, &D_800CB188, &D_800CB1C4,
    &D_800CB200, &D_800CB23C, &D_800CB278, &D_800CB2B4, &D_800CB2F0, &D_800CB32C, &D_800CB368, &D_800CB3A4,
    &D_800CB3E0, &D_800CB41C, &D_800CB458, &D_800CB494, &D_800CB4D0, &D_800CB50C, &D_800CB548, &D_800CB584,
    &D_800CB5C0, &D_800D05D0, &D_800D060C, &D_800D0648, &D_800C36F4, &D_800D0684, &D_800D06C0, &D_800D06FC,
    &D_800D0738, &D_800D0774, &D_800D07B0, &D_800D07EC, &D_800D0828, &D_800D0864, &D_800D08A0, &D_800D08DC,
    &D_800D0918, &D_800D0954, &D_800D0990, &D_800D09CC, &D_800D0A08, &D_800D0A44, &D_800D0A80, &D_800D0ABC,
    &D_800D0AF8, &D_800D0B34, &D_800D0B70, &D_800D0BAC, &D_800D0BE8, &D_800D0C24, &D_800D0C60, &D_800D0C9C,
    &D_800D0CD8, &D_800D0D14, &D_800D0D50, &D_800C5134, &D_800C5170, &D_800C51AC, &D_800C5224, &D_800C51E8,
    &D_800C5260, &D_800C529C, &D_800C52D8, &D_800C5350, &D_800C5314, &D_800D0D8C, &D_800D0DC8, &D_800D0E04,
    &D_800D0E40, &D_800D0E7C, &D_800D0EB8, &D_800D0EF4, &D_800D0F30, &D_800D0F6C, &D_800D0FA8, &D_800D0FE4,
    &D_800D1020, &D_800D105C, &D_800CB5FC, &D_800CB638, &D_800CB674, &D_800CB6B0, &D_800CB6EC, &D_800CB728,
    &D_800CB764, &D_800CB7A0, &D_800CB7DC, &D_800CB818, &D_800CB854, &D_800CB890, &D_800CB8CC, &D_800D1098,
    &D_800D10D4, &D_800D1110, &D_800D114C, &D_800D1188, &D_800D11C4, &D_800D1200, &D_800CB908, &D_800CB944,
    &D_800CB980, &D_800CB9BC, &D_800CB9F8, &D_800CBA34, &D_800CBA70, &D_800CBAAC, &D_800CBAE8, &D_800CBB24,
    &D_800CBB60, &D_800CBB9C, &D_800CBBD8, &D_800CBC14, &D_800CBC50, &D_800CBC8C, &D_800CBCC8, &D_800CBD04,
    &D_800CBD40, &D_800CBD7C, &D_800CBDB8, &D_800CBDF4, &D_800CBE30, &D_800CBE6C, &D_800CBEA8, &D_800CBEE4,
    &D_800D123C, &D_800D1278, &D_800D12B4, &D_800D12F0, &D_800C5ECC, &D_800C5F08, &D_800C5F44, &D_800C5F80,
    &D_800C5FBC, &D_800C5FF8, &D_800C6034, &D_800D132C, &D_800D1368, &D_800D13A4, &D_800D13E0, &D_800D141C,
    &D_800D1458, &D_800D1494, &D_800D14D0, &D_800D150C, &D_800D1548, &D_800D1584, &D_800D15C0, &D_800D15FC,
    &D_800D1638, &D_800D1674, &D_800D16B0, &D_800D16EC, &D_800D1728, &D_800D1764, &D_800D17A0, &D_800D17DC,
    &D_800D1818, &D_800D1854, &D_800D1890, &D_800D18CC, &D_800D1908, &D_800D1944, &D_800D1980, &D_800D19BC,
    &D_800D19F8, &D_800D1A34, &D_800D1A70, &D_800D1AAC, &D_800D1AE8, &D_800D1B24, &D_800D1B60, &D_800D1B9C,
    &D_800D1BD8, &D_800D1C14, &D_800D1C50, &D_800D1C8C, &D_800D1CC8, &D_800D1D04, &D_800D1D40, &D_800D1D7C,
    &D_800D1DB8, &D_800D1DF4, &D_800D1E30, &D_800D1E6C, &D_800D1EA8, &D_800D1EE4, &D_800D1F20, &D_800D1F5C,
    &D_800D1F98, &D_800D1FD4, &D_800D2010, &D_800D204C, &D_800D2088, &D_800D20C4, &D_800D2100, &D_800D213C,
    &D_800D2178, &D_800D21B4, &D_800D21F0, &D_800D222C, &D_800D2268, &D_800D22A4, &D_800D22E0, &D_800D231C,
    &D_800D2358, &D_800D2394, &D_800D23D0, &D_800D240C, &D_800D2448, &D_800D2484, &D_800D24C0, &D_800D24FC,
    &D_800D2538, &D_800D2574, &D_800D25B0, &D_800D25EC, &D_800D2628, &D_800D2664, &D_800D26A0, &D_800D26DC,
    &D_800D2718, &D_800D2754, &D_800D2790, &D_800D27CC, &D_800D2808, &D_800D2844, &D_800D2880, &D_800D28BC,
    &D_800D28F8, &D_800D2934, &D_800D2970, &D_800D29AC, &D_800C439C, &D_800C43D8, &D_800D29E8, &D_800D2A24,
    &D_800D2A60, &D_800D2A9C, &D_800D2AD8, &D_800D2B14, &D_800D2B50, &D_800D2B8C, &D_800D2BC8, &D_800D2C04,
    &D_800D2C40, &D_800D2C7C, &D_800D2CB8, &D_800D2CF4, &D_800D2D30, &D_800D2D6C, &D_800D2DA8, &D_800D2DE4,
    &D_800D2E20, &D_800D2E5C, &D_800D2E98, &D_800D2ED4, &D_800D2F10, &D_800D2F4C, &D_800D2F88, &D_800D2FC4,
    &D_800D3000, &D_800D303C, &D_800D3078, &D_800D30B4, &D_800D30F0, &D_800D312C, &D_800D3168, &D_800D31A4,
    &D_800D31E0, &D_800D321C, &D_800C6070, &D_800C60AC, &D_800C60E8, &D_800C6124, &D_800C6160, &D_800C619C,
    &D_800C61D8, &D_800C6214, &D_800C6250, &D_800C628C, &D_800C62C8, &D_800C6304, &D_800C6340, &D_800C637C,
    &D_800C63B8, &D_800C63F4, &D_800C6430, &D_800C646C, &D_800C64A8, &D_800C64E4, &D_800C6520, &D_800C655C,
    &D_800C6598, &D_800C65D4, &D_800C6610, &D_800C664C, &D_800C6688, &D_800C66C4, &D_800C6700, &D_800C673C,
    &D_800C67B4, &D_800C6778, &D_800C67F0, &D_800C682C, &D_800D3258, &D_800D3294, &D_800D32D0, &D_800D330C,
    &D_800D3348, &D_800D3384, &D_800D33C0, &D_800D33FC, &D_800D3438, &D_800D3474, &D_800D34B0, &D_800D34EC,
    &D_800D3528, &D_800D3564, &D_800D35A0, &D_800D35DC, &D_800D3618, &D_800D3654, &D_800D3690, &D_800D36CC,
    &D_800D3708, &D_800D3744, &D_800D3780, &D_800D37BC, &D_800D37F8, &D_800D3834, &D_800D3870, &D_800D38AC,
    &D_800D38E8, &D_800D3924, &D_800D3960, &D_800D399C, &D_800D39D8, &D_800D3A14, &D_800D3A50, &D_800D3A8C,
    &D_800D3AC8, &D_800D3B04, &D_800D3B40, &D_800D3B7C, &D_800D3BB8, &D_800D3BF4, &D_800D3C30, &D_800D3C6C,
    &D_800D3CA8, &D_800D3CE4, &D_800D3D20, &D_800D3D5C, &D_800D3D98, &D_800D3DD4, &D_800D3E10, &D_800D3E4C,
    &D_800D3E88, &D_800D3EC4, &D_800D3F00, &D_800D3F3C, &D_800D3F78, &D_800D3FB4, &D_800D3FF0, &D_800D402C,
    &D_800D4068, &D_800D40A4, &D_800D40E0, &D_800D411C, &D_800D4158, &D_800D4194, &D_800D41D0, &D_800D420C,
    &D_800D4248, &D_800D4284, &D_800D42C0, &D_800C88FC, &D_800C8938, &D_800C8974, &D_800C89B0, &D_800C89EC,
    &D_800C8A28, &D_800C8A64, &D_800C8AA0, &D_800C8ADC, &D_800C8B18, &D_800C8B54, &D_800C8B90, &D_800C8BCC,
    &D_800C8C08, &D_800C8C44, &D_800C4720, &D_800C4360, &D_800C4054, &D_800C4090, &D_800C40CC, &D_800C4108,
    &D_800C4144, &D_800C4180, &D_800C41BC, &D_800C41F8, &D_800C4234, &D_800C4270, &D_800C42AC, &D_800C42E8,
    &D_800C4324, &D_800CBF20, &D_800CBF5C, &D_800CBF98, &D_800CBFD4, &D_800CC010, &D_800CC04C, &D_800CC088,
    &D_800CC0C4, &D_800CC100, &D_800CC13C, &D_800CC178, &D_800CC1B4, &D_800CC1F0, &D_800D42FC, &D_800D4338,
    &D_800D4374, &D_800D43B0, &D_800D43EC, &D_800D4428, &D_800D4464, &D_800D44A0, &D_800D44DC, &D_800D4518,
    &D_800D4554, &D_800D4590, &D_800D45CC, &D_800D4608, &D_800D4644, &D_800D4680, &D_800D46BC, &D_800D46F8,
    &D_800D4734, &D_800D4770, &D_800CC22C, &D_800CC268, &D_800CC2A4, &D_800CC2E0, &D_800CC31C, &D_800CC358,
    &D_800CC394, &D_800CC3D0, &D_800CC40C, &D_800CC448, &D_800CC484, &D_800CC4C0, &D_800CC4FC, &D_800D47AC,
    &D_800D47E8, &D_800D4824, &D_800D4860, &D_800D489C, &D_800D48D8, &D_800D4914, &D_800D4950, &D_800D498C,
    &D_800D49C8, &D_800CA030, &D_800CA06C, &D_800D4A04, &D_800D4A40, &D_800D4A7C, &D_800CA0A8, &D_800CA0E4,
    &D_800CA120, &D_800D4AB8, &D_800D4AF4, &D_800CC538, &D_800CC574, &D_800CC5B0, &D_800CC5EC, &D_800CC628,
    &D_800CC664, &D_800CC6A0, &D_800CC6DC, &D_800CC718, &D_800CC754, &D_800CC790, &D_800CC7CC, &D_800CC808,
    &D_800D4B30, &D_800D4B6C, &D_800D4BA8, &D_800D4BE4, &D_800D4C20, &D_800CC844, &D_800CC880, &D_800CC8BC,
    &D_800CC8F8, &D_800CC934, &D_800CC970, &D_800CC9AC, &D_800CC9E8, &D_800CCA24, &D_800CCA60, &D_800CCA9C,
    &D_800CCAD8, &D_800CCB14, &D_800D4C5C, &D_800D4C98, &D_800D4CD4, &D_800D4D10, &D_800D4D4C, &D_800D4D88,
    &D_800D4DC4, &D_800D4E00, &D_800D4E3C, &D_800D4E78, &D_800D4EB4, &D_800D4EF0, &D_800D4F2C, &D_800D4F68,
    &D_800D4FA4, &D_800D4FE0, &D_800D501C, &D_800D5058, &D_800D5094, &D_800D50D0, &D_800D510C, &D_800D5148,
    &D_800D5184, &D_800D51C0, &D_800D51FC, &D_800D5238, &D_800D5274, &D_800D52B0,
};

static _D8D20UnkStruct2 *_blks[] = {
    &D_800D8120, &D_800D8144, &D_800D8168, &D_800D818C, &D_800D81B0, &D_800D81D4, &D_800D81F8, &D_800D821C,
    &D_800D8240, &D_800D8264, &D_800D8288, &D_800D82AC, &D_800D82D0, &D_800D82F4, &D_800D8318, &D_800D833C,
    &D_800D8360, &D_800D8384, &D_800D83A8, &D_800D83CC, &D_800D83F0, &D_800D8414, &D_800D8438, &D_800D845C,
    &D_800D8480, &D_800D84A4, &D_800D84C8, &D_800D84EC, &D_800D8510, &D_800D8534, &D_800D8558, &D_800D857C,
    &D_800D85A0, &D_800D85C4, &D_800D85E8, &D_800D860C, &D_800D8630, &D_800D8654, &D_800D8678, &D_800D869C,
    &D_800D86C0, &D_800D86E4, &D_800D8708, &D_800D872C, &D_800D8750, &D_800D8774, &D_800D8798, &D_800D87BC,
    &D_800D87E0, &D_800D8804, &D_800D8828, &D_800D884C, &D_800D8870, &D_800D8894, &D_800D88B8, &D_800D88DC,
    &D_800D8900, &D_800D8924, &D_800D8948, &D_800D896C, &D_800D8990, &D_800D89B4, &D_800D89D8, &D_800D89FC,
    &D_800D8A20, &D_800D8A44, &D_800D8A68, &D_800D8A8C, &D_800D8AB0, &D_800D8AD4, &D_800D8AF8, &D_800D8B1C,
    &D_800D8B40, &D_800D8B64, &D_800D8B88, &D_800D8BAC, &D_800D8BD0, &D_800D8BF4, &D_800D8C18, &D_800D8C3C,
    &D_800D8C60, &D_800D8C84, &D_800D8CA8, &D_800D8CCC, &D_800D8CF0, &D_800D8D14, &D_800D8D38, &D_800D8D5C,
    &D_800D8D80, &D_800D8DA4, &D_800D8DC8, &D_800D8DEC, &D_800D8E10, &D_800D8E34, &D_800D8E58, &D_800D8E7C,
    &D_800D8EA0, &D_800D8EC4, &D_800D8EE8, &D_800D8F0C, &D_800D8F30, &D_800D8F54, &D_800D8F78, &D_800D8F9C,
    &D_800D8FC0, &D_800D8FE4, &D_800D9008, &D_800D902C, &D_800D9050, &D_800D9074, &D_800D9098, &D_800D90BC,
    &D_800D90E0, &D_800D9104, &D_800D9128, &D_800D914C, &D_800D9170, &D_800D9194, &D_800D91B8, &D_800D91DC,
    &D_800D9200, &D_800D9224, &D_800D9248, &D_800D926C, &D_800D9290, &D_800D92B4, &D_800D92D8, &D_800D92FC,
    &D_800D9320, &D_800D9344, &D_800D9368, &D_800D938C, &D_800D93B0, &D_800D93D4, &D_800D93F8, &D_800D941C,
    &D_800D9440, &D_800D9464, &D_800D9488, &D_800D94AC, &D_800D94D0, &D_800D94F4, &D_800D9518, &D_800D953C,
    &D_800D9560, &D_800D9584, &D_800D95A8, &D_800D95CC, &D_800D95F0, &D_800D9614, &D_800D9638, &D_800D965C,
    &D_800D9680, &D_800D96A4, &D_800D96C8, &D_800D96EC, &D_800D9710, &D_800D9734, &D_800D9758, &D_800D977C,
    &D_800D97A0, &D_800D97C4, &D_800D97E8, &D_800D980C, &D_800D9830, &D_800D9854, &D_800D9878, &D_800D989C,
    &D_800D98C0, &D_800D98E4, &D_800D9908, &D_800D992C, &D_800D9950, &D_800D9974, &D_800D9998, &D_800D99BC,
    &D_800D99E0, &D_800D9A04, &D_800D9A28, &D_800D9A4C, &D_800D9A70, &D_800D9A94, &D_800D9AB8, &D_800D9ADC,
    &D_800D9B00, &D_800D9B24, &D_800D9B48, &D_800D9B6C,
};

MusicInfo *_musics[][4] = {
    {&gMusic.music[MUSIC_WESTERN1], &gMusic.music[MUSIC_WESTERN2], NULL, NULL},
    {&gMusic.music[MUSIC_WESTERN_MINE_CART], NULL, NULL, NULL},
    {&gMusic.music[MUSIC_PRESENT_DAY2], &gMusic.music[MUSIC_PRESENT_DAY1], &gMusic.music[MUSIC_TITLE_SCREEN], &gMusic.music[MUSIC_PRESENT_DAY_BOSS_HOG]},
    {&gMusic.music[MUSIC_VICTORIAN1], &gMusic.music[MUSIC_VICTORIAN2], NULL, NULL},
    {&gMusic.music[MUSIC_BOSS], NULL, NULL, NULL},
    {&gMusic.music[MUSIC_FINAL_ALIEN_MOTHER], NULL, NULL, NULL},
    {&gMusic.ambient[MUSIC_AMBIENT1], NULL, NULL, NULL},
    {&gMusic.ambient[MUSIC_AMBIENT2], NULL, NULL, NULL},
    {&gMusic.ambient[MUSIC_AMBIENT3], NULL, NULL, NULL},
    {&gMusic.ambient[MUSIC_AMBIENT4], NULL, NULL, NULL},
    {&gMusic.ambient[MUSIC_AMBIENT5], NULL, NULL, NULL},
    {&gMusic.ambient[MUSIC_AMBIENT6], NULL, NULL, NULL},
    {&gMusic.ambient[MUSIC_AMBIENT7], NULL, NULL, NULL},
    {&gMusic.ambient[MUSIC_AMBIENT8], NULL, NULL, NULL},
    {&gMusic.ambient[MUSIC_AMBIENT9], NULL, NULL, NULL},
};

MapInfo *_maps[] = {
    &gMapInfo[MAP_MEAN_STREETS],
    &gMapInfo[MAP_LIBERTY_OR_DEATH],
    &gMapInfo[MAP_NUCLEAR_WINTER],
    &gMapInfo[MAP_FALLOUT],
    &gMapInfo[MAP_UNDER_SIEGE],
    &gMapInfo[MAP_BOSS_HOG],
    &gMapInfo[MAP_WETWORLD],
    &gMapInfo[MAP_DRY_TOWN],
    &gMapInfo[MAP_JAIL_BREAK],
    &gMapInfo[MAP_UP_SHIP_CREEK],
    &gMapInfo[MAP_PROBING_THE_DEPTHS],
    &gMapInfo[MAP_FORT_ROSWELL],
    &gMapInfo[MAP_CYBORG_SCORPION],
    &gMapInfo[MAP_THE_WHITECHAPEL_KILLINGS],
    &gMapInfo[MAP_DAWN_OF_THE_DUKE],
    &gMapInfo[MAP_HYDROGEN_BOMB],
    &gMapInfo[MAP_GOING_DOWN_THE_RACK],
    &gMapInfo[MAP_THE_RACK],
    &gMapInfo[MAP_BRAINSTORM],
    &gMapInfo[MAP_THE_BROTHERS_NUKEM],
    &gMapInfo[MAP_ALIEN_MOTHER],
    &gMapInfo[MAP_ZERO_HOUR],
    &gMapInfo[MAP_THE_END],
    &gMapInfo[MAP_COOL_AS_ICE],
    &gMapInfo[MAP_MIND_FUNK],
    &gMapInfo[MAP_CHIMERA],
    &gMapInfo[MAP_SLIDERS],
    &gMapInfo[MAP_ATOMIC],
    &gMapInfo[MAP_HIGH_NOON],
    &gMapInfo[MAP_ROCKY_KNOLL],
    &gMapInfo[MAP_CASHING_IN],
    &gMapInfo[MAP_HIVE],
    &gMapInfo[MAP_CASTLEMANIA],
    &gMapInfo[MAP_TOXIC_SHOCK],
    &gMapInfo[MAP_FOGGING_HELL],
    &gMapInfo[MAP_CATACOMBS],
    &gMapInfo[MAP_INFERNO],
    &gMapInfo[MAP_BASE],
};

static void load_model(ModelInfo *info, FILE *fp)
{
    s32 i, size, count;
    code0UnkStruct18 *ptr;
    ModelTextureInfo *texinfo;

    if (info->vertex_info == NULL)
        return;

    fread(&size, 4, 1, fp);
    fread(_pBuffer, size, 1, fp);

    assert(info->unk8 == info->unkA);
    assert(info->vertex_info->size == info->vertex_size);

    ptr = (code0UnkStruct18 *)((intptr_t)_pBuffer + ((info->unk8 + 3) & ~3));
    assert((info->vertex_info->size % sizeof(code0UnkStruct18)) == 0);
    count = (info->vertex_info->size / sizeof(code0UnkStruct18));
    for (i = 0; i < count; i++)
    {
        ptr->x = SWAP_S16(ptr->x);
        ptr->y = SWAP_S16(ptr->y);
        ptr->z = SWAP_S16(ptr->z);
        ptr++;
    }

    assert(((info->cmd_off - info->texture_info_off) % sizeof(ModelTextureInfo)) == 0);
    count = ((info->cmd_off - info->texture_info_off) / sizeof(ModelTextureInfo));
    texinfo = (ModelTextureInfo *)((intptr_t)_pBuffer + info->texture_info_off);
    for (i = 0; i < count; i++)
    {
        texinfo->dimx = SWAP_S16(texinfo->dimx);
        texinfo->dimy = SWAP_S16(texinfo->dimy);
        texinfo->offset = SWAP_S32(texinfo->offset);
        texinfo++;
    }
    info->fileoff = 0;
    info->vertex_info->fileoff = 0;
    info->ramaddr = _pBuffer;
    info->vertex_info->ramaddr = (u8 *)((((intptr_t)info->ramaddr + info->unk8) + 3) & ~3);
    _pBuffer += size;
    ALIGN16(_pBuffer);
}

static void load_models(FILE *fp)
{
    s32 i;

    assert(sizeof(code0UnkStruct18) == 0x06);
    assert(sizeof(ModelLight) == 0x06);
    assert(sizeof(ModelTextureInfo) == 0x08);

    assert(offsetof(code0UnkStruct18, x) == 0x00);
    assert(offsetof(code0UnkStruct18, y) == 0x02);
    assert(offsetof(code0UnkStruct18, z) == 0x04);

    assert(offsetof(ModelLight, n) == 0x00);

    assert(offsetof(ModelTextureInfo, dimx) == 0x00);
    assert(offsetof(ModelTextureInfo, dimy) == 0x02);
    assert(offsetof(ModelTextureInfo, offset) == 0x04);

    for (i = 0; i < ARRAY_COUNT(_models); i++)
        load_model(_models[i], fp);
}

static void load_blk(_D8D20UnkStruct2 *blk, FILE *fp)
{
    s32 i, size, count;
    _D8D20UnkStruct3 *ptr;
    _D8D20UnkStruct1 *ptr2;

    fread(&size, 4, 1, fp);
    fread(_pBuffer, size, 1, fp);

    ptr = (_D8D20UnkStruct3 *)_pBuffer;
    for (i = 0; i < blk->unk14; i++)
    {
        ptr->unk0 = SWAP_S16(ptr->unk0);
        ptr->unk2 = SWAP_S16(ptr->unk2);
        ptr->unk4 = SWAP_S16(ptr->unk4);
        ptr++;
    }
    assert(((blk->unk10-(blk->unk14*sizeof(_D8D20UnkStruct3))) % sizeof(_D8D20UnkStruct1)) == 0);
    count = (blk->unk10-(blk->unk14*sizeof(_D8D20UnkStruct3))) / sizeof(_D8D20UnkStruct1);

    ptr2 = &((_D8D20UnkStruct1 *)_pBuffer)[blk->unk14];
    assert((intptr_t)ptr == (intptr_t)ptr2);
    for (i = 0; i < count; i++)
    {
        ptr2->unk0 = SWAP_U16(ptr2->unk0);
        ptr2->unk2 = SWAP_U16(ptr2->unk2);
        ptr2->unk4 = SWAP_U16(ptr2->unk4);
        ptr2++;
    }
    blk->fileoff = 0;
    blk->ramaddr = _pBuffer;
    blk->unk18 = (_D8D20UnkStruct3 *)blk->ramaddr;
    blk->unk1C = &((_D8D20UnkStruct1 *)blk->ramaddr)[blk->unk14];
    _pBuffer += size;
    ALIGN16(_pBuffer);
}

static void load_blks(FILE *fp)
{
    s32 i;

    assert(sizeof(_D8D20UnkStruct3) == 0x6);
    assert(sizeof(_D8D20UnkStruct1) == 0x6);

    assert(offsetof(_D8D20UnkStruct3, unk0) == 0x00);
    assert(offsetof(_D8D20UnkStruct3, unk2) == 0x02);
    assert(offsetof(_D8D20UnkStruct3, unk4) == 0x04);

    assert(offsetof(_D8D20UnkStruct1, unk0) == 0x00);
    assert(offsetof(_D8D20UnkStruct1, unk2) == 0x02);
    assert(offsetof(_D8D20UnkStruct1, unk4) == 0x04);

    for (i = 0; i < ARRAY_COUNT(_blks); i++)
        load_blk(_blks[i], fp);
}

static void load_map(MapInfo *map, FILE *fp)
{
    s32 i, count, size;
    Vertex *vertex;
    SectorType *sector;
    WallType *wall;
    SpriteType *sprite;
    u8 *start;

    fread(&size, 4, 1, fp);
    vertex = malloc(size);
    assert(vertex != NULL);
    fread(vertex, size, 1, fp);

    fread(&size, 4, 1, fp);
    wall = malloc(size);
    assert(wall != NULL);
    fread(wall, size, 1, fp);

    fread(&size, 4, 1, fp);
    sector = malloc(size);
    assert(sector != NULL);
    fread(sector, size, 1, fp);

    fread(&size, 4, 1, fp);
    sprite = malloc(size);
    assert(sprite != NULL);
    fread(sprite, size, 1, fp);

    start = _pBuffer;
    assert(decompressEDL(sector, _sectors) == 0);
    free(sector);

    for (i = 0; i < map->sectors; i++)
    {
        _sectors[i].ceilingz = SWAP_S32(_sectors[i].ceilingz);
        _sectors[i].floorz = SWAP_S32(_sectors[i].floorz);
        _sectors[i].wallptr = SWAP_S16(_sectors[i].wallptr);
        _sectors[i].wallnum = SWAP_S16(_sectors[i].wallnum);
        _sectors[i].ceilingstat = SWAP_S16(_sectors[i].ceilingstat);
        _sectors[i].floorstat = SWAP_S16(_sectors[i].floorstat);
        _sectors[i].ceilingpicnum = SWAP_S16(_sectors[i].ceilingpicnum);
        _sectors[i].ceilingheinum = SWAP_S16(_sectors[i].ceilingheinum);
        _sectors[i].floorpicnum = SWAP_S16(_sectors[i].floorpicnum);
        _sectors[i].floorheinum = SWAP_S16(_sectors[i].floorheinum);
        _sectors[i].unk18 = SWAP_S16(_sectors[i].unk18);
        _sectors[i].unk1A = SWAP_S16(_sectors[i].unk1A);
        _sectors[i].unk1C = SWAP_S16(_sectors[i].unk1C);
        _sectors[i].floorvtxptr = SWAP_U16(_sectors[i].floorvtxptr);
        _sectors[i].ceilingvtxptr = SWAP_U16(_sectors[i].ceilingvtxptr);
    }

    count = 0;
    for (i = 0; i < map->sectors; i++)
    {
        count += _sectors[i].floorvtxnum;
        count += _sectors[i].ceilingvtxnum;
    }
    count *= 3;

    assert(decompressEDL(vertex, _pBuffer) == 0);
    free(vertex);
    vertex = (Vertex *)_pBuffer;
    _pBuffer += (count * sizeof(Vertex));
    ALIGN16(_pBuffer);

    for (i = 0; i < count; i++)
    {
        vertex[i].v.ob[0] = SWAP_S16(vertex[i].v.ob[0]);
        vertex[i].v.ob[1] = SWAP_S16(vertex[i].v.ob[1]);
        vertex[i].v.ob[2] = SWAP_S16(vertex[i].v.ob[2]);
        vertex[i].v.tc[0] = SWAP_S16(vertex[i].v.tc[0]);
        vertex[i].v.tc[1] = SWAP_S16(vertex[i].v.tc[1]);
    }

    map->wall_offset = _pBuffer - start;
    assert(decompressEDL(wall, _pBuffer) == 0);
    free(wall);
    wall = (WallType *)_pBuffer;
    _pBuffer += (map->walls * sizeof(WallType));
    ALIGN16(_pBuffer);

    for (i = 0; i < map->walls; i++)
    {
        wall[i].x = SWAP_S32(wall[i].x);
        wall[i].y = SWAP_S32(wall[i].y);
        wall[i].point2 = SWAP_S16(wall[i].point2);
        wall[i].nextwall = SWAP_S16(wall[i].nextwall);
        wall[i].nextsector = SWAP_S16(wall[i].nextsector);
        wall[i].cstat = SWAP_S16(wall[i].cstat);
        wall[i].picnum = SWAP_S16(wall[i].picnum);
        wall[i].overpicnum = SWAP_S16(wall[i].overpicnum);
        wall[i].unk14 = SWAP_S16(wall[i].unk14);
        wall[i].unk16 = SWAP_S16(wall[i].unk16);
        wall[i].unk18 = SWAP_S16(wall[i].unk18);
        wall[i].sectnum = SWAP_U16(wall[i].sectnum);
    }

    map->sector_offset = _pBuffer - start;
    memcpy(_pBuffer, _sectors, map->sectors*sizeof(SectorType));
    _pBuffer += (map->sectors * sizeof(SectorType));
    ALIGN16(_pBuffer);

    map->sprite_offset = _pBuffer - start;
    assert(decompressEDL(sprite, _pBuffer) == 0);
    free(sprite);
    sprite = (SpriteType *)_pBuffer;
    _pBuffer += (map->sprites * sizeof(SpriteType));
    ALIGN16(_pBuffer);

    for (i = 0; i < map->sprites; i++)
    {
        sprite[i].x = SWAP_S32(sprite[i].x);
        sprite[i].y = SWAP_S32(sprite[i].y);
        sprite[i].z = SWAP_S32(sprite[i].z);
        sprite[i].cstat = SWAP_S16(sprite[i].cstat);
        sprite[i].picnum = SWAP_S16(sprite[i].picnum);
        sprite[i].sectnum = SWAP_S16(sprite[i].sectnum);
        sprite[i].statnum = SWAP_S16(sprite[i].statnum);
        sprite[i].ang = SWAP_S16(sprite[i].ang);
        sprite[i].unk16 = SWAP_S16(sprite[i].unk16);
        sprite[i].unk18 = SWAP_S16(sprite[i].unk18);
        sprite[i].unk1A = SWAP_S16(sprite[i].unk1A);
        sprite[i].unk1C = SWAP_S16(sprite[i].unk1C);
        sprite[i].lotag = SWAP_S16(sprite[i].lotag);
        sprite[i].hitag = SWAP_S16(sprite[i].hitag);
        sprite[i].unk22 = SWAP_S16(sprite[i].unk22);
    }

    assert(map->skybottom_r >= 0.0f && map->skybottom_r < 256.f);
    assert(map->skybottom_g >= 0.0f && map->skybottom_g < 256.f);
    assert(map->skybottom_b >= 0.0f && map->skybottom_b < 256.f);

    assert(map->skytop_r >= 0.0f && map->skytop_r < 256.f);
    assert(map->skytop_g >= 0.0f && map->skytop_g < 256.f);
    assert(map->skytop_b >= 0.0f && map->skytop_b < 256.f);
    
    map->rom_start = start;
    map->rom_end = _pBuffer;
}

static void load_maps(FILE *fp)
{
    s32 i;

    assert(sizeof(Vertex) == 0x0E);
    assert(sizeof(SectorType) == 0x30);
    assert(sizeof(WallType) == 0x28);
    assert(sizeof(SpriteType) == 0x2C);

    assert(offsetof(VertexN, ob) == 0x00);
    assert(offsetof(VertexN, tc) == 0x06);
    assert(offsetof(VertexN, n) == 0x0A);
    assert(offsetof(VertexN, a) == 0x0D);

    assert(offsetof(VertexV, ob) == 0x00);
    assert(offsetof(VertexV, tc) == 0x06);
    assert(offsetof(VertexV, cn) == 0x0A);

    assert(offsetof(SectorType, ceilingz) == 0x00);
    assert(offsetof(SectorType, floorz) == 0x04);
    assert(offsetof(SectorType, wallptr) == 0x08);
    assert(offsetof(SectorType, wallnum) == 0x0A);
    assert(offsetof(SectorType, ceilingstat) == 0x0C);
    assert(offsetof(SectorType, floorstat) == 0x0E);
    assert(offsetof(SectorType, ceilingpicnum) == 0x10);
    assert(offsetof(SectorType, ceilingheinum) == 0x12);
    assert(offsetof(SectorType, floorpicnum) == 0x14);
    assert(offsetof(SectorType, floorheinum) == 0x16);
    assert(offsetof(SectorType, unk18) == 0x18);
    assert(offsetof(SectorType, unk1A) == 0x1A);
    assert(offsetof(SectorType, unk1C) == 0x1C);
    assert(offsetof(SectorType, floorvtxptr) == 0x1E);
    assert(offsetof(SectorType, ceilingvtxptr) == 0x20);
    assert(offsetof(SectorType, ceilingshade) == 0x22);
    assert(offsetof(SectorType, ceilingpal) == 0x23);
    assert(offsetof(SectorType, pad) == 0x24);
    assert(offsetof(SectorType, floorshade) == 0x26);
    assert(offsetof(SectorType, floorpal) == 0x27);
    assert(offsetof(SectorType, pad2) == 0x28);
    assert(offsetof(SectorType, unk2A) == 0x2A);
    assert(offsetof(SectorType, floorvtxnum) == 0x2B);
    assert(offsetof(SectorType, ceilingvtxnum) == 0x2C);
    assert(offsetof(SectorType, pad3) == 0x2D);

    assert(offsetof(WallType, x) == 0x00);
    assert(offsetof(WallType, y) == 0x04);
    assert(offsetof(WallType, point2) == 0x08);
    assert(offsetof(WallType, nextwall) == 0x0A);
    assert(offsetof(WallType, nextsector) == 0x0C);
    assert(offsetof(WallType, cstat) == 0x0E);
    assert(offsetof(WallType, picnum) == 0x10);
    assert(offsetof(WallType, overpicnum) == 0x12);
    assert(offsetof(WallType, unk14) == 0x14);
    assert(offsetof(WallType, unk16) == 0x16);
    assert(offsetof(WallType, unk18) == 0x18);
    assert(offsetof(WallType, sectnum) == 0x1A);
    assert(offsetof(WallType, shade) == 0x1C);
    assert(offsetof(WallType, unk1D) == 0x1D);
    assert(offsetof(WallType, unk1E) == 0x1E);
    assert(offsetof(WallType, unk1F) == 0x1F);
    assert(offsetof(WallType, unk20) == 0x20);
    assert(offsetof(WallType, pal) == 0x21);
    assert(offsetof(WallType, xrepeat) == 0x22);
    assert(offsetof(WallType, yrepeat) == 0x23);
    assert(offsetof(WallType, xpanning) == 0x24);
    assert(offsetof(WallType, ypanning) == 0x25);
    assert(offsetof(WallType, pad3) == 0x26);

    assert(offsetof(SpriteType, x) == 0x00);
    assert(offsetof(SpriteType, y) == 0x04);
    assert(offsetof(SpriteType, z) == 0x08);
    assert(offsetof(SpriteType, cstat) == 0x0C);
    assert(offsetof(SpriteType, picnum) == 0x0E);
    assert(offsetof(SpriteType, sectnum) == 0x10);
    assert(offsetof(SpriteType, statnum) == 0x12);
    assert(offsetof(SpriteType, ang) == 0x14);
    assert(offsetof(SpriteType, unk16) == 0x16);
    assert(offsetof(SpriteType, unk18) == 0x18);
    assert(offsetof(SpriteType, unk1A) == 0x1A);
    assert(offsetof(SpriteType, unk1C) == 0x1C);
    assert(offsetof(SpriteType, lotag) == 0x1E);
    assert(offsetof(SpriteType, hitag) == 0x20);
    assert(offsetof(SpriteType, unk22) == 0x22);
    assert(offsetof(SpriteType, unk24) == 0x24);
    assert(offsetof(SpriteType, unk25) == 0x25);
    assert(offsetof(SpriteType, clipdist) == 0x26);
    assert(offsetof(SpriteType, xrepeat) == 0x27);
    assert(offsetof(SpriteType, yrepeat) == 0x28);
    assert(offsetof(SpriteType, unk29) == 0x29);
    assert(offsetof(SpriteType, unk2A) == 0x2A);
    assert(offsetof(SpriteType, unk2B) == 0x2B);

    for (i = 0; i < ARRAY_COUNT(_maps); i++)
        load_map(_maps[i], fp);
}

static void load_tile(TileInfo *tile, FILE *fp)
{
    s32 i, dsize, size;
    u8 *ptr;
    u8 val;
    u8 buf[3];

    fread(&size, 4, 1, fp);
    fread(buf, 3, 1, fp);

    tile->fileoff = 0;
    tile->ramaddr = _pBuffer;
    if (buf[0] == 'E' && buf[1] == 'D' && buf[2] == 'L')
    {
        ptr = malloc(size);
        assert(ptr != NULL);
        memcpy(ptr, buf, 3);
        fread(&ptr[3], size-3, 1, fp);
        dsize = SWAP_S32(*(u32 *)&ptr[8]);
        assert(decompressEDL(ptr, _pBuffer) == 0);
        _pBuffer += dsize;
        tile->filesize = dsize;
        free(ptr);
    }
    else
    {
        memcpy(_pBuffer, buf, 3);
        fread(&_pBuffer[3], size-3, 1, fp);
        _pBuffer += size;
        tile->filesize = size;
    }

    if (!(tile->flags & 0x80))
    {
        ptr = tile->ramaddr;
        for (i = 0; i < 16; i++)
        {
            val = *ptr;
            *ptr = *(ptr + 1);
            *(ptr + 1) = val;
            ptr += 2;
        }
    }
    ALIGN16(_pBuffer);
}

static void load_tiles(FILE *fp)
{
    s32 i;

    for (i = 0; i < ARRAY_COUNT(gTileInfo); i++)
        load_tile(&gTileInfo[i], fp);
}

static void swap_odd_lines(void *data, u32 numBytes, u32 bytesPerRow)
{
    u32 i;
    u32 tmp;
    u32 *data32 = (u32 *)data;
    u32 wordsPerRow = bytesPerRow / 4;
    u32 numWords = numBytes / 4;
    for (i = 0; i < numWords / 2; i++)
    {
        if ((2 * i) / wordsPerRow % 2 == 1)
        {
            tmp = data32[2 * i + 0];
            data32[2 * i + 0] = data32[2 * i + 1];
            data32[2 * i + 1] = tmp;
        }
    }
}

static void func_8007FD8C(u8 *data, _E0640UnkStruct *arg0)
{
    for (; (intptr_t)arg0->texture != -1; arg0++)
    {
        swap_odd_lines((void *)((intptr_t)data+arg0->texoff), (arg0->height*arg0->width), arg0->width);
    }
}

static void load_swapped_texture(Art *art, FILE *fp)
{
    s32 size, dsize;
    u8 *ptr;

    fread(&size, 4, 1, fp);
    ptr = malloc(size);
    assert(ptr != NULL);
    fread(ptr, size, 1, fp);

    dsize = SWAP_S32(*(u32 *)&ptr[8]);
    assert(decompressEDL(ptr, art->data) == 0);
    free(ptr);

    func_8007FD8C(art->data, art->info);
    _pBuffer += dsize;
    ALIGN16(_pBuffer);
}

static void load_special_model(Art *art, FILE *fp)
{
    s32 i, j, size, dsize;
    u8 *ptr;
    s16 *ptr2;
    s32 *offset;
    code0UnkStruct18 *ptr3;

    fread(&size, 4, 1, fp);
    ptr = malloc(size);
    assert(ptr != NULL);
    fread(ptr, size, 1, fp);

    dsize = SWAP_S32(*(u32 *)&ptr[8]);
    assert(decompressEDL(ptr, art->data) == 0);
    free(ptr);

    offset = art->edl->offset;

    for (i = 0; i < art->offset_count; i++)
    {
        ptr2 = (s16 *)&art->data[offset[i]];
        size = SWAP_S16(*ptr2);
        ptr3 = (code0UnkStruct18 *)&ptr2[1];
        for (j = 0; j < size; j++)
        {
            ptr3->x = SWAP_S16(ptr3->x);
            ptr3->y = SWAP_S16(ptr3->y);
            ptr3->z = SWAP_S16(ptr3->z);
            ptr3++;
        }
    }
    _pBuffer += dsize;
    ALIGN16(_pBuffer);
}

static void load_pbank(FILE *fp)
{
    s32 i, size, length;
    ptr_bank_t *ptrfile_addr;
    u8 *wave_list, *basenote, *detune;
    u8 *wave, *loop, *book;
    u8 *ptr, *start;
    ALWaveTable* _wave;

    fread(&size, 4, 1, fp);
    start = ptr = malloc(size);
    assert(ptr != NULL);
    fread(ptr, size, 1, fp);

    ptrfile_addr = (ptr_bank_t *)_pBuffer;
    _pBuffer += sizeof(ptr_bank_t);
    ALIGN16(_pBuffer);

    memcpy(ptrfile_addr->header_name, ptr, 16); ptr+=16;
    ptrfile_addr->flags = READ_U32(ptr);
    assert(ptrfile_addr->flags == 0);
    memcpy(ptrfile_addr->wbk_name, ptr, 12); ptr+=12;
    ptrfile_addr->count = READ_S32(ptr);
    basenote = start + READ_S32(ptr);
    detune = start + READ_S32(ptr);
    wave_list = start + READ_S32(ptr);

    assert((ptrfile_addr->count*4) == (basenote - wave_list));
    assert(wave_list < basenote);
    assert(basenote < detune);

    /*Allocate space for wave_list*/
    ptrfile_addr->wave_list = (ALWaveTable **)_pBuffer;
    _pBuffer += (ptrfile_addr->count*sizeof(ALWaveTable	*));
    ALIGN16(_pBuffer);

    /*Copy basenote*/
    ptrfile_addr->basenote = (unsigned char	*)_pBuffer;
    length = detune - basenote;
    memcpy(ptrfile_addr->basenote, basenote, length);
    _pBuffer += length;
    ALIGN16(_pBuffer);

    /*Copy detune*/
    ptrfile_addr->detune = (float *)_pBuffer;
    length = &start[size] - detune;
    memcpy(ptrfile_addr->detune, detune, length);
    _pBuffer += length;
    ALIGN16(_pBuffer);

    for (i = 0; i < ptrfile_addr->count; i++)
    {
        _wave = ptrfile_addr->wave_list[i] = (ALWaveTable *)_pBuffer;
        _pBuffer += sizeof(ALWaveTable);
        ALIGN16(_pBuffer);
        wave = start + READ_S32(wave_list);

        _wave->base = READ_S32(wave); /*offset in wbank*/
        _wave->len = READ_S32(wave);
        _wave->type = *wave++;
        _wave->flags = *wave++;
        wave += 2; /*padding*/
        assert(_wave->type == AL_ADPCM_WAVE);
        assert(_wave->flags == 0);
        loop = start + READ_S32(wave);
        book = start + READ_S32(wave);
        assert(book != 0); /*ALRAWWaveInfo?*/

        if (loop != start)
        {
            assert(book < loop);
            _wave->waveInfo.adpcmWave.loop = (ALADPCMloop *)_pBuffer;
            _pBuffer += sizeof(ALADPCMloop);
            ALIGN16(_pBuffer);

            _wave->waveInfo.adpcmWave.loop->start = READ_U32(loop);
            _wave->waveInfo.adpcmWave.loop->end = READ_U32(loop);
            _wave->waveInfo.adpcmWave.loop->count = READ_U32(loop);

            for (int j = 0; j < 16; j++)
            {
                _wave->waveInfo.adpcmWave.loop->state[j] = READ_S16(loop);
            }
            BASE_OFFSET(ptrfile_addr, wave_list[i]->waveInfo.adpcmWave.loop);
        }
        if (_wave->type == AL_ADPCM_WAVE)
        {
            _wave->waveInfo.adpcmWave.book = (ALADPCMBook *)_pBuffer;
            _pBuffer += sizeof(ALADPCMBook);
            ALIGN16(_pBuffer);

            _wave->waveInfo.adpcmWave.book->order = READ_U32(book);
            assert(_wave->waveInfo.adpcmWave.book->order == 2);
            _wave->waveInfo.adpcmWave.book->npredictors = READ_U32(book);
            assert(_wave->waveInfo.adpcmWave.book->npredictors == 4);

            for (int j = 0; j < 64; j++)
            {
                _wave->waveInfo.adpcmWave.book->book[j] = READ_S16(book);
            }
            BASE_OFFSET(ptrfile_addr, wave_list[i]->waveInfo.adpcmWave.book);
        }
        BASE_OFFSET(ptrfile_addr, wave_list[i]);
    }

    BASE_OFFSET(ptrfile_addr, wave_list);
    BASE_OFFSET(ptrfile_addr, basenote);
    BASE_OFFSET(ptrfile_addr, detune);

    free(start);
}

static void load_song(MusicInfo *info, FILE *fp)
{
    s32 i, size, length;
    song_t *song_addr;
    u8 *data_list, *volume_list, *pbend_list, *env_table,
       *drum_table, *wave_table, *master_track;
    u8 *ptr, *start, *data;

    fread(&size, 4, 1, fp);
    start = ptr = malloc(size);
    assert(ptr != NULL);
    fread(ptr, size, 1, fp);

    song_addr = (song_t *)_pBuffer;
    _pBuffer += sizeof(song_t);
    ALIGN16(_pBuffer);

    song_addr->version = READ_U32(ptr);
    song_addr->num_channels = READ_S32(ptr);
    song_addr->num_waves = READ_S32(ptr);
    data_list = start + READ_S32(ptr);
    volume_list = start + READ_S32(ptr);
    pbend_list = start + READ_S32(ptr);
    env_table = start + READ_S32(ptr);
    drum_table = start + READ_S32(ptr);
    wave_table = start + READ_S32(ptr);
    master_track = start + READ_S32(ptr);
    song_addr->flags = READ_U32(ptr);
    song_addr->reserved1 = READ_U32(ptr);
    song_addr->reserved2 = READ_U32(ptr);
    song_addr->reserved3 = READ_U32(ptr);

    assert(song_addr->flags == 0);
    assert(drum_table == env_table);

    assert(data_list < volume_list);
    assert(volume_list < pbend_list);
    assert(pbend_list < wave_table);
    assert(wave_table < env_table);
    assert(env_table < master_track);
    assert((song_addr->num_channels*4) == (volume_list - data_list));
    assert((song_addr->num_channels*4) == (pbend_list - volume_list));
    assert((song_addr->num_channels*4) == (wave_table - pbend_list));

    /*Allocate space for data_list*/
    song_addr->data_list = (unsigned char **)_pBuffer;
    _pBuffer += (song_addr->num_channels*sizeof(unsigned char **));
    ALIGN16(_pBuffer);

    /*Allocate space for volume_list*/
    song_addr->volume_list = (unsigned char **)_pBuffer;
    _pBuffer += (song_addr->num_channels*sizeof(unsigned char **));
    ALIGN16(_pBuffer);

    /*Allocate space for pbend_list*/
    song_addr->pbend_list = (unsigned char **)_pBuffer;
    _pBuffer += (song_addr->num_channels*sizeof(unsigned char **));
    ALIGN16(_pBuffer);

    /*Allocate space for wave_table*/
    song_addr->wave_table = (unsigned short	*)_pBuffer;
    _pBuffer += (song_addr->num_waves*sizeof(unsigned short));
    ALIGN16(_pBuffer);

    /*Copy wave_table*/
    for (i = 0; i < song_addr->num_waves; i++)
    {
        song_addr->wave_table[i] = READ_U16(wave_table);
    }

    /*Copy env_table/drum_table*/
    song_addr->env_table = (unsigned char *)_pBuffer;
    song_addr->drum_table = (drum_t*)_pBuffer;
    length = master_track - env_table;
    memcpy(song_addr->env_table, env_table, length);
    _pBuffer += length;
    ALIGN16(_pBuffer);

    /*Copy master_track*/
    song_addr->master_track = (unsigned char *)_pBuffer;
    length = &start[size] - master_track;
    memcpy(song_addr->master_track, master_track, length);
    _pBuffer += length;
    ALIGN16(_pBuffer);

    /*Copy data_list*/
    for (i = 0; i < song_addr->num_channels; i++)
    {
        data = start + READ_S32(data_list);
        if(data != start)
        {
            assert((data >= master_track) && (data <= &start[size]));
            song_addr->data_list[i] = song_addr->master_track + (data - master_track);
            BASE_OFFSET(song_addr, data_list[i]);
        }
    }

    /*Copy volume_list*/
    for (i = 0; i < song_addr->num_channels; i++)
    {
        data = start + READ_S32(volume_list);
        if(data != start)
        {
            assert((data >= env_table) && (data <= master_track));
            song_addr->volume_list[i] = song_addr->env_table + (data - env_table);
            BASE_OFFSET(song_addr, volume_list[i]);
        }
    }

    /*Copy pbend_list*/
    for (i = 0; i < song_addr->num_channels; i++)
    {
        data = start + READ_S32(pbend_list);
        if(data != start)
        {
            assert((data >= env_table) && (data <= master_track));
            song_addr->pbend_list[i] = song_addr->env_table + (data - env_table);
            BASE_OFFSET(song_addr, pbend_list[i]);
        }
    }

    BASE_OFFSET(song_addr, master_track);
    BASE_OFFSET(song_addr, wave_table);
    BASE_OFFSET(song_addr, drum_table);
    BASE_OFFSET(song_addr, env_table);
    BASE_OFFSET(song_addr, pbend_list);
    BASE_OFFSET(song_addr, volume_list);
    BASE_OFFSET(song_addr, data_list);

    info->music_start = (u8 *)song_addr;
    info->music_end = _pBuffer;

    free(start);
}

static void load_sfx(FILE *fp)
{
    s32 i, size, length;
    fx_header_t *header;
    u8 *ptr_addr, *wave_table, *effects;
    unsigned char *_effects, *fxdata;
    u8 *ptr, *start;

    fread(&size, 4, 1, fp);
    start = ptr = malloc(size);
    assert(ptr != NULL);
    fread(ptr, size, 1, fp);

    header = (fx_header_t *)_pBuffer;

    header->number_of_components = READ_S32(ptr);
    header->number_of_effects = READ_S32(ptr);
    header->num_waves = READ_S32(ptr);
    header->flags = READ_U32(ptr);
    ptr_addr = start + READ_S32(ptr);
    wave_table = start + READ_S32(ptr);
    assert(header->flags == 0);
    assert(ptr_addr == start);

    _pBuffer += sizeof(fx_header_t) - MEMBER_SIZE(fx_header_t, effects);
    _pBuffer += (header->number_of_components*sizeof(fx_t));
    ALIGN16(_pBuffer);

    /*Copy effects*/
    _effects = (unsigned char *)_pBuffer;
    effects = start + (24+header->number_of_components*8); /*effects base addr*/
    assert(effects < wave_table);
    length = wave_table - effects;
    memcpy(_pBuffer, effects, length);
    _pBuffer += length;
    ALIGN16(_pBuffer);

    /*Allocate space for wave_table*/
    header->wave_table = (unsigned short *)_pBuffer;
    _pBuffer += (header->num_waves*sizeof(unsigned short));
    ALIGN16(_pBuffer);

    /*Copy wave_table*/
    for (i = 0; i < header->num_waves; i++)
    {
        header->wave_table[i] = READ_U16(wave_table);
    }

    /*Copy effects table*/
    for (i = 0; i < header->number_of_components; i++)
    {
        fxdata = start + READ_S32(ptr);
        assert((fxdata - effects) >= 0);
        header->effects[i].fxdata = _effects + (fxdata - effects);
        header->effects[i].priority = READ_S32(ptr);
        BASE_OFFSET(header, effects[i].fxdata);
    }

    BASE_OFFSET(header, wave_table);

    free(start);
}

static void load_audio(FILE *fp)
{
    s32 i, j, size;
    u8 *wbank;
    u8 *pbank_start;
    u8 *pbank_end;
    MusicInfo *info;

    sounds_bank0_ptr_ROM_START = _pBuffer;
    load_pbank(fp);
    sounds_bank0_ptr_ROM_END = _pBuffer;

    sounds_bank0_wbk_ROM_START = _pBuffer;
    fread(&size, 4, 1, fp);
    fread(_pBuffer, size, 1, fp);
    _pBuffer += size;
    sounds_bank0_wbk_ROM_END = _pBuffer;

    sounds_sfx_bfx_ROM_START = _pBuffer;
    load_sfx(fp);
    sounds_sfx_bfx_ROM_END = _pBuffer;

    load_song(&gMusic.music[MUSIC_TRAINING_BASE], fp);
    gMusic.music[MUSIC_TRAINING_BASE].wbank_start = NULL;
    gMusic.music[MUSIC_TRAINING_BASE].pbank_start = NULL;
    gMusic.music[MUSIC_TRAINING_BASE].pbank_end = NULL;

    for (i = 0; i < ARRAY_COUNT(_musics); i++)
    {
        pbank_start = _pBuffer;
        load_pbank(fp);
        pbank_end = _pBuffer;

        wbank = _pBuffer;
        fread(&size, 4, 1, fp);
        fread(_pBuffer, size, 1, fp);
        _pBuffer += size;

        for (j = 0; j < ARRAY_COUNT(_musics[i]); j++)
        {
            info = _musics[i][j];
            if (info == NULL)
                break;

            load_song(info, fp);
            info->wbank_start = wbank;
            info->pbank_start = pbank_start;
            info->pbank_end = pbank_end;
        }
    }
}

static void load_art(Art *art, FILE *fp)
{
    s32 size, dsize;
    u8 *ptr;
    char buf[3];

    fread(&size, 4, 1, fp);
    fread(buf, 3, 1, fp);

    if (buf[0] == 'E' && buf[1] == 'D' && buf[2] == 'L')
    {
        ptr = malloc(size);
        assert(ptr != NULL);
        memcpy(ptr, buf, 3);
        fread(&ptr[3], size-3, 1, fp);
        dsize = SWAP_S32(*(u32 *)&ptr[8]);
        assert(decompressEDL(ptr, art->data) == 0);
        free(ptr);
        _pBuffer += dsize;
    }
    else
    {
        memcpy(art->data, buf, 3);
        fread(&art->data[3], size-3, 1, fp);
        _pBuffer += size;
    }
    ALIGN16(_pBuffer);
}

static void load_arts(FILE *fp)
{
    s32 i;
    Art *art = _arts;

    for (i = 0; i < ARRAY_COUNT(_arts); i++)
    {
        art->data = _pBuffer;
        art->edl->romstart = 0;
        art->edl->romend = 0;
        art->edl->handle = &art->data;

        if (art->info != NULL)
            load_swapped_texture(art, fp);
        else if (art->offset_count != -1)
            load_special_model(art, fp);
        else
            load_art(art, fp);

        art++;
    }
}

static void load_demos(FILE *fp)
{
    s32 i, size;

    for (i = 0; i < ARRAY_COUNT(D_801CBCE8); i++)
    {
        fread(&size, 4, 1, fp);
        fread(_pBuffer, size, 1, fp);
        D_801CBCE8[i] = _pBuffer;
        _pBuffer += size;
        ALIGN16(_pBuffer);
    }
}

void load_assets(void)
{
    FILE *fp;

#if VERSION_US
    fp = fopen("assets.us.bin", "rb");
#elif VERSION_FR
    fp = fopen("assets.fr.bin", "rb");
#elif VERSION_EU
    fp = fopen("assets.eu.bin", "rb");
#elif VERSION_PROTO
    fp = fopen("assets.proto.bin", "rb");
#endif
    assert(fp != NULL);
    _pBuffer = _buffer;
    ALIGN16(_pBuffer);
    memset(_buffer, 0, sizeof(_buffer));

    load_tiles(fp);
    load_models(fp);
    load_blks(fp);
    load_maps(fp);
    load_demos(fp);
    load_arts(fp);
    load_audio(fp);

#if VERSION_PROTO
    D_800D52B0.vertex_info->ramaddr = (u8 *)((((intptr_t)D_800C199C.ramaddr + D_800D52B0.unk8) + 3) & ~3);
#endif

    assert(((intptr_t)_pBuffer - (intptr_t)_buffer) <= sizeof(_buffer));
    fclose(fp);
}
