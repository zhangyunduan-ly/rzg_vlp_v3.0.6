#!/bin/sh

CURDIR=$(cd $(dirname $0) && pwd )

sdALLPath=/dev
sdALLName=sd*[0-9]*

usbDir=/mnt/usb
tarUsbPath=$usbDir/ifp
insPath=/usr/local/extapps
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
upgradePath=/data/mainapp/firmware/upgrade
DCUAPPFILE=$upgradePath/dcuapp.tar.gz
INSTALLFILE=$upgradePath/install.sh

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
		fi

		if [ -d $usbDir ]; then
			rm -rf $usbDir
		fi
	fi
}

#监控是否需要升级
INSTALLFILE_EXIT_CODE=0
UPGRADELOG="/home/root/upgrade.log"
UPGRADETEMPLOG="/home/root/upgrade_temp.log"
function app_monitor()
{
	if [ -f $DCUAPPFILE ]; then
		echo "find dcuapp file: $DCUAPPFILE"
		sleep 5

		tar -zxvf "$DCUAPPFILE" -C "$upgradePath"

		if [ -f $INSTALLFILE ]; then
			chmod +x $INSTALLFILE
			/bin/bash $INSTALLFILE
			INSTALLFILE_EXIT_CODE=$?

			echo "upgrade result $INSTALLFILE_EXIT_CODE"
			if [ $INSTALLFILE_EXIT_CODE -eq 0 ]; then
				currenttime=`date "+%Y-%m-%d %H:%M:%S"`
				echo "$currenttime $DCUAPPFILE upgrade fail" >> $UPGRADELOG
				tail -n 10 $UPGRADELOG > $UPGRADETEMPLOG && mv $UPGRADETEMPLOG $UPGRADELOG
			fi

			rm -rf /data/mainapp/firmware/upgrade/*

			echo "System will reboot now..."
			sleep 2
			systemctl reboot
		fi
	fi
}

SYSTEMDIR="/home/root/"
DISKTESTFILE="/data/disk_testfile"
SYSTEMERRORFILE=$SYSTEMDIR"fileSystemError"
disk_count=0

function disk_monitor()
{
	disk_count=$((disk_count + 1))

	if [ ! $disk_count -ge 600 ]; then
		return
	fi

	disk_count=0

	if [ -f $SYSTEMERRORFILE ]; then
		return
	fi

	# 尝试创建测试文件
	if ! touch "$DISKTESTFILE" 2>/dev/null; then
		# 创建失败，说明文件系统异常，创建异常标记文件
		filename=$SYSTEMDIR"fs.log"
		filesize=0
		maxsize=$((1024*256))
		totalcount=0
		dataformat=0
		onemonthsecs=2592000
		currnettime=0

		if [ -e $filename ];then
			#read file size
			filesize=`ls -l $filename | awk '{print $5}'`
			#read file system error counts
			totalcount=`cat $filename | tail -1 | awk -F, '{print $1}' | awk -F= '{print $2}'`
			if [ $totalcount -ge 2 ];then
				recentdate=`cat $filename | tail -2 | head -1 | awk -F, '{print $2}' | awk -F= '{print $2}'`
				recenttime=`date -d "$recentdate" +%s`
				dataformat=`cat $filename | tail -2 | tail -1 | awk -F, '{print $3}' | awk -F= '{print $2}'`
				
				#read datetime
				nowtime=`date +%s`

				period=`expr $nowtime - $recenttime`
				if [ $period -le $onemonthsecs ]; then
					dataformat=`expr $dataformat + 1`
				else
					dataformat=0
				fi
			fi
		fi

		currenttime=`date "+%Y-%m-%d %H:%M:%S"`
		totalcount=`expr $totalcount + 1`
		if [ $filesize -gt $maxsize ];then
			echo "errcount=$totalcount,date=$currenttime,format=$dataformat" > $filename
		else
			echo "errcount=$totalcount,date=$currenttime,format=$dataformat" >> $filename
		fi

		#format data paration
		if [ $dataformat -gt 0 ];then
			systemctl stop boa
			systemctl stop mainapp #stop mainapp service
			docker stop $(docker ps -aq) #stop all docker container

			# 获取所有容器名称，并遍历处理
			docker ps -a --format "{{.Names}}" | while read -r container_name; do
				mount_point="/data/${container_name}_datafile"
				umount "$mount_point"
			done

			#umount and format data paration
			umount /dev/mmcblk0p3
			mkfs.ext4 -F /dev/mmcblk0p3

			#mount data paration
			mount -t ext4 -o rw,relatime /dev/mmcblk0p3 /data

			#copy data
			cp -r /backup/* /data/
			
			if [ $dataformat -eq 3 ];then
				touch $SYSTEMERRORFILE
			fi
		fi

		#reboot DCU
		echo "system reboot now..."
		reboot
		exit 0

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

# Set 8821cs log level
function rtl8821cs_loglevel_set()
{
	if [ -f /proc/net/rtl8821cs/log_level ]; then
		echo 0 > /proc/net/rtl8821cs/log_level
	fi
}

# Configure Realtek Bluetooth communication parameters
function rtk_hci_start()
{
	if modinfo hci_uart >/dev/null 2>&1; then
		rtk_hciattach -n -s 115200 $RtkHciDevice rtk_h5 &
	fi
}

echo "system monitor will running !!!"

rtl8821cs_loglevel_set
rtk_hci_start

while true
do
	usb_monitor
	app_monitor
	cpu_freq_monitor
	disk_monitor
	sleep 1
done
