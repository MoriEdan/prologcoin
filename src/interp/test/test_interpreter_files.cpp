#include "test_files_infrastructure.hpp"

static void header( const std::string &str )
{
    std::cout << "\n";
    std::cout << "--- [" + str + "] " + std::string(60 - str.length(), '-') << "\n";
    std::cout << "\n";
}

int main( int argc, char *argv[] )
{
    header( "test_interpreter_files" );

    find_home_dir(argv[0]);
    fast_mode = is_fast(argc, argv);

    const std::string dir = "/src/interp/test/pl_files";

    if (argc == 2) {
	test_interpreter_files(dir, [](interpreter &){}, argv[1]);
    } else {
	test_interpreter_files(dir, [](interpreter &){});
    }

    return 0;
}

