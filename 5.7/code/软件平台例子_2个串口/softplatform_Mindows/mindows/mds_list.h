
#ifndef  MDS_LIST_H
#define  MDS_LIST_H


/*********************************** º¯ÊýÉùÃ÷ *************************************/
extern void MDS_DlistInit(M_DLIST* pstrList);
extern void MDS_DlistNodeAdd(M_DLIST* pstrList, M_DLIST* pstrNode);
extern M_DLIST* MDS_DlistNodeDelete(M_DLIST* pstrList);
extern void MDS_DlistCurNodeInsert(M_DLIST* pstrList, M_DLIST* pstrNode,
                                   M_DLIST* pstrNewNode);
extern M_DLIST* MDS_DlistCurNodeDelete(M_DLIST* pstrList, M_DLIST* pstrNode);
extern M_DLIST* MDS_DlistEmpInq(M_DLIST* pstrList);
extern M_DLIST* MDS_DlistNextNodeEmpInq(M_DLIST* pstrList, M_DLIST* pstrNode);


#endif

