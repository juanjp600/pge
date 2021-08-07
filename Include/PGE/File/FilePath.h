#ifndef PGE_FILEPATH_H_INCLUDED
#define PGE_FILEPATH_H_INCLUDED

#include <PGE/Types/Types.h>
#include <PGE/String/String.h>

namespace PGE {

// TODO: Possibly restructure iteration.
// TODO: Use wstr() for path construction consistently, or utilize C++20 char8_t for direct construction from UTF-8.
/// String wrapper utility to deal with paths.
/// Paths are always absolute and have sanitized path seperators.
class FilePath : private NoHeap {
    private:
        String name;
        bool valid;

        FilePath(const String& str) noexcept;

    public:
        /// Invalid path.
        /// Use for storing a path by value.
        FilePath() noexcept;
        /// Constructs a path from a string.
        /// Absolute paths are taken in as-is, relative paths are appended to the current working directory.
        /// 
        /// File seperators will be sanitized.
        static const FilePath fromStr(const String& str);

        /// Gets an OS-dependant folder for storing external data.
        /// Windows: C:/Users/*user*/AppData/Roaming/
        /// 
        /// MacOS: Volumes/User/*user*/Library/Application Support/
        /// 
        /// Linux: TODO
        static const FilePath& getDataPath();

        /// Returns whether two paths are considered equal.
        /// An invalid path will never equal another path.
        /// 
        /// Path seperators are always normalized.
        /// 
        /// Two paths differing in a trailing path seperator are not considered equal, as one could be a folder and the other a file.
        bool operator==(const FilePath& other) const noexcept;
        bool operator!=(const FilePath& other) const noexcept;

        /// Appends str to a path.
        /// The path must be valid.
        /// 
        /// If you want to simply go down in the hierarchy it is advisable to assign the value of #makeDirectory before appending in order
        /// to make sure that your path is correctly suffixed with a path seperator.
        /// @param[in] str Will have its path seperators sanitized.
        /// @throws #PGE::Exception If the path is not initialized.
        FilePath& operator+=(const String& str);

        /// Appends str to a valid path.
        /// The path must be valid.
        /// 
        /// If you want to simply go down in the hierarchy it is advisable to call #makeDirectory before appending in order to make sure
        /// that your path is correctly suffixed with a path seperator.
        /// @param[in] str Will have its path seperators sanitized.
        /// @throws #PGE::Exception If the path is not initialized.
        const FilePath operator+(const String& str) const;

        /// Gets if a path is considered valid.
        /// A path object is considered valid if it has been assigned a path. A default constructed path is not valid.
        bool isValid() const noexcept;

        /// Checks if a path is an existing directory.
        /// @throws #PGE::Exception If the path is not initialized.
        bool isDirectory() const;

        /// Check if a path exists on the system.
        /// @throws #PGE::Exception If the path is not initialized.
        bool exists() const;

        /// Causes a path to be interpreted as a directory.
        /// This does not actually create the directory, it simply appends a path separator at the end if there is not one already.
        /// @returns The path, guaranteed to have a path seperator as a suffix.
        /// @throws #PGE::Exception If the path is not initialized.
        const FilePath makeDirectory() const;

        /// Returns the path to the parent directory of the current path.
        /// @throws #PGE::Exception If the path is not initialized.
        const FilePath getParentDirectory() const;

        /// Gets a file's extension.
        /// A file's extension is defined as the suffix after the last occurence of the "." character.
        /// @returns The file extension, without a leading dot.
        /// @throws #PGE::Exception If the path is not initialized.
        const String getExtension() const;

        /// Returns a path, without its extension.
        /// A file's extension is defined as the suffix after the last occurence of the "." character.
        /// @returns The path, without the extension *and* without the trailing dot.
        /// @throws #PGE::Exception If the path is not initialized.
        const FilePath trimExtension() const;

        /// Creates a directory and all non-existant superdirectories.
        /// @returns Whether all directories were created successfully.
        ///          `false` is encountered if the directory already existed, since in that case no exception is raised.
        /// @throws #PGE::Exception If the path is not initialized or the directory creation failed.
        bool createDirectory() const;

        // TODO: Properly wrapping iteration?
        /// Gets all direct subdirectories.
        /// @throws #PGE::Exception If the path is not initialized.
        const std::vector<FilePath> enumerateFolders() const;

        /// Gets all regular files in a directory.
        /// @param[in] recursive Whether to recursively search subdirectories for files as well.
        /// @throws #PGE::Exception If the path is not initialized.
        const std::vector<FilePath> enumerateFiles(bool recursive = true) const;

        // Shorthand utility.

        /// Reads the entire file with normalized line-endings and UTF-8 encoding.
        /// Line endings are normalized to `\n`.
        /// @throws #PGE::Exception If the path is not initialized, the file could not be opened, or errors occured during the reading of the file.
        /// @see #PGE::TextReader
        String read() const;

        /// Reads all lines of a file into a vector.
        /// @throws #PGE::Exception If the path is not initialized, the file could not be opened, or errors occured during the reading of the file.
        /// @see #PGE::TextReader
        const std::vector<String> readLines(bool includeEmptyLines = false) const;

        /// Reads all bytes of a file into a vector.
        /// @throws #PGE::Exception If the path is not initialized, or the file could not be opened.
        const std::vector<byte> readBytes() const;

        /// Returns the internal string representation of the path.
        /// Always absolute and path sepeartors are sanitized to '/'.
        /// @throws #PGE::Exception If the path is not initialized.
        const String& str() const;
};

}

#endif // PGE_FILEPATH_H_INCLUDED
