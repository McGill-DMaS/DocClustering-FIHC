//---------------------------------------------------------------------------
// File:
//      DCStopwordHandler.h, DCStopwordHandler.cpp
// History:
//      Feb. 27, 2002   Created by Linda.
//---------------------------------------------------------------------------

#include "stdafx.h"

#if !defined(DCSTOPWORDHANDLER_H)
	#include "DCStopwordHandler.h"
#endif

#define KBYTE 1024
#define MAX_READ 8 * KBYTE
//#define MAX_READ 100
//#define TYPE_STOPWDLIST 0
//#define TYPE_CLEANFILE 1

//#include "Afx.h"        //CFile, CString
//#include "Afxcoll.h"    //CStringArray


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDCStopwordHandler::CDCStopwordHandler(LPCTSTR stopwordFile)
{
	m_stopwordFilePath = stopwordFile;

	initialize();	
}


CDCStopwordHandler::~CDCStopwordHandler()
{
	m_stopwordArray.RemoveAll();
	m_cleanFile.RemoveAll();

}


void CDCStopwordHandler::initialize()
{
	for(int i = 0; i < LETTER_NUM; i ++)
	{
		m_beginPosition[i] = -1;
	}
}

//////////////////////////////////////////////////////////////////////
// getCleanFile(): return the clean file with stop word removed
//////////////////////////////////////////////////////////////////////
CStringArray * CDCStopwordHandler::getCleanFile()
{
	return &m_cleanFile;
}



//////////////////////////////////////////////////////////////////////
// readFileStream(): read the file into a buffer
//////////////////////////////////////////////////////////////////////
int CDCStopwordHandler::readFileStream(LPCTSTR path, char * buffer, int bufsize)
{
	CFile * dFile = NULL;
	
	TRY
	{
		dFile = new CFile(path, CFile::modeRead);
	}
	CATCH(CFileException, pEX)
	{
		printf("Error in opening file %s\n", path);
		
		if (dFile == NULL)
		{
			printf("dFile is NULL!\n");
		}

		else
		{
			delete dFile;
		}

		return 0;	
	}
	END_CATCH
	
	int wCount = dFile -> Read(buffer, bufsize - 1);
	dFile -> Close();	
	delete dFile;

	buffer[wCount] = '\n';		//append a carriage at the end of buffer

	return wCount + 1;
}

/*
//////////////////////////////////////////////////////////////////////
// restoreWords(): 
// For stop word file, restore store  into m_
// For clean file, restore 
//////////////////////////////////////////////////////////////////////
void CDCStopwordHandler::restoreWords()
{

}
*/

//////////////////////////////////////////////////////////////////////
// createStopwordArray(): create a stop word list from the stop word file
// all the words in the stop word file must be in low case
//////////////////////////////////////////////////////////////////////
int CDCStopwordHandler::createStopwordArray()
{
	char buffer[MAX_READ];
	int count = readFileStream(m_stopwordFilePath, buffer, MAX_READ);

	CString oneWord;
	int i, len = 0;
	
	char currentLetter = ' ';


	for(i = 0; i < count; i ++)
	{
		if( buffer[i] != '\n' )
		{
			oneWord += tolower(buffer[i]);
		}
		
		else
		{
			oneWord.TrimRight();
			oneWord.TrimLeft();
			
			if( oneWord.GetLength() != 0 )
			{
				m_stopwordArray.Add(oneWord);
				len ++;

				if( oneWord.GetAt(0) != currentLetter )	// all the words are in low case
				{
					currentLetter = oneWord.GetAt(0);
					m_beginPosition[currentLetter - 'a'] = len - 1;
				}

				oneWord.Empty();
			}
		}
	}

	m_stopwordNum = m_stopwordArray.GetSize();

	return 0;

}


//////////////////////////////////////////////////////////////////////
// isStopword(): to check whether a word is in the stop word list
//////////////////////////////////////////////////////////////////////
BOOL CDCStopwordHandler::isStopword(CString oneWord)
{
	CString wd(oneWord);
	wd.MakeLower();
	
	int index = wd.GetAt(0) - 'a';
	
	int begin = m_beginPosition[index];	
	if(begin == -1)				// the stop word list does not contains this word.
	{
		//printf("%s: begin = -1, it is not stop word\n", oneWord);
		return false;
	}

	int end, i;
	for(i = index + 1; i < LETTER_NUM; i ++)
	{
		if(m_beginPosition[i] != -1)
		{
			end = m_beginPosition[i] - 1;
			break;
		}
	}
	if(i == LETTER_NUM)
	{
		end = m_stopwordNum - 1;
	}

	//printf("%s: begin = %d, end = %d, ", oneWord, begin, end);

	int position = bisectSearch(m_stopwordArray, wd, begin, end); 	

	if(position != -1)
	{
		//printf("position = %d, it is stop word\n", position);
		return true;
	}
	
	//printf("position = -1, it is not stop word\n");
	return false;
}


//////////////////////////////////////////////////////////////////////
// bisectSearch(): search for a given word in the stop word array
//////////////////////////////////////////////////////////////////////
int CDCStopwordHandler::bisectSearch(CStringArray& array, CString wd, int begin, int end)
{
	int mid;
	int comp;

	while(begin <= end) 
	{
		mid = (begin + end) / 2;
		
		comp = wd.Compare(array[mid]); 
		//comp = wd.CompareNoCase(array[mid]); 

		if( comp == 0) 
		{
			return mid;
		}

		if(comp < 0) 
		{
			end = mid - 1;
		}
		else
		{
			begin = mid + 1;
		}
	}

	return -1;
}


//////////////////////////////////////////////////////////////////////
// isValidLetter(): check whether the char is a~z or A~Z
//////////////////////////////////////////////////////////////////////
BOOL CDCStopwordHandler::isValidLetter(char c)
{
	
	if ( ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) )
	{
		return true;
	}
	return false;
	
}


//////////////////////////////////////////////////////////////////////
// rmStopwords(): the stop word removal function called by other class
//////////////////////////////////////////////////////////////////////
void CDCStopwordHandler::rmStopwords(char * dirtyFilePath)
{
	char buffer[MAX_READ];
	int count =	readFileStream(dirtyFilePath, buffer, MAX_READ);

	m_cleanFile.RemoveAll();

	CString oneWord;
	char c;
	int i;
	
	for(i = 0; i < count; i ++)
	{
		c = buffer[i];
		if( isValidLetter(c) )
		{
			oneWord += c;
		}
		
		else
		{
			if( oneWord.GetLength()!=0 ) 
			{
				if(!isStopword(oneWord))
				{
					oneWord.MakeLower();
					m_cleanFile.Add(oneWord);
				}
				oneWord.Empty();
			}	
		}
	}


	/* ---------------------- for test only:start ------------------------ 
	
	int j;

	printf("\n----------------- Stop word list ----------------\n");
	int size = m_stopwordArray.GetSize();
	for(j =0; j < size; j ++)
	{
		CString S = m_stopwordArray.GetAt(j);
		printf("%s ", S);
	}
	printf("");

	printf("\n\n---------------- Before stop word removing ------------\n%s\n\n", buffer);

	printf("------------------- After stop word removing ----------------\n");
	size = m_cleanFile.GetSize();
	for(j =0; j < size; j ++)
	{
		CString S = m_cleanFile.GetAt(j);
		printf("%s ", S);
	}
	
	printf("\n");

	 ---------------------- for test only: end ------------------------ */
	
}





