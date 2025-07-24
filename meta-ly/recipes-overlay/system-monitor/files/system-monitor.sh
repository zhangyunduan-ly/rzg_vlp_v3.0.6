#!/bin/sh

CURDIR=$(cd $(dirname $0) && pwd )

sdALLPath=/dev
sdALLName=sd*

usbDir=/mnt/usb
tarUsbPath=$usbDir/ifp
insPath=/data/application/extapps
InsName=installer

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

# app目录
firmwarePath=/data/mainapp/firmware
upgradePath=/data/mainapp/firmware/upgrade
appName=mainapp
appActive=0
appRun=0

# CPU frequency
curCpuFreq=0
cpuMonitorInterval=0

# Bluetooth serial device
RtkHciDevice=ttySC3

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
		if [ ! -d "$insPath" ];then	
			echo "$insPath dir is non created"
			mkdir -p $insPath
		fi
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

# 监控U盘对应的功能
function usb_monitor()
{
	# 检测到有U盘设备
	sdnum=`find $sdALLPath -name $sdALLName | wc -l`
	# 检测到有U盘设备数量大于0
	if [ $sdnum -gt 0 ]  ;then
		# 判断本次插入U盘是否是第一次扫描
		if [ $MSdFalg == 0 ] ;then
			MSdFalg=1
			# 查找文件位置，得到所有存在该文件的路径信息数组，如aaa.jar
			tempLines=`find $sdALLPath -name $sdALLName`
			# 循环遍历数组
			for line in $tempLines
			do
				# 循环遍历数组
				cur_sd=$line
				echo "cur_sd: $cur_sd"
				mount_sda_plat

				# 检测到用户设备，停止挂载扫描
				if [ $RSdFalg == 1 ] ;then
					break
				fi
			done
		else
			# 当用户设备去除时进行标志清零操作
			if [ ! $(ls /dev | grep -w $MSdCname) ];then
				MSdFalg=0
				# 检测到u盘拔出，则清空标志，卸载目录，需要考虑还有其他sd设备挂载的情况
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

		# 检测到u盘拔出，则清空标志，卸载目录，所有sd设备均卸载时
		RSdFalg=0
		mounted=$(mount | grep $cur_sd | awk '{print $2}')
		if [ "$mounted" == "on" ];then
			echo "umount $usbDir"
			umount $usbDir
			rm -rf $usbDir
		fi
	fi
}

function get_mainapp_file()
{
	local dir_path="$1"  # 获取传入的第一个参数作为目录路径
	local file

	# 使用 for 循环和通配符列出以 mainapp 为前缀的文件
	for file in "$dir_path/mainapp"*; do
		if [ -f "$file" ]; then
			# 输出完整的文件路径
			echo "$file"
			return 0
		fi
	done

	# 如果没有找到文件，返回非零值
	return 1
}

#监控mainapp是否升级
function app_monitor()
{
	file_path=$(get_mainapp_file "$upgradePath")

	# 检查是否找到文件
	if [ -n "$file_path" ]; then
		echo "find mainapp file: $file_path"
		#卸载mainapp
		rpm -e mainapp
		rm -rf $firmwarePath/mainapp*

		#安装mainapp
		cp $file_path $firmwarePath
		rm -rf $file_path
		package=$(get_mainapp_file "$firmwarePath")
		rpm -Uvh --force $package
	else
		package=$(get_mainapp_file "$firmwarePath")
		# 检查服务是否安装
		if find /lib/systemd/system -type f -name "mainapp*" | grep -q .; then
			# 检查服务是否正在运行
			status=$(systemctl is-active mainapp)
			if [ "$status" = "active" ]; then
				#echo "mainapp service is running."
				appActive=0
				appRun=0
			else
				appActive=$((appActive + 1))
				if [ $appActive -gt 5 ]; then
					appActive=0
					appRun=$((appRun + 1))
					if [ $appRun -gt 3 ]; then
						rpm -e mainapp
						rpm -Uvh --force $package
					else
						echo "mainapp service is not running."
						# 使能mainapp service
						systemctl enable mainapp
						systemctl start mainapp
					fi
				fi
			fi
		else
			echo "mainapp service is unstalled"
			rpm -e mainapp
			rpm -Uvh --force $package
		fi
	fi
}

# configure cpu voltage
function set_pmic_cpu_volt()
{
	echo "set pmic cpu volt $1"

	# unlock
	i2cset -y 0 0x51 0x3E 0x00
	i2cset -y 0 0x51 0x3F 0xB0
	i2cset -y 0 0x51 0x3F 0xA9
	i2cset -y 0 0x51 0x3F 0x8A
	i2cset -y 0 0x51 0x3F 0xA7
	i2cset -y 0 0x51 0x3F 0xA8
	i2cset -y 0 0x51 0x3F 0xB1

	# set
	i2cset -y 0 0x51 0x0F $1

	# lock
	i2cset -y 0 0x51 0x3E 0x00
	i2cset -y 0 0x51 0x3F 0x00
}

# CPU frequency monitor
function cpu_freq_monitor()
{
	# check every 10 seconds
	((cpuMonitorInterval++))
	if [ $cpuMonitorInterval -lt 10 ]; then
		return
	fi
	cpuMonitorInterval=0

	# Adjust the CPU voltage according to the CPU frequency
	cpuFreq=$(cat /sys/devices/system/cpu/cpufreq/policy0/scaling_cur_freq)
	if [ $cpuFreq -ne $curCpuFreq ]; then
		curCpuFreq=$cpuFreq
		case $cpuFreq in
			1200000 | 600000)
				set_pmic_cpu_volt 0x6E
			;;
			300000 | 150000)
				set_pmic_cpu_volt 0x69
			;;
		esac
	fi
}

# Configure Realtek Bluetooth communication parameters
function rtk_hci_start()
{
	rtk_hciattach -n -s 115200 $RtkHciDevice rtk_h5 &
}

echo "system monitor will running !!!"

rtk_hci_start

while true
do
	usb_monitor
	# app_monitor
	cpu_freq_monitor
	sleep 1
done
