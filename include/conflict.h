#ifndef CCONFLICT_H
#define CCONFLICT_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

#include "choice.h"
#include "base/sat.h"

template <SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
class CConflict
{

    typedef CClause<_ALG, _WAT, _CL> Clause;
    typedef typename ClausePtr<_ALG, _WAT, _CL>::type ClausePtr;
    typedef typename VariablePtr<_ALG, _WAT, _CL>::type VariablePtr;
    typedef CLiteral<_ALG, _WAT, _CL> Literal;
    typedef typename std::list<Literal>::iterator LitIter;
    typedef CSAT<_ALG, _WAT, _CL> SAT;

public:
    static void activate(){
        conflict_activated=true;
    }
    static void desactivate(){
        conflict_activated=false;
    }
    static void conflictManager(SAT&, ClausePtr , Literal, unsigned);

    // used as initializer
    static void setArgs(bool _i, bool _d)
        { conflict_activated = _i, genDoc = _d; m_histStep = -1;}

    static void generateDotConflictGraph(SAT&, ClausePtr, Literal, unsigned);
    static void updHistory(SAT&, ClausePtr, Literal, unsigned);
    static void loadHistory(int);
    static void parseHist();

    static void resolutionProof(SAT& sat, Literal, unsigned);
    static void clauseInFile(std::ostream & out, CClause<_ALG,_WAT,_CL>&, CLiteral<_ALG,_WAT,_CL> current);

private:
    CConflict(){}
    virtual ~CConflict(){}
    static bool conflict_activated;
    static bool genDoc;
    static int m_conflictsToJump;
    static std::ofstream m_history;
    static int m_histStep;
};

///============================= IMPLEMENTATION ===========================================


template <SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
bool CConflict<_ALG,_WAT,_CL>::conflict_activated = true;

template <SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
bool CConflict<_ALG,_WAT,_CL>::genDoc = true;

template <SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
int CConflict<_ALG,_WAT,_CL>::m_conflictsToJump = 0;

template  <SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
std::ofstream CConflict<_ALG,_WAT,_CL>::m_history("/dev/null");

template  <SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
int CConflict<_ALG,_WAT,_CL>::m_histStep = -1;



template <SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
void CConflict<_ALG,_WAT,_CL>::conflictManager(SAT& sat, ClausePtr learned, Literal UIP, unsigned level)
{
    if(genDoc)
    {
        updHistory(sat, learned, UIP, level);
    }
    if(!conflict_activated)
        return;

    if (m_conflictsToJump > 0)
    {
        m_conflictsToJump--;
        return;
    }
    std::cout << "Conflit détecté\n";
    std::string command;
    while(true)
    {
        std::cout << "> ";
        std::cin >> command;
        if (command == std::string("g"))
        {
            generateDotConflictGraph(sat, learned, UIP, level);
            std::cout << "Graph info stored in graph.dot\n";
        // génération du graphe des conflits
        }
        else if (command == std::string("r"))
        {
                //afficher la preuve par résolution qui aboutit à la clause qui est ajoutée lors du conflit courrant
                resolutionProof(sat, UIP, level);

                 std::cout << "Resolution proof stored into proof.tex\n";

        }
        else if (command == std::string("s"))
        {
            //Pour continuer et afficher n conflits plus loin
            std::cin >> m_conflictsToJump;
            break;
        }
        else if (command == std::string("t")) //pour terminer l'execution sans s'arrêter
        {
            desactivate();
            break;
        }
        else if (command == std::string("c")) //continuer jusqu'au prochain conflit
            break;
        else
        {
            std::cerr << "Commande non reconnue : " << command << std::endl;
        }
    }
}

template <SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
void CConflict<_ALG,_WAT,_CL>::resolutionProof(SAT& sat, Literal UIP,  unsigned level){
    ClausePtr cl = sat.getConflictClause();

    std::ofstream out("proof.tex");
    out << "\\documentclass{article}";
    out <<"\\usepackage{mathpartir}\n" << "\\usepackage[utf8]{inputenc}\n";
    out << "\\newcommand{\\non}[1]{\\overline{#1}}\n" << "\\newcommand{\\varv}[1]{x_{#1}}\n" << "\\newcommand{\\varf}[1]{\\non{\\varv{#1}}}\n" << "\\newcommand{\\cl}[1]{\\mathtt{C_{#1}}:~}\n" << "\\newcommand{\\preuve}[1]{\\mathtt{\\Pi_{#1}}}\n" << "\\begin{document}\n";

    out << "Preuve de résolution pour la clause : \n ";

    out << "\\begin{mathpar}";
    std::stringstream stream;
    std::stringstream crap;
    CLiteral<_ALG,_WAT,_CL> lit;
    clauseInFile(out, *cl, lit);
    clauseInFile(stream, *cl, lit);

    out << '\\' << '\\';
    std::stringstream streamTemp;
    std::list<CLiteral<_ALG,_WAT,_CL> > clForPrint;
    CClause<_ALG,_WAT,_CL> clauseEnCours = *cl;

    bool stopWhile = true;
    bool onlyLearned = false;
    do{

        stopWhile=true;
        onlyLearned = true;
        for(LitIter it = clauseEnCours.assigneds(false).begin(); it != clauseEnCours.assigneds(false).end(); it++){
            if (it->variable()->level() == level && (UIP.variable() != it->variable())){
                onlyLearned = false;
                streamTemp << " \\inferrule{";
                ClausePtr other = it->variable()->deducClause();
                clauseInFile(streamTemp, *other, (*it));
                streamTemp << " \\and";
                streamTemp << stream.str();
                stream.str("");
                streamTemp << '}';
                streamTemp << '{';

                std::list<CLiteral<_ALG,_WAT,_CL> > futureclForPrint;
                for(typename std::list<CLiteral<_ALG,_WAT,_CL> >::iterator it2 = clForPrint.begin(); it2 != clForPrint.end(); it2++){
                    if (it2->variable() != it->variable()){
                        futureclForPrint.push_back(*it2);
                    }
                }

                for(LitIter it2 = other->assigneds(false).begin(); it2 !=other->assigneds(false).end(); it2++){
                    if (it2->variable() != it->variable()){
                        futureclForPrint.push_back(*it2);
                     }
                }

                CClause<_ALG,_WAT,_CL> secondClause;//The last clause generated
                for (typename std::list<CLiteral<_ALG,_WAT,_CL> >::iterator it2 = futureclForPrint.begin(); it2 != futureclForPrint.end(); it2++){
                        secondClause.addLiteral((*it2));
                }

                clForPrint = futureclForPrint;
                clauseInFile(streamTemp, secondClause, (*it));

                streamTemp << '}';
                stream << streamTemp.str();
                streamTemp.str("");
            }
        }
        if (!onlyLearned){
            CClause<_ALG,_WAT,_CL> futureClauseEnCours;
            for (typename std::list<CLiteral<_ALG,_WAT,_CL> >::iterator it2 = clForPrint.begin(); it2 != clForPrint.end(); it2++){
                    futureClauseEnCours.addLiteral((*it2));
                    stopWhile = false;
            }
            clauseEnCours = futureClauseEnCours;
        }
        }
        while(!stopWhile);
    out <<stream.str();
    streamTemp.str("");
    out << " \\end{mathpar}";
    out << " \\end{document}";
}


template <SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
void CConflict<_ALG,_WAT,_CL>::clauseInFile(std::ostream & out, CClause<_ALG,_WAT,_CL> & cl, CLiteral<_ALG,_WAT,_CL> current){
    bool first = true;
	for(LitIter it = cl.assigneds(false).begin(); it != cl.assigneds(false).end(); it++){
		if (!first){
			out << " \\lor";
			first = false;
		}
		if (it->logicValue()){

		    if (current.variable() == it->variable()){
		        out << "\\mathbf{\\varv{" << it->variable()->name() << "}}";
		    }
		    else{
		        out << " \\varv{" << it->variable()->name() << "}";
		    }
		}
		else{
		     if (current.variable() == it->variable()){
		        out << "\\mathbf{\\varf{" << it->variable()->name() << "}}";
		    }
		    else{
			out << " \\varf{" << it->variable()->name() << "}";
		    }
		}
	}
	for(LitIter it = cl.assigneds(true).begin(); it != cl.assigneds(true).end(); it++){
		if (!first){
			out << "\\lor";
			first = false;
		}
		if (it->logicValue()){
			out << "\\mathbf{\\varv{" << it->variable()->name() << "}}";
		}
		else{
			out << "\\mathbf{\\varf{" << it->variable()->name() << "}}";
		}
	}
}

template <SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
void CConflict<_ALG,_WAT,_CL>::generateDotConflictGraph(SAT& sat, ClausePtr learned, Literal UIP, unsigned level)
{
    // clause that create the conflict
    ClausePtr conflict = sat.getConflictClause();
    //clauses to parse
    std::list<ClausePtr> parse;
    parse.push_front(conflict);
    // first parse to put all descr to 0
    while(!parse.empty())
    {
        ClausePtr cl = parse.front();
        parse.pop_front();
        for(LitIter it = cl->assigneds(false).begin(); it != cl->assigneds(false).end(); it++)
        {
            it->variable()->graphDescr() = 0;
            if(it->variable()->level() == level && !it->variable()->bet())
                parse.push_back(it->variable()->deducClause());
        }
    }
    // set a descriptor of -1 for all the learned clause's literals
    for(LitIter it = learned->assigneds(false).begin(); it != learned->assigneds(false).end(); it++)
        it->variable()->graphDescr() = -1;
    // now we can create the file
    std::ofstream out("graph.dot");
    out << "digraph G {\nsize =\"4,4\";\nrankdir=\"LR\";\n";
    out << "conflict [style=filled,color=red];\n";

    std::list<Literal> parseVar;
    std::string current = "conflict";
    ClausePtr cl = conflict;


    while(true)
    {
        for(LitIter it = cl->assigneds(false).begin(); it != cl->assigneds(false).end(); it++)
        {

            if(it->variable()->graphDescr()<=0)
            {
                Literal label = (it->variable()->level() != level)?(*it):(!(*it));
                out << "lit_" << it->variable()->name() << " [label=\"" << label << "\"";
                if(*it == UIP) //UIP
                {
                    if(it->variable()->bet())
                        out << ",shape=box";
                    out << ",style=filled,color=yellow]";
                }
                else if(it->variable()->level() != level)
                { // variable of different level
                    out << ",fontsize=8";
                    if (it->variable()->graphDescr() == -1)
                        out << ",style=filled,color=\".7 .3 1.0\""; // the one of the clause
                    out << ",shape=circle]";
                }
                else if(it->variable()->bet()) // the bet one
                    out << ",shape=box,style=filled,color=green]";
                else // normal ones
                    out << ",shape=circle,style=filled,color=blue]";
                out << ";\n";
            }


            out << "lit_" << it->variable()->name() << " -> " << current << ";\n";

            if(it->variable()->graphDescr()<=0 && !it->variable()->bet() && it->variable()->level() == level)
                parseVar.push_back(*it);

            it->variable()->graphDescr() = 1;
        }
        if(parseVar.empty())
            break;
        Literal lit = parseVar.front();
        parseVar.pop_front();
        current = std::string("lit_") + lit.variable()->name();
        cl = lit.variable()->deducClause();
    }
    out << "}";
}

template <SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
void CConflict<_ALG,_WAT,_CL>::updHistory(SAT& sat, ClausePtr learned, Literal UIP, unsigned level)
{
    if(m_histStep++<0)
    {
        m_history.close();
        m_history.open("history.slh");
    }

    m_history<< "#"<<m_histStep<<"\n";

    // clause that create the conflict
    ClausePtr conflict = sat.getConflictClause();
    //clauses to parse
    std::list<ClausePtr> parse;
    parse.push_front(conflict);
    // first parse to put all descr to 0
    while(!parse.empty())
    {
        ClausePtr cl = parse.front();
        parse.pop_front();
        for(LitIter it = cl->assigneds(false).begin(); it != cl->assigneds(false).end(); it++)
        {
            it->variable()->graphDescr() = 0;
            if(it->variable()->level() == level && !it->variable()->bet())
                parse.push_back(it->variable()->deducClause());
        }
    }
    // then we make another pass to create all variables
    parse.push_front(conflict);
    while(!parse.empty())
    {
        ClausePtr cl = parse.front();
        parse.pop_front();
        for(LitIter it = cl->assigneds(false).begin(); it != cl->assigneds(false).end(); it++)
            if(!it->variable()->graphDescr())
        {
            it->variable()->graphDescr() = 1;
            m_history << ((it->variable()->bet())?'b':'d') << ((it->variable()->level() == level)?'i':'e');
            m_history << it->variable()->name() << ' ';
            if(it->variable()->level() == level && !it->variable()->bet())
                parse.push_back(it->variable()->deducClause());
        }
    }
    m_history << ";\n";

    //then we do a third pass to store the clauses. The first one is the confict one, the other begin by the deduced var
    parse.push_front(conflict);
    while(!parse.empty())
    {
        ClausePtr cl = parse.front();
        parse.pop_front();
        m_history << "C ";
        if(!cl->assigneds(true).empty())
        {
            Literal ded = cl->assigneds(true).front();
            m_history << ((ded.logicValue())?'+':'-') << ded.variable()->name() <<' ';
        }

        for(LitIter it = cl->assigneds(false).begin(); it != cl->assigneds(false).end(); it++)
        {
            m_history << ((it->logicValue())?'+':'-') << it->variable()->name() <<' ';
            if(it->variable()->graphDescr())
            {
                it->variable()->graphDescr() = 0;
                if(it->variable()->level() == level && !it->variable()->bet())
                    parse.push_back(it->variable()->deducClause());
            }
        }
        m_history << ";\n";
    }
    m_history << "L ";
    // print learned clause
    for(LitIter it = learned->assigneds(false).begin(); it != learned->assigneds(false).end(); it++)
        m_history<< ((it->logicValue())?'+':'-') << it->variable()->name() << " ";
    m_history << ";\n";

    //print UIP
    m_history<< ((UIP.logicValue())?'+':'-') << UIP.variable()->name() << "\n";

}

template <SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
void CConflict<_ALG,_WAT,_CL>::loadHistory(int n)
{
    std::ifstream in("history.slh");
    int cur = -1;
    char ch = '\0';
    while(cur != n)
    {
        while(ch != '#')
            if(!in.get(ch))
            {
                std::cout << "No as much entries in history : " << n << std::endl;
                return;
            }
        in >> cur;
    }

    SAT sat;
    std::map<std::string, VariablePtr> table;

    // load the vars
    while(true)
    {
        in.get(ch);
        if (ch == ' ' || ch == '\n')
            continue;
        if(ch == ';')
            break;
        bool bet = (ch=='b');
        in.get(ch);
        int lev = (ch=='i')?1:0;
        std::string nm;
        in >> nm;
        VariablePtr p = sat.createVar(nm);
        p->level() = lev;
        p->bet() = bet;
        table[nm] = p;
    }

    // load the clauses

    int cnt = 0;
    int cnt2 = 0;
    Clause cl;
    Literal first;

    while(true)
    {
        in >> ch;
        if (ch=='L')
            break;
        if (ch == ';')
        {
            ClausePtr cp = sat.addClause(cl);
            if(cnt == 0)
                sat.getConflictClause() = cp;
            else
            {
                sat.assignVar(first.variable(), first.logicValue());
                first.variable()->deducClause() = cp;
            }
            for(LitIter it = cp->unassigneds().begin(); it != cp->unassigneds().end();)
            {
                LitIter tmp = it++;
                sat.assignVar(tmp->variable(), !tmp->logicValue());
            }
            cnt++;
            cl = Clause();
            cnt2 = 0;
            continue;
        }

        if(ch == ' ' || ch == 'C' || ch == '\n')
            continue;

        std::string nm;
        in >> nm;
        Literal nlit(table[nm], (ch=='+'));
        cl.addLiteral(nlit);
        if(!cnt2)
            first = nlit;
        cnt2++;
    }

    std::cout << "Backtrack " << n << ":\nClause Learned: ( ";

    cl = Clause();

    bool b = true;

    while(true)
    {
        in >> ch;
        if (ch == ';')
            break;
        if(ch == ' ')
            continue;
        std::string nm;
        in >> nm;
        Literal nlit(table[nm], (ch=='+'));
        cl.addLiteral(nlit);
        if(!b)
            std::cout << "| ";
        else
            b = false;
        std::cout << ((ch=='-')?"!":"") << nm << ' ';
    }
    ClausePtr learned = sat.addClause(cl);

    std::cout << ")\n";
    ch = ' ';
    while (ch == ' ' || ch == '\n')
        in >> ch;
    std::string nm;
    in >> nm;
    Literal UIP(table[nm], (ch=='+'));

    generateDotConflictGraph(sat, learned, UIP, 1);
    std::cout << "Graph info stored in graph.dot\n";
}

template <SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
void CConflict<_ALG,_WAT,_CL>::parseHist()
{
    std::ifstream in;
    in.open("history.slh");
    if(!in.is_open())
    {
        std::cout << "Unable to open \"history.slh\"\n";
        std::cout << "Make sure to launch the program with -l flag before\n";
        return;
    }
    in.close();
    std::cout << "History parser\n";
    std::cout << "Which entry do you want to see ? (-1 to quit)\n";
    int p;
    while(true)
    {
        std::cout << "> ";
        std::cin >> p;
        if (p == -1)
            return;
        if (p<0)
            std::cout << "Invalid entry\n";
        else
            loadHistory(p);
    }

}
#endif // CCONFLICT_H
