#!/bin/sh

CURDIR=$(cd $(dirname $0) && pwd )

sdALLPath=/dev
sdALLName=sd*

usbDir=/mnt/usb
tarUsbPath=$usbDir/ifp
insPath=/data/app/extapps
InsName=installer_gui

# 用户U盘挂载标志
MSdFalg=0
RSdFalg=0
# 用户U盘挂载名称
MSdCname=sda1
cur_sd=sda1

# 用户U盘挂载标志
InsUsb=0


logUsbPath=$usbDir/log
logSHName=logcopy.sh
logUsb=0

# 上锁防止多sh运行
lockFile="/tmp/lockFile"
if [ -f ${lockFile} ]
then
	echo "someon do the same thing"
	exit
else
	touch ${lockFile}
fi


function mount_sda_plat()
{
	mounted=$(mount | grep $cur_sd | awk '{print $2}')	
	if [ "$mounted" != "on" ];then
		if [ ! -d "$usbDir" ];then
			echo "$cur_sd dir is non created"		
			mkdir -p $usbDir
		fi		
		echo "mount $cur_sd /mnt/usb"
		mount -o rw $cur_sd /mnt/usb
	fi
	
	# 日志拷贝
	if [ -f "$logUsbPath/$logSHName" ] && [ $logUsb == 0 ];then
		echo "find $logUsbPath/$logSHName"
		cp $logUsbPath/$logSHName  $insPath/
		chmod 777 $insPath/$logSHName
		$insPath/$logSHName			
		logUsb=1
	fi	
	
	# 安装包检测
	if [ -f "$tarUsbPath/$InsName" ] && [ $InsUsb == 0 ];then
		echo "find $tarUsbPath/$InsName "
		
		# 创建安装包路径		
		if [ ! -d "$insPath" ];then	
			echo "$insPath dir is non created"
			mkdir -p $insPath
		fi
		
		echo "cp -rf $tarUsbPath/*  $insPath/"				
		cp -rf $tarUsbPath/*  $insPath/
		chmod 777 $insPath/$InsName			
		$insPath/$InsName
		InsUsb=1
	fi				
	
	echo "InsUsb:$InsUsb"
	if [ $InsUsb == 0 ] || [ $logUsb == 0 ];then
		tmp=$cur_sd
		MSdCname=$(echo $tmp | awk -F "/" '{print $3}')
		echo "MSdCname : $MSdCname"
		RSdFalg=1;
		echo "user run !! will ensure $MSdCname "

	else
		mounted=$(mount | grep $cur_sd | awk '{print $2}')	
		if [ "$mounted" == "on" ];then
			echo "umount $usbDir"
			umount $usbDir
			rm -rf $usbDir
		fi
	fi



	logUsb=0
	InsUsb=0			
}

echo "auto_usb will running !!!"
#循环检测U盘对应的功能
while true
do
	#检测到有U盘设备
	sdnum=`find $sdALLPath -name $sdALLName | wc -l`
	#检测到有U盘设备数量大于0
	if [ $sdnum -gt 0 ]  ;then
		#判断本次插入U盘是否是第一次扫描
		if [ $MSdFalg == 0 ] ;then
			MSdFalg=1
			#查找文件位置，得到所有存在该文件的路径信息数组，如aaa.jar
			tempLines=`find $sdALLPath -name $sdALLName`
			#循环遍历数组
			for line in $tempLines
			do
				#循环遍历数组
				cur_sd=$line
				echo "cur_sd: $cur_sd"
				mount_sda_plat

				#检测到用户设备，停止挂载扫描
				if [ $RSdFalg == 1 ] ;then
					break
				fi

			done
		else
			#当用户设备去除时进行标志清零操作
			if [ ! $(ls /dev | grep -w $MSdCname) ];then
				MSdFalg=0
				#检测到u盘拔出，则清空标志，卸载目录，需要考虑还有其他sd设备挂载的情况
				RSdFalg=0
				mounted=$(mount | grep $cur_sd | awk '{print $2}')	
				echo "mounted: $mounted"
				if [ "$mounted" == "on" ];then
					echo "umount $usbDir"
					umount $usbDir
					rm -rf $usbDir
				fi				

			fi
		fi
	else
		MSdFalg=0

		#检测到u盘拔出，则清空标志，卸载目录，所有sd设备均卸载时
		RSdFalg=0
		mounted=$(mount | grep $cur_sd | awk '{print $2}')	
		if [ "$mounted" == "on" ];then
			echo "umount $usbDir"
			umount $usbDir
			rm -rf $usbDir
		fi				

	fi
	sleep 1
done 
