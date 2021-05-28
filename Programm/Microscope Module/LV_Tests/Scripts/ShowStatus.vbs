Public Const StatusUnknown = -1
Public Const StatusFalse = 0
Public Const StatusTrue = 1

dim nikonLV
set nikonLV = CreateObject ("Nikon.LvMic.NikonLV")

dim strMsg
strMsg = "LV status:" & vbCr & vbCr

If (nikonLV.Nosepiece.IsMounted = StatusTrue) Then
	strMsg = strMsg & CStr ("Nosepiece.Position: ") & nikonLV.Nosepiece.Position & vbCr
End If

If (nikonLV.FilterBlockCassette.IsMounted = StatusTrue) Then
	strMsg = strMsg & CStr ("FilterBlockCassette.Position: ") & nikonLV.FilterBlockCassette.Position & vbCr
End If

If (nikonLV.EpiApertureStop.IsMounted = StatusTrue) Then
	strMsg = strMsg & CStr ("EpiApertureStop.ApertureStop: ") & nikonLV.EpiApertureStop.ApertureStop & nikonLV.EpiApertureStop.Unit & vbCr
End If

If (nikonLV.EpiLamp.IsMounted = StatusTrue) Then
	strMsg = strMsg & CStr ("EpiLamp.Voltage: ") & nikonLV.EpiLamp.Voltage.DisplayString & nikonLV.EpiLamp.Unit & vbCr
End If

If (nikonLV.EpiShutter.IsMounted = StatusTrue) Then
	strMsg = strMsg & CStr ("EpiShutter.IsOpened: ") & nikonLV.EpiShutter.IsOpened.DisplayString & vbCr
End If

If (nikonLV.NDFilter.IsMounted = StatusTrue) Then
	strMsg = strMsg & CStr ("NDFilter.Transmission: ") & nikonLV.NDFilter.Position.DisplayString & vbCr
End If

If (nikonLV.DiaLamp.IsMounted = StatusTrue) Then
	strMsg = strMsg & CStr ("DiaLamp.Voltage: ") & nikonLV.DiaLamp.Voltage.DisplayString & nikonLV.DiaLamp.Unit & vbCr
End If

If (nikonLV.ZDrive.IsMounted = StatusTrue) Then
	strMsg = strMsg & CStr ("ZDrive.Position: ") & nikonLV.ZDrive.Position & nikonLV.ZDrive.Unit & vbCr
End If

If (nikonLV.DICPrism.IsMounted = StatusTrue) Then
	strMsg = strMsg & CStr ("DICPrism.Position: ") & nikonLV.DICPrism.Value & nikonLV.DICPrism.Unit & vbCr
End If

MsgBox strMsg
