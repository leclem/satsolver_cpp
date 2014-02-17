#include "tseitin/tseitin.h"
#include <stdlib.h>     /* atoi */
#include <iostream>
#include <cstdio>
#include <sstream>
bool CTseitin::m_error;
int CTseitin::m_nbreVars;
int CTseitin::m_nbreClauses;
std::map<int, std::string> CTseitin::m_convertion;
std::map<std::string, int> CTseitin::m_convertionInverse;
std::vector<int> CTseitin::variablesTseitin;



void CTseitin::countAndConvertVars(std::stringstream & retour, std::istream & in){
    std::string nextElem;
    while ((in >> nextElem)){

        if ((nextElem.compare("=>") == 0) || nextElem.compare("(") == 0 || nextElem.compare(")") == 0 ||(nextElem.compare("/\\") == 0) || (nextElem.compare("&") == 0) || (nextElem.compare("\\/") == 0) || (nextElem.compare("|") == 0) || (nextElem.compare("0") == 0)){
            retour << nextElem << ' ' ;
        }
        else{
            if (nextElem[0] == '-'){//Dans le dans d'un élément négatif
                retour << '-' ;
                nextElem.erase (0, 1);
            }
            if (nextElem.compare("!") == 0){

                in >> nextElem;
                if (nextElem.compare("(") != 0){
                    retour << '-' ;
                }
                else{
                    retour << '!' << ' ' << '(' << ' ';
                    continue;
                }
            }
            if (m_convertionInverse.count(nextElem) > 0){ // Si on a déjà l'élément
                retour << m_convertionInverse[nextElem] << ' ';
            }
            else{
                m_nbreVars++;
                m_convertionInverse[nextElem] = m_nbreVars;
                m_convertion[m_nbreVars] = nextElem;
                retour << m_nbreVars << ' ' ;
            }
        }
    }
    //std::cerr << retour.str();
}

void CTseitin::syntaxModifier(std::stringstream & retour, std::istream & in){
    std::string elem;
 std::stringstream elemWithAnd;
 char currentChar;
 bool emptyLine = true;

 elemWithAnd << '(' << ' ';
while (in.good()){
    currentChar = in.get();
    while (currentChar == '\n' && in.peek() == '\n'){
        in.get();
    }
    if (currentChar == EOF){
        break;
    }
    if (!emptyLine && currentChar == '\n' && in.peek() != EOF){
        elemWithAnd << ' '<<')' <<' ' << '&' << ' ' << '(' << ' ';
        emptyLine = true;
    }
    else if (in.good() && currentChar!= '\n'){
        if (currentChar != ' ')
            emptyLine = false;
        elemWithAnd << currentChar;
    }

}
elemWithAnd << ' '<<')';
 std::stringstream temp;
 std::stringstream temp2;
    while (elemWithAnd >> elem){
        // Placer les parentheses autour des AND
        // espacer les caractères

        if (elem.compare("=>") == 0 || elem.compare ("\\/") == 0 || elem.compare ("(") == 0 || elem.compare (")") == 0 || elem.compare("/\\") == 0 || elem.compare("!") == 0 || elem.compare("&") == 0 || elem.compare("|") == 0){
            retour << elem << ' ';
        }
        else{
        //On parcourt tout elem à la recherche d'erreurs
        for (unsigned i=0;i<elem.size();i++){
            //We first correct usual syntax

            if (

                (
                (elem[i] == '\\' && elem[i+1] == '/')
                  ||
                (elem[i] == '/' && elem[i+1] == '\\')
                  ||
                (elem[i] == '=' && elem[i+1] == '>')
                )
                )
                {
                    retour << ' '<<elem[i] << elem[i+1] << ' ';
                    i++;
                }
            else if (

                !
                (
                  (elem[i] >= 'A' && elem[i] <= 'Z')
                  ||
                (elem[i] >= 'a' && elem[i] <= 'z')
                  ||
                (elem[i] >= '0' && elem[i] <= '9')
                  ||
                elem[i] == '_'
                  ||
                (elem[i] == ' ')
                  ||
                (elem[i] == '-')
                  )

                ){
               // fin = fin+2;
                retour << ' ' << elem[i] << ' ';
            }
            else{
                retour << elem[i];
            }
        }
        retour << ' ';
        }
        //retour << elem << ' ';
    }
   // std::cerr << retour.str();
}

void CTseitin::translateIn(std::istream& in, std::ostream& out)
{
    m_error = false;
    std::stringstream result;
    std::stringstream sin;
    m_nbreVars = 0;
    m_nbreClauses= 0;
    int finalVar;

    std::stringstream inCorrect;
    syntaxModifier(inCorrect, in);
    countAndConvertVars(sin, inCorrect);
    variablesTseitin.resize(m_nbreVars+1);
    for (int i=0;i < m_nbreVars; i++)
        variablesTseitin[i] = 0;
    CTseitin::tseitinSub(result, sin, 4, &finalVar);


    out << "p cnf " << m_nbreVars << ' ' << m_nbreClauses << '\n';
    out << result.str();
    if (m_error == true){
        std::stringstream trash;
        std::cerr << "Erreur : mauvais formatage des données \n";
        trash << out;
        out << "0 0";
    }
}
int CTseitin::fresh(){
    m_nbreVars++;
    return m_nbreVars;
}

//Level : 3 = => ; 4 = /\ ; 2 = \/ ; 1 = ~
void CTseitin::tseitinSub(std::stringstream& out,  std::istream& in, int level, int * variableFormula){
    std::stringstream left;
    std::stringstream middle;
    std::stringstream right;
    std::string nextElem;
    int variableOne;
    int variableTwo;
    bool negativeParenthesis = false;; // If we are making a arenthesis negative
    bool onlyleft = true; //If there isn't a right part, we didn't found the symbol we were looking for
    bool parenthesisRight = true; // If there is something on the right of the parenthesis
    bool thereIsParenthesis = false; // If we found a parenthesis
    int inHowManyParenthesisWeAre = 0; // We don't want to split our formula if we are in a parenthesis because we will get a ( on one side and a ) on the other side
    int numberOfLeftParenthesis = 0;
    //We look if the first character is a parenthesis
    in.ignore(0, ' ');//We discard the first character if it is a space
    char firstChar = in.peek();
    //std::cerr << '\n' << (char) firstChar << 'b';

    if (firstChar == '(' || firstChar == '!'){ // We found a parenthesis, so we read priorities differently
        if (firstChar == '!'){
            in.get();
            in.get();
            negativeParenthesis = true;
        }


        thereIsParenthesis = true;
        in >> nextElem;

        while (in >> nextElem && ! ( nextElem[0] == ')' && numberOfLeftParenthesis == 0)){
            left << nextElem << ' ' ;

            if (nextElem[0] == '('){
                numberOfLeftParenthesis++;
               // std::cerr << '1';
            }
            if (nextElem[0] == ')'){
                numberOfLeftParenthesis--;
              //  std::cerr << '-';
            }
        }
        std::string nextElemRight;



        if (numberOfLeftParenthesis > 0){
            while (numberOfLeftParenthesis != 0){
                in >> nextElem;
                left << nextElem << ' ';
                if (nextElem[0] == ')')
                    numberOfLeftParenthesis --;
            }
        }
        in >> nextElem;
        if (!in.eof()){
                onlyleft = false;
                parenthesisRight = false;
        }
        if (parenthesisRight){
            if (negativeParenthesis){
                *variableFormula = fresh();
                int variableNeg;
                tseitinSub(out, left, 4, &variableNeg);
                out << *variableFormula*(-1) << ' '<<variableNeg*(-1) << ' ' << 0 << '\n';
                out << *variableFormula << ' '<<variableNeg << ' ' << 0 << '\n';
                m_nbreClauses = m_nbreClauses+2;
                return;
            }
            else{
                tseitinSub(out, left, 4, variableFormula);
                return;
            }
        }
        /*if (!parenthesisRight && negativeParenthesis){
            std::string newstring = left.str();
            newstring.insert(newstring.size()," )");
            //thereIsParenthesis=false;
            left.str(newstring);
            left << "! ( ";
            std::cerr << left.str() << '\n';
        }*/
    }
    else if (level != 1){

        while (in >> nextElem){
                if (
                (inHowManyParenthesisWeAre == 0)
                &&
                (
               (nextElem.compare("=>") == 0 && level == 4)
               ||
               (((nextElem.compare("/\\") == 0) || (nextElem[0] == '&')) && level == 2)
               ||
                (((nextElem.compare("\\/") == 0) || (nextElem[0] == '|') ) && level == 3))
                )
        {
            break;
        }
        else{
            if (nextElem[0] == '('){
                inHowManyParenthesisWeAre ++;
            }
            if (nextElem[0] == ')'){
                inHowManyParenthesisWeAre--;
            }
            left << nextElem << ' ' ;
        }
        }


        std::string rightElem;
        if (!in.eof()){
            onlyleft = false;
        }
        if (onlyleft){
            tseitinSub(out, left, level-1, variableFormula);
        }
    }
    //First we search "=>" because it has the highest priority
        //We search for the first =>

    if ((nextElem.compare("=>") == 0)){ // If we found a =>
                int newVar = CTseitin::fresh();
                *variableFormula = newVar;
                if (thereIsParenthesis){


                    tseitinSub(out, left, 4, &variableOne);
                    if (negativeParenthesis){
                        out << *variableFormula*(-1) << ' '<<variableOne*(-1) << ' ' << 0 << '\n';
                        out << *variableFormula << ' '<<variableOne << ' ' << 0 << '\n';
                        m_nbreClauses = m_nbreClauses+2;
                    }
                }
                else{
                    tseitinSub(out, left, 3, &variableOne); // We are sure that there is no more => on the right
                }
                tseitinSub(out, in, 4, &variableTwo);
                //µn le transforme en ça puis on convertira plus tard avec tseitin
                std::stringstream newForm;
                out << variableOne*(-1) << ' '<<variableTwo << ' ' << 0 << '\n';
                m_nbreClauses ++;
        }
    else if (((nextElem.compare("/\\") == 0) || (nextElem[0] == '&'))){
                if (thereIsParenthesis){
                    tseitinSub(out, left, 4, &variableOne);
                    if (negativeParenthesis){
                        std::cerr << 'r';
                        out << *variableFormula*(-1) << ' '<<variableOne*(-1) << ' ' << 0 << '\n';
                        out << *variableFormula << ' '<<variableOne << ' ' << 0 << '\n';
                        m_nbreClauses = m_nbreClauses+2;
                    }
                }
                else{
                    tseitinSub(out, left, 1, &variableOne); // We are sure that there is no more /\ on the right
                }
                tseitinSub(out, in, 2, &variableTwo);
                int newVar = CTseitin::fresh();
                *variableFormula = newVar;

                out << newVar*(-1) << ' ' << variableOne << ' ' << '0' << '\n' << newVar*(-1) << ' ' << variableTwo << ' ' << '0' << '\n' << newVar << ' ' << variableOne*(-1) << ' ' << variableTwo*(-1) << ' ' << '0' << '\n';
                m_nbreClauses = m_nbreClauses+3;
    }
    else if (((nextElem.compare("\\/") == 0) || (nextElem[0] == '|'))){

                if (thereIsParenthesis){
                    tseitinSub(out, left, 4, &variableOne);
                    if (negativeParenthesis){
                        out << *variableFormula*(-1) << ' '<<variableOne*(-1) << ' ' << 0 << '\n';
                        out << *variableFormula << ' '<<variableOne << ' ' << 0 << '\n';
                        m_nbreClauses = m_nbreClauses+2;
                    }
                }
                else{
                    tseitinSub(out, left, 2, &variableOne);
                }
                tseitinSub(out, in, 3, &variableTwo);
                int newVar = CTseitin::fresh();
                *variableFormula = newVar;
                m_nbreClauses = m_nbreClauses+3;
                out << newVar*(-1) << ' ' << variableOne << ' ' << variableTwo << ' ' << '0' << '\n' << newVar << ' ' << variableOne*(-1) << ' ' << '0' << '\n' << newVar << ' ' << variableTwo*(-1) << ' ' << '0' << '\n';
    }
    else if (level == 1){
        // Les - et les +
        /*
            -p1 : (-p or -p1) ^ (p or 1)
            p1 : (-p or p1) ^ (p ^-p1)
        */
        int nextInt;
       // bool lastElemZero = false;
        int newVar;
        in >> nextInt;
        bool neg = false;
        if (nextInt < 0){
            if (nextInt < 0){
                neg = true;
                nextInt = nextInt * (-1);
            }
            if (variablesTseitin[nextInt] == 0){
                 newVar = CTseitin::fresh();
                *variableFormula = newVar;
                variablesTseitin[nextInt] = newVar;

            }
            else{
                *variableFormula = variablesTseitin[nextInt];

            }
            if (neg){
                nextInt = nextInt * (-1);
            }

            m_nbreClauses = m_nbreClauses+2;
                out << (*variableFormula*(-1)) << ' ' << nextInt << ' ' << 0 << '\n' << *variableFormula << ' ' << (nextInt*(-1)) << ' ' << '0' << '\n';
           //}
        }
        else{
            *variableFormula = nextInt;
        }
       /* if (nextInt != 0){
            if (nextInt < 0){
                neg = true;
                nextInt = nextInt * (-1);
            }
            if (variablesTseitin[nextInt] == 0){
                 newVar = CTseitin::fresh();
                *variableFormula = newVar;
                variablesTseitin[nextInt] = newVar;

            }
            else{
                *variableFormula = variablesTseitin[nextInt];

            }
            if (neg){
                nextInt = nextInt * (-1);
            }

            m_nbreClauses = m_nbreClauses+2;
                out << (*variableFormula*(-1)) << ' ' << nextInt << ' ' << 0 << '\n' << *variableFormula << ' ' << (nextInt*(-1)) << ' ' << '0' << '\n';
           //}
        }*/
    }
}
void CTseitin::translateOut(std::istream& out, std::ostream& result){

    std::string nextElem;
    int nextNumber;
    bool negative = false;
    while ((out>> nextElem)){

        if ((nextElem.compare("v") == 0)){ // We only translate variables
            out >> nextNumber;
            //If the variable is negative we make it positive to find it on the map
            if (nextNumber < 0){
                negative = true;
                nextNumber = nextNumber*(-1);
            }
            if (m_convertion.count(nextNumber)){//We only care about the variables that ere here on the first place. We discard others
                if (negative)
                    result << '!'  << m_convertion[nextNumber] << ' ' << '\n';
                else
                    result  << m_convertion[nextNumber] << ' ' << '\n';
            }
        }
        else{
            result << nextElem << ' ';
            if (nextElem.compare("SATISFIABLE") == 0 || nextElem.compare("UNSATISFIABLE") == 0){
                result << '\n';
            }
        }
        negative = false;
    }
}
