#pragma once
#include <string>
#include <vector>
#include <initializer_list>

/// <summary>
/// Check if the directory in path exists in the filesystem.
/// </summary>
bool directory_exist(std::string path);

/// <summary>
/// Check if the file in path exists in the filesystem.
/// </summary>
bool file_exist(std::string fileName);

/// <summary>
/// Gets the directory of the executable binary.
/// </summary>
std::string get_app_directory();

/// <summary>
/// Creates a new directory.
/// </summary>
/// <param name="directory_name">Full path of the new directory.</param>
void create_directory(std::string directory_name);

/// <summary>
/// Cross-platform union of path elements.
/// </summary>
/// <param name="path">List of path elements.</param>
/// <returns>The full path.</returns>
std::string filename_join(std::initializer_list<std::string> path);

/// <summary>
/// Get the base name of a file.
/// </summary>
std::string file_basename(const std::string path);

/// <summary>
/// Get the extension of a file.
/// </summary>
std::string file_extension(const std::string filepath);

/// <summary>
/// Fetch all the files in path.
/// </summary>
/// <param name="path">Full path of directory.</param>
/// <returns>A list of paths of all the files that were found.</returns>
std::vector<std::string> files_in_directory(const std::string path);