## Overview

## compile
Cmds For keil project generating should be executed in Windows Git Bash.
`	aos make linkkitapp@xmc4800-relax -c config IDE=keil
	aos make IDE=keil `

These two cmds automatically generate the keil project in **projects\Keil\linkkitapp@xmc4800-relax\keil_project** dir.

## use keil project
1、choose **j-link** debug 
![](https://i.imgur.com/fECFt1r.png)

2、select j-link **Settings**, click **Flash Download** tab, and import **Programming Algorithm**
![](https://i.imgur.com/vRyAE3G.png)
