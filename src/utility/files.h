//
// Created by User on 02/02/2026.
//

#ifndef SDL3_FIRST_FILES_H
#define SDL3_FIRST_FILES_H
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <SDL3/SDL_dialog.h>
#include <miniz/miniz.h>

struct File {
    std::string name;
    std::vector<uint8_t> data;
};

class Files {
private:
    static void SDLCALL file_callback(std::string *string, const char* const* filelist, int filter) {
        if (!filelist) {
            SDL_Log("An error occured: %s", SDL_GetError());
            return;
        } else if (!*filelist) {
            SDL_Log("The user did not select any file.");
            SDL_Log("Most likely, the dialog was canceled.");
            return;
        }

        while (*filelist) {
            SDL_Log("Full path to selected file: '%s'", *filelist);
            std::cout << " File chosen|!";
            string->append(*filelist);
            return;
        }

        if (filter < 0) {
            SDL_Log("The current platform does not support fetching "
                    "the selected filter, or the user did not select"
                    " any filter.");
            return;
        }


    };
public:
    static std::string open_file_dialogue_blocking(SDL_DialogFileFilter filter) {
        /*
         * Opens a file dialogue and returns the file paths as strings, this pauses the forward-renderer thread until a file is selected.
         */

        std::string path;
        SDL_ShowOpenFileDialog(SDL_DialogFileCallback(Files::file_callback), &path, NULL, &filter, 1, NULL, false);
        std::cout << "STARTING..." << std::endl;
        while (path.empty()) {
            std::cout << "WAITING..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        std::cout << "FINISHED..." << std::endl;
        return path;
    }

    static void open_file_dialogue(SDL_DialogFileFilter filter, std::string path) {
        /*
         * Opens a file dialogue and returns the file paths as strings without pausing the forward-renderer program.
         * The string is immediately set once a result is chosen. This function is asynchronous.
         */
        SDL_ShowOpenFileDialog(SDL_DialogFileCallback(Files::file_callback), &path, NULL, &filter, 1, NULL, false);

    }

    static std::vector<std::string> open_file_dialogue_blocking_multiple(SDL_DialogFileFilter filter) {
        /*
         * Opens a file dialogue and returns the file paths as strings, this pauses the forward-renderer thread until a file is selected.
         */

        std::vector<std::string> path;
        SDL_ShowOpenFileDialog(SDL_DialogFileCallback(Files::file_callback), &path, NULL, &filter, 1, NULL, true);
        while (path.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        return path;
    }

    static void open_file_dialogue_multiple(SDL_DialogFileFilter filter, std::vector<std::string> path) {
        /*
         * Opens a file dialogue and returns the file paths as strings without pausing the forward-renderer program.
         * The string is immediately set once a result is chosen. This function is asynchronous.
         */
        SDL_ShowOpenFileDialog(SDL_DialogFileCallback(Files::file_callback), &path, NULL, &filter, 1, NULL, true);

    }

    static std::vector<uint8_t> load_file_to_memory(std::string path) {
        /*
         * Loads a file into memory as a vector of uint8_t, must be given a path, can use `open_file_dialogue()`.
         */
        std::ifstream file(path, std::ios::binary);
        return std::vector<uint8_t>(
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()
        );
    }

    static std::vector<File> unzip(const std::vector<uint8_t>& data) {
        /*
         * Intended for use when data from `load_file_to_memory()` needs to be unzipped;
         * Returns a list of `file` structs, recursed zips may require further unzipping;
         */
        mz_zip_archive zip{};
        mz_zip_reader_init_mem(&zip, data.data(), data.size(), 0);

        std::vector<File> files;

        int count = mz_zip_reader_get_num_files(&zip);
        for (int i = 0; i < count; i++)
        {
            if (mz_zip_reader_is_file_a_directory(&zip, i))
                continue;

            mz_zip_archive_file_stat stat{};
            mz_zip_reader_file_stat(&zip, i, &stat);

            File f;
            f.name = stat.m_filename;
            f.data.resize(stat.m_uncomp_size);

            mz_zip_reader_extract_to_mem(
                &zip, i, f.data.data(), f.data.size(), 0
            );

            files.push_back(std::move(f));
        }

        mz_zip_reader_end(&zip);
        return files;
    }


};












#endif //SDL3_FIRST_FILES_H