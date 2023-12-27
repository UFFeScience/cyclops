/**
 * \file src/model/data.h
 * \brief Contains the \c Data class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2020
 *
 * This header file contains the \c Data class.
 */

#ifndef APPROXIMATE_SOLUTIONS_SRC_MODEL_DATA_H_
#define APPROXIMATE_SOLUTIONS_SRC_MODEL_DATA_H_

#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include <math.h>
#include <cstdlib>
// #include <array>

// using namespace std;

class Data {
public:
  // Data(int qtParam, char *instance);
  Data(std::string);
  ~Data();

  // static int dim;
  void readData();
  int getDimension() { return m_pNumber; };
  inline int getEdgeValue(int i, int j){ return m_edgeMatrix[i][j]; };
  inline int **getEdegeMatrix(){ return m_edgeMatrix; };

  static std::string getInstanceName(std::string); // Get instance's name

private:
  const std::string m_instanceName;
  int m_pNumber;
  int m_eNumber;

  // int array[0][0];

  int **m_edgeMatrix;
  // int m_edgeMatrix[][];
  // double *xCoord, *yCoord;

  // Computing Distances
  // static double CalcDistEuc(double *, double *, int , int);
  // static double CalcDistAtt(double *, double *, int , int);
  // static double CalcDistGeo(double *, double *, int , int);
  // static void CalcLatLong(double *, double *, int , double *, double *);
};

#endif  // APPROXIMATE_SOLUTIONS_SRC_MODEL_DATA_H_
