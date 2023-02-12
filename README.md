# WindowsDriver
window驱动开发记录

## 驱动安装

### 数字签名

[1] 打开 Developer Command Prompt for VS 2022
 [1.1] 把当前目录切换到驱动程序所在的目录，如：
 ```
 cd G:\zuoye\2022\Project\2023\WindowsDriver\SpecialStartIOTest
 g:
 ```
 
 [2] 制作cer证书，输入命令：
```
makecert -r -pe -ss root -n CN="My Company CA" -$ commercial -b 01/01/2020 -e 01/01/2100 MyCert.cer
```
注：可以根据自己的需求修改上述命令，`makecert`命令说明在 https://docs.microsoft.com/en-us/windows/win32/seccrypto/makecert
命令输入后，弹出安全警告，点击"是"即可，此时产生了MyCert.cer根证书

[3]导出PFX格式证书
在Windows搜索栏输入 MMC 并打开，在"文件"->“添加管理单元"中添加"证书"至我的账户，并确认：在"受信任的根证书颁发机构”->“证书"中找到刚才安装的证书"My Company CA”，右击"所有任务"->“导出"进入证书导出向导：
按照如上步骤，设置好密码，最后通过"浏览"设置文件导出路径即可完成，我这里把文件名设置为"Mycert.pfx”，至此我们产生了2个证书文件：

[4]给INF文件签名
生成Cat目录文件
将INF文件"MyDirver.inf"与证书文件放至同一目录：
```
inf2cat /v /os:XP_X86,Vista_X86,Vista_X64,7_X86,7_X64,8_X86,8_X64,6_3_X86,6_3_X64,10_X86,10_X64 /driver:.
```
签名Cat文件
同样在命令行工具下执行：
```
Signtool sign /v /fd sha256 /f MyCert.pfx /p cs123456 /n "My Company" /t http://timestamp.digicert.com/rfc3161 *.cat
```

signtool 命令使用说明如下：
https://docs.microsoft.com/zh-cn/dotnet/framework/tools/signtool-exe

其中"/p 123456"是导出pfx时设置的密码
命令执行成功后，在cat文件的属性中能够看到已经被签名：
