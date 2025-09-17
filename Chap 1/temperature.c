#include <stdio.h>

main(){
  float farenheit, celsius;
  float lower, upper, step;

  lower = 0;
  upper = 300;
  step = 20;
  farenheit = lower;
  while (farenheit <= upper) {
    celsius = (5.0/9.0) * (farenheit-32.0);
    printf("%3.0f %6.1f\n", farenheit, celsius);
    farenheit = farenheit + step;
  }
  /*
  for (fahr = 0; fahr <= 300; fahr = fahr + 20){
    printf("%3d %6.1f\n", fahr, (5.0/9.0)*(fahr-32));
  }
  */

}