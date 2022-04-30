#include "coder.h"
#include "decoder.h"

int main(int argc, char* argv[]) {
    try {
        if (argc < 2) {
            std::cout << "Running the program without parameters" << std::endl;
            return 0;
        }
        std::string parameter(argv[1]);
        if (parameter == "-c") {
            Coder coder(argc, argv);
            coder.Code();
        } else if (parameter == "-d") {
            Decoder decoder(argv[2]);
            decoder.Decode();
        } else if (parameter == "-h") {
            std::cout
                    << "archiver -c archive_name file1 [file2 ...] - archive files \"file1\", \"file2\", ..."
                       "and save the result in \"archive_name\"."
                    << std::endl;
            std::cout
                    << "archiver -d archive_name - unzip files from the archive \"archive_name\""
                       "and put them in the current directory."
                    << std::endl;
            std::cout << "archiver -h - print help on using the program." << std::endl;
        }
    } catch(const std::exception& e) {
        std::cout << "Error: " << e.what();
    }
    return 0;
}
