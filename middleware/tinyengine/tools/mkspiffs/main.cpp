//
//  main.cpp
//  make_spiffs
//
//  Created by Ivan Grokhotkov on 13/05/15.
//  Copyright (c) 2015 Ivan Grokhotkov. All rights reserved.
//
#define TCLAP_SETBASE_ZERO 1

#include <iostream>
#include "spiffs.h"
#include <vector>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <memory>
#include <cstdlib>
#include "tclap/CmdLine.h"
#include "tclap/UnlabeledValueArg.h"

#ifdef _WIN32
#include <direct.h>
#endif

static std::vector<uint8_t> s_flashmem;

static std::string s_dirName;
static std::string s_imageName;
static int s_imageSize;
static int s_pageSize;
static int s_blockSize;

enum Action { ACTION_NONE, ACTION_PACK, ACTION_UNPACK, ACTION_LIST, ACTION_VISUALIZE };
static Action s_action = ACTION_NONE;

static spiffs s_fs;

static std::vector<uint8_t> s_spiffsWorkBuf;
static std::vector<uint8_t> s_spiffsFds;
static std::vector<uint8_t> s_spiffsCache;

static int s_debugLevel = 0;
static bool s_addAllFiles;

// Unless -a flag is given, these files/directories will not be included into the image
static const char* ignored_file_names[] = {
    ".DS_Store",
    ".git",
    ".gitignore",
    ".gitmodules"
};

static s32_t api_spiffs_read(u32_t addr, u32_t size, u8_t *dst)
{
    memcpy(dst, &s_flashmem[0] + addr, size);
    return SPIFFS_OK;
}

static s32_t api_spiffs_write(u32_t addr, u32_t size, u8_t *src)
{
    memcpy(&s_flashmem[0] + addr, src, size);
    return SPIFFS_OK;
}

static s32_t api_spiffs_erase(u32_t addr, u32_t size)
{
    memset(&s_flashmem[0] + addr, 0xff, size);
    return SPIFFS_OK;
}

static int checkArgs();
static size_t getFileSize(FILE* fp);


//implementation

int spiffsTryMount()
{
    spiffs_config cfg = {0};

    cfg.phys_addr = 0x0000;
    cfg.phys_size = (u32_t) s_flashmem.size();

    cfg.phys_erase_block = s_blockSize;
    cfg.log_block_size = s_blockSize;
    cfg.log_page_size = s_pageSize;

    cfg.hal_read_f = api_spiffs_read;
    cfg.hal_write_f = api_spiffs_write;
    cfg.hal_erase_f = api_spiffs_erase;

    const int maxOpenFiles = 4;
    s_spiffsWorkBuf.resize(s_pageSize * 2);
    s_spiffsFds.resize(32 * maxOpenFiles);
    s_spiffsCache.resize((32 + s_pageSize) * maxOpenFiles);

    return SPIFFS_mount(&s_fs, &cfg,
                        &s_spiffsWorkBuf[0],
                        &s_spiffsFds[0], s_spiffsFds.size(),
                        &s_spiffsCache[0], s_spiffsCache.size(),
                        NULL);
}

bool spiffsMount(bool printError = true)
{
    if (SPIFFS_mounted(&s_fs)) {
        return true;
    }
    int res = spiffsTryMount();
    if (res != SPIFFS_OK) {
        if (printError) {
            std::cerr << "SPIFFS mount failed with error: " << res << std::endl;
        }
        return false;
    }
    return true;
}

bool spiffsFormat()
{
    spiffsMount(false);
    SPIFFS_unmount(&s_fs);
    int formated = SPIFFS_format(&s_fs);
    if (formated != SPIFFS_OK) {
        return false;
    }
    return (spiffsTryMount() == SPIFFS_OK);
}

void spiffsUnmount()
{
    if (SPIFFS_mounted(&s_fs)) {
        SPIFFS_unmount(&s_fs);
    }
}

int addFile(char* name, const char* path)
{
    FILE* src = fopen(path, "rb");
    if (!src) {
        std::cerr << "error: failed to open " << path << " for reading" << std::endl;
        return 1;
    }

    spiffs_file dst = SPIFFS_open(&s_fs, name, SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);

    // read file size
    size_t size = getFileSize(src);

    if (s_debugLevel > 0) {
        std::cout << "file size: " << size << std::endl;
    }

    size_t left = size;
    uint8_t data_byte;
    while (left > 0) {
        if (1 != fread(&data_byte, 1, 1, src)) {
            std::cerr << "fread error!" << std::endl;

            fclose(src);
            SPIFFS_close(&s_fs, dst);
            return 1;
        }
        int res = SPIFFS_write(&s_fs, dst, &data_byte, 1);
        if (res < 0) {
            std::cerr << "SPIFFS_write error(" << s_fs.err_code << "): ";

            if (s_fs.err_code == SPIFFS_ERR_FULL) {
                std::cerr << "File system is full." << std::endl;
            } else {
                std::cerr << "unknown";
            }
            std::cerr << std::endl;

            if (s_debugLevel > 0) {
                std::cout << "data left: " << left << std::endl;
            }

            fclose(src);
            SPIFFS_close(&s_fs, dst);
            return 1;
        }
        left -= 1;
    }

    SPIFFS_close(&s_fs, dst);
    fclose(src);

    return 0;
}

int addFiles(const char* dirname, const char* subPath)
{
    DIR *dir;
    struct dirent *ent;
    bool error = false;
    std::string dirPath = dirname;
    dirPath += subPath;

    // Open directory
    if ((dir = opendir (dirPath.c_str())) != NULL) {

        // Read files from directory.
        while ((ent = readdir (dir)) != NULL) {

            // Ignore dir itself.
            if ((strcmp(ent->d_name, ".") == 0) || (strcmp(ent->d_name, "..") == 0)) {
                continue;
            }

            if (!s_addAllFiles) {
                bool skip = false;
                size_t ignored_file_names_count = sizeof(ignored_file_names) / sizeof(ignored_file_names[0]);
                for (size_t i = 0; i < ignored_file_names_count; ++i) {
                    if (strcmp(ent->d_name, ignored_file_names[i]) == 0) {
                        std::cerr << "skipping " << ent->d_name << std::endl;
                        skip = true;
                        break;
                    }
                }
                if (skip) {
                    continue;
                }
            }

            std::string fullpath = dirPath;
            fullpath += ent->d_name;
            struct stat path_stat;
            stat (fullpath.c_str(), &path_stat);

            if (!S_ISREG(path_stat.st_mode)) {
                // Check if path is a directory.
                if (S_ISDIR(path_stat.st_mode)) {
                    // Prepare new sub path.
                    std::string newSubPath = subPath;
                    newSubPath += ent->d_name;
                    newSubPath += "/";

                    if (addFiles(dirname, newSubPath.c_str()) != 0) {
                        std::cerr << "Error for adding content from " << ent->d_name << "!" << std::endl;
                    }

                    continue;
                } else {
                    std::cerr << "skipping " << ent->d_name << std::endl;
                    continue;
                }
            }

            // Filepath with dirname as root folder.
            std::string filepath = subPath;
            filepath += ent->d_name;
            std::cout << filepath << std::endl;

            // Add File to image.
            if (addFile((char*)filepath.c_str(), fullpath.c_str()) != 0) {
                std::cerr << "error adding file!" << std::endl;
                error = true;
                if (s_debugLevel > 0) {
                    std::cout << std::endl;
                }
                break;
            }
        } // end while
        closedir (dir);
    } else {
        std::cerr << "warning: can't read source directory" << std::endl;
        return 1;
    }

    return (error) ? 1 : 0;
}

void listFiles()
{
    spiffs_DIR dir;
    spiffs_dirent ent;

    SPIFFS_opendir(&s_fs, 0, &dir);
    spiffs_dirent* it;
    while (true) {
        it = SPIFFS_readdir(&dir, &ent);
        if (!it) {
            break;
        }

        std::cout << it->size << '\t' << it->name << std::endl;
    }
    SPIFFS_closedir(&dir);
}

/**
 * @brief Check if directory exists.
 * @param path Directory path.
 * @return True if exists otherwise false.
 *
 * @author Pascal Gollor (http://www.pgollor.de/cms/)
 */
bool dirExists(const char* path)
{
    DIR *d = opendir(path);

    if (d) {
        closedir(d);
        return true;
    }

    return false;
}

/**
 * @brief Create directory if it not exists.
 * @param path Directory path.
 * @return True or false.
 *
 * @author Pascal Gollor (http://www.pgollor.de/cms/)
 */
bool dirCreate(const char* path)
{
    // Check if directory also exists.
    if (dirExists(path)) {
        return false;
    }

    // platform stuff...
#if defined(_WIN32)
    if (_mkdir(path) != 0) {
#else
    if (mkdir(path, S_IRWXU | S_IXGRP | S_IRGRP | S_IROTH | S_IXOTH) != 0) {
#endif
        std::cerr << "Can not create directory!!!" << std::endl;
        return false;
    }

    return true;
}

/**
 * @brief Unpack file from file system.
 * @param spiffsFile SPIFFS dir entry pointer.
 * @param destPath Destination file path path.
 * @return True or false.
 *
 * @author Pascal Gollor (http://www.pgollor.de/cms/)
 */
bool unpackFile(spiffs_dirent *spiffsFile, const char *destPath)
{
    u8_t buffer[spiffsFile->size];
    std::string filename = (const char*)(spiffsFile->name);

    // Open file from spiffs file system.
    spiffs_file src = SPIFFS_open(&s_fs, (char *)(filename.c_str()), SPIFFS_RDONLY, 0);

    // read content into buffer
    SPIFFS_read(&s_fs, src, buffer, spiffsFile->size);

    // Close spiffs file.
    SPIFFS_close(&s_fs, src);

    // Open file.
    FILE* dst = fopen(destPath, "wb");

    // Write content into file.
    fwrite(buffer, sizeof(u8_t), sizeof(buffer), dst);

    // Close file.
    fclose(dst);


    return true;
}

/**
 * @brief Unpack files from file system.
 * @param sDest Directory path as std::string.
 * @return True or false.
 *
 * @author Pascal Gollor (http://www.pgollor.de/cms/)
 *
 * todo: Do unpack stuff for directories.
 */
bool unpackFiles(std::string sDest)
{
    spiffs_DIR dir;
    spiffs_dirent ent;

    // Add "./" to path if is not given.
    if (sDest.find("./") == std::string::npos && sDest.find("/") == std::string::npos) {
        sDest = "./" + sDest;
    }

    // Check if directory exists. If it does not then try to create it with permissions 755.
    if (! dirExists(sDest.c_str())) {
        std::cout << "Directory " << sDest << " does not exists. Try to create it." << std::endl;

        // Try to create directory.
        if (! dirCreate(sDest.c_str())) {
            return false;
        }
    }

    // Open directory.
    SPIFFS_opendir(&s_fs, 0, &dir);

    // Read content from directory.
    spiffs_dirent* it = SPIFFS_readdir(&dir, &ent);
    while (it) {
        // Check if content is a file.
        if ((int)(it->type) == 1) {
            std::string name = (const char*)(it->name);
            std::string sDestFilePath = sDest + name;
            size_t pos = name.find_first_of("/", 1);

            // If file is in sub directories?
            while (pos != std::string::npos) {
                // Subdir path.
                std::string path = sDest;
                path += name.substr(0, pos);

                // Create subddir if subdir not exists.
                if (!dirExists(path.c_str())) {
                    if (!dirCreate(path.c_str())) {
                        return false;
                    }
                }

                pos = name.find_first_of("/", pos + 1);
            }

            // Unpack file to destination directory.
            if (! unpackFile(it, sDestFilePath.c_str()) ) {
                std::cout << "Can not unpack " << it->name << "!" << std::endl;
                return false;
            }

            // Output stuff.
            std::cout
                    << it->name
                    << '\t'
                    << " > " << sDestFilePath
                    << '\t'
                    << "size: " << it->size << " Bytes"
                    << std::endl;
        }

        // Get next file handle.
        it = SPIFFS_readdir(&dir, &ent);
    } // end while

    // Close directory.
    SPIFFS_closedir(&dir);

    return true;
}

// Actions

int actionPack()
{
    if (!dirExists(s_dirName.c_str())) {
        std::cerr << "error: can't read source directory" << std::endl;
        return 1;
    }

    if (s_imageSize == 0) {
        s_imageSize = 0x10000;
        if (s_debugLevel > 0) {
            std::cout << "image size not specifed, using default: " << s_imageSize << std::endl;
        }
    }

    int err = checkArgs();
    if (err != 0) {
        return err;
    }

    s_flashmem.resize(s_imageSize, 0xff);

    FILE* fdres = fopen(s_imageName.c_str(), "wb");
    if (!fdres) {
        std::cerr << "error: failed to open image file" << std::endl;
        return 1;
    }

    spiffsFormat();
    int result = addFiles(s_dirName.c_str(), "/");
    spiffsUnmount();

    fwrite(&s_flashmem[0], 4, s_flashmem.size() / 4, fdres);
    fclose(fdres);

    return result;
}

static size_t getFileSize(FILE* fp)
{
    fseek(fp, 0L, SEEK_END);
    size_t size = (size_t) ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    return size;
}

/**
 * @brief Unpack action.
 * @return 0 success, 1 error
 *
 * @author Pascal Gollor (http://www.pgollor.de/cms/)
 */
int actionUnpack(void)
{
    int ret = 0;

    // open spiffs image
    FILE* fdsrc = fopen(s_imageName.c_str(), "rb");
    if (!fdsrc) {
        std::cerr << "error: failed to open image file" << std::endl;
        return 1;
    }

    if (s_imageSize == 0) {
        s_imageSize = getFileSize(fdsrc);
    }

    int err = checkArgs();
    if (err != 0) {
        return err;
    }

    s_flashmem.resize(s_imageSize, 0xff);

    // read content into s_flashmem
    fread(&s_flashmem[0], 4, s_flashmem.size() / 4, fdsrc);

    // close file handle
    fclose(fdsrc);

    // mount file system
    if (!spiffsMount()) {
        std::cerr << "error: failed to mount image" << std::endl;
        return 1;
    }

    // unpack files
    if (! unpackFiles(s_dirName)) {
        ret = 1;
    }

    // unmount file system
    spiffsUnmount();

    return ret;
}


int actionList()
{
    FILE* fdsrc = fopen(s_imageName.c_str(), "rb");
    if (!fdsrc) {
        std::cerr << "error: failed to open image file" << std::endl;
        return 1;
    }

    if (s_imageSize == 0) {
        s_imageSize = getFileSize(fdsrc);
    }

    int err = checkArgs();
    if (err != 0) {
        return err;
    }

    s_flashmem.resize(s_imageSize, 0xff);

    fread(&s_flashmem[0], 4, s_flashmem.size() / 4, fdsrc);
    fclose(fdsrc);

    if (!spiffsMount()) {
        std::cerr << "error: failed to mount image" << std::endl;
        return 1;
    }

    listFiles();
    spiffsUnmount();
    return 0;
}

int actionVisualize()
{
    FILE* fdsrc = fopen(s_imageName.c_str(), "rb");
    if (!fdsrc) {
        std::cerr << "error: failed to open image file" << std::endl;
        return 1;
    }

    if (s_imageSize == 0) {
        s_imageSize = getFileSize(fdsrc);
    }

    int err = checkArgs();
    if (err != 0) {
        return err;
    }

    s_flashmem.resize(s_imageSize, 0xff);


    fread(&s_flashmem[0], 4, s_flashmem.size() / 4, fdsrc);
    fclose(fdsrc);

    if (!spiffsMount()) {
        std::cerr << "error: failed to mount image" << std::endl;
        return 1;
    }

    SPIFFS_vis(&s_fs);
    uint32_t total, used;
    SPIFFS_info(&s_fs, &total, &used);
    std::cout << "total: " << total <<  std::endl << "used: " << used << std::endl;
    spiffsUnmount();

    return 0;
}

#define PRINT_INT_MACRO(def_name) \
    std::cout << "  " # def_name ": " << def_name << std::endl;

class CustomOutput : public TCLAP::StdOutput
{
public:
    virtual void version(TCLAP::CmdLineInterface& c)
    {
        std::cout << "mkspiffs ver. " VERSION << std::endl;
        const char* configName = BUILD_CONFIG_NAME;
        if (configName[0] == '-') {
            configName += 1;
        }
        std::cout << "Build configuration name: " << configName << std::endl;
        std::cout << "SPIFFS ver. " SPIFFS_VERSION << std::endl;
        const char* buildConfig = BUILD_CONFIG;
        std::cout << "Extra build flags: " << (strlen(buildConfig) ? buildConfig : "(none)") << std::endl;
        std::cout << "SPIFFS configuration:" << std::endl;
        PRINT_INT_MACRO(SPIFFS_OBJ_NAME_LEN);
        PRINT_INT_MACRO(SPIFFS_OBJ_META_LEN);
        PRINT_INT_MACRO(SPIFFS_USE_MAGIC);
#if SPIFFS_USE_MAGIC == 1
        PRINT_INT_MACRO(SPIFFS_USE_MAGIC_LENGTH);
#endif
        PRINT_INT_MACRO(SPIFFS_ALIGNED_OBJECT_INDEX_TABLES);
    }
};

#undef PRINT_INT_MACRO

void processArgs(int argc, const char** argv)
{
    TCLAP::CmdLine cmd("", ' ', VERSION);
    CustomOutput output;
    cmd.setOutput(&output);

    TCLAP::ValueArg<std::string> packArg( "c", "create", "create spiffs image from a directory", true, "", "pack_dir");
    TCLAP::ValueArg<std::string> unpackArg( "u", "unpack", "unpack spiffs image to a directory", true, "", "dest_dir");
    TCLAP::SwitchArg listArg( "l", "list", "list files in spiffs image", false);
    TCLAP::SwitchArg visualizeArg( "i", "visualize", "visualize spiffs image", false);
    TCLAP::UnlabeledValueArg<std::string> outNameArg( "image_file", "spiffs image file", true, "", "image_file"  );
    TCLAP::ValueArg<int> imageSizeArg( "s", "size", "fs image size, in bytes", false, 0, "number" );
    TCLAP::ValueArg<int> pageSizeArg( "p", "page", "fs page size, in bytes", false, 256, "number" );
    TCLAP::ValueArg<int> blockSizeArg( "b", "block", "fs block size, in bytes", false, 4096, "number" );
    TCLAP::SwitchArg addAllFilesArg( "a", "all-files", "when creating an image, include files which are normally ignored; currently only applies to '.DS_Store' files and '.git' directories", false);
    TCLAP::ValueArg<int> debugArg( "d", "debug", "Debug level. 0 means no debug output.", false, 0, "0-5" );

    cmd.add( imageSizeArg );
    cmd.add( pageSizeArg );
    cmd.add( blockSizeArg );
    cmd.add( addAllFilesArg );
    cmd.add( debugArg );
    std::vector<TCLAP::Arg*> args = {&packArg, &unpackArg, &listArg, &visualizeArg};
    cmd.xorAdd( args );
    cmd.add( outNameArg );
    cmd.parse( argc, argv );

    if (debugArg.getValue() > 0) {
        std::cerr << "Debug output enabled" << std::endl;
        s_debugLevel = debugArg.getValue();
    }

    if (packArg.isSet()) {
        s_dirName = packArg.getValue();
        s_action = ACTION_PACK;
    } else if (unpackArg.isSet()) {
        s_dirName = unpackArg.getValue();
        s_action = ACTION_UNPACK;
    } else if (listArg.isSet()) {
        s_action = ACTION_LIST;
    } else if (visualizeArg.isSet()) {
        s_action = ACTION_VISUALIZE;
    }

    s_imageName = outNameArg.getValue();
    s_imageSize = imageSizeArg.getValue();
    s_pageSize  = pageSizeArg.getValue();
    s_blockSize = blockSizeArg.getValue();
    s_addAllFiles = addAllFilesArg.isSet();
}

static int checkArgs()
{
    // Argument checks don't use TCLAP's Constraint interface because
    // a) we need to check one argument against another, while Constraints only check
    // each argument individually, and b) image size might only be known when image
    // file is opened.

    if (s_imageSize < 0 || s_pageSize < 0 || s_blockSize < 0) {
        std::cerr << "error: Image, block, page sizes should not be negative" << std::endl;
        return 1;
    }

    if (s_imageSize % s_blockSize != 0) {
        std::cerr << "error: Image size should be a multiple of block size" << std::endl;
        return 1;
    }

    if (s_blockSize % s_pageSize != 0) {
        std::cerr << "error: Block size should be a multiple of page size" << std::endl;
        return 1;
    }

    const int physicalFlashEraseBlockSize = 4096;
    if (s_blockSize % physicalFlashEraseBlockSize != 0) {
        std::cerr << "error: Block size should be multiple of flash erase block size (" <<
                     physicalFlashEraseBlockSize << ")" << std::endl;
        return 1;
    }

    return 0;
}

int main(int argc, const char * argv[])
{

    try {
        processArgs(argc, argv);
    } catch (...) {
        std::cerr << "error: Invalid arguments" << std::endl;
        return 1;
    }

    switch (s_action) {
    case ACTION_PACK:
        return actionPack();
        break;
    case ACTION_UNPACK:
        return actionUnpack();
        break;
    case ACTION_LIST:
        return actionList();
        break;
    case ACTION_VISUALIZE:
        return actionVisualize();
        break;
    default:
        break;
    }

    return 1;
}
