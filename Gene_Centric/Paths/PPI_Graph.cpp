#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "PPI_Graph.h"
//#include "asa241.h"

PPI_Node listNodes[maxNumNode];
int numNodes=0;
int coExpresGeneNum=0;
int totalSevereMutInCases=0;
int totalMissenseMutInCases=0;
int totalSevereMutInControl=0;
int totalLengthGenes=0;
float coexpressionPValue[102];
float meanCoExpression, varianceCoExpression;

coExpresionGeneHash coExpresionGeneHashTable[maxNumNode];
float coExpresionMatrix[maxNumNode][maxNumNode];// The node is is the same as PPI node id. The nodes which are not in PPI network are discarded.

int createCoExpresionMatix(FILE *fpCoExpresionMatrix)
{

/* First we have to convert the geneNames in each instance of coexpression matrix into the id used in the PPI_Node.
 * Assuming that the coExpressionMatrix is orderd by the ids in coExpresionGeneHasTable.*/

	int firstGeneCount=0, secondGeneCount=0;
	char geneName1[geneNameLen], geneName2[geneNameLen], prevGeneName[geneNameLen];
	float geneCoExpr;
	while(fscanf(fpCoExpresionMatrix, "%s %s %f\n", geneName1, geneName2, &geneCoExpr)!=EOF)
	{
		geneCoExpr=pow(geneCoExpr, 0.3333333);
		if (strcmp(geneName1, prevGeneName)!=0)
		{
			strcpy(prevGeneName, geneName1);
			firstGeneCount++;// Each new gene (first gene that comes) we increase the count for the id. The second gene starts exactly after that//
			secondGeneCount=firstGeneCount+1;
		}else{
			secondGeneCount++;
		}

		// assuing the gene number of gene1 and gene2 in coexpression gene list is firstGeneCount and secondGeneCount using the coExpresionGeneHashTable we can find their respected id in PPI_Node//

		if (coExpresionGeneHashTable[firstGeneCount].nodeId>-1 && coExpresionGeneHashTable[secondGeneCount].nodeId>-1)
		{
			coExpresionMatrix[coExpresionGeneHashTable[firstGeneCount].nodeId][coExpresionGeneHashTable[secondGeneCount].nodeId]=geneCoExpr;
			coExpresionMatrix[coExpresionGeneHashTable[secondGeneCount].nodeId][coExpresionGeneHashTable[firstGeneCount].nodeId]=geneCoExpr;
		}
		//	printf("%s %s %s %s\n", geneName1, geneName2, listNodes[coExpresionGeneHashTable[firstGeneCount].nodeId].nodeName, listNodes[coExpresionGeneHashTable[secondGeneCount].nodeId].nodeName);

	}


calculateCoExpressionPValue();


/*	for (int count=0; count<numNodes; count++)
	{
	for (int count2=count+1; count2<numNodes; count2++)
	{
		printf("%s %s %f\n", listNodes[count].nodeName, listNodes[count2].nodeName, coExpresionMatrix[count][count2]);
	}
	} 
*/
}

int isConnectedPPI(int node1, int node2)
{
	for (int count=0; count<listNodes[node1].degree; count++)
	{
		
		if (listNodes[node1].neighbours[count]==node2)
			return 1;
	} 
return 0;
}


int createPPI_Graph(FILE *fpSeed, FILE *inputPPI_FP)
{
char geneName1[geneNameLen], geneName2[geneNameLen];
int nodeId1, nodeId2; 
	while (fscanf(inputPPI_FP, "%s\t%s\n", geneName1, geneName2)!=EOF)
	{
		//printf("%s %s\n", geneName1, geneName2);
		if (strcmp(geneName1, geneName2)!=0)
		{
			nodeId1=-1;
			nodeId2=-1;
		
			for (int count=0; count<numNodes; count++)
			{
				if (strcmp(geneName1, listNodes[count].nodeName)==0)
					nodeId1=count;
				if (strcmp(geneName2, listNodes[count].nodeName)==0)
					nodeId2=count;
			}		
			if (nodeId1==-1)
			{
				listNodes[numNodes].nodeId=numNodes;
				strcpy(listNodes[numNodes].nodeName, geneName1);
				listNodes[numNodes].degree=0;
				listNodes[numNodes].weightCases=0;
				listNodes[numNodes].weightControl=0;
				nodeId1=numNodes;
				numNodes++;
			}
			if (nodeId2==-1)
			{
			
				listNodes[numNodes].nodeId=numNodes;
				strcpy(listNodes[numNodes].nodeName, geneName2);
				listNodes[numNodes].degree=0;
				listNodes[numNodes].weightCases=0;
				listNodes[numNodes].weightControl=0;
				nodeId2=numNodes;
				numNodes++;
			}
		
			listNodes[nodeId1].degree++;
			listNodes[nodeId2].degree++;
			listNodes[nodeId1].neighbours=(int *)realloc(listNodes[nodeId1].neighbours, listNodes[nodeId1].degree*sizeof(int));
			listNodes[nodeId2].neighbours=(int *)realloc(listNodes[nodeId2].neighbours, listNodes[nodeId2].degree*sizeof(int));
			listNodes[nodeId1].neighbours[listNodes[nodeId1].degree-1]=nodeId2;
			listNodes[nodeId2].neighbours[listNodes[nodeId2].degree-1]=nodeId1;
		}

	}



////////////////////The seedGene which is not in the PPI network is connected to a dummy node so the coexpression can be calculated////////////
bool foundSeed=false;
char seedGeneName[geneNameLen];
fscanf(fpSeed, "%s\n", seedGeneName);
for (int count=0; count<numNodes; count++)
{
	if (strcmp(seedGeneName,listNodes[numNodes].nodeName)==0)
	{
		foundSeed=true;
	} 
}
if (foundSeed==false)
{
	printf("SeedNotFound\n");

	//listNodes[numNodes].nodeId=numNodes;
	//strcpy(listNodes[numNodes].nodeName, "DUMMYGENE\n");
	//listNodes[numNodes].degree=0;
	//listNodes[numNodes].weightCases=0;
	//listNodes[numNodes].weightControl=0;
	//numNodes++;
	listNodes[numNodes].nodeId=numNodes;
	strcpy(listNodes[numNodes].nodeName, seedGeneName);	
	listNodes[numNodes].degree=0;
	listNodes[numNodes].weightCases=0;
	listNodes[numNodes].weightControl=0;
	listNodes[numNodes].neighbours=NULL;
	numNodes++;
	//nodeId1=numNodes-2;
	//nodeId2=numNodes-1;
		
	/*listNodes[nodeId1].degree++;
	listNodes[nodeId2].degree++;
	listNodes[nodeId1].neighbours=(int *)realloc(listNodes[nodeId1].neighbours, listNodes[nodeId1].degree*sizeof(int));
	listNodes[nodeId2].neighbours=(int *)realloc(listNodes[nodeId2].neighbours, listNodes[nodeId2].degree*sizeof(int));
	listNodes[nodeId1].neighbours[listNodes[nodeId1].degree-1]=nodeId2;
	listNodes[nodeId2].neighbours[listNodes[nodeId2].degree-1]=nodeId1;
	*/
}
//////////////////The seedGene additions to the dummpy Node///////////////////

for (int count=0; count<numNodes; count++)
{
	if (strcmp(listNodes[count].nodeName, "CHD8")==0)
	{
		for (int count2=0; count2<listNodes[count].degree; count2++)
		{
			printf("CHD8 %s\n", listNodes[listNodes[count].neighbours[count2]].nodeName);
		} 
	}
}
printf("%i\n", numNodes);
}


double log_N_Choose_M(int n, int m)
{
double val=0;

if (m>n)
	printf("L127 ERRRROORORORORORO %i %i %i %i\n", n, m, totalSevereMutInCases, totalMissenseMutInCases);

	for (int count=n-m+1; count<=n; count++)
	{
		val=val+log(count);
	}
	for (int count=1; count<=m; count++)
	{
		val=val-log(count);
	}

/*	for (int count=1; count<=n-m; count++)
	{
		val=val-log(count);
	}
*/
if (m==0)
	printf("Value zero %e\n", val);

return val;

}
/*
double log_NumNode_Choose_M(int m)
{
double val=0;
}
*/

float tScorePValue(float *listOfCoExpressionValues, int numValues)
{
float tScore=0;
float meanValueOfList=0;
float varianceValudOfList=0;
float varianceValudOfListNormalized, varianceCoExpressionNormalized;
float degreeFree;
	for (int count=0; count<numValues; count++)
	{	
		meanValueOfList=meanValueOfList+listOfCoExpressionValues[count];
	}
	meanValueOfList=(float)meanValueOfList/(float)numValues;
	
	for (int count=0; count<numValues; count++)
	{
		varianceValudOfList=varianceValudOfList+(listOfCoExpressionValues[count]-meanValueOfList)*(listOfCoExpressionValues[count]-meanValueOfList);
	}

	varianceValudOfList= (float)varianceValudOfList/(float)(numValues);
	varianceValudOfListNormalized= (float)varianceValudOfList/(float)(numValues);
	varianceCoExpressionNormalized= (float)2*varianceCoExpression/(float)(numNodes * numNodes - numNodes);
	
	tScore=(float)(meanValueOfList-meanCoExpression)/(float)(sqrt(varianceValudOfListNormalized + varianceCoExpressionNormalized));

	degreeFree=(float)((varianceValudOfListNormalized + varianceCoExpressionNormalized)*(varianceValudOfListNormalized + varianceCoExpressionNormalized))/(float)(((float)varianceValudOfListNormalized*varianceValudOfListNormalized/(float)numValues)+((float)varianceCoExpressionNormalized*varianceCoExpressionNormalized/(float)(numNodes * numNodes - numNodes)));


	//printf("Tscore %f %f\n", tScore, degreeFree);		
	return tScore;

		

}

double calNewProbValue(int nodeId)
{
/*	
	double weight=0;
	double prob_1=0;
	double prob_2=0;
	for (int count=listNodes[nodeId].numSevereMutInCases; count<10; count++)
	{
	
		weight=log_N_Choose_M(totalSevereMutInCases, count) + count * log(listNodes[nodeId].prob) + (totalSevereMutInCases-count)*log(1-listNodes[nodeId].prob);
		prob_1=prob_1+exp(weight);		
	}
	
	if(totalMissenseMutInCases>0)
	{	
		for (int count=listNodes[nodeId].numMissenseMutInCases; count<10; count++)
		{
			weight=log_N_Choose_M(totalMissenseMutInCases, count) + count * log(listNodes[nodeId].prob)+(totalMissenseMutInCases-count)*log(1-listNodes[nodeId].prob);
			prob_2=prob_2+exp(weight);
		}
	} else prob_2=1;	
	
*/	
	

	listNodes[nodeId].weightCases=log_N_Choose_M(totalSevereMutInCases, listNodes[nodeId].numSevereMutInCases) + listNodes[nodeId].numSevereMutInCases * log(listNodes[nodeId].prob) + (totalSevereMutInCases-listNodes[nodeId].numSevereMutInCases)*log(1-listNodes[nodeId].prob);
	//printf("%i %i %i %e %e %e %e\n", totalSevereMutInCases, listNodes[nodeId].numSevereMutInCases, listNodes[nodeId].length  ,log_N_Choose_M(totalSevereMutInCases, listNodes[nodeId].numSevereMutInCases), log(listNodes[nodeId].prob), log(1-listNodes[nodeId].prob), listNodes[nodeId].prob);
	//printf("%lf %i %i %lf %lf\n", listNodes[nodeId].weightCases, totalSevereMutInCases, listNodes[nodeId].numSevereMutInCases, listNodes[nodeId].numSevereMutInCases * log(listNodes[nodeId].prob), log_N_Choose_M(totalSevereMutInCases, listNodes[nodeId].numSevereMutInCases));
	listNodes[nodeId].weightCases=listNodes[nodeId].weightCases+log_N_Choose_M(totalMissenseMutInCases, listNodes[nodeId].numMissenseMutInCases)+ listNodes[nodeId].numMissenseMutInCases * log(listNodes[nodeId].prob)+(totalMissenseMutInCases-listNodes[nodeId].numMissenseMutInCases)*log(1-listNodes[nodeId].prob);
	
	listNodes[nodeId].weightCases=-1*listNodes[nodeId].weightCases;
	//printf("%lf \n", exp(listNodes[nodeId].weightCases));
	//listNodes[nodeId].weightCases=r8_normal_01_cdf_inverse(1-exp(listNodes[nodeId].weightCases));
	//listNodes[nodeId].weightCases=r8_normal_01_cdf_inverse(1-prob_1*prob_2);
	if (listNodes[nodeId].numSevereMutInCases + listNodes[nodeId].numMissenseMutInCases==0)
		listNodes[nodeId].weightCases=0;
	printf("%lf\n", listNodes[nodeId].weightCases);
}


int assignScorePrecalculated(FILE *fpPredefined, FILE *fpControl)
{
char geneName[geneNameLen];
float weight;
int numSevereMutInCases, numMissenseInCases;
int controlCount;
float length;
totalLengthGenes=0;
	while(fscanf(fpPredefined,"%s %f %i %i %f 0.000000\n", geneName, &weight, &numSevereMutInCases, &numMissenseInCases, &length)!=EOF)
	{
		for (int count=0; count<numNodes; count++)
		{
			if (strcmp(geneName, listNodes[count].nodeName)==0)
			{
				listNodes[count].numSevereMutInCases=numSevereMutInCases;
				listNodes[count].weightCases=weight;
				listNodes[count].numMissenseMutInCases=numMissenseInCases;
				listNodes[count].length=(int)length;
				totalLengthGenes=totalLengthGenes+(int)length;
				totalSevereMutInCases=totalSevereMutInCases+numSevereMutInCases;
				totalMissenseMutInCases=totalMissenseMutInCases+numMissenseInCases;
			///THE CONTROL MUTATIONS IS NOT ADDED YET (YOU SHOULD ADD IT)	
			}
		}
	
	}

	while(fscanf(fpControl,"%s\t%i\n", geneName, &controlCount)!=EOF)
	{
		for (int count=0; count<numNodes; count++)
		{
			if (strcmp(geneName, listNodes[count].nodeName)==0)
			{
				//printf("added control %s %i\n", geneName,controlCount);
				listNodes[count].numSevereMutInControl=controlCount;
				//printf("added control %s %i\n", listNodes[count].nodeName, listNodes[count].weightControl);
			}
		}	
	}

}



float calculateGeneScore_Pvalue(float score, int gene1, int gene2)
{
int count1;
int highScore1=0, highScore2=0;
for (count1=0; count1<numNodes; count1++)
{
	if (coExpresionMatrix[count1][gene1]<score)		
	{
		highScore1++;	
	}
	if (coExpresionMatrix[count1][gene2]<score)
	{
		highScore2++;
	}
}	

//return ((float)(highScore1*highScore2)/(float)(numNodes*numNodes));
return 1-((float)(highScore1*highScore2)/(float)(numNodes*numNodes));

}


int assignScoreToBothControlandCases(FILE *fpCases, FILE *fpControl, FILE *fpGeneLen, FILE *fpOutputGeneScore)
{
char geneName[geneNameLen];
int numTruncatingControl;
double variantScoreCases;
char variantSubtype[20];
int countTotal=0, randId;
double len;
double temp;
srand(time(NULL));

char geneNameTargets[100][50];// the name of the gene targets to clauclate the score based on coexpression network 
int geneNameTargetsId[100];
int geneNameTargetsCount=0;



while(fscanf(fpCases,"%s\n", geneNameTargets[geneNameTargetsCount])!=EOF)
{
	for (int count=0; count<numNodes; count++)
	{
		if( strcmp(geneNameTargets[geneNameTargetsCount], listNodes[count].nodeName)==0)
		{
			geneNameTargetsId[geneNameTargetsCount]=count;
		}
	}

	geneNameTargetsCount++;
}

for (int count=0; count<geneNameTargetsCount; count++)
{
	for (int count2=0; count2<numNodes; count2++)
	{
		if (geneNameTargetsId[count]==count2)
		{
			//listNodes[count2].weightCases++;
			//listNodes[count2].weightCases=10;
			listNodes[count2].weightCases=0;
		}else
		{


			listNodes[count2].weightCases=listNodes[count2].weightCases+calculateGeneScore_Pvalue(coExpresionMatrix[geneNameTargetsId[count]][count2], geneNameTargetsId[count], count2);
			listNodes[count2].weightCases=-1*log10(listNodes[count2].weightCases);
			//listNodes[count2].weightCases=listNodes[count2].weightCases+coExpresionMatrix[geneNameTargetsId[count]][count2];
		
			//coExpresionMatrix[coExpresionGeneHashTable[secondGeneCount].nodeId][coExpresionGeneHashTable[firstGeneCount].nodeId]=geneCoExpr;
		}
	}
}






/*3n-non-frameshifting
frameshift
missense
nonsense
splice
splice-indel

        int numSevereMutInCases;
        int numMissenseMutInCases;
        int numSevereMutInControl;

        double pValue;
*/


/*
	while(fscanf(fpCases, "%s\t%i\t%lf%s\n", geneName, &numTruncatingControl, &variantScoreCases, variantSubtype)!=EOF)
	{
		for (int count=0; count<numNodes; count++)
		{
			if (strcmp(geneName, listNodes[count].nodeName)==0 && (strcmp(variantSubtype,"3n-non-frameshifting")==0 || strcmp(variantSubtype,"frameshift")==0 || strcmp(variantSubtype,"nonsense")==0 || strcmp(variantSubtype,"splice")==0 || strcmp(variantSubtype,"splice-indel")==0 || strcmp(variantSubtype,"3n-non-frameshifting-stop")==0 || strcmp( variantSubtype,"frameshift-near-splice")==0  || strcmp(variantSubtype, "stop_gained")==0 || strcmp(variantSubtype,"splice_acceptor")==0 || strcmp(variantSubtype,"splice_donor")==0))
			{
				listNodes[count].numSevereMutInCases++;
				listNodes[count].weightCases=listNodes[count].weightCases+1;
				countTotal++;
				totalSevereMutInCases++;
			}else if(strcmp(geneName, listNodes[count].nodeName)==0 && strcmp(variantSubtype,"missense")==0)
			{
				listNodes[count].numMissenseMutInCases++;
				listNodes[count].weightCases=listNodes[count].weightCases+1;
				countTotal++;
				totalMissenseMutInCases++;
			}
		}		
	}

	printf("%i %i\n", totalSevereMutInCases, totalMissenseMutInCases);
*/
	while(fscanf(fpControl, "%s\t%i\n", geneName, &numTruncatingControl)!=EOF)
	{
		
		for (int count=0; count<numNodes; count++)
		{
			if (strcmp(geneName, listNodes[count].nodeName)==0)
			{
				//listNodes[count].numSevereMutInControl++;
				listNodes[count].weightControl=numTruncatingControl;
				totalSevereMutInControl++;
			}
		}		
	}
/*	while(fscanf(fpGeneLen, "%s\t%lf\n", geneName, &len)!=EOF)
	{
		for (int count=0; count<numNodes; count++)
		{
			if (strcmp(geneName, listNodes[count].nodeName)==0)
			{
				//listNodes[count].weightCases = ((double)listNodes[count].weightCases/(double)len)-(double)listNodes[count].weightControl/(double)len;
				listNodes[count].log_length=len;
				listNodes[count].length=len;
				totalLengthGenes=totalLengthGenes+len;
			}
		}
	}


	for (int count=0; count<numNodes; count++)
	{
		if (listNodes[count].log_length==0)
		{
			listNodes[count].log_length=3300;
			//listNodes[count].weightCases=((double)listNodes[count].weightCases/((double)listNodes[count].log_length));//+(double)listNodes[count].weightControl));
			//listNodes[count].weightCases = listNodes[count].weightCases-((double)listNodes[count].weightControl/(double)listNodes[count].log_length);
			listNodes[count].length=3300;
			totalLengthGenes=totalLengthGenes+3300;
		}
	}


for (int count=0; count<numNodes; count++)
{
	listNodes[count].prob=(double)listNodes[count].length/(double)totalLengthGenes;
	//printf("%lf %i\n", log(listNodes[count].prob), listNodes[count].numMissenseMutInCases);
	listNodes[count].weightCases=0;
	calNewProbValue(count);
	
}

*/

for (int count=0; count<numNodes; count++)
{
	fprintf(fpOutputGeneScore,"%s %lf %i %i %i %i\n", listNodes[count].nodeName, listNodes[count].weightCases, listNodes[count].numSevereMutInCases, listNodes[count].numMissenseMutInCases, listNodes[count].weightControl, listNodes[count].length);
}
	
fclose(fpOutputGeneScore);
printf("%i\n", countTotal);
}

int createCoExpresionGeneHash(FILE *fp)
{
srand(time(NULL));
int hashId;
coExpresGeneNum=0;
int coExprId=0;
char ensName[100];
char geneName[geneNameLen];
	while (fscanf(fp, "%i\t%s\t%s\n", &coExprId, ensName, geneName)!=EOF)
	{
		coExpresGeneNum++;
		hashId=-1;
		for (int count=0; count<numNodes; count++)
		{
			if (strcmp(listNodes[count].nodeName, geneName)==0)
			{
				hashId=count;
			}
		} 
		strcpy(coExpresionGeneHashTable[coExprId].geneName, geneName);
		coExpresionGeneHashTable[coExprId].hashId=coExprId;
		coExpresionGeneHashTable[coExprId].nodeId=hashId;
	}
}


bool includesTheNode(int nodeId, int *listNodes, int sizeList)
{
	for (int count=0; count<sizeList; count++)
	{
		if (listNodes[count]==nodeId)
			return true;
	}
return false;
}


int calculateCoExpressionPValue()
{

float totalPairWise=0;
	for (int count=0; count<numNodes; count++)
	{
		for (int count2=count+1; count2<numNodes; count2++)
		{
			coexpressionPValue[(int)floor(coExpresionMatrix[count][count2]*100)]++;
			totalPairWise++;
			meanCoExpression=meanCoExpression+coExpresionMatrix[count][count2];
		}
	}

	meanCoExpression=2*(float)meanCoExpression/(float)(numNodes*numNodes - numNodes);

	for (int count=0; count<numNodes; count++)
	{
		for (int count2=0; count2<numNodes; count2++)
		{
			varianceCoExpression=varianceCoExpression+(coExpresionMatrix[count][count2] - meanCoExpression)*(coExpresionMatrix[count][count2] - meanCoExpression); 
		}
	}

	varianceCoExpression = 2*(float)varianceCoExpression/(float)(numNodes*numNodes - numNodes);


for (int count=0; count<101; count++)
{
	printf("%i %f\n", count, coexpressionPValue[count]);
	coexpressionPValue[count]=(float)coexpressionPValue[count]/(float)totalPairWise;
}

printf("Mean and Variance %f %f\n", meanCoExpression, varianceCoExpression);
	
}


bool isSubGraphConnectedComponent(int *listSubgraphNodes, int sizeSubgraph)
{
	int *listNodesCovered;
	int indexQueue, endOfQueue;
	listNodesCovered= (int *) malloc(sizeSubgraph * sizeof(int));
	listNodesCovered[0]=listSubgraphNodes[0];
	
	indexQueue=0;
	endOfQueue=1;

	do{
			for (int count2=0; count2<listNodes[listNodesCovered[indexQueue]].degree; count2++)
			{
				if (includesTheNode(listNodes[listNodesCovered[indexQueue]].neighbours[count2], listNodesCovered, endOfQueue)==false && includesTheNode(listNodes[listNodesCovered[indexQueue]].neighbours[count2], listSubgraphNodes, sizeSubgraph)==true)
				{
					listNodesCovered[endOfQueue]=listNodes[listNodesCovered[indexQueue]].neighbours[count2];
					endOfQueue++;
				} 	
			}
			indexQueue++;	


	}while(indexQueue!=endOfQueue);

if (endOfQueue==sizeSubgraph)
	return true;
else return false;
	
}
/*
int main(int argv, char *argc[])
{
	FILE *fp=fopen(argc[1],"r");
	FILE *fp2=fopen(argc[2],"r");
	FILE *fp3=fopen(argc[3],"r");
	FILE *fp4=fopen(argc[4],"r");
	FILE *fp5=fopen(argc[5],"r");
	FILE *fp6=fopen(argc[6],"r");
	createPPI_Graph(fp);
	assignScoreToBothControlandCases(fp2, fp3, fp4);
	createCoExpresionGeneHash(fp5);	
	createCoExpresionMatix(fp6);	
}*/
