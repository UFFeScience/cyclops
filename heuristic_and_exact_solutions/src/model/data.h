/**
 * \file src/model/data.h
 * \brief Contains the \c Data class declaration
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo.raps@gmail.com\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2024
 *
 * This header file contains the \c Data class
 */

#ifndef APPROXIMATE_SOLUTIONS_SRC_MODEL_DATA_H_
#define APPROXIMATE_SOLUTIONS_SRC_MODEL_DATA_H_


#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include <math.h>
#include <cstdlib>

class Data {
public:
    Data(std::string &);

    ~Data();

    void readData();

    int getDimension() { return m_pNumber; };

    inline int getEdgeValue(int i, int j) { return m_edgeMatrix[i][j]; };

    inline int **getEdegeMatrix() { return m_edgeMatrix; };

    static std::string getInstanceName(std::string); // Get instance's name

private:
    const std::string m_instanceName;
    int m_pNumber{};
    int m_eNumber{};
    int **m_edgeMatrix{};
};

#endif  // APPROXIMATE_SOLUTIONS_SRC_MODEL_DATA_H_
