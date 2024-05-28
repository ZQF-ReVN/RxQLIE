# QLIE_Extract
QLIE Extract Files via Hook TStream  
事情真多，五六天前结构就解析好了，一直没空写 :) 能用在官中上纯属巧合哈，勿念！  
由于QLIE免封包和封包都有现成的方法，暂时没计划改成FileHook，此项目仅研究学习Delphi逆向！  

# 用法
很简单啦，不过只适配了 ハッピーライヴ ショウアップ ，其它游戏也可以找地址编译也能用。  
把version.dll放到游戏目录下，然后运行游戏，就会弹出个控制台，显示Waiting For Command...  
输入p 是打印封包信息  
输入r 是打印具体某个封包的所有文件名（需要一个参数从p封包信息里获取）  
输入f 是把所有封包信息和文件名都保存到 QLIEFileNameList.txt 文件里（在游戏目录下鸭）   
输入d 是解包，同样需要一个参数从p获取，输入后会解包封包内所有文件到游戏目录下。  

# Usage
For ハッピーライヴ ショウアップ！ Happy Live, Show Up!  
Other games require you to modify the address to compile  
Put version.dll into game directory and run the game  

Type in the console  
p show pack information  
r show filelist -> the Sequence see pack information  
f out all pack information and filelist to QLIEFileNameList.txt  
d extract files -> the Sequence see pack information  

# Test
![0](TestImage/test0.png?raw=true)
![1](TestImage/test1.png?raw=true)
