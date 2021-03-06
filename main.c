/*
 *  main.c
 *
 *
 *  Created by Earl Lawrence on 9/17/09.
 *
 *  This program was prepared by Los Alamos National Security, LLC at Los Alamos National Laboratory (LANL)
 *  under contract No. DE-AC52-06NA25396 with the U.S. Department of Energy (DOE). All rights in the program
 *  are reserved by the DOE and Los Alamos National Security, LLC.  Permission is granted to the public to
 *  copy and use this software without charge, provided that this Notice and any statement of authorship are
 *  reproduced on all copies.  Neither the U.S. Government nor LANS makes any warranty, express or implied,
 *  or assumes any liability or responsibility for the use of this software.
 *
 */


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

void mainLoop(int ,double [],char [], int );

int main(int argc, char **argv) {
  double xstar[7];
  float index;
  int i;
  double boundaries[7][3];

  boundaries[0][0] = 0.0215;  //omega b
  boundaries[0][1] = 0.0235;
  boundaries[0][2] = 0.0005;

  boundaries[1][0] = 0.120; //omega m
  boundaries[1][1] = 0.155;
  boundaries[1][2] = 0.005;

  boundaries[2][0] = 0.85;
  boundaries[2][1] = 1.05; // ns
  boundaries[2][2] = 0.05;

  boundaries[3][0] = 55.0;
  boundaries[3][1] = 85.0;   //H0
  boundaries[3][2] = 5.0;

  boundaries[4][0] = -1.30;
  boundaries[4][1] = -0.70;  //w
  boundaries[4][2] = 0.05;

  boundaries[5][0] = 0.61;  //sigma 8
  boundaries[5][1] = 0.9;
  boundaries[5][2] = 0.05;

  boundaries[6][0] = 0;  //z
  boundaries[6][1] = 1;
  boundaries[6][2] = 0.01;
  char filename[256];
  for(i = 0; i < 7; i++){
    for(int j = 0; j < 7; j++){
        xstar[j]=(boundaries[j][1]-boundaries[j][0])/2;
        xstar[j]+=boundaries[j][0];
    }
    for(index = boundaries[i][0]+boundaries[i][2]; index < boundaries[i][1]; index+=boundaries[i][2]){
      for(int k=0;k<3;k++){
        xstar[i] = index;
        printf("%f, %f, %f, %f, %f, %f, %f, %f, index=%f\n",
        xstar[0],xstar[1],xstar[2],xstar[3],xstar[4],xstar[5],xstar[6],index);
        sprintf(filename,"files/%u/%u/0/data_%f.dat",i,k,index);
        mainLoop(0, xstar, filename,k);
        sprintf(filename,"files/%u/%u/1/data_%f.dat",i,k,index);
        mainLoop(1, xstar, filename,k);
      }
    }
  }
    return 0;
}

void mainLoop(int cmbh,double xstar[7],char fname_nl[256], int type){
  int i,j, writeout=0;
  double ystar[2*582], stuff[4], xstarcmb[6];
  FILE *fp;

/*
  printf("Enter filename for output : ");
  scanf("%s",fname_nl);
  printf("Output will be written to: %s.\n", fname_nl);

  printf("Will you be using h as derived from CMB constraints (0 for no, 1 for yes)?\n");
  scanf("%d", &cmbh);

  printf("Enter omega_b (= Omega_b*h^2): ");
  scanf("%lf",&xstar[0]);
  printf("%g\n",xstar[0]);
  printf("Enter omega_m (= Omega_m*h^2): ");
  scanf("%lf",&xstar[1]);
  printf("%g\n",xstar[1]);
  printf("Enter n_s: ");
  scanf("%lf",&xstar[2]);
  printf("%g\n",xstar[2]);
  if(cmbh == 0) {
      printf("Enter H0: ");
      scanf("%lf",&xstar[3]);
      printf("%g\n",xstar[3]);
  }
  printf("Enter w: ");
  scanf("%lf",&xstar[4]);
  printf("%g\n",xstar[4]);
  printf("Enter sigma_8: ");
  scanf("%lf",&xstar[5]);
  printf("%g\n",xstar[5]);
  printf("Enter z: ");
  scanf("%lf",&xstar[6]);
  printf("%g\n",xstar[6]);

  printf("Enter output type (0: Delta^2/k^1.5; 1: Delta^2; 2: P(k)): ");
  scanf("%i",&type);
  printf("%i\n", type);
*/
  if(cmbh == 1) {
      xstarcmb[0] = xstar[0];
      xstarcmb[1] = xstar[1];
      xstarcmb[2] = xstar[2];
      xstarcmb[3] = xstar[4];
      xstarcmb[4] = xstar[5];
      xstarcmb[5] = xstar[6];
      emu_noh(xstarcmb, ystar, &type);
      getH0fromCMB(xstarcmb, stuff);
      xstar[3] = 100.*stuff[3];
  } else {
      emu(xstar, ystar, &type);
  }

  // Write the nonlinear file
  if ((fp = fopen(fname_nl,"w"))==NULL) {
      printf("cannot open %s \n",fname_nl);
      exit(1);
  }

  fprintf(fp, "# Parameters:\n");
  fprintf(fp, "# omega_b = %f, omega_m = %f, n_s = %f, h = %f, w = %f,sigma_8 = %f\n", xstar[0], xstar[1], xstar[2], xstar[3], xstar[4], xstar[5]);
  fprintf(fp, "# z = %f\n", xstar[6]);
  fprintf(fp, "#\n");
  fprintf(fp, "# k[1/Mpc] ");

  switch(type) {
      default:
          fprintf(fp, "# Delta^2 / k^1.5:\n");
          break;
      case 1:
          fprintf(fp, "# Delta^2:\n");
          break;
      case 2:
          fprintf(fp, "# P(k):\n");
          break;
  }

  for(j=0; j<582; j++) {
      fprintf(fp ,"%e %e \n", ystar[j], ystar[582+j]);
  }
  fclose(fp);

}
