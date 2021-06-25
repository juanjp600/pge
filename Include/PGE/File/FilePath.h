#ifndef PGE_FILEPATH_H_INCLUDED
#define PGE_FILEPATH_H_INCLUDED

#include <PGE/Types/Types.h>
#include <PGE/String/String.h>

namespace PGE {

// TODO: Possibly restructure iteration.
// TODO: Use wstr() for path construction consistently, or utilize C++20 char8_t for direct construction from UTF-8.
/// String wrapper utility to deal with paths.
/// Paths are always absolute and have sanitized path seperators.
class FilePath {
    private:
        String name;
        bool valid;

        FilePath(const String& str);

    public:
        /// Invalid path.
        /// Use for storing a path by value.
        FilePath();
        /// Constructs a path from a string.
        /// Absolute paths are taken in as-is, relative paths are appended to the current working directory.
        /// 
        /// File seperators will be sanitized.
        static FilePath fromStr(const String& str);

        /// Gets the OS-dependant folder for storing external data.
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
        bool operator==(const FilePath& other) const;
        bool operator!=(const FilePath& other) const;

        /// Appends str to a valid path.
        /// The path must be valid.
        /// 
        /// If you want to simply go down in the hierarchy it is advisable to assign the value of #makeDirectory before appending in order
        /// to make sure that your path is correctly suffixed with a path seperator.
        /// @param[in] str Will have its path seperators sanitized.
        /// @throws PGE::Exception If the path is not initialized.
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
        bool isValid() const;
        /// Checks if a path is an existing directory.
        /// @throws #PGE::Exception If the path is not initialized.
        bool isDirectory() const;
        /// Check if a path exists on the system.
        /// Could be folder, file or symbolic link.
        /// @throws #PGE::Exception If the path is not initialized.
        bool exists() const;

        /// Causes a path to be interpreted as a directory.
        /// This does not actually create the directory, it simply appends a path seperator at the end if there is not one already.
        /// @returns The path, guaranteed to have a path seperator as a suffix.
        /// @throws #PGE::Exception If the path is not initialized.
        const FilePath makeDirectory() const;
        /// Goes up exactly one step in the hierarchy.
        /// @throws #PGE::Exception If the path is not initialized.
        const FilePath up() const;

        /// Gets a file's extension.
        /// A file's extension is defined as the suffix after the last occurence of the "." character.
        /// @returns The file extension, without a trailing dot.
        /// @throws #PGE::Exception If the path is not initialized.
        const String getExtension() const;
        /// Returns a path, without its extension.
        /// A file's extension is defined as the suffix after the last occurence of the "." character.
        /// @returns The path, without the extension *and* without the trailing dot.
        const FilePath trimExtension() const;

        // TODO: Reconsider design.
        bool createDirectory() const;

        // Returns all direct subdirectories.
        std::vector<FilePath> enumerateFolders() const;

        // Enumerates subdirectories.
        std::vector<FilePath> enumerateFiles(bool recursive = true) const;

        // Returns the entire file with normalized line-endings.
        String read() const;

        // Returns all lines from a file.
        std::vector<String> readLines(bool includeEmptyLines = false) const;

        // Returns all bytes in a file.
        std::vector<byte> readBytes() const;

        const String& str() const;
        const char* cstr() const;
};

}

#endif // PGE_FILEPATH_H_INCLUDED
