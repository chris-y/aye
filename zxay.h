/* AYE ZXAYEMUL metadata editor
 * by Chris Young 2021, 2022
 * https://www.unsatisfactorysoftware.co.uk
 */
 
/* ZXAY file structure */
 
#ifndef ZXAY_H
#define ZXAY_H 1

#define PAUTHOR_OFFSET         12
#define PMISC_OFFSET           14
#define PSONGSSTRUCTURE_OFFSET 18

struct zxay_header {
	char FileID[4];
	char TypeID[4];
	char FileVersion;
	char PlayerVersion;
	char PSpecialPlayer[2];
	char PAuthor[2];
	char PMisc[2];
	char NumOfSongs;
	char FirstSong;
	char PSongsStructure[2];
};

#define PSONGNAME_OFFSET 0
#define PSONGDATA_OFFSET 2

struct zxay_song {
	char PSongName[2];
	char PSongData[2];
};

#define PPOINTS_OFFSET 10
#define PADDRESSES_OFFSET 12

struct zxay_songdata {
	char AChan;
	char BChan;
	char CChan;
	char Noise;
	char SongLength[2];
	char FadeLength[2];
	char HiReg;
	char LoReg;
	char PPoints[2];
	char PAddresses[2];
};

struct zxay_songptrs {
	char Stack[2];
	char INIT[2];
	char INTERRUPT[2];
};

#define OFFSET_OFFSET 4

struct zxay_songblks {
	char Address[2];
	char Length[2];
	char Offset[2];
};

#endif
