
cd "$1"
rm -rf ./lidbg_log
mkdir lidbg_log
IFS=$'\n';
for i in * 
do

	echo --------------------------$i--------------------------------------------  >>  ./lidbg_log/reset_count.txt
	cat $i | grep "Linux version" >>  ./lidbg_log/reset_count.txt

	echo --------------------------$i--------------------------------------------  >>  ./lidbg_log/kill_process.txt
	cat $i | grep "killing any children in process group" >>  ./lidbg_log/kill_process.txt

	echo --------------------------$i--------------------------------------------  >>  ./lidbg_log/lowmemkill.txt
	cat $i | grep "send sigkill to" >> ./lidbg_log/lowmemkill.txt

	echo --------------------------$i--------------------------------------------  >>  ./lidbg_log/I2C.txt
	cat $i | grep "I2C slave addr" >> ./lidbg_log/i2c_err.txt



	echo --------------------------$i--------------------------------------------  >>  ./lidbg_log/wakelock.txt
	cat $i | grep -E "force_unlock" >> ./lidbg_log/force_unlock.txt

	echo ---------------------------$i---------------------------------------------  >> ./lidbg_log/acc_count.txt
	cat $i | grep "PM_AUTOSLEEP_STORE1" >>  ./lidbg_log/acc_count.txt



	cat $i | grep -E "send sigkill to|killing any children in process group|Linux version|force_unlock" >>  ./lidbg_log/$i
done

	date >>  ./lidbg_log/finish.txt

