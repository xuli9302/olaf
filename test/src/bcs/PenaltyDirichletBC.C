/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/
#include "PenaltyDirichletBC.h"
#include "Function.h"

template<>
InputParameters validParams<PenaltyDirichletBC>()
{
  InputParameters params = validParams<IntegratedBC>();
  params.addParam<Real>("penalty",1e5,"Penalty scalar");
  params.addParam<Real>("value", 0.0, "Boundary value of the variable");
  params.addRequiredParam<FunctionName>("function", "Forcing function");

  return params;
}

PenaltyDirichletBC::PenaltyDirichletBC(const std::string & name, InputParameters parameters) :
    IntegratedBC(name, parameters),
    _func(getFunction("function")),
    _p(getParam<Real>("penalty")),
    _v(getParam<Real>("value"))
{}

Real
PenaltyDirichletBC::computeQpResidual()
{
  return _p*_test[_i][_qp]*(-_v + _u[_qp]);
}

Real
PenaltyDirichletBC::computeQpJacobian()
{
  return _p*_phi[_j][_qp]*_test[_i][_qp];
}
