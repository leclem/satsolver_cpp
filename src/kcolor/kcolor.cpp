#include "kcolor/kcolor.h"
#include "tseitin/tseitin.h"
#include <iostream>

#include <map>
#include <fstream>
#include <list>
#include <sstream>
#include <vector>
#include <algorithm>
std::map<int, std::list<int> > CKColor::connections;
std::string CKColor::fichierGraphe;
//retourne true si le fichier est bien formaté
bool lectureFile(std::ifstream & file, std::map<int, std::list<int> > & connections){
    bool error = false;
    int currentVertex;
    int otherVertex;
    std::string nextElem;
    while (file >> nextElem){

        if (nextElem.compare("e") == 0){
            file >> currentVertex;
            if (connections.count(currentVertex) == 0){
                std::list<int> newList;
                connections[currentVertex] = newList;
            }
            file >> otherVertex;
            connections[currentVertex].push_back(otherVertex);
        }
        else{
            std::cerr << currentVertex << ' '<< otherVertex << ' '<<nextElem;
            error = true;
            break;
        }
    }
    return !error;
}


void CKColor::translateIn(std::istream& in, std::ostream& result){
    int nbreColors;
    int nbreVertices;
    int nbreEdges;
    CTseitin::m_nbreVars = 0;
    int nbreClauses = 0;
    std::string nextElem;
    in >> nbreColors;

    in >> fichierGraphe;
    std::ifstream file;
    std::stringstream nonTraductedResult;

    file.open(fichierGraphe.c_str());
    if (file){
        file >> nextElem;
        while (nextElem.compare("c") == 0){
            getline(file, nextElem);
            file >> nextElem;
        }
        if (nextElem.compare("p") == 0 && file >> nextElem && nextElem.compare("edge") == 0 &&  (file >> nbreVertices >> nbreEdges) && lectureFile(file, connections)){
            //We are going to associate to each vertex the vertex that are connected to it


           // std::cout << nbreColors << ' '<< nbreEdges << ' ' << nbreVertices;
            //Every vertex has to be colored by a color
            for (int i=1;i<=nbreVertices;i++){
                for (int j=1;j<=nbreColors; j++){
                    nonTraductedResult << "X_" << i << '_' << j << ' ';
                }
                nbreClauses ++;
                nonTraductedResult << 0 << '\n';
            }

            //Every vertex is colored once
             for (int i=1;i<=nbreVertices;i++)
                for (int j=1;j<=nbreColors; j++)
                    for (int k=1;k<=nbreColors; k++)
                        if (j != k){
                            nonTraductedResult << "-X_" << i << '_' << j << ' ' << "-X_" << i << '_' << k << ' ' << 0 << '\n';
                            nbreClauses++;
                        }

            //We don't want the same color to be connected
            for (int i = 1; i <= nbreVertices;i++){
                for (int j=1;j<=nbreColors; j++){
                    for (std::list<int>::iterator it = connections[i].begin(); it != connections[i].end(); it++){
                         nonTraductedResult << "-X_" << i << '_' << j << ' ' << "-X_" << (*it) << '_' << j << ' ' << 0 << '\n';
                         nbreClauses++;
                    }
                }
            }
        }
        else{
            result << "Erreur : Mauvais formatage du fichier d'entrée \n";
        }
    }
    else{
        result << "Erreur : le fichier donné en argument n'existe pas \n";
    }
    std::stringstream traductedResult;
    CTseitin::countAndConvertVars(traductedResult, nonTraductedResult);
    result << "p cnf " << CTseitin::m_nbreVars << ' ' << nbreClauses << ' ';
    result << traductedResult.str();
}

void CKColor::translateOut(std::istream& out, std::ostream& result){
    std::stringstream streamTraduit;
    CTseitin::translateOut(out, streamTraduit);
    std::string nextElem;
    int numeroSommet;
    int numeroCouleur;
    unsigned i;
    std::vector<int> colorAssociated(CTseitin::m_nbreVars+1);
    streamTraduit >> nextElem >> nextElem;
    int nbreSommets = 0;
    if (nextElem.compare("SATISFIABLE") == 0){
        while (streamTraduit >> nextElem){
            if (nextElem[0] != '!'){
                //nextElem[1] = X nextElem[2] = _
                i = 2;
                numeroSommet = 0;
                while (nextElem[i] != '_'){
                    numeroSommet = numeroSommet*10 + (nextElem[i] - '0');
                    i++;
                }
                i++;
                numeroCouleur = 0;
                while (i != nextElem.size()){
                    numeroCouleur = numeroCouleur*10 + (nextElem[i] - '0');
                    i++;
                }
                result << "v "<< numeroSommet << " " << numeroCouleur << '\n';
                colorAssociated[numeroSommet] = numeroCouleur;
                nbreSommets++;
            }

        }
    }
    else{
        result << 's' << ' '<< nextElem << '\n';
    }
    std::ofstream file;
    fichierGraphe.insert(fichierGraphe.size(),".dot");
    file.open(fichierGraphe.c_str());

    file << "digraph G {\n";
    for (int i=1;i<=nbreSommets;i++){
        srand(colorAssociated[i]);
        file << i << " [style=filled,color=\"";
        float leftColor = (rand() % 1000)/1500.0 + (0.33333 / colorAssociated[i]);
        float middleColor= (rand() % 1000)/1500.0 + (0.3333 / colorAssociated[i]);
        float rightColor= (rand() % 1000)/1500.0 + (0.33333 / colorAssociated[i]);
        leftColor += 0.5 / colorAssociated[i];
        file << leftColor << ' ' << middleColor << ' ' << rightColor;
        file << "\"];\n";
    }


    for (int i = 1; i <= nbreSommets;i++){
        //we make the next color
        //colorAssociated[numeroSommet]
        /*file << "node [style=filled,color=\"";
        float leftColor = (colorAssociated[i] % 15) / 30 + (0.5 / colorAssociated[i]);
        float middleColor=(colorAssociated[i] % 10) / 30 + (0.5 / colorAssociated[i]);
        float rightColor=(colorAssociated[i] % 5) / 30 + (0.5 / colorAssociated[i]);
        leftColor += 0.5 / colorAssociated[i];
        file << leftColor << ' ' << middleColor << ' ' << rightColor;
        file << "\"];\n";*/
        for (std::list<int>::iterator it = connections[i].begin(); it != connections[i].end(); it++){


            //parse -> execute;
            file << i << " -> " << (*it) << '\n';
        }
    }
    file << "}";
  /*  std::stringstream nonTraductedResult;

    */
    //result << streamTraduit.str();
}
