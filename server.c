#include <stdio.h>

float power(float x, int n){
	if(n==0){
		return 1;
	}
	else{
		float result = 1;
		for(int i=0; i<n; i++){
			result = result * x;
			
		}
		return result;
	}
	
}

int main(){
	printf("%f",power(2,3));
}
