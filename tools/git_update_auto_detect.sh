
DIR_LIDBG_PATH=`cd ../ && pwd`
DIR_BUILD_PATH=$DIR_LIDBG_PATH/build
DIR_TOOLS_PATH=$DIR_LIDBG_PATH/tools

main_loop_times=0
pre_commit=('' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '')
commit_times=(0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 )

#$SYSTEM_DIR $SYSTEM_DIR_PASSWORD
function git_pull()
{
	cd $1
	expect $DIR_TOOLS_PATH/pull master $2 
}
mkdir -p /dev/shm/auto_git
rm /dev/shm/auto_git/*
while true ;do
#depository_request
echo "============loops:$main_loop_times  status:["${commit_times[0]}"] ["${commit_times[1]}"] ["${commit_times[2]}"] ["${commit_times[3]}"] ["${commit_times[4]}"]  ==============="
	thiswhile_loop=0
	while read line
	do
	compile_info=$line
	compile_info=($compile_info)
	#echo ==${compile_info[0]} ${compile_info[1]} ${compile_info[2]} ${compile_info[3]} ${compile_info[4]} ${compile_info[5]} ${compile_info[6]} ${compile_info[7]} ${compile_info[8]}
	
	cd ${compile_info[3]}
	git log --pretty=format:"%s  "  > /dev/shm/auto_git/${compile_info[1]}_$thiswhile_loop.txt && new_commitinfo=$(head /dev/shm/auto_git/${compile_info[1]}_$thiswhile_loop.txt --lines 1)
	
	thiscommit_info=${pre_commit[$thiswhile_loop]}
	thiscommit_times=${commit_times[$thiswhile_loop]}
	echo "==$thiswhile_loop==["${compile_info[1]}"]==== "
	
	if [[ $thiscommit_info == "" ]]; then
		echo "pre_commit[$thiswhile_loop] is NULL"
	else
		if [[ "$thiscommit_info" != "$new_commitinfo" ]]; then
		let thiscommit_times++
		echo $(date) $thiscommit_times/$main_loop_times ${compile_info[1]} ["$thiscommit_info"] ["$new_commitinfo"]  >> /dev/shm/auto_git/git_autodetec_log.txt
		echo "detect a new commit: "$thiscommit_times ["$thiscommit_info"] ["$new_commitinfo"]
		sleep 1

		awk '{print $0}' /dev/shm/auto_git/${compile_info[1]}_$thiswhile_loop-old.txt /dev/shm/auto_git/${compile_info[1]}_$thiswhile_loop.txt |sort|uniq -u > /dev/shm/auto_git/diff_commit.txt  
		diff_commit=$(cat /dev/shm/auto_git/diff_commit.txt)
		#echo @@@@@@@@@@@@@@ $diff_commit @@@@@@@@@@@@@@
		
		cd $DIR_TOOLS_PATH
		./basesystem.sh ${compile_info[0]} ${compile_info[1]} ${compile_info[2]} ${compile_info[3]} ${compile_info[4]} ${compile_info[5]} ${compile_info[6]} ${compile_info[7]} ${compile_info[8]} ${compile_info[9]} ${compile_info[10]} ${compile_info[11]} "${diff_commit}" 
		fi
	fi
	
	pre_commit[$thiswhile_loop]=$new_commitinfo
	commit_times[$thiswhile_loop]=$thiscommit_times
	cp /dev/shm/auto_git/${compile_info[1]}_$thiswhile_loop.txt /dev/shm/auto_git/${compile_info[1]}_$thiswhile_loop-old.txt

	git_pull ${compile_info[3]} ${compile_info[4]}
	let thiswhile_loop++
	done <$DIR_TOOLS_PATH/git_update_auto_detect.conf
let main_loop_times++
#depository_release
sleep 30
done
