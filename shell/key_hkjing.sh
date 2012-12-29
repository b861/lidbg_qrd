echo
echo            你现在运行echo脚本,获得帮助运行    help
echo
while :;do
echo "请输入参数"
read Parameter0 Parameter1 Parameter2 Parameter3
echo "参数1=$Parameter0 2=$Parameter1 3=$Parameter2 "
if [ $$Parameter0 == ];then
hkjing=0
else 
hkjing=1
fi

if [ $hkjing -eq 1 ];then #大if判断有没参数输入

if [ $Parameter0 == help ];then
	echo "**************************************************"
	echo "*按键操作："
	echo "*	home键：  home or h"
	echo "*	back键：  back or b"
	echo "*	enter键： enter or e"
	echo "*	end键：   end"
	echo "*	menu_end键： menu_end"
	echo "**************************************************"
	echo "**************************************************"
	echo "*读取TC358746xbg寄存器："
	echo "*	配置 NTSCi    NTSCi or ni"
	echo "*	配置 NTSCp    NTSCp or np"
	echo "*	配置 PALi     PALi  or pi"
	echo "*	配置 NTSCp    PALp or  pp"
	echo "*	配置 TVP5150  5150"
	echo "**************************************************"
	echo "**************************************************"
	echo "*初始化TC358746xbg："
	echo "*	读取              358w 寄存器 内容 位宽"
	echo "*	写入              358r 寄存器 位宽"
	echo "*	读取全部寄存器     358r all	 "
	echo "**************************************************"
	echo "**************************************************"
	echo "*启动 Music 和 Video"
	echo "*	music"
	echo "*	video"      
	echo "**************************************************"
	echo "**************************************************"
	echo "*启动 Camera"
	echo "* camera"
	echo "**************************************************"
	echo "**************************************************"
	echo "*启动 Camera 测试脚本"
	echo "* Testc xx 其中 xx 可以是pp（PAL 隔行输入逐行输出） np ppp（PAL 逐行输入输出） npp "
	echo "* 如： Testc np"
	echo "**************************************************"
	echo "**************************************************"
	echo "*启动 Browser"
	echo "* browser"
	echo "**************************************************"
	echo "**************************************************"
	echo "*拨打电话 xxx"
	echo "* call xxx（电话号码如：10086）"
	echo "**************************************************"
	echo "**************************************************"
	echo "*打开地图定位到xxx"
	echo "* map xxx（定位地址如：shanghai）"
	echo "**************************************************"
	
fi

if [ $Parameter0 == music ];then 
	am start -n com.android.music/com.android.music.MusicBrowserActivity
	echo 启动 music 
elif [ $Parameter0 == video ];then
	am start -n com.android.music/com.android.music.VideoBrowserActivity
	echo 启动 video
elif [ $Parameter0 == camera -o $Parameter0 == c ];then
	am start -n com.android.camera/com.android.camera.Camera
	echo 启动 camera
	if [ $Parameter1 == pp ];then 
	echo 配置PALp，请按任意键继续
	read
	echo "c vedio ResetPALp" > /dev/mlidbg0	
	fi 
	if [ $Parameter1 == np ];then 
	echo 配置NTSCp，请按任意键继续
	read
	echo "c video ResetNTSCp" > /dev/mlidbg0	
	fi 
elif [ $Parameter0 == Testcpp ];then
while :;do		
	echo "c video ResetPALp"	
	am start -n com.android.camera/com.android.camera.Camera
	sleep 1
	echo "c video ResetPALp" > /dev/mlidbg0
	echo "sleep 20"
	sleep 4
	echo "c key back 2" > /dev/mlidbg0
	echo 	成功操作啦 back 键
	sleep 2
	echo again
done
elif [ $Parameter0 == huang ];then	
while :;do		
	am start -n com.android.camera/com.android.camera.Camera
	sleep 10
	echo "c key back 2" > /dev/mlidbg0
	echo 	成功操作啦 back 键
	sleep 5
	echo again
done
elif [ $Parameter0 == Testcnp ];then
while :;do		
	echo "c vedio ResetNTSCp"	
	am start -n com.android.camera/com.android.camera.Camera
	sleep 1
	echo "c vedio ResetNTSCp" > /dev/mlidbg0
	echo "sleep 20"
	sleep 4
	echo "c key back 2" > /dev/mlidbg0
	echo 	成功操作啦 back 键
	sleep 2
	echo again
done
elif [ $Parameter0 == Testcnpp ];then
while :;do		
	echo "c vedio ResetNTSCpp"	
	am start -n com.android.camera/com.android.camera.Camera
	sleep 1
	echo "c vedio ResetNTSCpp" > /dev/mlidbg0
	echo "sleep 20"
	sleep 4
	echo "c key back 2" > /dev/mlidbg0
	echo 	成功操作啦 back 键
	sleep 2
	echo again
done
elif [ $Parameter0 == Testcppp ];then
while :;do		
	echo "c vedio ResetPALpp"	
	am start -n com.android.camera/com.android.camera.Camera
	sleep 1
	echo "c vedio ResetPALpp" > /dev/mlidbg0
	echo "sleep 20"
	sleep 4
	echo "c key back 2" > /dev/mlidbg0
	echo 	成功操作啦 back 键
	sleep 2
	echo 0k
done
elif [ $Parameter0 == Tc358np ];then
while :;do		
	echo "c vedio ResetPALpp"	
	am start -n com.android.camera/com.android.camera.Camera
	sleep 1
	echo "c TC358 np" > /dev/mlidbg0
	echo "sleep 20"
	sleep 4
	echo "c key back 2" > /dev/mlidbg0
	echo 	成功操作啦 back 键
	sleep 2
	echo 0k
done
elif [ $Parameter0 == Tc358pp ];then
while :;do		
	echo "c vedio ResetPALpp"	
	am start -n com.android.camera/com.android.camera.Camera
	sleep 1
	echo "c TC358 pp" > /dev/mlidbg0
	echo "sleep 20"
	sleep 4
	echo "c key back 2" > /dev/mlidbg0
	echo 	成功操作啦 back 键
	sleep 2
	echo 0k
done
elif [ $Parameter0 == wcam ];then
while :;do		
	echo "c vedio ResetPALpp"	
	am start -n com.android.camera/com.android.camera.Camera
	echo "sleep 10"
	sleep 4
	echo "c key back 2" > /dev/mlidbg0
	echo 	成功操作啦 back 键
	sleep 2
	echo 0k
done
elif [ $Parameter0 == zzz ];then
#while :;do	
	echo i2c_io_config	

	echo "c video TW9912 write 0x2 0x73" > /dev/mlidbg0
	echo "c video TW9912 Read 0x2" > /dev/mlidbg0

	echo "c video TW9912 write 0x3 0x20" > /dev/mlidbg0
	echo "c video TW9912 Read 0x3" > /dev/mlidbg0

	echo "c video TW9912 write 0x5 0x20" > /dev/mlidbg0
	echo "c video TW9912 Read 0x5" > /dev/mlidbg0

	echo "c video TW9912 write 0x6 0x00" > /dev/mlidbg0
	echo "c video TW9912 Read 0x6" > /dev/mlidbg0

	echo "c video TW9912 write 0x7 0x21" > /dev/mlidbg0
	echo "c video TW9912 Read 0x7" > /dev/mlidbg0

	echo "c video TW9912 write 0x8 0x18" > /dev/mlidbg0
	echo "c video TW9912 Read 0x8" > /dev/mlidbg0

	echo "c video TW9912 write 0x9 0x40" > /dev/mlidbg0
	echo "c video TW9912 Read 0x9" > /dev/mlidbg0

	echo "c video TW9912 write 0xa 0x00" > /dev/mlidbg0
	echo "c video TW9912 Read 0xa" > /dev/mlidbg0

	echo "c video TW9912 write 0xb 0x70" > /dev/mlidbg0
	echo "c video TW9912 Read 0xb" > /dev/mlidbg0
	sleep 1
#done
elif [ $Parameter0 == browser ];then
	am start -n com.android.browser/com.android.browser.BrowserActivity
	echo 启动 browser
elif [ $Parameter0 == call ];then
	am start -a android.intent.action.CALL -d tel:$Parameter1
	echo 呼叫 $Parameter1
elif [ $Parameter0 == map ];then
	am start -a android.intent.action.VIEW geo:0,0?q=$Parameter1
	echo 地图定位到：$Parameter1
fi

if [ $Parameter0 == ins ];then 
insmod /system/lib/modules/out/lidbg_touch.ko    
insmod /system/lib/modules/out/lidbg_key.ko   
insmod /system/lib/modules/out/lidbg_i2c.ko    
insmod /system/lib/modules/out/soc_tcc8803.ko    
insmod /system/lib/modules/out/lidbg_io.ko    
insmod /system/lib/modules/out/lidbg_ad.ko    
insmod /system/lib/modules/out/mlidbg_cmn.ko     
insmod /system/lib/modules/out/fly_soc.ko
insmod /system/lib/modules/out/soc_devices.ko   
insmod /system/lib/modules/out/mlidbg.ko  
fi


if [ $Parameter0 == home -o $Parameter0 == h ];then #按键操作
echo "c key home 2" > /dev/mlidbg0
echo 	成功操作啦 home 键
elif [ $Parameter0 == back -o $Parameter0 == b ];then
adecho "c key back 2" > /dev/mlidbg0
echo 	成功操作啦 back 键
elif [ $Parameter0 == enter -o $Parameter0 == e ];then
echo "c key enter 2" > /dev/mlidbg0
echo 	成功操作啦 enter 键
elif [ $Parameter0 == end ];then
echo "c key end 2" > /dev/mlidbg0
echo 	成功操作啦 end 键
elif [ $Parameter0 == menu_end ];then
echo "c menu end 2" > /dev/mlidbg0
echo 	成功操作啦 menu end 键
fi


if [ $Parameter0 == NTSCi -o $Parameter0 == ni ];then #初始化配置TW9912 和 TC358746的配置
	echo "c TC358 ResetNTSCi" > /dev/mlidbg0
	echo 	配置了 NTSCi
elif [ $Parameter0 == NTSCp -o $Parameter0 == np ];then
	echo "c TC358 ResetNTSCp" > /dev/mlidbg0
	echo 	配置了 NTSCP
elif [ $Parameter0 == PALi -o $Parameter0 == pi ];then
	echo "c TC358 ResetPALi" > /dev/mlidbg0
	echo 	配置了 PALi
elif [ $Parameter0 == PALp -o $Parameter0 == pp ];then
	echo "c TC358 ResetPALp" > /dev/mlidbg0
	echo 	配置了 PALp
elif [ $Parameter0 == 5150 ];then
	echo "c TVP5150 Reset" > /dev/mlidbg0
	echo 	配置了 5150
fi


if [ $Parameter0 == 358w ];then #读取TC358746xbg的寄存器操作
	echo "c TC358 write $Parameter1 $Parameter2 $Parameter3" > /dev/mlidbg0
	echo 对TC358746xbg寄存器 $Parameter1 写入了 $Parameter2 ，该寄存器位宽是 $Parameter3
elif [ $Parameter0 == 358r ];then

	if [ $Parameter1 == all ];then
		echo "c TC358 read_all_register" > /dev/mlidbg0
		echo 对TC358746xbg所有寄存器进行啦读取
	else
		echo "c TC358 read $Parameter1 $Parameter2" > /dev/mlidbg0
		echo 对TC358746xbg寄存器 $Parameter1 进行了读取，该寄存器位宽是 $Parameter2
	fi
fi



fi #大if判断有没参数输入
done

