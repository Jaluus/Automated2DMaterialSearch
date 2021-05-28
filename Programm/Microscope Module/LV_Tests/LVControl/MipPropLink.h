//==============================================================================================
//	FILE      :	PropertyBag.h
//	ABSTRACT  :	Property bag for camera settings
//	HISTORY   :	2005.11.05 Nikon Corp. - Created
//
//	Copyright (c) 2005, Nikon Corp.  All rights reserved.
//==============================================================================================
#ifndef _MIPPROPLINK_H
#define _MIPPROPLINK_H

class CMipPropLink
{
public:
	IMipParameterPtr	  m_pProperty;   // a microscope-interface-parameter object
	IMipParamEventSinkPtr m_pLink;       // async link to sink parameter events

// methods
	CMipPropLink()
		: m_pProperty(NULL)
		, m_pLink(NULL)
	{
	}

	~CMipPropLink()
	{
		UnAdviseAsync();
	}


	//////////////////////////////////////////////////////////////////////////////////
	// The AdviseAsync() method creates a subscription to the events fired by the m_pProperty
	// object. When the link object receives an event, it will post a specified uMsg message 
	// to the specified hWnd with the specified wParam argument. The lParam argument depends
	// on the event received: EMIPPAR_VALUECHANGED, EMIPPAR_INFOCHANGED or EMIPPAR_DISCONNECT.
	// Call UnAdviseAsync() to unsubscribe from this event stream.
	//////////////////////////////////////////////////////////////////////////////////
	HRESULT AdviseAsync(long hWnd, long uMsg,long wParam) // resolves Path and Property and creates connection
	{
		HRESULT hr;
		if(m_pProperty == NULL)
		{
			return E_FAIL;
		}
		hr = m_pProperty->AdviseAsync(hWnd, uMsg,wParam,&m_pLink);
		return hr;
	}

	//////////////////////////////////////////////////////////////////////////////////
	// The UnAdviseAsync() method unsubscribes from the MIP parameter event stream.
	// This method is called automatically on a EMIPPAR_DISCONNECT event.
	//////////////////////////////////////////////////////////////////////////////////
	HRESULT UnAdviseAsync(void)
	{
		HRESULT hr = S_OK;
		if(m_pLink)
		{
			hr = m_pLink->UnAdvise();
			m_pLink = NULL;
		}
		return hr;
	}

	//////////////////////////////////////////////////////////////////////////////////
	// Call ResetValueChangedFlag at the start of the handler of a EMIPPAR_VALUECHANGED
	// event. By default, a message is posted only on reception of the first event.
	// To receive more messages, the value-changed flag must be reset.
	//////////////////////////////////////////////////////////////////////////////////
	void ResetValueChangedFlag(void)
	{
		if(m_pLink)
		{
			m_pLink->ResetValueChangedFlag();
		}
	}

	//////////////////////////////////////////////////////////////////////////////////
	// Call ResetInfoChangedFlag at the start of the handler of a EMIPPAR_INFOCHANGED
	// event. By default, a message is posted only on reception of the first event.
	// To receive more messages, the value-changed flag must be reset.
	//////////////////////////////////////////////////////////////////////////////////
	void ResetInfoChangedFlag(void)
	{
		if(m_pLink)
		{
			m_pLink->ResetInfoChangedFlag();
		}
	}

	//////////////////////////////////////////////////////////////////////////////////
	// The following operators allow this class to be cast as a IMipParameter *
	//////////////////////////////////////////////////////////////////////////////////
	IMipParameter * operator -> (void) { return m_pProperty; }
	operator IMipParameter * (void) { return m_pProperty; }

	//////////////////////////////////////////////////////////////////////////////////
	// The following operators allow this class to be used in if-tests
	//////////////////////////////////////////////////////////////////////////////////
	bool operator ! (void) { return m_pProperty == NULL; }
	operator bool (void) { return m_pProperty != NULL; }
};

#endif _MIPPROPLINK_H