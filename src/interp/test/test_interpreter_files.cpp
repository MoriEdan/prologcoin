#include "../../common/test/test_home_dir.hpp"
#include "../../common/term_tools.hpp"
#include "../../common/term_parser.hpp"
#include "../interpreter.hpp"
#include <fstream>
#include <dirent.h>
#include <boost/algorithm/string.hpp>

using namespace prologcoin::common;
using namespace prologcoin::interp;

static void header( const std::string &str )
{
    std::cout << "\n";
    std::cout << "--- [" + str + "] " + std::string(60 - str.length(), '-') << "\n";
    std::cout << "\n";
}

static std::vector<std::string> parse_expected(std::string &comments)
{
    std::vector<std::string> expected;
    size_t i = 0;
    while (i != std::string::npos) {
	size_t i1 = comments.find("Expect:", i);
	if (i1 == std::string::npos) {
	    break;
	}
	size_t i2 = comments.find("Expect:", i1+7);
	std::string exp;
	if (i2 == std::string::npos) {
	    exp = comments.substr(i1+7);
	} else {
	    exp = comments.substr(i1+7,(i2-i1));
	}
	// Remove trailing %
	exp = exp.substr(0, exp.find_last_of("%"));
	// Remove trailing whitespace
	boost::trim(exp);
	expected.push_back(exp);
	i = i2;
    }
    return expected;
}

static bool match_strings(const std::string &actual,
			  const std::string &expect)
{
    // Compare at token level
    std::stringstream in_actual(actual);
    std::stringstream in_expect(expect);

    term_token_diff diff(in_actual, in_expect);
    if (!diff.check()) {
	std::cout << "Error. Difference spotted." << std::endl;
	return false;
    }
    return true;
}


static bool test_interpreter_file(const std::string &filepath)
{
    std::cout << "Process file: " << filepath << std::endl << std::endl;

    interpreter interp;

    std::ifstream infile(filepath);

    term_tokenizer tokenizer(infile);
    term_parser parser(tokenizer, interp.env().get_heap(), interp.env().get_ops());

    con_cell query_op("?-", 1);


    try {
	while (!infile.eof()) {
	    term t = parser.parse();

	    bool is_query = interp.env().is_functor(t, query_op);

	    // Once parsing is done we'll copy over the var-name bindings
	    // so we can pretty print the variable names.
	    parser.for_each_var_name( [&](const term  &ref,
					  const std::string &name)
				      { interp.env().set_name(ref,name); } );
	    parser.clear_var_names();

	    std::cout << interp.env().to_string(t,
		is_query ? term_emitter::STYLE_TERM : term_emitter::STYLE_PROGRAM)
		      << "\n";

	    std::vector<std::string> expected;

	    if (is_query) {
		std::cout << std::string(67, '-') << std::endl;

		std::string comments = parser.get_comments_string();
		expected = parse_expected(comments);
		if (expected.size() == 0) {
		    std::cout << "Error. Found no expected results for this query. "
		              << "At line " << parser.get_comments()[0].pos().line()
			      << "\n";
		    assert(false);
		}
	    }

	    if (!is_query) {
		interp.load_clause(t);
	    }

	    if (is_query) {
		std::cout << std::string(67, '-') << std::endl;

		term q = interp.env().arg(t, 0);

		if (!interp.execute(q)) {
		    std::cout << "  Failed!\n";
		    return false;
		}

		std::string result = interp.get_result(false);
		std::cout << "Actual: " << result << std::endl;
		std::cout << "Expect: " << expected[0] << std::endl;
		match_strings(result, expected[0]);
	    }
	}

	infile.close();

	return true;

    } catch (token_exception &ex) {
	infile.close();

	std::cout << "Parse error at line " << ex.pos().line() << " and column " << ex.pos().column() << ": " << ex.what() << "\n";
	return false;
    } catch (term_parse_error_exception &ex) {
	infile.close();

	auto tok = ex.token();
	std::cout << "Parse error at line " << tok.pos().line() << " and column " << tok.pos().column() << ": " << ex.what() << "\n";
	return false;
    }
}

static void test_interpreter_files()
{
    header( "test_interpreter_files" );

    const std::string &home_dir = find_home_dir();

    std::string files_dir = home_dir + "/src/interp/test/pl_files";

    DIR *dir = opendir(files_dir.c_str());
    struct dirent *ent;
    while ((ent = readdir(dir)) != nullptr) {
	std::string filepath = files_dir + "/" + ent->d_name;
	if (boost::ends_with(filepath, ".pl")) {
	    bool r = test_interpreter_file(filepath);
	    assert(r);
	}
    }
    closedir (dir);    
}

int main( int argc, char *argv[] )
{
    find_home_dir(argv[0]);

    test_interpreter_files();
}

