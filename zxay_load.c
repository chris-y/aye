#include <stdint.h>
#include <stdio.h>
#include <malloc.h>

#include "zxay_file.h"
#include "zxay_load.h"
#include "zxay_misc.h"

static int16_t readoffset(char *rel, int16_t addoffset)
{
	int16_t offset = zxay_read_int16(rel);
	if(offset == 0) return 0;
	
	offset += addoffset;
	return offset;
}

static int aye_read_string(FILE *fp, char *buffer)
{
	int chars = 0;
	char c;
	
	do {
		c = fgetc(fp);
		if(buffer != NULL) {
			*buffer = c;
			buffer++;
		}
		chars++;
	} while(c);
	
	return chars;
}

static char *aye_read_generic(FILE *fp, int16_t offset)
{
	int chars = 0;
	char *string = NULL;
	
	if(offset == 0) return NULL;
	
	if(fseek(fp, offset, SEEK_SET) == 0) {
		chars = aye_read_string(fp, NULL);
	}
	
	if(chars != 0) {
		if(string = malloc(chars)) {
			if(fseek(fp, offset, SEEK_SET) == 0) {
				aye_read_string(fp, string);
			}
		}
	}
	
	return string;
}

static char *aye_read_author(FILE *fp, struct zxay_header *header)
{
	int16_t offset = readoffset(header->PAuthor, PAUTHOR_OFFSET);
	return aye_read_generic(fp, offset);
}

static char *aye_read_misc(FILE *fp, struct zxay_header *header)
{
	int16_t offset = readoffset(header->PMisc, PMISC_OFFSET);
	return aye_read_generic(fp, offset);
}

static char *aye_read_songname(FILE *fp, int16_t songstructureoffset, struct zxay_song *songstructure, int16_t song)
{
	songstructure += song;
	int16_t offset = readoffset(songstructure->PSongName, songstructureoffset + PSONGNAME_OFFSET + (sizeof(struct zxay_song) * song));

	return aye_read_generic(fp, offset);
}

static struct zxay_songdata *aye_read_songdata(FILE *fp, int16_t songstructureoffset, int16_t *songdataoffset, struct zxay_song *songstructure, int16_t song)
{
	struct zxay_songdata *songdata = NULL;
	size_t size = 0;
	int16_t offset = 0;
	
	songstructure += song;
	offset = readoffset(songstructure->PSongData, songstructureoffset + PSONGDATA_OFFSET + (sizeof(struct zxay_song) * song));

	songdata = malloc(sizeof(struct zxay_songdata));
	if(songdata == NULL) return NULL;
	
	if(fseek(fp, offset, SEEK_SET) == 0) {
		size = fread(songdata, sizeof(struct zxay_songdata), 1, fp);
	
		if(size != 1) {
			printf("Error reading song data\n");
			free(songdata);
			return NULL;
		}
	}
	
	*songdataoffset = offset;
	return songdata;
}

static struct zxay_songptrs *aye_read_songptrs(FILE *fp, int16_t songdataoffset, struct zxay_songdata *songdata)
{
	struct zxay_songptrs *songptrs = NULL;
	size_t size = 0;
	int16_t offset = readoffset(songdata->PPoints, songdataoffset + PPOINTS_OFFSET);

	songptrs = malloc(sizeof(struct zxay_songptrs));
	if(songptrs == NULL) return NULL;
	
	if(fseek(fp, offset, SEEK_SET) == 0) {
		size = fread(songptrs, sizeof(struct zxay_songptrs), 1, fp);
	
		if(size != 1) {
			printf("Error reading song pointers\n");
			free(songptrs);
			return NULL;
		}
	}
	
	return songptrs;
}

static struct zxay_songblks *aye_read_songdatablks(FILE *fp, int16_t *songblksoffset, int16_t *numdatablocks, int16_t songdataoffset, struct zxay_songdata *songdata)
{
	struct zxay_songblks *songblks = NULL;
	struct zxay_songblks tmpblk;
	size_t size = 0;
	int16_t offset = readoffset(songdata->PAddresses, songdataoffset + PADDRESSES_OFFSET);
	int16_t blocks = 0;

	if(fseek(fp, offset, SEEK_SET) == 0) {
		do {
			size = fread(&tmpblk, sizeof(struct zxay_songblks), 1, fp);
			blocks++;
	
			if(size != 1) {
				printf("Error seeking song data blocks at count=%d \n", blocks);
				return NULL;
			}
			
			//printf("%x %x\n", tmpblk.Address[0], tmpblk.Address[1]);
		} while((tmpblk.Address[0] != 0) || (tmpblk.Address[1] != 0));
	}

	songblks = calloc(sizeof(struct zxay_songblks), blocks);
	if(songblks == NULL) return NULL;
	
	if(fseek(fp, offset, SEEK_SET) == 0) {
		size = fread(songblks, sizeof(struct zxay_songblks), blocks, fp);
	
		if(size != blocks) {
			printf("Error reading song data blocks\n");
			free(songblks);
			return NULL;
		}
	}
	
	*songblksoffset = offset;
	*numdatablocks = blocks - 1;
	return songblks;
}

static char *aye_read_datablocks(FILE *fp, int16_t songblksoffset, int16_t numdatablocks, int32_t *datasize, struct zxay_songblks *songblks)
{
	char *datablock = NULL, *p = NULL;
	size_t size = 0;
	int i = 0;
	int16_t offset = 0;
	int16_t len = 0;
	int16_t addr = 0;
	int32_t datasizetotal = 0;
	struct zxay_songblks *songblk = songblks;
	
	for(i = 0; i < numdatablocks; i++) {
		len = zxay_read_int16(songblk->Length);
		addr = zxay_read_int16(songblk->Address);
		datasizetotal += len;
		songblk++;
	}

	datablock = calloc(datasizetotal, 1);
	if(datablock == NULL) return NULL;
	
	p = datablock;
	songblk = songblks;
	
	for(i = 0; i < numdatablocks; i++) {
		offset = readoffset(songblk->Offset, songblksoffset + OFFSET_OFFSET);
		printf("offset: %x %x + %x = calc offset: %x   ", songblk->Offset[0], songblk->Offset[1], songblksoffset + OFFSET_OFFSET, offset);
		len = zxay_read_int16(songblk->Length);
		printf("len: %x\n", len);
		if(fseek(fp, offset, SEEK_SET) == 0) {
			size = fread(p, len, 1, fp);
			p += len;
			
			if(size != 1) {
				printf("Error reading data block %d\n", i);
				free(datablock);
				return NULL;
			}
		}
		songblk ++;
		songblksoffset += sizeof(struct zxay_songblks);
	}
	
	*datasize = datasizetotal;
	return datablock;
}

static struct zxay_song *aye_read_songstructure(FILE *fp, struct zxay_header *header, int16_t *offset)
{
	size_t size = 0;
	struct zxay_song *songsstruct = NULL;
	int32_t bufsize = sizeof(struct zxay_song) * (header->NumOfSongs + 1);
	int16_t off = readoffset(header->PSongsStructure, PSONGSSTRUCTURE_OFFSET);
	
	songsstruct = malloc(bufsize);
	if(songsstruct == NULL) return NULL;
	
	if(fseek(fp, off, SEEK_SET) == 0) {
		size = fread(songsstruct, sizeof(struct zxay_song), (header->NumOfSongs + 1), fp);
	
		if(size != (header->NumOfSongs + 1)) {
			printf("Error reading songs\n");
			free(songsstruct);
			return NULL;
		}
	}

	*offset = off;
	return songsstruct;
}

static struct zxay_header *aye_read_header(FILE *fp)
{
	size_t size = 0;
	struct zxay_header *header;
	
	header = malloc(sizeof(struct zxay_header));
	if(header == NULL) {
		printf("Error allocating memory\n");
		return NULL;
	}
	
	size = fread(header, sizeof(struct zxay_header), 1, fp);
	
	if(size != 1) {
		free(header);
		return NULL;
	}
	
	if((header->FileID[0] == 'Z') &&
		(header->FileID[1] == 'X') &&
		(header->FileID[2] == 'A') &&
		(header->FileID[3] == 'Y')) {
		/* ZXAY file */
		if((header->TypeID[0] == 'E') &&
			(header->TypeID[1] == 'M') &&
			(header->TypeID[2] == 'U') &&
			(header->TypeID[3] == 'L')) {
			/* ZXAY EMUL */
			return header;
		} else {
			printf("This is an unsupported ZXAY type %c%c%c%c\n",
				header->TypeID[0], header->TypeID[1], header->TypeID[2], header->TypeID[3]);
		}
	} else {
		printf("Not a ZXAY file!\n");
	}
	
	return NULL;
}

static FILE *aye_open(char *filename)
{
	FILE *fp = 0;
	
	fp = fopen(filename, "r");
	if(fp == 0) return NULL;

	return fp;
}

static void aye_close(FILE *fp)
{
	fclose(fp);
}

void zxay_free(void *zxayemul)
{
	struct zxay_file *zxay = (struct zxay_file *)zxayemul;
	
	/* free song names */
	for(int i = 0; i <= zxay->header->NumOfSongs; i++) {
		if(zxay->songname[i]) free(zxay->songname[i]);
		if(zxay->songdata[i]) free(zxay->songdata[i]);
		if(zxay->songptrs[i]) free(zxay->songptrs[i]);
	}
	if(zxay->songname) free(zxay->songname);
	if(zxay->songdata) free(zxay->songdata);
	if(zxay->songsstruct) free(zxay->songsstruct);
	if(zxay->songptrs) free(zxay->songptrs);
	if(zxay->songblks) free(zxay->songblks);
	if(zxay->off_songdata) free(zxay->off_songdata);
	if(zxay->off_songblks) free(zxay->off_songblks);
	if(zxay->songblkcount) free(zxay->songblkcount);
	if(zxay->misc) free(zxay->misc);
	if(zxay->author) free(zxay->author);
	if(zxay->header) free(zxay->header);

	if(zxay) free(zxay);
}

void *zxay_load(char *filename)
{
	FILE *fp;
	struct zxay_file *zxay = NULL;

	zxay = calloc(1, sizeof(struct zxay_file));
	if(zxay == NULL) return NULL;

	if(fp = aye_open(filename)) {
		if(zxay->header = aye_read_header(fp)) {
			zxay->author = aye_read_author(fp, zxay->header);
			zxay->misc = aye_read_misc(fp, zxay->header);
			zxay->songsstruct = aye_read_songstructure(fp, zxay->header, &zxay->off_songstruct);
			zxay->songname = calloc(1, (zxay->header->NumOfSongs + 1) * sizeof(char *));
			zxay->songdata = calloc(1, (zxay->header->NumOfSongs + 1) * sizeof(char *));
			zxay->songptrs = calloc(1, (zxay->header->NumOfSongs + 1) * sizeof(char *));
			zxay->songblks = calloc(1, (zxay->header->NumOfSongs + 1) * sizeof(char *));
			zxay->datablks = calloc(1, (zxay->header->NumOfSongs + 1) * sizeof(char *));
			zxay->off_songdata = calloc(1, (zxay->header->NumOfSongs + 1) * sizeof(int16_t *));
			zxay->off_songblks = calloc(1, (zxay->header->NumOfSongs + 1) * sizeof(int16_t *));
			zxay->songblkcount = calloc(1, (zxay->header->NumOfSongs + 1) * sizeof(int16_t *));
			zxay->datablk_size = calloc(1, (zxay->header->NumOfSongs + 1) * sizeof(int32_t *));
			zxay->datablk_size = calloc(1, (zxay->header->NumOfSongs + 1) * sizeof(int32_t *));
			if(zxay->songsstruct &&
					zxay->songname &&
					zxay->songdata &&
					zxay->songptrs &&
					zxay->songblks &&
					zxay->datablks &&
					zxay->off_songdata &&
					zxay->off_songblks &&
					zxay->songblkcount &&
					zxay->datablk_size) {
				for(int i = 0; i <= zxay->header->NumOfSongs; i++) {
					zxay->songname[i] = aye_read_songname(fp, zxay->off_songstruct, zxay->songsstruct, i);
					zxay->songdata[i] = aye_read_songdata(fp, zxay->off_songstruct, &zxay->off_songdata[i], zxay->songsstruct, i);
					zxay->songptrs[i] = aye_read_songptrs(fp, zxay->off_songdata[i], zxay->songdata[i]);
					zxay->songblks[i] = aye_read_songdatablks(fp, &zxay->off_songblks[i], &zxay->songblkcount[i], zxay->off_songdata[i], zxay->songdata[i]);
					zxay->datablks[i] = aye_read_datablocks(fp, zxay->off_songblks[i], zxay->songblkcount[i], &zxay->datablk_size[i], zxay->songblks[i]);
				}
			} else {
				printf("Unable to allocate memory\n");
				free(zxay);
				zxay = NULL;
			}
		} else {
			printf("Error reading header\n");
			free(zxay);
			zxay = NULL;
		}
		aye_close(fp);
	} else {
		printf("Error opening file\n");
		free(zxay);
		zxay = NULL;
	}

	return (void *)zxay;
	
}

