<img decoding="async" src="../../static/images/leijiu.png" width="100%">


##### windows安装git
[官网下载](https://git-scm.com/download/)
[国内镜像下载](https://registry.npmmirror.com/binary.html?path=git-for-windows/)

下载.exe安装程序

因为Git是分布式版本控制系统，所以，每个机器都必须自报家门：你的名字和Email地址

``` shell
> git config --global user.name "Your Name"
> git config --global user.email "email@example.com"
```
注意git config命令的--global参数，用了这个参数，表示你这台机器上所有的Git仓库都会使用这个配置，当然也可以对某个仓库指定不同的用户名和Email地址。

``` shell
$ git init
# 创建新文件
$ git add .
$ git commit -m "init files"
```

##### git远程仓库 - github
``` shell
# 在用户主目录下看有没有.ssh, 里面有没有id_rsa和id_rsa.pub这两个文件, 没有的话需要创建ssh key
# C:\Users\laixx\.ssh

# 创建SSH KEY:
# win+R打开powershell
> ssh-keygen -t rsa -C "youremail@example.com"
# 一直enter就好
```


``` shell
$ git remote add origin git@github.com:laixxiong/my-tutorials.git
$ git push -u origin master
# 后面推送就不用加 -u了........
```

nihao

##### 参考
[廖雪峰的git教程](https://www.liaoxuefeng.com/wiki/896043488029600)