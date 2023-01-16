#pragma once
#include <string>
#include <vector>
#include <initializer_list>

/**
 * Utilities for manipulating files and directories.
 */
class FileSystem {
public:

    /**
     * Check if the directory in path exists in the filesystem.
     * @param path
     * @return
     */
    static bool DirectoryExist(const std::string& path);

    /**
     * Get the name of the last directory in path.
     * @param path
     * @return
     */
    static std::string DirectoryName(const std::string& path);

    /**
     * Check if the file in path exists in the filesystem.
     * @param fileName
     * @return
     */
    static bool FileExist(const std::string& fileName);

    /**
     * Gets the directory of the this binary.
     * @return
     */
    static std::string GetAppDirectory();

    /**
     * Creates a new directory.
     * @param directory_name
     */
    static void CreateDirectory(const std::string& directory_name);

    /**
     * Cross-platform union of path elements.
     * @param path List of path elements.
     * @return The full path.
     */
    static std::string FilenameJoin(const std::initializer_list<std::string>& path);

    /**
     * Get the base name of a file.
     * @param path
     * @return
     */
    static std::string FileBasename(const std::string& path);

    /**
     * Get the directory name of a file.
     * @param path
     * @return
     */
    static std::string FileDirectory(const std::string& path);

    /**
     * Get the extension of a file.
     * @param filepath
     * @return
     */
    static std::string FileExtension(const std::string& filepath);

    /**
     * Fetch all the files in path.
     * @param path Full path of directory.
     * @return A list of paths of all the files that were found.
     */
    static std::vector<std::string> FilesInDirectory(const std::string& path);
};