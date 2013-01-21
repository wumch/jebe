
#ifndef _MATRIX_H
#define _MATRIX_H
    
#include<assert.h>
#include<stdlib.h>
#include<stdio.h>
  
//初始化一个二维矩阵
double** getMatrix(int rows,int columns){
    double **rect=(double**)calloc(rows,sizeof(double*));
    int i;
    for(i=0;i<rows;++i)
        rect[i]=(double*)calloc(columns,sizeof(double));
    return rect;
}
  
//返回一个单位矩阵
double** getIndentityMatrix(int rows){
    double** IM=getMatrix(rows,rows);
    int i;
    for(i=0;i<rows;++i)
        IM[i][i]=1.0;
    return IM;
}
    
//返回一个矩阵的副本
double** copyMatrix(double** matrix,int rows,int columns){
    double** rect=getMatrix(rows,columns);
    int i,j;
    for(i=0;i<rows;++i)
        for(j=0;j<columns;++j)
            rect[i][j]=matrix[i][j];
    return rect;
}
  
//从一个一维矩阵得到一个二维矩阵
void getFromArray(double** matrix,int rows,int columns,double *arr){
    int i,j,k=0;
    for(i=0;i<rows;++i){
        for(j=0;j<columns;++j){
            matrix[i][j]=arr[k++];
        }
    }
}
  
//打印二维矩阵
void printMatrix(double** matrix,int rows,int columns){
    int i,j;
    for(i=0;i<rows;++i){
        for(j=0;j<columns;++j){
            printf("%-10f\t",matrix[i][j]);
        }
        printf("\n");
    }
}
  
//释放二维矩阵
void freeMatrix(double** matrix,int rows){
    int i;
    for(i=0;i<rows;++i)
        free(matrix[i]);
    free(matrix);
}
    
//获取二维矩阵的某一行
double* getRow(double **matrix,int rows,int columns,int index){
    assert(index<rows);
    double *rect=(double*)calloc(columns,sizeof(double));
    int i;
    for(i=0;i<columns;++i)
        rect[i]=matrix[index][i];
    return rect;
}
  
//获取二维矩阵的某一列  
double* getColumn(double **matrix,int rows,int columns,int index){
    assert(index<columns);
    double *rect=(double*)calloc(rows,sizeof(double));
    int i;
    for(i=0;i<rows;++i)
        rect[i]=matrix[i][index];
    return rect;
}
  
//设置二维矩阵的某一列   
void setColumn(double **matrix,int rows,int columns,int index,double *arr){
    assert(index<columns);
    int i;
    for(i=0;i<rows;++i)
        matrix[i][index]=arr[i];
}
  
//交换矩阵的某两列
void exchangeColumn(double **matrix,int rows,int columns,int i,int j){
    assert(i<columns);
    assert(j<columns);
    int row;
    for(row=0;row<rows;++row){
        double tmp=matrix[row][i];
        matrix[row][i]=matrix[row][j];
        matrix[row][j]=tmp;
    }
}
  
  
//得到矩阵的转置
double** getTranspose(double **matrix,int rows,int columns){
    double **rect=getMatrix(columns,rows);
    int i,j;
    for(i=0;i<columns;++i){
        for(j=0;j<rows;++j){
            rect[i][j]=matrix[j][i];
        }
    }
    return rect;
}
  
//计算两向量内积
double vectorProduct(double *vector1,double *vector2,int len){
    double rect=0.0;
    int i;
    for(i=0;i<len;++i)
        rect+=vector1[i]*vector2[i];
    return rect;
}
  
//两个矩阵相乘
double** matrixProduct(double **matrix1,int rows1,int columns1,double **matrix2,int columns2){
    double **rect=getMatrix(rows1,columns2);
    int i,j;
    for(i=0;i<rows1;++i){
        for(j=0;j<columns2;++j){
            double *vec1=getRow(matrix1,rows1,columns1,i);
            double *vec2=getColumn(matrix2,columns1,columns2,j);
            rect[i][j]=vectorProduct(vec1,vec2,columns1);
            free(vec1);
            free(vec2);
        }
    }
    return rect;
}
  
//得到某一列元素的平方和
double getColumnNorm(double** matrix,int rows,int columns,int index){
    assert(index<columns);
    double* vector=getColumn(matrix,rows,columns,index);
    double norm=vectorProduct(vector,vector,rows);
    free(vector);
    return norm;
}
  
//打印向量
void printVector(double* vector,int len){
    int i;
    for(i=0;i<len;++i)
        printf("%-15.8f\t",vector[i]);
    printf("\n");
}
    
#endif
