#include <stdio.h>
#include <string.h>


int main(){
	
	char a[10] = "ab";
	char b[10] = "cd";
	char c[10] = "ef";
	char d[50];
	snprintf(d,sizeof(d),"%s ~ %s ~ %s",a,b,c);

	printf("%s",d);

	return 0;
}

