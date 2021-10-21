/* AYE ZXAYEMUL metadata editor
 * by Chris Young 2021
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


#endif
