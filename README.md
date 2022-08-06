<!--  --># HelloWorld
Private repository,Create it to learn Git command   



此项目是本人学习驱动过程中编写的测试项目。  
学习参照的是B站Up主：写驱动的女装大佬   
链接：https://t.bilibili.com/299322983895240176?share_source=pc_native   
所有项目代码均经过测试验证。   
   
环境：本机：win11,vs2019，c++开发包和驱动开发包为最新，win10虚拟机.   
vs项目路径：./LearnCommand/LearnGitCommand.sln   

应用层和驱动层通过IRP来通信(可以理解为应用层的消息)   
1.硬件驱动      2.模块   
应用层          应用层   
////            ////    
驱动层          驱动层   
////            ////   
硬件设备        虚拟设备/内存/   
   
此项目不会关联任何硬件，所有的操作都是都自己虚拟的设备进行操作。  
