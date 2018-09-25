#include "stdafx.h"
#include "editsession.h"

//////////////////////////////////////////////////////////////////////////
CEditSessionBase::CEditSessionBase(CSinstar3Tsf *pTextService, ITfContext *pContext)
:_pTextService(pTextService)
,_pContext(pContext)
{
}


//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CEsKeyHandler::DoEditSession(TfEditCookie ec)
{
	BOOL fEaten = FALSE;
	GetTextService()->_bInKeyProc = TRUE;
	GetSinstar3()->ProcessKeyStoke(GetContext(),(UINT)_wParam,_lParam, TRUE, &fEaten);
	if(fEaten)
	{
		GetSinstar3()->TranslateKey(GetContext(),(UINT)_wParam, MapVirtualKey((UINT)_wParam,0), TRUE, &fEaten);
	}
	GetTextService()->_bInKeyProc = FALSE;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
CEsStartComposition::CEsStartComposition(CSinstar3Tsf *pTextService, ITfContext *pContext) 
: CEditSessionBase(pTextService, pContext)
{

}

STDMETHODIMP CEsStartComposition::DoEditSession(TfEditCookie ec)
{
	SLOG_INFO("TfEditCookie:"<<ec);
	CComPtr<ITfInsertAtSelection> pInsertAtSelection;
	CComPtr<ITfRange> pRangeInsert;
	CComPtr<ITfContextComposition> pContextComposition;
	CComPtr<ITfComposition> pComposition;
	HRESULT hr = E_FAIL;

	// A special interface is required to insert text at the selection
	hr = _pContext->QueryInterface(IID_ITfInsertAtSelection, (void **)&pInsertAtSelection);
	ASSERT_HR(hr);

	// insert the text
	hr = pInsertAtSelection->InsertTextAtSelection(ec, TF_IAS_QUERYONLY, NULL, 0, &pRangeInsert);
	ASSERT_HR(hr);

	// get an interface on the context to deal with compositions
	hr = _pContext->QueryInterface(IID_ITfContextComposition, (void **)&pContextComposition);
	ASSERT_HR(hr);

	// start the new composition
	hr = pContextComposition->StartComposition(ec, pRangeInsert, _pTextService, &pComposition);
	ASSERT_HR(hr);
	ASSERT_RET(pComposition, return E_FAIL);

	// 
	//  set selection to the adjusted range
	// 
	TF_SELECTION tfSelection;
	tfSelection.range = pRangeInsert;
	tfSelection.style.ase = TF_AE_NONE;
	tfSelection.style.fInterimChar = FALSE;
	_pContext->SetSelection(ec, 1, &tfSelection);

	// Store the pointer of this new composition object in the instance 
	// of the CTextService class. So this instance of the CTextService 
	// class can know now it is in the composition stage.
	_pTextService->OnStartComposition(ec,pComposition);

	//trigger layout changed
	CComPtr<ITfContextView> pCtxView;
	hr = _pContext->GetActiveView(&pCtxView);
	if(SUCCEEDED(hr))
	{
		_pTextService->OnLayoutChange(_pContext,TF_LC_CHANGE,pCtxView);
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
CEsEndComposition::CEsEndComposition(CSinstar3Tsf *pTextService, ITfContext *pContext) 
: CEditSessionBase(pTextService, pContext)
{

}

STDMETHODIMP CEsEndComposition::DoEditSession(TfEditCookie ec)
{
	SLOG_INFO("TfEditCookie:"<<ec);
	ITfComposition * pComposition = _pTextService->GetITfComposition();
	if(!pComposition)
	{
		SLOG_WARN("CEditSessionEndComposition::DoEditSession not in compositing");
		return E_FAIL;
	}
	CComPtr<ITfRange> pRange;
	if ( pComposition->GetRange( &pRange) == S_OK && pRange != NULL)
	{
		TF_SELECTION sel={0};
		sel.style.ase = TF_AE_NONE;
		sel.style.fInterimChar = FALSE;
		sel.range=pRange;
		pRange->Collapse(ec,TF_ANCHOR_END);
		_pContext->SetSelection(ec,1,&sel);
	}

	pComposition->EndComposition(ec);
	_pTextService->_TerminateComposition(ec,_pContext);
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
CEsGetTextExtent::CEsGetTextExtent(CSinstar3Tsf *pTextService, ITfContext *pContext, ITfContextView *pContextView) 
: CEditSessionBase(pTextService, pContext)
, _pContextView(pContextView)
{
}


STDMETHODIMP CEsGetTextExtent::DoEditSession(TfEditCookie ec)
{
	SLOG_INFO("TfEditCookie:"<<ec);

	CComPtr<ITfRange> pRange;
	ISinstar * pSinstar3 = GetSinstar3();
	if(!pSinstar3 || !_pTextService->_pComposition) return S_FALSE;

	CComPtr<ITfRange> range;
	if ( _pTextService->_pComposition->GetRange( &range) == S_OK && range != NULL)
	{
		BOOL fClip = FALSE;
		RECT rc;
		_pContextView->GetTextExt(ec,range,&rc,&fClip);
		pSinstar3->OnSetCaretPosition( *(POINT*)&rc, rc.bottom - rc.top);			
		SLOGFMTI("SetCaret pos:%d,%d, height: %d",rc.left,rc.top, rc.bottom - rc.top);
	}

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
CEsChangeComposition::CEsChangeComposition(CSinstar3Tsf *pTextService, ITfContext *pContext,int nLeft,int nRight,LPCWSTR pszBuf,int nLen) : CEditSessionBase(pTextService, pContext)
{
	if(nLen) 
	{
		m_pszBuf=new WCHAR[nLen];
		wcsncpy(m_pszBuf,pszBuf,nLen);
	}else
	{
		m_pszBuf=NULL;
	}
	m_nLeft=nLeft,m_nRight=nRight;
	m_nLen=nLen;
}

CEsChangeComposition::~CEsChangeComposition()
{
	if(m_pszBuf) delete []m_pszBuf;
}


STDMETHODIMP CEsChangeComposition::DoEditSession(TfEditCookie ec)
{
	SLOG_INFO("TfEditCookie:"<<ec);

	if(!_pTextService->IsCompositing()) 
		_pTextService->_StartComposition(_pContext);
	CComPtr<ITfRange> pRangeComposition;
	CComPtr<ITfRange> pRangeSel;
	TF_SELECTION tfSelection;
	ULONG cFetched;
	BOOL fCovered;
	// first, test where a keystroke would go in the document if an insert is done
	if (_pContext->GetSelection(ec, TF_DEFAULT_SELECTION, 1, &tfSelection, &cFetched) != S_OK || cFetched != 1)
		return S_FALSE;

	pRangeSel.Attach(tfSelection.range);

	// is the insertion point covered by a composition?
	if (_pTextService->_pComposition->GetRange(&pRangeComposition) == S_OK)
	{
		fCovered = IsRangeCovered(ec, pRangeSel, pRangeComposition);
		if(fCovered)
		{
			if(m_nLeft==0 && m_nRight==-1)
			{//全部替换
				pRangeSel->ShiftStartToRange(ec,pRangeComposition,TF_ANCHOR_START);
				pRangeSel->ShiftEndToRange(ec,pRangeComposition,TF_ANCHOR_END);
			}else	if(m_nLeft!=-1 && m_nRight!=-1)
			{//替换指定范围
				LONG cch=0;
				pRangeSel->ShiftStartToRange(ec,pRangeComposition,TF_ANCHOR_START);
				pRangeSel->ShiftStart(ec,m_nLeft,&cch,NULL);
				pRangeSel->Collapse(ec,TF_ANCHOR_START);
				pRangeSel->ShiftEnd(ec,m_nRight-m_nLeft,&cch,NULL);
			}
		}

		if (!fCovered)
		{
			return S_OK;
		}
	}

	// insert the text
	// Use SetText here instead of InsertTextAtSelection because a composition is already started
	// Don't allow the app to adjust the insertion point inside our composition
	if (pRangeSel->SetText(ec, 0, m_pszBuf, m_nLen) != S_OK)
		return S_OK;

	// update the selection, and make it an insertion point just past
	// the inserted text.
	pRangeSel->Collapse(ec, TF_ANCHOR_END);
	_pContext->SetSelection(ec, 1, &tfSelection);
	//
	// set the display attribute to the composition range.
	//
	_pTextService->UpdateCompAttr(_pContext,ec,pRangeComposition);
	if(_pTextService->m_pSinstar3) _pTextService->m_pSinstar3->OnCompositionChanged();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
CEsMoveCaret::CEsMoveCaret(CSinstar3Tsf *pTextService, 
											 ITfContext *pContext,
											 int nPos,
											 BOOL bSet,
											 ITfComposition *pComposition) 
											 : CEditSessionBase(pTextService, pContext)
											 ,m_pComposition(pComposition)
											 ,m_nPos(nPos)
											 ,m_bSet(bSet)
{
}


STDMETHODIMP CEsMoveCaret::DoEditSession(TfEditCookie ec)
{
	SLOG_INFO("TfEditCookie:"<<ec);

	CComPtr<ITfRange> pRangeComposition;
	TF_SELECTION tfSelection;
	ULONG cFetched;
	BOOL fCovered;

	// first, test where a keystroke would go in the document if an insert is done
	if (_pContext->GetSelection(ec, TF_DEFAULT_SELECTION, 1, &tfSelection, &cFetched) != S_OK || cFetched != 1)
		return S_FALSE;

	// is the insertion point covered by a composition?
	if (m_pComposition->GetRange(&pRangeComposition) == S_OK)
	{
		fCovered = IsRangeCovered(ec, tfSelection.range, pRangeComposition);
		if(fCovered)
		{
			LONG cch=0;
			if(m_bSet)
			{
				tfSelection.range->ShiftStartToRange(ec,pRangeComposition,TF_ANCHOR_START);
				tfSelection.range->ShiftStart(ec,m_nPos,&cch,NULL);
			}else
			{
				tfSelection.range->ShiftStart(ec,m_nPos,&cch,NULL);
			}
			tfSelection.range->Collapse(ec,TF_ANCHOR_START);
		}

		if (!fCovered)
		{
			return S_OK;
		}
	}
	_pContext->SetSelection(ec, 1, &tfSelection);
	if(_pTextService->m_pSinstar3) _pTextService->m_pSinstar3->OnCompositionChanged();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
CEsUpdateResultAndComp::CEsUpdateResultAndComp(CSinstar3Tsf *pTextService, 
																 ITfContext *pContext,
																 LPCWSTR pszResultStr,
																 int nResStrLen,
																 LPCWSTR pszCompStr,
																 int nCompStrLen) 
																 : CEditSessionBase(pTextService, pContext)
{
	if(pszResultStr)
	{
		if(nResStrLen==-1) nResStrLen= (int)wcslen(pszResultStr);
		m_nResStrLen=nResStrLen;
		m_pszResultStr=new WCHAR[m_nResStrLen];
		wcsncpy(m_pszResultStr,pszResultStr,m_nResStrLen);
	}else
	{
		m_pszResultStr=NULL;
		m_nResStrLen=0;
	}
	if(pszCompStr)
	{
		if(nCompStrLen==-1) nCompStrLen=(int)wcslen(pszCompStr);
		m_nCompStrLen=nCompStrLen;
		m_pszCompStr=new WCHAR[m_nCompStrLen];
		wcsncpy(m_pszCompStr,pszCompStr,m_nCompStrLen);
	}else
	{
		m_pszCompStr=NULL;
		m_nCompStrLen=0;
	}
}

CEsUpdateResultAndComp::~CEsUpdateResultAndComp()
{
	if(m_pszCompStr) delete []m_pszCompStr;
	if(m_pszResultStr) delete []m_pszResultStr;
}

STDMETHODIMP CEsUpdateResultAndComp::DoEditSession(TfEditCookie ec)
{
	SLOG_INFO("TfEditCookie:"<<ec);

	CComPtr<ITfRange> pRangeComposition;
	CComPtr<ITfProperty> pDisplayAttributeProperty;


	BOOL fEmpty=TRUE;

	SASSERT(_pTextService->_IsComposing());
	CComPtr<ITfComposition> pCompostion=_pTextService->GetITfComposition();
	//将当前数据上屏
	pCompostion->GetRange(&pRangeComposition);
	if(!pRangeComposition)
	{
		SLOG_WARN("CEsUpdateResultAndComp::DoEditSession getRange return null");
		return E_FAIL;
	}
	// get our the display attribute property
	if (_pContext->GetProperty(GUID_PROP_ATTRIBUTE, &pDisplayAttributeProperty) == S_OK)
	{
		// clear the value over the range
		pDisplayAttributeProperty->Clear(ec, pRangeComposition);
	}
	
	if(m_pszResultStr)
	{
		pRangeComposition->SetText(ec,0,m_pszResultStr,m_nResStrLen);
	}

	if(pRangeComposition->IsEmpty(ec,&fEmpty)==S_OK && !fEmpty)
	{
		pRangeComposition->Collapse(ec,TF_ANCHOR_END);
		pCompostion->ShiftStart(ec,pRangeComposition);
	}

	TF_SELECTION tfSelection;
	tfSelection.style.ase = TF_AE_NONE;
	tfSelection.style.fInterimChar = FALSE;
	tfSelection.range=pRangeComposition;
	//插入新的编码
	if(m_pszCompStr)
	{
		_pTextService->_bPosSaved = FALSE;

		POINT pt={-1,-1};
		_pTextService->m_pSinstar3->OnSetFocusSegmentPosition(pt,0);
		pRangeComposition->SetText(ec,0,m_pszCompStr,m_nCompStrLen);

		pRangeComposition->Clone(&tfSelection.range);
		tfSelection.range->Collapse(ec,TF_ANCHOR_END);
		_pContext->SetSelection(ec,1,&tfSelection);
		tfSelection.range->Release();

		_pTextService->UpdateCompAttr(_pContext,ec,pRangeComposition);

		if(_pTextService->m_pSinstar3) _pTextService->m_pSinstar3->OnCompositionChanged();
	}else
	{
		_pContext->SetSelection(ec,1,&tfSelection);
	}
	return S_OK;
}
