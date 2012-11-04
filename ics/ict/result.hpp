//////////////////////////////////////////////////////////////////////
//ICTCLAS简介：计算所汉语词法分析系统ICTCLAS(Institute of Computing Technology, Chinese Lexical Analysis System)，
//             功能有：中文分词；词性标注；未登录词识别。
//             分词正确率高达97.58%(973专家评测结果)，
//             未登录词识别召回率均高于90%，其中中国人名的识别召回率接近98%;
//             处理速度为31.5Kbytes/s。
//著作权：  Copyright?2002-2005中科院计算所 职务著作权人：张华平 刘群
//遵循协议：自然语言处理开放资源许可证1.0
//Email: zhanghp@software.ict.ac.cn
//Homepage:www.nlp.org.cn;mtgroup.ict.ac.cn
//Result.h: interface for the CResult class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESULT_H__DEB31BCA_0410_4D5E_97EA_78F9B16B8938__INCLUDED_)
#define AFX_RESULT_H__DEB31BCA_0410_4D5E_97EA_78F9B16B8938__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <string.h>
#include "dictionary.hpp"
#include "segment.hpp"
#include "span.hpp"
#include "unknowword.hpp"
#define _ICT_DEBUG 0

class CResult;
class COutput
{
private:
	CResult* r;
public:
	COutput(CResult* r_);

	bool operator()(PWORD_RESULT pItem, char *sResult,bool bFirstWordIgnore);
};

class CResult  
{
	friend class COutput;
public:
	double m_dSmoothingPara;
	bool Processing(char *sSentence,unsigned int nCount);
	bool ParagraphProcessing(char *sParagraph,char *sResult);
	bool ParagraphProcessing(char *sParagraph, unsigned int pLen, char *sResult);

	template<typename Callback>
	bool ParagraphProcessing(char *sParagraph, unsigned int pLen, char *sResult, Callback& callbak);

	template<typename Callback>
	bool ParagraphWalk(char *sParagraph, unsigned int pLen, Callback& callbak)
	{
		char *sSentence,sChar[3];
		//	char *sSentenceResult;
			unsigned int nLen=pLen+13;
			sSentence=new char[nLen];//malloc buffer
		//	sSentenceResult=new char[nLen*3];//malloc buffer
			sSentence[0]=0;
			unsigned int nPosIndex=0,nParagraphLen=pLen;//nSentenceIndex=0;
			sChar[2]=0;
		//	sResult[0]=0;//Init the result
			bool bFirstIgnore=true;
			strcpy(sSentence,SENTENCE_BEGIN);//Add a sentence begin flag
			while(nPosIndex<nParagraphLen)
			{//Find a whole sentence which separated by ! . \n \r
				sChar[0]=sParagraph[nPosIndex];//Get a char
				sChar[1]=0;
				if(sParagraph[nPosIndex]<0)
				{//double byte char
					nPosIndex+=1;
					sChar[1]=sParagraph[nPosIndex];
				}
				nPosIndex+=1;
		/*
		#define  SEPERATOR_C_SENTENCE "。！？：；…"
		#define  SEPERATOR_C_SUB_SENTENCE "、，（）“”‘’"
		#define  SEPERATOR_E_SENTENCE "!?:;"
		#define  SEPERATOR_E_SUB_SENTENCE ",()\042'"
		#define  SEPERATOR_LINK "\n\r 　"
		*/
				if(CC_Find(SEPERATOR_C_SENTENCE,sChar)||CC_Find(SEPERATOR_C_SUB_SENTENCE,sChar)||strstr(SEPERATOR_E_SENTENCE,sChar)||strstr(SEPERATOR_E_SUB_SENTENCE,sChar)||strstr(SEPERATOR_LINK,sChar))
				{//Reach end of a sentence.Get a whole sentence
					if(!strstr(SEPERATOR_LINK,sChar))//Not link seperator
					{
						strcat(sSentence,sChar);
					}
					if(sSentence[0]!=0&&strcmp(sSentence,SENTENCE_BEGIN)!=0)
					{
						if(!strstr(SEPERATOR_C_SUB_SENTENCE,sChar)&&!strstr(SEPERATOR_E_SUB_SENTENCE,sChar))
							strcat(sSentence,SENTENCE_END);//Add sentence ending flag

						Processing(sSentence,1);//Processing and output the result of current sentence.
						callbak(m_pResult[0],bFirstIgnore);//Output to the imediate result
						//bFirstIgnore=true;
					}
					if(strstr(SEPERATOR_LINK,sChar))//Link the result with the SEPERATOR_LINK
					{
		//				strcat(sResult,sChar);
						strcpy(sSentence,SENTENCE_BEGIN);//Add a sentence begin flag

						//sSentence[0]=0;//New sentence, and begin new segmentation
						//bFirstIgnore=false;
					}
					else if(strstr(SEPERATOR_C_SENTENCE,sChar)||strstr(SEPERATOR_E_SENTENCE,sChar))
					{
						strcpy(sSentence,SENTENCE_BEGIN);//Add a sentence begin flag
						//sSentence[0]=0;//New sentence, and begin new segmentation
						//bFirstIgnore=false;
					}
					else
					{
						strcpy(sSentence,sChar);//reset current sentence, and add the previous end at begin position
					}
				}
				else //Other chars and store in the sentence buffer
					strcat(sSentence,sChar);
			}
			if(sSentence[0]!=0&&strcmp(sSentence,SENTENCE_BEGIN)!=0)
			{
				strcat(sSentence,SENTENCE_END);//Add sentence ending flag
				Processing(sSentence,1);//Processing and output the result of current sentence.
				callbak(m_pResult[0],bFirstIgnore);//Output to the imediate result
			}
			delete []  sSentence;//FREE sentence buffer
			return true;
	}

	bool FileProcessing(char *sFilename,char *sResultFile);
	PWORD_RESULT *m_pResult;
	//The buffer which store the segment and POS result
	//and They stored order by its possibility
	ELEMENT_TYPE m_dResultPossibility[MAX_SEGMENT_NUM];
	int m_nResultCount;
	bool Output(PWORD_RESULT pItem,char *sResult,bool bFirstWordIgnore=false);
	CResult();
	virtual ~CResult();
	int m_nOperateType;//0:Only Segment;1: First Tag; 2:Second Type
	int m_nOutputFormat;//0:PKU criterion;1:973 criterion; 2: XML criterion
private:
	CSegment m_Seg;//Seg class
	CDictionary m_dictCore,m_dictBigram;//Core dictionary,bigram dictionary
	CSpan m_POSTagger;//POS tagger
	CUnknowWord m_uPerson,m_uTransPerson,m_uPlace;//Person recognition
protected:
	bool ChineseNameSplit(char *sPersonName,char *sSurname, char *sSurname2,char *sGivenName,CDictionary &personDict);
	bool PKU2973POS(int nHandle,char *sPOS973);
	bool Adjust(PWORD_RESULT pItem,PWORD_RESULT pItemRet);
	ELEMENT_TYPE ComputePossibility(PWORD_RESULT pItem);
	bool Sort();
};

#endif // !defined(AFX_RESULT_H__DEB31BCA_0410_4D5E_97EA_78F9B16B8938__INCLUDED_)
