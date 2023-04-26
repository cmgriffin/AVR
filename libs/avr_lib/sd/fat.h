/**
 * @brief fat.h v1.0
 *
 * Updated from work by original author below
 *
Description:
        File system driver for FAT16/32

        For complete details visit:
        https://www.programming-electronics-diy.xyz/2022/07/sd-memory-card-library-for-avr.html

Author:
        Liviu Istrate
        istrateliviu24@yahoo.com
        www.programming-electronics-diy.xyz

Donate:
        Software development takes time and effort so if you find this useful
consider a small donation at: paypal.me/alientransducer


/* ----------------------------- LICENSE - GNU GPL v3
-----------------------------------------------

* This license must be included in any redistribution.

* Copyright (C) 2022 Liviu Istrate, www.programming-electronics-diy.xyz
(istrateliviu24@yahoo.com)

* Project URL:
https://www.programming-electronics-diy.xyz/2022/07/sd-memory-card-library-for-avr.html

* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program. If not, see <https://www.gnu.org/licenses/>.

--------------------------------- END OF LICENSE
--------------------------------------------------*/

#ifndef FAT_H_
#define FAT_H_

#include <avrlibdefs.h>

/*************************************************************
        USER DEFINED SETTINGS
**************************************************************/
#define FAT_SUPPORT_FAT32 1 // set to 0 to support only FAT16 and exclude FAT32

// FAT supports file names up to 260 characters including path
// but that would take a lot of space so
// shorter file names could be used instead
#define FAT_MAX_FILENAME_LENGTH 30

typedef int32_t INT_SIZE; // can be int32_t (default) or int64_t

/*************************************************************
        SYSTEM DEFINES
**************************************************************/
#define FAT_TASK_SEARCH_SFN        1
#define FAT_TASK_TABLE_COUNT_FREE  2
#define FAT_TASK_TABLE_FIND_FREE   3
#define FAT_TASK_MKDIR             4
#define FAT_TASK_MKFILE            5
#define FAT_TASK_TABLE_SET         6
#define FAT_TASK_TABLE_GET_NEXT    7
#define FAT_TASK_OPEN_DIR          8
#define FAT_TASK_COUNT_ITEMS       9
#define FAT_TASK_FIND_FILE         10
#define FAT_TASK_SET_FILESIZE      11
#define FAT_TASK_SET_START_CLUSTER 12
#define FAT_TASK_TABLE_READ_SET    13

// Directory entry
#define FAT_DIR_FREE_SLOT 0x00 // root directory entry is free
#define FAT_FILE_DELETED \
    0xE5 // the filename has been used, but the file has been deleted
#define FAT_FILE_E5_CHAR \
    0x05 // the first character of the filename is actually 0xE5
#define FAT_LFN_MAX_CHARS 13 // a LFN entry can hold 13 characters

// FAT Table
#define FAT16_FREE_CLUSTER 0x0000 // the cluster is free
#define FAT16_BAD_CLUSTER  0xFFF7 // indicates a bad (defective) cluster
#define FAT16_EOF_CLUSTER \
    0xFFFF // cluster is allocated and is the final cluster for the file
           // (indicates end-of-file)

#define FAT32_FREE_CLUSTER 0x0000000 // the cluster is free
#define FAT32_BAD_CLUSTER  0xFFFFFF7 // indicates a bad (defective) cluster
#define FAT32_EOF_CLUSTER \
    0x0FFFFFFF // cluster is allocated and is the final cluster for the file
               // (indicates end-of-file)
// NOTE: No FAT32 volume should ever be configured containing cluster numbers
// available for allocation >= 0xFFFFFF7.
#define FAT32_MAX_CLUSTER 0xFFFFFF7

// The maximum Windows filename length to the operating system is 260
// characters, however that includes a number of required characters that lower
// the effective number. From the 260, you must allow room for the following:
// Drive letter, Colon after drive letter, Backslash after drive letter,
// End-of-Line character, Backslashes that are part of the filename path (e.g.
// c:\dir-name\dir-name\filename). So, that takes the 260 down to 256 characters
// as an absolute maximum. Absolute (relative) maximum file length - including
// path - is 256 characters.
#define FS_MAX_PATH \
    260 - 4 - 1 - 8 // subtract 1 so max_length is 255 to fit in 1 byte minus 8
                    // for drive label

/* File system type (FATFS.fs_type) */
#define FS_FAT12 0
#define FS_FAT16 1 // must be 1 so bit shifting << will multiply by 2
#define FS_FAT32 2 // must be 2 so bit shifting << will multiply by 4
#define FS_EXFAT 3

/* Offsets */
/* MBR */
#define FAT_MBR_PARTITION_ENTRY_OFFSET \
    0x1BE // offset at which partition information starts
#define FAT_MBR_PARTITION_TYPE \
    0x04 // partition type offset relative to each 16-bit partition slot
#define FAT_MBR_PARTITION_START \
    0x08 // relative offset to the partition in sectors (LBA)
#define FAT_MBR_PARTITION_SIZE 0x0C // size of the partition in sectors

/* Boot Record */
#define FAT_BPB_BYTES_PER_SECTOR    0x0B // sector size in bytes
#define FAT_BPB_SECTORS_PER_CLUSTER 0x0D // number of sectors per cluster
#define FAT_BPB_RESERVED_SECTORS \
    0x0E                        // reserved sectors (including the boot sector)
#define FAT_BPB_NR_OF_FATS 0x10 // number of FATs
#define FAT_BPB_ROOT_DIR_ENTRIES \
    0x11 // number of directory entries in the root directory (N.A. for FAT32)
#define FAT_BPB_TOT_SEC_16 0x13 // total number of sectors on the disk/partition
#define FAT_BPB_FAT_SZ_16 \
    0x16 // number of sectors occupied by a FAT (N.A. for FAT32)
#define FAT_BPB_TOT_SEC_32 \
    0x20 // total number of sectors of the FAT volume in new 32-bit field

#define FAT16_BS_VOL_ID 39 // volume serial number (FAT12/16)

#define FAT32_BPB_FAT_SZ_32 \
    36 // this field is the FAT32 32-bit count of sectors occupied by one FAT
#define FAT32_BPB_EXT_FLAFS \
    40 // indicates how many FATs are used and if only 1 which one
#define FAT32_BPB_FS_VER 42 // the version number of the FAT32 volume
#define FAT32_BPB_ROOT_CLUST \
    44 // cluster number of the first cluster of the root directory
#define FAT32_BPB_FS_INFO \
    48 // sector number of FSINFO structure in the reserved area of the FAT32
       // volume. Usually 1
#define FAT32_BPB_BK_BOOT_SECTOR \
    50 // if non-zero, indicates the sector number in the reserved area of the
       // volume of a copy of the boot record
#define FAT32_BPB_RESERVED 52 // must be set to 0x0
#define FAT32_BS_DRV_NUM   64 // set value to 0x80 or 0x00
#define FAT32_BS_RESERVED1 65 // must be set to 0x0
#define FAT32_BS_BOOT_SIG  66 // extended boot signature
#define FAT32_BS_VOL_ID    67 // volume serial number (FAT32)
#define FAT32_BS_VOL_LABEL 71 // volume label (FAT32)
#define FAT32_BS_FS_TYPE   82 // set to the string:"FAT32 "

/* Directory Entry */
#define FAT_DIR_NAME              0x00
#define FAT_DIR_ATTR              11
#define FAT_DIR_NT_RES            12
#define FAT_DIR_CREAT_TIME_MILLIS 13
#define FAT_DIR_CREAT_TIME        14
#define FAT_DIR_CREAT_DATE        16
#define FAT_DIR_LAST_ACC_DATE     18
#define FAT_DIR_FIRST_CLUS_HIGH   20
#define FAT_DIR_WRITE_TIME        22
#define FAT_DIR_WRITE_DATE        24
#define FAT_DIR_FIRST_CLUS_LOW    26
#define FAT_DIR_FILE_SIZE         28

/* File attributes Offsets and Masks */
#define FAT_FILE_ATTR_READ_ONLY 0x01
#define FAT_FILE_ATTR_HIDDEN    0x02
#define FAT_FILE_ATTR_SYSTEM    0x04
#define FAT_FILE_ATTR_VOLUME_ID 0x08
#define FAT_FILE_ATTR_DIRECTORY 0x10
#define FAT_FILE_ATTR_ARCHIVE   0x20
#define FAT_FILE_ATTR_LONG_NAME                                              \
    (FAT_FILE_ATTR_READ_ONLY | FAT_FILE_ATTR_HIDDEN | FAT_FILE_ATTR_SYSTEM | \
     FAT_FILE_ATTR_VOLUME_ID)
#define FAT_FILE_ATTR_LONG_NAME_MASK                                         \
    (FAT_FILE_ATTR_READ_ONLY | FAT_FILE_ATTR_HIDDEN | FAT_FILE_ATTR_SYSTEM | \
     FAT_FILE_ATTR_VOLUME_ID | FAT_FILE_ATTR_DIRECTORY |                     \
     FAT_FILE_ATTR_ARCHIVE)

/* FAT Long Directory Entry Structure Offsets and Masks */
#define FAT_LONG_DIR_ORDER \
    0 // the order of this entry in the sequence of long dir entries
#define FAT_LAST_LONG_ENTRY_MASK 0x40
#define FAT_LONG_DIR_NAME \
    1 // characters 1-5 of the long-name sub-component in this dir entry
#define FAT_LONG_DIR_ATTR 11 // attributes - must be ATTR_LONG_NAME
#define FAT_LONG_DIR_TYPE \
    12 // if zero, indicates a directory entry that is a sub-component of a long
       // name
#define FAT_LONG_DIR_CHECKSUM \
    13 // checksum of name in the short dir entry at the end of the long dir set
#define FAT_LONG_DIR_NAME2 \
    14 // characters 6-11 of the long-name sub-component in this dir entry
#define FAT_LONG_DIR_FIRST_CLST_LOW \
    26 // must be ZERO. This is an artifact of the FAT "first cluster"
#define FAT_LONG_DIR_NAME3 \
    28 // characters 12-13 of the long-name sub-component in this dir entry

/*************************************************************
        GLOBALS
**************************************************************/
/* Function pointers to low lever card interface functions */
// uint8_t (*card_read_single_block) (uint32_t addr, uint8_t *buf, uint8_t
// *token);

typedef uint32_t FSIZE_t;    // file size in bytes
typedef uint32_t SECTSIZE_t; // sectors

#if FAT_SUPPORT_FAT32 == 0
typedef uint16_t CLSTSIZE_t; // clusters
#else
typedef uint32_t CLSTSIZE_t; // clusters
#endif

/* Volume mounting return codes (FAT_MOUNT_RESULT) */
typedef enum
{
    MR_OK = 0,           // Succeeded
    MR_DEVICE_INIT_FAIL, // An error occurred during device initialization
    MR_ERR,              // An error occurred in the low level disk I/O layer
    MR_NO_PARTITION,     // No partition has been found
    MR_FAT_ERR,          // General FAT error
    MR_UNSUPPORTED_FS,   // Unsupported file system
    MR_UNSUPPORTED_BS    // Unsupported block size
} FAT_MOUNT_RESULT;

/* File function return codes (FAT_FRESULT) */
typedef enum
{
    FR_OK = 0,          // Succeeded
    FR_EOF,             // End of file
    FR_NOT_FOUND,       // Could not find the file
    FR_NO_PATH,         // Could not find the path
    FR_NO_SPACE,        // Not enough space to create file
    FR_EXIST,           // File exists
    FR_INCORRECT_ENTRY, // Some entry parameters are incorrect
    FR_DENIED, // Access denied due to prohibited access or directory full
    FR_PATH_LENGTH_EXCEEDED, // Path too long
    FR_NOT_A_DIRECTORY,
    FR_ROOT_DIR, // When going back the directory path this is returned when the
                 // active dir is root
    FR_INDEX_OUT_OF_RANGE,
    FR_DEVICE_ERR // A hard error occurred in the low level disk I/O layer
} FAT_FRESULT;

/* File system object structure (FAT) */
typedef struct
{
    uint32_t fs_partition_offset; // relative offset to the partition in sectors
                                  // (LBA)
    uint32_t BPB_TotSec32; // total number of sectors of the FAT volume in new
                           // 32-bit field
    uint32_t EOC;          // End Of Chain cluster value
    CLSTSIZE_t CountofClusters; // count of clusters
    uint32_t FATSz;             // number of sectors for one FAT table
    uint16_t BPB_BytsPerSec;    // sector size in bytes
    uint16_t RootDirSectors; // count of sectors occupied by the root directory
                             // (N.A for FAT32)
    uint32_t
        RootFirstCluster; // first cluster of the root directory (FAT32 only)
    uint16_t RootFirstSector; // first sector of the root	directory
    uint16_t FirstDataSector; // this sector number is relative to the first
                              // sector of the volume that contains the BPB
    uint16_t Fat1StartSector;
    uint16_t Fat2StartSector;
    uint8_t FATDataSize; // 2-bytes if FAT16, 4-bytes if FAT32
    uint8_t
        entries_per_sector; // number of entries in a sector given a 32 bytes
                            // entry. For a 512 bytes per sector: 512 / 32 = 16
    uint8_t fs_low_level_code; // low level status code from card controller
    uint8_t fs_type;           // file system type (0:not mounted)
    uint8_t BPB_SecPerClus;    // sectors per cluster
} FAT;

/* Directory object structure (FAT_DIR) */
typedef struct
{
    bool dir_open;
    bool dir_open_by_idx;
    uint16_t
        dir_nr_of_entries; // number of files and folders inside a directory
    CLSTSIZE_t dir_start_cluster;
    CLSTSIZE_t dir_active_cluster;
    SECTSIZE_t dir_start_sector;
    uint16_t dir_active_sector; // max 2^16 sectors per cluster
    uint16_t dir_active_item;   // index of selected item inside directory
                                // starting from 1
    uint8_t dir_entry_offset; // entry offset inside the sector of selected item
    uint16_t dir_nr_of_items;
    uint16_t find_by_index;
    uint8_t filename_length;
    const char *ptr_path_buff;
} FAT_DIR;

/* File information structure (FAT_FILE) */
typedef struct
{
    uint8_t file_err;    // abort flag (error code)
    FSIZE_t fptr;        // file read/write pointer (zeroed on file open)
    uint8_t file_attrib; // file attribute
    // uint16_t	file_last_access_date; // this is the date of last read or write
    uint16_t file_write_time; // time of last write. Note that file creation is
                              // considered a write
    uint16_t file_write_date; // date of last write. Note that file creation is
                              // considered a write
    FSIZE_t file_size;        // file size in bytes
    CLSTSIZE_t file_start_cluster; // entry's first cluster number
    CLSTSIZE_t file_active_cluster;
    uint32_t file_start_sector;
    uint16_t file_active_sector; // incremented after each sector read
    uint16_t buffer_idx;
    SECTSIZE_t entry_start_sector; // sector number that holds the file entry
    uint8_t entry_offset; // offset inside the sector where the entry starts
    bool file_update_size;
    bool file_open;
    bool w_sec_changed; // write sector changed
    bool eof;
} FAT_FILE;

/*************************************************************
        MACRO FUNCTIONS
**************************************************************/
/* Character code support macros */
#define IsUpper(c)     ((c) >= 'A' && (c) <= 'Z')
#define IsLower(c)     ((c) >= 'a' && (c) <= 'z')
#define IsDigit(c)     ((c) >= '0' && (c) <= '9')
#define IsSeparator(c) ((c) == '/' || (c) == '\\')

/*************************************************************
        FUNCTION PROTOTYPES
**************************************************************/
// Volume
FAT_MOUNT_RESULT FAT_mountVolume(void);
/*______________________________________________________________________________________________
        Return volume free space in bytes
_______________________________________________________________________________________________*/
uint64_t FAT_volumeFreeSpace(void);
/*______________________________________________________________________________________________
        Return volume capacity in bytes
_______________________________________________________________________________________________*/
uint64_t FAT_volumeCapacity(void);
/*______________________________________________________________________________________________
        Return volume capacity in KiB
_______________________________________________________________________________________________*/
float FAT_volumeCapacityKB(void);
/*______________________________________________________________________________________________
        Return volume capacity in MiB
_______________________________________________________________________________________________*/
float FAT_volumeCapacityMB(void);
/*______________________________________________________________________________________________
        Return volume capacity in GiB
_______________________________________________________________________________________________*/
float FAT_volumeCapacityGB(void);
/*______________________________________________________________________________________________
        Returns the label and serial number of a volume

        label		Pointer to the buffer to store the volume label.
                                If the volume has no label, a null-string will
be returned. The buffer array must be of type char and 12 bytes in size: 11 for
                                volume label and 1 for the null.

        vol_sn		Pointer to a uint32_t variable to store the volume
serial number. Pass 0 if not needed.

        Example:
                                char label[12];
                                uint32_t vol_sn = 0;
                                FAT_getLabel(label, &vol_sn);
_______________________________________________________________________________________________*/
FAT_FRESULT FAT_getLabel(char *label, uint32_t *vol_sn);

// Other
/*______________________________________________________________________________________________
        Create time and date fields in FAT format with dummy values that could
be substituted with values from a RTC
_______________________________________________________________________________________________*/
uint8_t FAT_createTimeMilli(void);
uint16_t FAT_createTime(void);
uint16_t FAT_createDate(void);

// Directory
/*______________________________________________________________________________________________
        Create a subdirectory at the specified path. Name of the subdirectory is
the name after the last slash '/' in path.
_______________________________________________________________________________________________*/
FAT_FRESULT FAT_makeDir(const char *path);
/*______________________________________________________________________________________________
        Open a directory using the given path
_______________________________________________________________________________________________*/
FAT_FRESULT FAT_openDir(FAT_DIR *dir_p, const char *path);
/*______________________________________________________________________________________________
        Open a directory with the given index inside the active directory.
        FAT_openDir() must be used before running this function.
_______________________________________________________________________________________________*/
FAT_FRESULT FAT_openDirByIndex(FAT_DIR *dir_p, FAT_FILE *finfo_p, uint16_t idx);
/*______________________________________________________________________________________________
        Go to parent directory of active directory. If the active directory is
Root, then the function will return FR_ROOT_DIR and active directory will remain
Root.
_______________________________________________________________________________________________*/
FAT_FRESULT FAT_dirBack(FAT_DIR *dir_p);

/*______________________________________________________________________________________________
        Get file info of the item in the directory with a specific index
position. Index must start from 1. If index is greater than the number of items
inside the directory, then FR_NOT_FOUND will be returned.
_______________________________________________________________________________________________*/
FAT_FRESULT FAT_findByIndex(FAT_DIR *dir_p, FAT_FILE *finfo_p, uint16_t idx);
/*______________________________________________________________________________________________
        Get file info of the first or next item in the directory that was opened
previously
_______________________________________________________________________________________________*/
FAT_FRESULT FAT_findNext(FAT_DIR *dir_p, FAT_FILE *finfo_p);
/*______________________________________________________________________________________________
        Return the total number of files and folders inside the active directory
_______________________________________________________________________________________________*/
uint16_t FAT_dirCountItems(FAT_DIR *dir_p);

// File
/*______________________________________________________________________________________________
        Create a file at the specified path. Name of the file is the name after
        the last slash '/' in path.
_______________________________________________________________________________________________*/
FAT_FRESULT FAT_makeFile(const char *path);
/*______________________________________________________________________________________________
        Wrapper function of fwrite() used to convert a float number
        into a string and write it to a file

        decimals	number of digits after the dot
_______________________________________________________________________________________________*/
FAT_FRESULT FAT_fwriteFloat(FAT_FILE *fp, float nr, uint8_t nrOfDecimals);
/*______________________________________________________________________________________________
        Wrapper function of fwrite() used to convert a number into a string and
write it to a file
_______________________________________________________________________________________________*/
FAT_FRESULT FAT_fwriteInt(FAT_FILE *fp, INT_SIZE nr);

/*______________________________________________________________________________________________
        Wrapper function of fwrite() used to write a string
_______________________________________________________________________________________________*/
FAT_FRESULT FAT_fwriteString(FAT_FILE *fp, const char *string);
/*______________________________________________________________________________________________
        Write data to the file at the file offset pointed by read/write pointer.
        The write pointer advances with each byte written.
        CAUTION: running other functions will overwrite the common data buffer
causing the loss of unsaved data. Use fsync() before using any other function
including fseek().

        fp			Pointer to the file object structure
        buff		Pointer to the data to be written
        btw			Number of bytes to write
        bw			Pointer to the variable to return number of
bytes written
_______________________________________________________________________________________________*/
FAT_FRESULT FAT_fwrite(FAT_FILE *fp, const void *buff, uint16_t btw,
                       uint16_t *bw);
/*______________________________________________________________________________________________
        Truncates the file size to the current file read/write pointer
_______________________________________________________________________________________________*/
FAT_FRESULT FAT_ftruncate(FAT_FILE *fp);
/*______________________________________________________________________________________________
        Flush cached data of the writing file
_______________________________________________________________________________________________*/
FAT_FRESULT FAT_fsync(FAT_FILE *fp);
/*______________________________________________________________________________________________
        Open a file using it's name. The search will be made inside the active
directory.
_______________________________________________________________________________________________*/
FAT_FRESULT FAT_fopen(FAT_DIR *dir_p, FAT_FILE *file_p, char *file_name);
/*______________________________________________________________________________________________
        Open a file by index. The search will be made inside the active
directory.
_______________________________________________________________________________________________*/
FAT_FRESULT FAT_fopenByIndex(FAT_DIR *dir_p, FAT_FILE *file_p, uint16_t idx);
/*______________________________________________________________________________________________
        Read data from a file. Each time the function will return a pointer to
the main buffer array containing a block of data that must be used before
running other functions that might overwrite the main buffer. The main buffer is
used to preserve RAM. The file must be opened using the appropriate function
before it can be read.
_______________________________________________________________________________________________*/
uint8_t *FAT_fread(FAT_FILE *file_p);
/*______________________________________________________________________________________________
        Return the file pointer
_______________________________________________________________________________________________*/
FSIZE_t FAT_getFptr(FAT_FILE *fp);
/*______________________________________________________________________________________________
        Move the file pointer to end of file. Wrapper of fseek().
_______________________________________________________________________________________________*/
void FAT_fseekEnd(FAT_FILE *fp);
/*______________________________________________________________________________________________
        Move the file pointer x number of bytes. fptr must not be greater
        than the file size in bytes.
_______________________________________________________________________________________________*/
void FAT_fseek(FAT_FILE *fp, FSIZE_t fptr);
/*______________________________________________________________________________________________
        Used to check for the End Of File
_______________________________________________________________________________________________*/
bool FAT_feof(FAT_FILE *fp);
/*______________________________________________________________________________________________
        Used to check if an error occurs during file read
_______________________________________________________________________________________________*/
uint8_t FAT_ferror(FAT_FILE *fp);
/*______________________________________________________________________________________________
        Clear the error flag
_______________________________________________________________________________________________*/
void FAT_fclear_error(FAT_FILE *fp);
/*______________________________________________________________________________________________
        The filename is available immediately only after a function that
provides the file info. To preserve memory only a single filename array is used
so when multiple files are active or functions that open a path are used those
will modify the filename buffer.
_______________________________________________________________________________________________*/
char *FAT_getFilename(void);
/*______________________________________________________________________________________________
        Return the index of the active item inside the opened directory
_______________________________________________________________________________________________*/
uint16_t FAT_getItemIndex(FAT_DIR *dir_p);
/*______________________________________________________________________________________________
        Return the file size in bytes
_______________________________________________________________________________________________*/
FSIZE_t FAT_getFileSize(FAT_FILE *finfo_p);
uint16_t FAT_getWriteYear(FAT_FILE *finfo_p);
uint8_t FAT_getWriteMonth(FAT_FILE *finfo_p);
uint8_t FAT_getWriteDay(FAT_FILE *finfo_p);
uint8_t FAT_getWriteHour(FAT_FILE *finfo_p);
uint8_t FAT_getWriteMinute(FAT_FILE *finfo_p);
uint8_t FAT_getWriteSecond(FAT_FILE *finfo_p);
bool FAT_attrIsFolder(FAT_FILE *finfo_p);
bool FAT_attrIsFile(FAT_FILE *finfo_p);
bool FAT_attrIsHidden(FAT_FILE *finfo_p);
bool FAT_attrIsSystem(FAT_FILE *finfo_p);
bool FAT_attrIsReadOnly(FAT_FILE *finfo_p);
bool FAT_attrIsArchive(FAT_FILE *finfo_p);

#endif /* FAT_H_ */