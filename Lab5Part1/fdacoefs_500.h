/*
 * Filter Coefficients (C Source) generated by the Filter Design and Analysis Tool
 * Generated by MATLAB(R) 9.4 and Signal Processing Toolbox 8.0.
 * Generated on: 02-Dec-2018 23:44:23
 */

/*
 * Discrete-Time FIR Filter (real)
 * -------------------------------
 * Filter Structure  : Direct-Form FIR
 * Filter Length     : 501
 * Stable            : Yes
 * Linear Phase      : Yes (Type 1)
 */

/* General type conversion for MATLAB generated C-code  */
/* 
 * Expected path to tmwtypes.h 
 * C:\Program Files\MATLAB\R2018a\extern\include\tmwtypes.h 
 */
/*
 * Warning - Filter coefficients were truncated to fit specified data type.  
 *   The resulting response may not match generated theoretical response.
 *   Use the Filter Design & Analysis Tool to design accurate
 *   single-precision filter coefficients.
 */
const int BL = 501;
const float B[501] = {
  -0.001192671945,-0.0003509034868,-5.18943404e-17,0.0003540957114, 0.001214470947,
  -0.002082715044,5.884208183e-17, 0.002101904014,-0.001236953423,-0.0003639739589,
  4.405157631e-17,0.0003673714236, 0.001260153833,-0.002161317272,-3.756025743e-18,
   0.002181763295,-0.001284108846,-0.0003778967075,-7.368754522e-17,0.0003815209202,
   0.001308857696,-0.002245142125,7.899050748e-17, 0.002266979078,-0.001334442524,
  -0.0003927623329,2.905841155e-17,0.0003966379154, 0.001360908151,-0.002334755147,
  9.023806608e-17, 0.002358136699,-0.001388303237,-0.0004086748522,-1.923837644e-17,
  0.0004128302098, 0.001416679588,-0.002430806402, 2.10244943e-17, 0.002455910901,
    -0.0014460932,-0.0004257541732,-7.160023766e-17, 0.000430222135, 0.001476604375,
  -0.002534048399,3.069696701e-17, 0.002561081899,-0.001508278307,-0.0004441393539,
  -4.183354444e-17,0.0004489581042, 0.001541185193,-0.002645355416,4.121416152e-17,
   0.002674558666,-0.001575401286,-0.0004639924737,-9.364290857e-18,0.0004692067159,
   0.001611009124,-0.002765750047,5.268041344e-17, 0.002797405003,-0.001648098347,
  -0.0004855037259,2.614699222e-17,0.0004911660799, 0.001686766511,-0.002896435326,
  6.521858556e-17,   0.0029308761,-0.001727120019,-0.0005088975886,6.510140384e-17,
  0.0005150704528, 0.001769274939,-0.003038837342,7.897437415e-17, 0.003076461144,
  -0.001813358278,-0.0005344412057,3.602394315e-18,0.0005411991733, 0.001859509386,
  -0.003194658784,9.412189082e-17, 0.003235942218,-0.001907881233,-0.0005624546902,
  -6.440653242e-17,0.0005698876921, 0.001958642388,-0.003365949262, 5.44545041e-17,
   0.003411469283,-0.002011978999,-0.0005933252396,-2.398472886e-17,0.0006015424733,
   0.002068096772,-0.003555198433, 1.02606734e-17, 0.003605660284,-0.002127224347,
  -0.0006275253836,-4.018096993e-17,0.0006366611342, 0.002189615974,-0.003765461734,
  2.393932823e-17, 0.003821735969,-0.002255555242,-0.0006656381884,-5.834818883e-17,
  0.0006758591626, 0.002325359965,-0.004000529181,-2.779332814e-17, 0.004063704517,
  -0.002399386372,-0.0007083911914,-9.554622635e-18,0.0007199072279,  0.00247803703,
  -0.004265162162,1.283713239e-16, 0.004336615559,-0.002561766189,-0.0007567045395,
  4.597053938e-17,0.0007697830442, 0.002651090268,-0.004565422889,7.666595791e-17,
   0.004646925721,-0.002746598097,-0.0008117589168, 3.01555012e-17,0.0008267465164,
   0.002848964185,-0.004909154028,9.956069599e-17, 0.005003021564,-0.002958965022,
  -0.0008750947309,-7.392013517e-17,0.0008924487629, 0.003077498404,-0.005306684412,
  3.691521124e-17, 0.005416001193,-0.003205609042,-0.0009487607749,-9.621697056e-18,
  0.0009690971929, 0.003344519529,-0.005771894939,6.028856264e-17, 0.005900864489,
  -0.003495669691, -0.00103554246,6.659911691e-17, 0.001059711096, 0.003660767106,
  -0.006323894486,-1.838883713e-17, 0.006478395779,-0.003841853002,-0.001139325439,
  -6.562667677e-17, 0.001168532879, 0.004041386768,-0.006989731453,1.219705871e-16,
   0.007178246509,-0.004262360279,-0.001265698927,-4.079553865e-17, 0.001301716897,
   0.004508448299,-0.007808999624,3.183121361e-17, 0.008044230752,-0.004784216639,
  -0.001423003734,-9.669753768e-18, 0.001468543778, 0.005095411092,-0.008842064068,
  6.708246832e-17, 0.009143927135,-0.005449359305,-0.001624263357,3.043299382e-17,
   0.001683693728, 0.005855563562, -0.01018573623,2.680648843e-17,  0.01058731508,
  -0.006326564588,-0.001891006134,-9.686601783e-18, 0.001971842023, 0.006879264954,
   -0.01200567093,7.400575829e-17,  0.01256622095,-0.007537008729,-0.002261563437,
  -6.593352769e-17, 0.002377910307, 0.008332965896, -0.01461079344,7.973426197e-17,
    0.01544785406,-0.009315919131,-0.002811436076,-9.698645533e-18, 0.002993190894,
    0.01056066435, -0.01865091175,8.301051923e-18,   0.0200346373, -0.01218803134,
  -0.003712767037,-9.702863327e-18, 0.004035992082,  0.01440672018, -0.02576601878,
  1.048284402e-16,  0.02848044224, -0.01761094667,-0.005461731926,-9.705875919e-18,
    0.00619034702,  0.02264545858, -0.04163301364, 4.85346537e-17,  0.04920494556,
   -0.03170659393, -0.01031868625,-9.707684135e-18,   0.0132672945,   0.0528476052,
    -0.1082609594,4.854067888e-17,   0.1804377288,  -0.1585477293, -0.09287538379,
     0.2490471601, -0.09287538379,  -0.1585477293,   0.1804377288,4.854067888e-17,
    -0.1082609594,   0.0528476052,   0.0132672945,-9.707684135e-18, -0.01031868625,
   -0.03170659393,  0.04920494556, 4.85346537e-17, -0.04163301364,  0.02264545858,
    0.00619034702,-9.705875919e-18,-0.005461731926, -0.01761094667,  0.02848044224,
  1.048284402e-16, -0.02576601878,  0.01440672018, 0.004035992082,-9.702863327e-18,
  -0.003712767037, -0.01218803134,   0.0200346373,8.301051923e-18, -0.01865091175,
    0.01056066435, 0.002993190894,-9.698645533e-18,-0.002811436076,-0.009315919131,
    0.01544785406,7.973426197e-17, -0.01461079344, 0.008332965896, 0.002377910307,
  -6.593352769e-17,-0.002261563437,-0.007537008729,  0.01256622095,7.400575829e-17,
   -0.01200567093, 0.006879264954, 0.001971842023,-9.686601783e-18,-0.001891006134,
  -0.006326564588,  0.01058731508,2.680648843e-17, -0.01018573623, 0.005855563562,
   0.001683693728,3.043299382e-17,-0.001624263357,-0.005449359305, 0.009143927135,
  6.708246832e-17,-0.008842064068, 0.005095411092, 0.001468543778,-9.669753768e-18,
  -0.001423003734,-0.004784216639, 0.008044230752,3.183121361e-17,-0.007808999624,
   0.004508448299, 0.001301716897,-4.079553865e-17,-0.001265698927,-0.004262360279,
   0.007178246509,1.219705871e-16,-0.006989731453, 0.004041386768, 0.001168532879,
  -6.562667677e-17,-0.001139325439,-0.003841853002, 0.006478395779,-1.838883713e-17,
  -0.006323894486, 0.003660767106, 0.001059711096,6.659911691e-17, -0.00103554246,
  -0.003495669691, 0.005900864489,6.028856264e-17,-0.005771894939, 0.003344519529,
  0.0009690971929,-9.621697056e-18,-0.0009487607749,-0.003205609042, 0.005416001193,
  3.691521124e-17,-0.005306684412, 0.003077498404,0.0008924487629,-7.392013517e-17,
  -0.0008750947309,-0.002958965022, 0.005003021564,9.956069599e-17,-0.004909154028,
   0.002848964185,0.0008267465164, 3.01555012e-17,-0.0008117589168,-0.002746598097,
   0.004646925721,7.666595791e-17,-0.004565422889, 0.002651090268,0.0007697830442,
  4.597053938e-17,-0.0007567045395,-0.002561766189, 0.004336615559,1.283713239e-16,
  -0.004265162162,  0.00247803703,0.0007199072279,-9.554622635e-18,-0.0007083911914,
  -0.002399386372, 0.004063704517,-2.779332814e-17,-0.004000529181, 0.002325359965,
  0.0006758591626,-5.834818883e-17,-0.0006656381884,-0.002255555242, 0.003821735969,
  2.393932823e-17,-0.003765461734, 0.002189615974,0.0006366611342,-4.018096993e-17,
  -0.0006275253836,-0.002127224347, 0.003605660284, 1.02606734e-17,-0.003555198433,
   0.002068096772,0.0006015424733,-2.398472886e-17,-0.0005933252396,-0.002011978999,
   0.003411469283, 5.44545041e-17,-0.003365949262, 0.001958642388,0.0005698876921,
  -6.440653242e-17,-0.0005624546902,-0.001907881233, 0.003235942218,9.412189082e-17,
  -0.003194658784, 0.001859509386,0.0005411991733,3.602394315e-18,-0.0005344412057,
  -0.001813358278, 0.003076461144,7.897437415e-17,-0.003038837342, 0.001769274939,
  0.0005150704528,6.510140384e-17,-0.0005088975886,-0.001727120019,   0.0029308761,
  6.521858556e-17,-0.002896435326, 0.001686766511,0.0004911660799,2.614699222e-17,
  -0.0004855037259,-0.001648098347, 0.002797405003,5.268041344e-17,-0.002765750047,
   0.001611009124,0.0004692067159,-9.364290857e-18,-0.0004639924737,-0.001575401286,
   0.002674558666,4.121416152e-17,-0.002645355416, 0.001541185193,0.0004489581042,
  -4.183354444e-17,-0.0004441393539,-0.001508278307, 0.002561081899,3.069696701e-17,
  -0.002534048399, 0.001476604375, 0.000430222135,-7.160023766e-17,-0.0004257541732,
    -0.0014460932, 0.002455910901, 2.10244943e-17,-0.002430806402, 0.001416679588,
  0.0004128302098,-1.923837644e-17,-0.0004086748522,-0.001388303237, 0.002358136699,
  9.023806608e-17,-0.002334755147, 0.001360908151,0.0003966379154,2.905841155e-17,
  -0.0003927623329,-0.001334442524, 0.002266979078,7.899050748e-17,-0.002245142125,
   0.001308857696,0.0003815209202,-7.368754522e-17,-0.0003778967075,-0.001284108846,
   0.002181763295,-3.756025743e-18,-0.002161317272, 0.001260153833,0.0003673714236,
  4.405157631e-17,-0.0003639739589,-0.001236953423, 0.002101904014,5.884208183e-17,
  -0.002082715044, 0.001214470947,0.0003540957114,-5.18943404e-17,-0.0003509034868,
  -0.001192671945
};
