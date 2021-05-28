Public Const StatusUnknown = -1
Public Const StatusFalse = 0
Public Const StatusTrue = 1

dim nikonLV
set nikonLV = CreateObject ("Nikon.LvMic.NikonLV")

If (nikonLV.Nosepiece.IsMounted = StatusTrue) Then
	nikonLV.Nosepiece.Reverse
	dim lPos
	lPos = nikonLV.Nosepiece.Position
	dim strMsg
	strMsg = "Nosepiece.Position: " & CStr(lPos) & vbCr
	strMsg = strMsg & "Name: " & nikonLV.Database.Objectives(lPos).Name & vbCr
	strMsg = strMsg & "Description: " & nikonLV.Database.Objectives(lPos).Description & vbCr
	MsgBox strMsg
Else
	MsgBox "Nosepiece is not mounted."
End If
