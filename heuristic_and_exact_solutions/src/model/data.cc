/**
 * \file src/model/data.cc
 * \brief Contains the \c Data class definition
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2020
 *
 * This source file contains the \c Data class definition
 */

#include "src/model/data.h"

Data::Data(std::string instance) : m_instanceName(instance) {
    // if (qtParam < 2) {
    //   cout << "Missing parameters\n";
    //   cout << " ./gcp [Instance] " << endl;
    //   exit(1);
    // }
    // if (qtParam > 2) {
    //   cout << "Too many parameters\n";
    //   cout << " ./gcp [Instance] " << endl;
    //   exit(1);
    // }
} // Data::Data(int qtParam, char *instance) {

Data::~Data() {
    for (int i = 0; i < m_pNumber; i++) {
        delete[] m_edgeMatrix[i];
    }
} // Data::~Data() {

void Data::readData() {
    int u, v;
    std::string line;
    std::string e, p;
    std::ifstream inGCP(m_instanceName.c_str());

    if (!inGCP) {
        std::cout << "File not found" << std::endl;
        exit(1);
    }

    getline(inGCP, line);
    std::istringstream iss(line);
    iss >> p >> e >> m_pNumber >> m_eNumber;

    // Alocar matriz 2D
    m_edgeMatrix = new int *[m_pNumber]; //memoria dinâmica (matrix 2D)

    for (int i = 0; i < m_pNumber; i++) {
        m_edgeMatrix[i] = new int[m_pNumber];
    }

    for (int i = 0; i < m_pNumber; i++) {
        for (int j = 0; j < m_pNumber; j++) {
            m_edgeMatrix[i][j] = 0;
        }
    }

    while (getline(inGCP, line)) {
        std::istringstream iss_2(line);
        if (!(iss_2 >> e >> u >> v)) {
            break;
        }
        m_edgeMatrix[u - 1][v - 1] = 1;
        m_edgeMatrix[v - 1][u - 1] = 1;
    }

    inGCP.close();
} // void Data::readData() {
