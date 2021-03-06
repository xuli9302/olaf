/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/
#ifndef FINITESTRAINCRYSTALPLASTICITY_H
#define FINITESTRAINCRYSTALPLASTICITY_H

#include "FiniteStrainMaterial.h"
#include "ElementPropertyReadFile.h"

/**
 * FiniteStrainCrystalPlasticity uses the multiplicative decomposition of deformation gradient
 * and solves the PK2 stress residual equation at the intermediate configuration to evolve the material state.
 * The internal variables are updated using an interative predictor-corrector algorithm.
 * Backward Euler integration rule is used for the rate equations.
 */
class FiniteStrainCrystalPlasticity;

template<>
InputParameters validParams<FiniteStrainCrystalPlasticity>();

class FiniteStrainCrystalPlasticity : public FiniteStrainMaterial
{
public:
  FiniteStrainCrystalPlasticity(const std::string & name, InputParameters parameters);

protected:
  /**
   * This function updates the stress at a quadrature point.
   */
  virtual void computeQpStress();

  /**
   * This function updates the elasticity tensor at a quadrature point.
   * Presently void.
   */
  virtual void computeQpElasticityTensor();

  /**
   * This function initializes the stateful properties such as
   * stress, plastic deformation gradient, slip system resistances, etc.
   */
  virtual void initQpStatefulProperties();

  /**
   * This function calls the residual and jacobian functions used in the
   * stress update algorithm.
   */
  virtual void calc_resid_jacob(RankTwoTensor &, RankFourTensor &);

  /**
   * This function updates the slip increments.
   * And derivative of slip w.r.t. resolved shear stress.
   */
  virtual void getSlipIncrements();

  //Override to modify slip system resistance evolution
  /**
   * This function updates the slip system resistances.
   */
  virtual void update_slip_system_resistance();

  //Old function: Kept to avoid code break in computeQpStress
  /**
   * This function updates the slip system resistances.
   */
  virtual void updateGss();

  /**
   * This function reads slip system from file - see test.
   */
  virtual void getSlipSystems();
  /**
   * This function read euler angles from user object (optional) - see test.
   */
  virtual void getEulerAngles();

  /**
   * This function assign initial values of slip system resistances/internal variables
   * read from getSlipSystems().
   */
  virtual void assignSlipSysRes();

  /**
   * This function read slip system resistances from file - see test.
   */
  virtual void readFileInitSlipSysRes();

  /**
   * This function assign slip system resistances - see test.
   * .i input file format start_slip_sys_num, end_slip_sys_num, value.
   */
  virtual void getInitSlipSysRes();

  /**
   * This function read flow rate parameters from file - see test.
   */
  virtual void readFileFlowRateParams();

  /**
   * This function assign flow rate parameters - see test.
   * .i input file format start_slip_sys_num, end_slip_sys_num, value1, value2
   */
  virtual void getFlowRateParams();

  /**
   * This function read hardness parameters from file.
   */
  virtual void readFileHardnessParams();

  /**
   * This function assign flow rate parameters from .i file - see test.
   */
  virtual void getHardnessParams();

  /**
   * This function initializes slip system resistances.
   */
  virtual void initSlipSysProps();

  /**
   * This function initializes additional parameters.
   */
  virtual void initAdditionalProps();

  /**
   * This function set variables for stress and internal variable solve.
   */
  virtual void preSolveQp();

  /**
   * This function solves stress and internal variables.
   */
  virtual void solveQp();

  /**
   * This function update stress and internal variable after solve.
   */
  virtual void postSolveQp();

  /**
   * This function set variables for internal variable solve.
   */
  virtual void preSolveStatevar();

  /**
   * This function solves internal variables.
   */
  virtual void solveStatevar();

  /**
   * This function update internal variable after solve.
   */
  virtual void postSolveStatevar();

  /**
   * This function set variables for stress solve.
   */
  virtual void preSolveStress();

  /**
   * This function solves for stress, updates plastic deformation gradient.
   */
  virtual void solveStress();

  /**
   * This function update stress and plastic deformation gradient after solve.
   */
  virtual void postSolveStress();

  /**
   * This function calculate stress residual.
   */
  virtual void calcResidual( RankTwoTensor & );

  /**
   * This function calculate jacobian.
   */
  virtual void calcJacobian( RankFourTensor & );

  /**
   * This function calculates rotation tensor from Euler angles.
   */
  virtual void getEulerRotations();

  /**
   * This function calculate the tangent moduli for preconditioner.
   * Default is the elastic stiffness matrix.
   * Exact jacobian is currently implemented.
   * tan_mod_type can be modified to exact in .i file to turn it on.
   */
  virtual ElasticityTensorR4 calcTangentModuli();

  /**
   * This function calculate the elastic tangent moduli for preconditioner.
   */
  virtual ElasticityTensorR4 elasticTangentModuli();

  /**
   * This function calculate the exact tangent moduli for preconditioner.
   */
  virtual ElasticityTensorR4 elastoPlasticTangentModuli();

  /**
   * This function perform RU decomposition to obtain the rotation tensor.
   */
  RankTwoTensor get_current_rotation(const RankTwoTensor & a);

  ////Old function: Kept to avoid code break in computeQpStress
  /**
   * This function perform RU decomposition to obtain the rotation tensor.
   */
  RankTwoTensor getMatRot(const RankTwoTensor & a);

  /**
   * This function calculate the Schmid tensor.
   */
  void calc_schmid_tensor();

  /// Number of slip system resistance
  const unsigned int _nss;

  std::vector<Real> _gprops;
  std::vector<Real> _hprops;
  std::vector<Real> _flowprops;

  ///File should contain slip plane normal and direction. See test.
  std::string _slip_sys_file_name;

  ///File should contain initial values of the slip system resistances.
  std::string _slip_sys_res_prop_file_name;

  ///File should contain values of the flow rate equation parameters.
  ///Values for every slip system must be provided.
  ///Should have the same order of slip systens as in slip_sys_file. See test.
  ///The option of reading all the properties from .i is still present.
  std::string _slip_sys_flow_prop_file_name;

  ///The hardening parameters in this class are read from .i file. The user can override to read from file.
  std::string _slip_sys_hard_prop_file_name;

  ///Stress residual equation tolerance
  Real _rtol;
  ///Internal variable update equation tolerance
  Real _gtol;
  ///Slip increment tolerance
  Real _slip_incr_tol;

  ///Maximum number of iterations for stress update
  unsigned int _maxiter;
  ///Maximum number of iterations for internal variable update
  unsigned int _maxiterg;

  ///Number of slip system flow rate parameters
  unsigned int _num_slip_sys_flowrate_props;

  ///Element property read user object
  ///Presently used to read Euler angles -  see test
  const ElementPropertyReadFile * _read_prop_user_object;

  ///Type of tangent moduli calculation
  MooseEnum _tan_mod_type;

  ///Read from options for initial values of internal variables
  MooseEnum _intvar_read_type;

  ///Number of slip system specific properties provided in the file containing slip system normals and directions
  unsigned int _num_slip_sys_props;

  ///Flag to save euler angle as Material Property
  bool _save_euler_angle;

  bool _gen_rndm_stress_flag;

  ///Input option for scaling variable to generate random stress when convergence fails
  bool _input_rndm_scale_var;

  ///Scaling value
  Real _rndm_scale_var;

  ///Seed value
  unsigned int _rndm_seed;

  MaterialProperty<RankTwoTensor> & _fp;
  MaterialProperty<RankTwoTensor> & _fp_old;
  MaterialProperty<RankTwoTensor> & _pk2;
  MaterialProperty<RankTwoTensor> & _pk2_old;
  MaterialProperty<RankTwoTensor> & _lag_e;
  MaterialProperty<RankTwoTensor> & _lag_e_old;
  MaterialProperty<std::vector<Real> > & _gss;
  MaterialProperty<std::vector<Real> > & _gss_old;
  MaterialProperty<Real> & _acc_slip;
  MaterialProperty<Real> & _acc_slip_old;
  MaterialProperty<RankTwoTensor> & _update_rot;
  MaterialProperty<RankTwoTensor> & _update_rot_old;

  ///Save Euler angles for output only when save_euler_angle = true in .i file
  MaterialProperty< std::vector<Real> > * _euler_ang;
  MaterialProperty< std::vector<Real> > * _euler_ang_old;

  std::vector<Real> _mo;
  std::vector<Real> _no;

  std::vector<Real> _a0;
  std::vector<Real> _xm;

  RankTwoTensor _crysrot;

  Real _h0;
  Real _tau_sat;
  Real _tau_init;
  Real _r;

  RankTwoTensor _dfgrd_tmp;
  RankTwoTensor _fe, _fp_old_inv, _fp_inv;
  std::vector< Real > _slip_incr, _tau, _dslipdtau;
  std::vector<RankTwoTensor> _s0;

  RankTwoTensor _pk2_tmp;
  Real _accslip_tmp, _accslip_tmp_old;
  std::vector<Real> _gss_tmp;

  std::vector<Real> _slip_sys_props;

  bool _read_from_slip_sys_file;

  bool _err_tol;///Flag to check whether convergence is achieved

};

#endif //FINITESTRAINCRYSTALPLASTICITY_H
