/* AYE ZXAYEMUL metadata editor
 * by Chris Young 2021, 2022
 * https://www.unsatisfactorysoftware.co.uk
 */
 
/* ZXAY file structure */
 
#ifndef ZXAY_H
#define ZXAY_H 1

#include <stdint.h>

#define PAUTHOR_OFFSET         12
#define PMISC_OFFSET           14
#define PSONGSSTRUCTURE_OFFSET 18

struct zxay_header {
	uint8_t FileID[4];
	uint8_t TypeID[4];
	uint8_t FileVersion;
	uint8_t PlayerVersion;
	uint8_t PSpecialPlayer[2];
	uint8_t PAuthor[2];
	uint8_t PMisc[2];
	uint8_t NumOfSongs;
	uint8_t FirstSong;
	uint8_t PSongsStructure[2];
};

#define PSONGNAME_OFFSET 0
#define PSONGDATA_OFFSET 2

struct zxay_song {
	uint8_t PSongName[2];
	uint8_t PSongData[2];
};

#define PPOINTS_OFFSET 10
#define PADDRESSES_OFFSET 12

struct zxay_songdata {
	uint8_t AChan;
	uint8_t BChan;
	uint8_t CChan;
	uint8_t Noise;
	uint8_t SongLength[2];
	uint8_t FadeLength[2];
	uint8_t HiReg;
	uint8_t LoReg;
	uint8_t PPoints[2];
	uint8_t PAddresses[2];
};

struct zxay_songptrs {
	uint8_t Stack[2];
	uint8_t INIT[2];
	uint8_t INTERRUPT[2];
};

#define OFFSET_OFFSET 4

struct zxay_songblks {
	uint8_t Address[2];
	uint8_t Length[2];
	uint8_t Offset[2];
};

#endif
