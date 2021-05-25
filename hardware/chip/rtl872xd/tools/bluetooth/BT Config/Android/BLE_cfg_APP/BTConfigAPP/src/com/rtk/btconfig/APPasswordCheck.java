package com.rtk.btconfig;

import android.util.Log;

public class APPasswordCheck {
	private static String TAG = "APPasswordCheck";
	
	public static boolean isAllAscii(byte[] password)
	{  
		for (int i =0; i< password.length; i++)
		{    
			if ((password[i] >=0) &&  (password[i]<=127))
			{  
				continue;
			}
			else
			{
				return false;
			}
			
		}  
		return true;
	}
	
	public static boolean isAllHex(byte[] password)
	{  
		for (int i =0; i< password.length; i++)
		{    
			if (	(password[i] >='0' && password[i]<='9')
					||
					(password[i] >='a' && password[i]<='f')
					||
					(password[i] >='A' && password[i]<='F')
				)
			{  
				continue;
			}
			else
			{
				return false;
			}
			
		}  
		return true;
	}
	
	public static int checkWifiPassWord(byte[] password, byte encryptMethod)
	{
		
		Log.d(TAG,""+password.length);
		if(encryptMethod == 2)
		{
			
			if((password.length==5) ||(password.length==13) )
			{
				 Log.d(TAG,"password.length==13/5");
				if(isAllAscii(password)==true)
				{
					/*5/13 ascii */
					Log.d(TAG,"RIGHT_PASSWORD_FORMAT");
					return 1;
				}
				else
				{
					Log.e(TAG,"WEP_WRONG_PASSWORD_FORMAT");
					return 0;
				}
			}
			else if((password.length==10) || (password.length==26))
			{
				 Log.e(TAG,"password.length==26/10");
				if(isAllHex(password)==true)
				{
					/*10/26 hex */
					 Log.e(TAG,"RIGHT_PASSWORD_FORMAT");
					return 1;
				}
				else
				{
					 Log.e(TAG,"WEP_WRONG_PASSWORD_FORMAT");
					return 0;
				}
		
			}
			else
			{
			
				return 0;
			}
		
		}
		else if(encryptMethod == 1)
		{
			if(password.length>=8)
			{
				return 1;
			}
			else
			{
				return 0;
			}
			
		}
		return 0;
	}
	
	
	
}
