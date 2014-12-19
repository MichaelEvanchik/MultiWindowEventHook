/*
FileName:MultiWindowEventHook.cpp

Author- Michael Evanchik




*/
#include <windows.h>
#include <fstream>
using namespace std;
LRESULT CALLBACK WindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);//Window CallBack Procedure


//Initialized Data to be shared with all instance of the dll
#pragma data_seg("Shared")
HWND hTarget=NULL;
HWND hApp = NULL;
int num=0 ;// Number of the subclassed window handle ,for use in the dll
bool done=FALSE;
HINSTANCE hInstance=NULL;
#pragma data_seg()
// Initialised data End of data share


// Uninitialised Data to be shared with all instance of the dll
#pragma bss_seg("Shared1")
HWND hndll[100];  // array to store handles
int form[100] ;  // Forms which we need to subclass
long OldWndHndl[100] ; //array to store old window handles
BOOL blnsubclassed[100];
HHOOK hWinHook;
long MESSAGE;
long NEW_MESSAGE;
long NEW_MESSAGE_WPARAM;
#pragma bss_seg()

//End Uninitialized data

BOOL WINAPI DllMain(
  HANDLE hinstDLL,  // handle to the DLL module
  DWORD fdwReason,     // reason for calling function
  LPVOID lpvReserved   // reserved
)

{

	switch(fdwReason)
	{
	
	case DLL_PROCESS_ATTACH:
		{
			hInstance=(HINSTANCE)hinstDLL;
		}
		break;

	case DLL_PROCESS_DETACH:
		{

		//	UnSubClass();
		//	UnhookWindowsHookEx(hWinHook);

		}
		break;

	}

return TRUE;

}



//This function wld get all the handles from the Our application and store in in the array
int WINAPI FillHandleArray(HWND hwndSubclass,int intFrmNUm)  
{ 

	hndll[num]=hwndSubclass; // fill the array with the handle
    form[num]=intFrmNUm;  //fill the corresponding array for the form number
	blnsubclassed[num]=FALSE;// set the state to not subclassed
	num=num+1;
	return 1;
}// End of the fill array function


// Function to set the original window procedure of each subclassed window
int WINAPI UnSubclass()
{
	int count;
	for(count=0;count<num;count++)
	{
		if((int)hndll[count]>1)
		{
			SetWindowLong(hndll[count],GWL_WNDPROC,OldWndHndl[count]);   //Set back the old window procedure
			return 1;
		}		
	}	
return 0;
		
}//End UnSubclass function



//The CBT hook Proc(Computer Based Training Hook)
LRESULT CALLBACK CBTProc(int nCode,WPARAM wParam,LPARAM lParam)
{


	if (nCode==HCBT_ACTIVATE)  //Called when the application window is activated
	{
	
		if((HWND)(wParam)==hTarget)  //check if the window activated is Our Targer App
		{   
					
			int count;
			for (count=0;count<num;count++)
			{
				if (blnsubclassed[count]==FALSE)
				{	
					if(((int)hndll[count])>1)
					{
						OldWndHndl[count]=SetWindowLong(hndll[count],GWL_WNDPROC,(long)WindowProc);  //Subclass !!!!
					}
										
					blnsubclassed[count]=TRUE;	// Set state as subclassed
				}
			}

		}		
	}
	if (nCode==HCBT_DESTROYWND) //Called when the application window is destroyed
	{

		if((HWND)wParam==hTarget)
			SendNotifyMessage(hApp,WM_APP +1024,(WPARAM)wParam,(LPARAM)lParam);// Send the message  to the vb app
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}//End of the hook procedure

// Get the handles of the Targetwindow and of the Our application
int WINAPI SetHandle(HWND HandleofTarget ,HWND HandleofApp, long iMessageToTrap, long iMessageToReplace, long iMessageToReplace_wParam)
{

	hTarget=HandleofTarget;
	hApp=HandleofApp;
	MESSAGE=iMessageToTrap;
	NEW_MESSAGE=iMessageToReplace;
	NEW_MESSAGE_WPARAM = iMessageToReplace_wParam;
	FillHandleArray(HandleofTarget,1);
	hWinHook=SetWindowsHookEx(WH_CBT,(HOOKPROC)CBTProc,hInstance,GetWindowThreadProcessId(hTarget,NULL));
//	if(hWinHook==NULL)
//		return 0;
//	else
		return 1;

}//End this function
//Window Procedures of the subclassed windows
LRESULT CALLBACK WindowProc(
  HWND hwnd,
  UINT uMsg,
  WPARAM wParam,
  LPARAM lParam
)
{	
	long val;
	int count;
	for(count=0;count<num;count++)
	{
		if(hndll[count]==hwnd)
		{
			val=count;   // this gets us the exact position of this window procedure in the array
		}
	}


	if(uMsg==MESSAGE)
	{
		if(HIWORD(wParam)==0)
		{
			//char h[5];
			//itoa(NEW_MESSAGE_WPARAM,h,10);
			//MessageBox(NULL,h,"df",0);
			//			char hi[5];
			//itoa(NEW_MESSAGE,hi,10);
			//MessageBox(NULL,hi,"df2",0);
			return CallWindowProc((WNDPROC)OldWndHndl[val],hwnd,NEW_MESSAGE,NEW_MESSAGE_WPARAM,lParam);
		}
		else
		{
			return CallWindowProc((WNDPROC)OldWndHndl[val],hwnd,uMsg,wParam,lParam);
		}
	}
	else
	{
		return CallWindowProc((WNDPROC)OldWndHndl[val],hwnd,uMsg,wParam,lParam);
	}
	//long result;
	//if(uMsg==273) //Message Implying Menu Clicks
		//if(HIWORD(wParam)==0)
				//result=SendNotifyMessage(hApp,WM_APP +1024,(WPARAM)(LOWORD(wParam)),(LPARAM)uMsg);// Send the message  to the vb app

	
}//End Procedure


