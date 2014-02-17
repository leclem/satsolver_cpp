#include <iostream>
#include <fstream>

#include "dpll.h"
#include "rand.h"
#include "moms.h"
#include "dlis.h"

int main(int argc, char** argv)
{
    // stream of file input
    std::ifstream inf;
    std::ofstream of;

    // flag "-o" given
    bool is_out = false;

    // flag for learned clauses
    bool bInteractive = true;
    bool bDoc = true;

    // file names
    std::string in_filename  = "";
    std::string out_filename = "";

    // show the help
    bool showHelp = false;

    // algorthim used to solve the problem
    SATheuristic solveAlgo = DPLL;

    // classic or watched literals
    LitChoice literals = CLASSIC;

    // Standart or learned clauses
    ClauseChoice clauses = STANDART;

    // first arg to parse
    int parseArg = 1;
    // arguments
    if(argc>=2)
    {
        // options
        if(std::string(argv[1]) == "--DPLL")
            parseArg = 2;
        else if(std::string(argv[1]) == "--rand")
        {
            parseArg = 2;
            solveAlgo = RND;
        }
        else if(std::string(argv[1]) == "--MOMS")
        {
            parseArg = 2;
            solveAlgo = MOMS;
        }
        else if(std::string(argv[1]) == "--DLIS")
        {
            parseArg = 2;
            solveAlgo = DLIS;
        }
        else if(std::string(argv[1]) == "--hist")
        {
            CConflict<DPLL, CLASSIC, LEARNED>::parseHist();
            return 0;
        }

    }
    if (argc >= parseArg+1 && std::string(argv[parseArg])[0] == '-')
    {
        std::string flags = std::string(argv[parseArg]);
        if(flags.size() == 1)
        {
            std::cout << "Unexpected : '-' with no flags\n";
            showHelp = true;
        }
        else
        for (int i = 1; i<(int)flags.size(); i++)
        {
            if(flags[i] == 'w')
                literals = WATCHED;
            else if(flags[i] == 'l')
                clauses = LEARNED;
            else if(flags[i] == 'i')
                bInteractive = false;
            else if(flags[i] == 'd')
                bDoc = false;
            else
            {
                std::cout << "Unexpected flag : -" << flags[i] << std::endl;
                showHelp = true;
                break;
            }
        }
        parseArg++;
    }
    for (int i=parseArg; i<argc; i++)
        if (std::string(argv[i]) == "--help")  //help
        {
            showHelp = true;
            break;
        }
        else if (std::string(argv[i]) == "-o") // output
            is_out = true;
        else if (is_out) // out file
        {
            if (out_filename != "")
            {
                std::cout << "Unexpected : -o " << argv[i] << std::endl;
                showHelp = true;
            }
            else
                out_filename = argv[i];
            is_out = false;
        }
        else // in file
        {
            if (in_filename != "")
            {
                std::cout << "Unexpected : " << argv[i] << std::endl;
                showHelp = true;
            }
            else
                in_filename = argv[i];
        }
    if(is_out) // expected args
    {
        std::cout << "Expected <filename out> after '-o'\n";
        showHelp = true;
    }

    // file opening
    if(!showHelp)
    {
        if (out_filename != "")
        {
            of.open(out_filename.c_str());
            if (!of || !of.good())
            {
                std::cerr << "Impossible to open in file : " << out_filename << std::endl;
                showHelp = true;
            }
        }
        if (in_filename != "")
        {
            inf.open(in_filename.c_str());
            if (!inf || !inf.good())
            {
                std::cerr << "Impossible to open out file: " << in_filename << std::endl;
                showHelp = true;
            }
        }
    }

    // Aide
    if(showHelp)
    {
        std::cout << "Sat solver: help\n";
        std::cout << "Syntax: sat [option] [-flags] [<filename in>] [-o <filename out>]\n";
        std::cout << "With one of these options:\n";
        std::cout << " --DPLL        (by default), use DPLL algorithm\n";
        std::cout << " --rand        DPLL with random acces\n";
        std::cout << " --MOMS        MOMS heuristic\n";
        std::cout << " --DLIS        DLIS heuristic\n";
        std::cout << " --hepl        show this help\n";
        std::cout << " --hist        open a prompt to parse history.slh (learned clauses)\n";
        std::cout << "And the following flags: \n";
        std::cout << " w      use the watched literals\n";
        std::cout << " l      use learned clauses\n";
        std::cout << "If the l flag is set, these others flags can be used too: \n";
        std::cout << " i      disable interactive mode\n";
        std::cout << " d      disable documentation file generation\n";
        return 0;
    }

    // create in/out stream
    std::istream& in  = (in_filename  == "") ? std::cin  : inf;
    std::ostream& out = (out_filename == "") ? std::cout : of ;




    // solve the problem
    switch(solveAlgo)
    {
    case DPLL:
        SETCONFLICTARGS(literals, clauses, DPLL, bInteractive, bDoc);
        SOLVE(literals, clauses, DPLL, in, out);
        break;
    case RND:
        SETCONFLICTARGS(literals, clauses, RND, bInteractive, bDoc);
        SOLVE(literals, clauses, RND, in, out);
        break;
    case MOMS:
        SETCONFLICTARGS(literals, clauses, MOMS, bInteractive, bDoc);
        SOLVE(literals, clauses, MOMS, in, out);
        break;
    case DLIS:
        SETCONFLICTARGS(literals, clauses, DLIS, bInteractive, bDoc);
        SOLVE(literals, clauses, DLIS, in, out);
        break;
    }

    return 0;
}
