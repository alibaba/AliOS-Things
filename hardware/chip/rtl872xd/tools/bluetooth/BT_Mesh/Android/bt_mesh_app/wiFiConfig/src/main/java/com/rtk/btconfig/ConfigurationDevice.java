package com.rtk.btconfig;

import android.graphics.drawable.Drawable;

public class ConfigurationDevice {
	
	public static class DeviceInfo
	{
		private int aliveFlag;
		private String name;
		private String IP;
		private String macAdrress;
		private int securityType;
		private Drawable img;
		private boolean connectedflag;
		
		public int getaliveFlag()
		{
			return this.aliveFlag;
		} 
		public void setaliveFlag(int aliveFlag)
		{
			this.aliveFlag = aliveFlag;
		}
		public String getName()
		{
			return this.name;
		}
		public void setName(String name)
		{
			this.name= name;
		}
		public String getIP()
		{
			return this.IP;
		}
		public void setIP(String IP)
		{
			this.IP= IP;
		}
		public String getmacAdrress()
		{
			return this.macAdrress;
		}
		public void setmacAdrress(String macAdrress)
		{
			this.macAdrress= macAdrress;
		}
		
		public int getsecurityType()
		{
			return this.securityType;
		}
		public void setsecurityType(int securityType)
		{
			this.securityType= securityType;
		}
		
		public Drawable getimg()
		{
			return this.img;
		}
		public void setimg(Drawable img)
		{
			this.img= img;
		}
		public boolean getconnectedflag()
		{
			return this.connectedflag;
		}
		public void setconnectedflag(boolean connectedflag)
		{
			this.connectedflag= connectedflag;
		}
	}

}
