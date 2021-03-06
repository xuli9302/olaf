/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/
#include "INSCourant.h"

template<>
InputParameters validParams<INSCourant>()
{
  InputParameters params = validParams<AuxKernel>();

  // Coupled variables
  params.addRequiredCoupledVar("u", "x-velocity");
  params.addCoupledVar("v", "y-velocity"); // only required in 2D and 3D
  params.addCoupledVar("w", "z-velocity"); // only required in 3D

  return params;
}

INSCourant::INSCourant(const std::string & name, InputParameters parameters)
  :AuxKernel(name, parameters),
  _u_vel(coupledValue("u")),
  _v_vel(_mesh.dimension() >= 2 ? coupledValue("v") : _zero),
  _w_vel(_mesh.dimension() == 3 ? coupledValue("w") : _zero)
{}

Real
INSCourant::computeValue()
{
  RealVectorValue U(_u_vel[_qp], _v_vel[_qp], _w_vel[_qp]);

  Real vel_mag = U.size();

  // Don't divide by zero...
  vel_mag = std::max(vel_mag, std::numeric_limits<Real>::epsilon());

  return _current_elem->hmin() / vel_mag;
}
