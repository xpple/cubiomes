// a program to generate g_biome_para_range in finders.c  need the btree.h
// the line 17 contains the biome id
#include "btree264s3.h"
#include<stdio.h>
#include<limits.h>
int min(int a,int b){return a<b?a:b;}
int max(int a,int b){return a>b?a:b;}
int main(){
	int templ=INT_MAX,temph=INT_MIN;
	int humil=INT_MAX,humih=INT_MIN;
	int contl=INT_MAX,conth=INT_MIN;
	int erosl=INT_MAX,erosh=INT_MIN;
	int deptl=INT_MAX,depth=INT_MIN;
	int weirl=INT_MAX,weirh=INT_MIN;
	for(int i=0;i<sizeof(btree264s3_nodes)/sizeof(uint64_t);i++){
		uint64_t x=btree264s3_nodes[i];
		if((x>>48)==0xFFBB){
			templ=min(templ,btree264s3_param[x&0xFF][0]);
			temph=max(temph,btree264s3_param[x&0xFF][1]);
			humil=min(humil,btree264s3_param[(x>>8)&0xFF][0]);
			humih=max(humih,btree264s3_param[(x>>8)&0xFF][1]);
			contl=min(contl,btree264s3_param[(x>>16)&0xFF][0]);
			conth=max(conth,btree264s3_param[(x>>16)&0xFF][1]);
			erosl=min(erosl,btree264s3_param[(x>>24)&0xFF][0]);
			erosh=max(erosh,btree264s3_param[(x>>24)&0xFF][1]);
			deptl=min(deptl,btree264s3_param[(x>>32)&0xFF][0]);
			depth=max(depth,btree264s3_param[(x>>32)&0xFF][1]);
			weirl=min(weirl,btree264s3_param[(x>>40)&0xFF][0]);
			weirh=max(weirh,btree264s3_param[(x>>40)&0xFF][1]);
		}
	}
	printf("%d %d\n",templ,temph);
	printf("%d %d\n",humil,humih);
	printf("%d %d\n",contl,conth);
	printf("%d %d\n",erosl,erosh);
	printf("%d %d\n",deptl,depth);
	printf("%d %d\n",weirl,weirh);
	return 0;
}
