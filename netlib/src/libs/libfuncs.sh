##
# library of functions 

if [ "$_COLOR_" != "false" ]; then
txtrst=$(tput sgr0) # Text reset
txtred=$(tput setaf 1) # Red
txtgrn=$(tput setaf 2) # Green
txtylw=$(tput setaf 3) # Yellow
txtblu=$(tput setaf 4) # Blue
txtpur=$(tput setaf 5) # Purple
txtcyn=$(tput setaf 6) # Cyan
txtwht=$(tput setaf 7) # White
bgrred=$(tput setab 1) # Red
bgrgrn=$(tput setab 2) # Green
bgrylw=$(tput setab 3) # Yellow
bgrblu=$(tput setab 4) # Blue
bgrpur=$(tput setab 5) # Purple
bgrcyn=$(tput setab 6) # Cyan
bgrwht=$(tput setab 7) # White

bold=$(tput bold)
smul=$(tput smul)
rmul=$(tput rmul)
else
txtrst= # Text reset
txtred= # Red
txtgrn= # Green
txtylw= # Yellow
txtblu= # Blue
txtpur= # Purple
txtcyn= # Cyan
txtwht= # White
bgrred= # Red
bgrgrn= # Green
bgrylw= # Yellow
bgrblu= # Blue
bgrpur= # Purple
bgrcyn= # Cyan
bgrwht= # White

bold=
smul=
rmul=
fi

#Binary class file to check a string of date
#TESTDATE=/root/svn-local/tools/src/testDate
_NAME=$BASH_SOURCE
_FILEDIR=`dirname $(which  $_NAME)`
JAVA_SRC=$_FILEDIR/src
TESTDATE=$_FILEDIR/src/testDate
TMP_FILES=""
_LANG=0 # english

#trap clean 2
source $_FILEDIR/lang.common

register_tmp_file () {
	_f=$1
	TMP_FILES+=" $_f"
}

clean () {
	print_left "    ${CLEANING[_LANG]}"
	#echo removing files $TMP_FILES
	rm -rf $TMP_FILES 2>/dev/null >/dev/null
	print_right "${DONE[_LANG]}"
	unset TMP_FILES
}

quit () {
	clean
	exit
	#return
}

make_iso_file () {
	local directory=$1
	local output=$2
	mkisofs -o $output -J $directory
}

is_date_string () {
	local _format=$1
	local _value=$2
	local _classpath=`dirname ${TESTDATE}`
	ret=`java -classpath ${_classpath} testDate ${_value} ${_format} `
	echo $ret
}

is_number () {
	local _str=$1
	local _value=`echo $_str | grep "^[0-9]*$"`
  #$echo value = $_value
	if [ "X$_value" == "X" ]; then
		echo "false"
	else
		echo "true"
	fi
}
#is_number "9"

sync_file () {
	local _f1=$1
	local _f2=$2
	local _sfkey=$3
	local _tf=/tmp/`basename $_f1`.`date +%s`
	rm -f $_tf
	cat $_f1 |grep "$_sfkey" > $_tf
	while read _l; do
		insert_entry_2_setfile "$_f2" "$_l" "$_l"
	done < $_tf

	rm -f $_tf
	cat $_f2 |grep "$_sfkey" > $_tf
	while read _l; do
		insert_entry_2_setfile "$_f1" "$_l" "$_l"
	done < $_tf
	rm -f $_tf
}

get_file_size () {
	local _file=$1
	local _testf=`file $_file |grep ERROR`
	if [ -e "$_file" ]; then
		_value=`ls -l $_file 2>/dev/null | awk  '{print $5}' 2>/dev/null`
		echo $_value
	fi
}

is_file_existing () {
	local _file=$1
	if [ -e "$_file" ]; then
		echo true
	else
		echo false
	fi
}

get_abs_path () {
	local _p=$1
	local _curdir=`pwd`
	if [ -e "$_p" ]; then
		local _basen=`basename $_p`
		local _dirn=`dirname $_p`
		cd $_dirn 
    if [ $? -ne 0 ]; then
      return
    fi
    if [ "$_basen" == "." ]; then
      echo `pwd`
    else
		  echo `pwd`/$_basen
    fi
    cd $_curdir
	fi
}

list_app_by_user () {
	local _user=$1
	ps -U $_user -o comm= 2>/dev/null | tr '\n' " " 2>/dev/null
}

run_remote_cmd () {
	local _host=$1
	local _script=""
	local _all=($@)
	for ((_i=1;_i<$#;_i++)); do
		_script+="${_all[_i]} "
	done
	ssh $_host "${_script}"
}

check_app () {
	local _a=$1
	which $_a 2>/dev/null > /dev/null 
	if [ $? -ne 0 ]; then
		echo false
	else 
		echo true
	fi
}

check_service () {
	local _s=$1
	if [ ! -f /etc/init.d/$_s ]; then
		echo false
	else 
		echo true
	fi
}

print_right () {
	local _s=$1
	printf "%-20s\n" "${_s}"
}

print_left () {
        local _s=$1
        printf "%-59s" "${_s}"
}

print_info () {
	local str=$1
	echo ${txtylw}INFO: "${str}"${txtrst}
}

print_error () {
	local str=$1
	echo ${txtred}ERROR: "${str}"${txtrst}
}

print_warn () {
        local str=$1
        echo ${txtblu}WARNING: "${str}"${txtrst}
}

reset_mysql_password () {
	local _pw=$1
	if [ "X$_pw" != "X" ]; then
		echo "UPDATE mysql.user SET Password=PASSWORD('$_pw') WHERE User='root';" > ./resetMysqlPassword.sql
		echo "FLUSH PRIVILEGES;" >> ./resetMysqlPassword.sql
		/etc/init.d/mysqld stop
		mysqld_safe --skip-grant-tables &
		sleep 1
		sync
		mysql -u root  < ./resetMysqlPassword.sql
		/etc/init.d/mysqld stop
		_mid=`ps -A | grep mysqld | awk '{print $1}'`
		for _i in $_mid; do
			echo kill $_i
			kill -9 $_i
		done
		service mysqld start
		rm -f ./resetMysqlPassword.sql
	fi
}

first_bash_source () {
	local _srcbashID=${#BASH_SOURCE[@]}
	basename ${BASH_SOURCE[_srcbashID-1]}
}

daily_log () {
	local _fn=/tmp/`first_bash_source`.`date +%Y%m%d`
	local _str="$@"
	echo -e "------------------------------------------------------\n$(date)\n${_str}" >> $_fn
}

send_daily_log () {
	local _emails=$1
	local _content=$2
	local _pn=`first_bash_source`
	local _yesterday=`date +%Y%m%d -d "1 day ago"`
	_fn=/tmp/$_pn.$_yesterday
	if [ -e "$_fn" ]; then
		_subject="[$_pn] `date`: Daily log file from `hostname`"
		#echo $_subject
		echo $_content | mutt -s "${_subject}" -a $_fn $_emails 2>/dev/null
		rm -fr $_fn
	fi
}

cp_install_pkg_remotely () {
	local _pkgpath=$1
	local _fn=`basename $_pkgpath`
	local _host=$2
	local _remotedir=$3
	local _cmd=$4
	echo $_cmd
	scp -r $_pkgpath $_host:$_remotedir
	ssh $_host "cd $_remotedir/$_fn; chmod +x $_cmd; $_cmd" 
}

check_urlstyle_name () {
	local _str=$1
	local _str=($(echo $_str | sed -e 's/\./ /g'))
	if [ ${#_str[*]} -gt 2 ]; then
		echo true
	else
		echo false
	fi
}

get_ip () {
	ifconfig eth0 2>/dev/null | grep 'inet addr:'| cut -d: -f2 |awk '{print $1}'
}

get_remote_ip () {
# First try
	local _host=$1
	local _ip=(`host $_host`)
	if [ "X${#_ip[@]}" == "X4" ]; then
		echo ${_ip[3]}
	else
		echo
	fi
}

insert_entry_2_setfile () {
	_f=$1	#file
	_key=$2	# key id
	_str=$3	# string store in file
	_check=$(cat $_f |grep "$_key")
	if [ "X$_check" == "X" ]; then
		echo -e "$_str" >> $_f
	fi
}

change_hostname () {
	_hostname=$1

	#change /etc/sysconfig/network
	_tmp_file=/tmp/network.`date +%s`
	echo > $_tmp_file
	while read _line; do
		_fline=($(echo $_line | sed -e 's/=/ /'))
		_var=${_fline[0]}
		if [ "X$_var" != "XHOSTNAME" ]; then
			echo $_line >> $_tmp_file
		else
			echo HOSTNAME=${_hostname} >> $_tmp_file
		fi
	done < /etc/sysconfig/network
	mv /etc/sysconfig/network /etc/sysconfig/network.backup
	cp $_tmp_file /etc/sysconfig/network
	rm -f $_tmp_file

	#change /etc/host
	_ipaddr=`get_ip`
	if [ "X$_ipaddr" != "X" ]; then
		_tmp_file=/tmp/hosts.`date +%s`
		echo > $_tmp_file
		FLAG_HOST_CHANGED=false
		while read _ip _remain; do
			if [ "X$_ip" != "X$_ipaddr" ]; then
				echo $_ip $_remain >> $_tmp_file
			else
				echo -e "$_ip \t ${_hostname} \t ${_hostname}" >> $_tmp_file
				FLAG_HOST_CHANGED=true
			fi
		done < /etc/hosts
		if [ "$FLAG_HOST_CHANGED" == "false" ]; then
			echo -e "$_ipaddr \t ${_hostname} \t ${_hostname}" >> $_tmp_file
		fi

		mv /etc/hosts /etc/hosts.backup
		cp $_tmp_file /etc/hosts
		rm -rf $_tmp_file

		#change /etc/sysconfig/networking/profiles/default/hosts
		_tmp_file=/tmp/hosts.`date +%s`
		echo > $_tmp_file
		FLAG_HOST_CHANGED=false
		while read _ip _remain; do
			if [ "X$_ip" != "X$_ipaddr" ]; then
				echo $_ip $_remain >> $_tmp_file
			else
				echo -e "$_ip \t ${_hostname} \t ${_hostname}" >> $_tmp_file
				FLAG_HOST_CHANGED=true
			fi
		done < /etc/sysconfig/networking/profiles/default/hosts
		if [ "$FLAG_HOST_CHANGED" == "false" ]; then
			echo -e "$_ipaddr \t ${_hostname} \t ${_hostname}" >> $_tmp_file
		fi
		mv /etc/sysconfig/networking/profiles/default/hosts /etc/sysconfig/networking/profiles/default/hosts.backup
		cp $_tmp_file /etc/sysconfig/networking/profiles/default/hosts
		rm -rf $_tmp_file
	fi

	#change /etc/sysconfig/networking/profiles/default/network
	_tmp_file=/tmp/network.`date +%s`
	echo > $_tmp_file
	while read _line; do
		_fline=($(echo $_line | sed -e 's/=/ /'))
		_var=${_fline[0]}
		if [ "X$_var" != "XHOSTNAME" ]; then
			echo $_line >> $_tmp_file
		else
			echo HOSTNAME=${_hostname} >> $_tmp_file
	        fi
	done < /etc/sysconfig/networking/profiles/default/network
	mv /etc/sysconfig/networking/profiles/default/network /etc/sysconfig/networking/profiles/default/network.backup
	cp $_tmp_file /etc/sysconfig/networking/profiles/default/network
	rm -f $_tmp_file

	echo $_hostname > /proc/sys/kernel/hostname
}

create_ssh_keygen () {
	_h=$1
	_u=$2
	if [ "X$_u" == "X" ]; then
		_u=root
	fi
	_myhost=`hostname`
	ssh-keygen -t rsa
	scp ~/.ssh/id_rsa.pub $_u@$_h:/tmp/
	ssh $_u@$_h "cat /tmp/id_rsa.pub >> ~$_u/.ssh/authorized_keys; ssh-keygen -t rsa"
	scp $_u@$_h:.ssh/id_rsa.pub /tmp/
	cat /tmp/id_rsa.pub >> ~/.ssh/authorized_keys
}

logo_show () {
	_logofile=$1
	_time=$2
	_i=0
	if [ "X$_time" == "X" ]; then
		_time=3	# default is 3 second
	fi

	tput sc
	logo_continue
	while [ 1 ]; do
		_val=`cat /tmp/logo.lock 2>/dev/null`
		if [ "X$_val" == "X0" ]; then
			tput cup 0 0
			tput setaf $_i
			cat ./small-vngrid-logo.txt
			tput sgr0
		elif [ "X$_val" == "X-1" ]; then
			tput rc
			return
		fi
		#echo -e -n "${_cursors[_i]}";
		tput rc
		_i=`echo $_i |awk '{print ($1+1)%7}'`
		sleep $_time
		tput sc
		
	done
	
}

logo_postpone () {
	echo 1 > /tmp/logo.lock
}

logo_stop () {
	echo -1 > /tmp/logo.lock
}

logo_continue () {
	echo 0 > /tmp/logo.lock
	
}

# Dialog support
DIALOG_TITLE_ID=0
DIALOG_INFO_ID=1
DIALOG_TYPE_ID=2
DIALOG_OK_ID=4
DIALOG_CANCEL_ID=5
DIALOG_VALUE_ID=3
DIALOG_ENTRIES_ID=6

DIALOG_OK=0
DIALOG_CANCEL=1
DIALOG_HELP=2
DIALOG_EXTRA=3
DIALOG_ITEM_HELP=4
DIALOG_ESC=255

dialog_get_title () {
        local _v=$1
        eval echo \${$_v[DIALOG_TITLE_ID]}
}

dialog_get_type () {
        local _v=$1
        eval echo \${$_v[DIALOG_TYPE_ID]}
}

dialog_get_info () {
        local _v=$1
        eval echo \${$_v[DIALOG_INFO_ID]}
}

dialog_get_last_value () {
        local _v=$1
        eval echo \${$_v[DIALOG_VALUE_ID]}
}

dialog_get_all_entries () {
        local _v=$1
        eval echo \${$_v[@]:DIALOG_ENTRIES_ID}

}

dialog_get_ok_callback () {
	local _v=$1
	eval echo \${$_v[DIALOG_OK_ID]}
}

dialog_get_cancel_callback () {
	local _v=$1
	eval echo \${$_v[DIALOG_CANCEL_ID]}
}

dialog_show () {
	local d=$1	# Dialog variable
	local bt=$2	# background title
	#local o=$3	# Ouput file
	local allopt=("$@")
	local opt=${allopt[*]:2}
	DIALOG=$(which dialog)
	tempfile=`tempfile 2>/dev/null` || tempfile=/tmp/test$$
	#trap "rm -f $tempfile" 0 1 2 5 15

	local t=`dialog_get_title $d`
	local tp=`dialog_get_type $d`
	local entries=`dialog_get_all_entries $d`
	local info=`dialog_get_info $d`
	local dh=0 #dialog height
	local width=0
	local height=0

	case "$tp" in
		"radiolist"|"mixedform"|"form")	dh=0 ;;
		"checklist"|"menu"|"inputmenu") dh=0 ;;
		"passwordform") dh=0 ;;
		"fselect"|"dselect") 
			dh=""
			width=35
			height=0
			;;
		*) dh="";;
	esac
	
	local cmd="$DIALOG	--ascii-lines 
				--backtitle \"${bt}\" --item-help --clear
				--aspect 50 --title \"${t}\" $opt --insecure 
				--${tp} \"${info}\" ${height} ${width} ${dh} 
				${entries} 2>${tempfile}"
	#echo $cmd
	local pressedkey=5
	eval $cmd
	local pressedkey=$?
	#killall dialog
	case $pressedkey in
		$DIALOG_OK) #Next 
			local callback=`dialog_get_ok_callback $d`
			$callback $tempfile
			#quit
			
			;;
		$DIALOG_CANCEL)
			local callback=`dialog_get_cancel_callback $d`
			$callback
			#quit
			
			;;
		$DIALOG_HELP) 
			echo "Help pressed."
			
			;;
		$DIALOG_EXTRA) 
			echo "Extra button pressed."
			
			;;
		$DIALOG_ITEM_HELP) 
			echo "Item-help button pressed."
			
			;;
		$DIALOG_ESC) 
			echo "ESC pressed."
			#NEXT_DIALOG=MAIN_MENU
			;;
	esac
	rm -f $tempfile
	
}


## Manipulate Parsing Options
OPT_VALS=()
option_config_add () {
  local opt=$1
  local val=$2
  local has_arg=$3
  local meaning=$4

  if [ "$opt" == "" ]; then
    print_error "Option string is not provided. Stop." 
    return;
  fi
  if [ "$val" == "" ]; then
    print_error "Variable string name for option ${opt} is not provided. Stop."
    return;
  fi
  if [ "$has_arg" == "" ]; then
    print_warn "Variable string name is not provided. Stop."
    has_arg=0
  fi  

  OPT_VALS=( "${OPT_VALS[@]}" "$opt" "$val" "$has_arg" "$meaning")
}

option_parse () {
  local allopt=("$@")
  local i=0
  local optlen=${#allopt[@]}
  local optvalslen=${#OPT_VALS[@]}
  local now_nilopt_pos=0

  for ((i=0;i<$optlen;i++)); do
    flag="${allopt[i]}"
    local nilopt_pos=0
    local nilopt_var=""
    local nilopt_val=""

    for ((j=0;j<$optvalslen;j+=4)); do
      local opt="${OPT_VALS[j]}"
      local val="${OPT_VALS[j+1]}"
      local has_arg="${OPT_VALS[j+2]}"
      local meaning="${OPT_VALS[j+3]}"

      if [ "$opt" == "_" ]; then
        if [ "${nilopt_pos}" == "${now_nilopt_pos}" ]; then
          nilopt_var=$val
          if [ "$nilopt_var" != "" ]; then
            printf -v nilopt_val %b "${flag}"
          fi
        fi
        nilopt_pos=$((nilopt_pos+1))
        continue
      fi

      if [ "$flag" == "$opt" ]; then 
        if [ "$has_arg" == "1" ]; then
          local tempval=`eval echo \\${$val}`
          if [ "$tempval" == "" ]; then
            tempval="${allopt[i+1]}"
          else
            tempval="$tempval":"${allopt[i+1]}"
          fi
          printf -v $val %b "${tempval}"
          i=$((i+has_arg))
        else
          printf -v $val %b "true"
        fi
        break
      fi
    done
    if [ "$j" == "$optvalslen" ]; then
      if [ "$nilopt_val" == "" ]; then
        print_warn "Option $flag is not supported."
      else
        printf -v $nilopt_var %b "${nilopt_val}"
        now_nilopt_pos=$((now_nilopt_pos+1))
      fi
    fi
  done
}

option_usage_print() {
  local optvalslen=${#OPT_VALS[@]}
  local description=""
  local exefile=""
  for ((j=0;j<$optvalslen;j+=4)); do
    local opt="${OPT_VALS[j]}"
    local val="${OPT_VALS[j+1]}"
    local has_arg="${OPT_VALS[j+2]}"
    local meaning="${OPT_VALS[j+3]}"
    if [ "$opt" == "DESCRIPTION" ]; then
      description=$meaning
      exefile=$val
      break
    fi
  done

  echo -e "\

   ${bold}DESCRIPTION${txtrst}
     ${description}
   
   ${bold}SYNOPSIS${txtrst}
     ${txtylw}${bold}${exefile}${txtrst}
   "

  for ((j=0;j<$optvalslen;j+=4)); do
    local opt="${OPT_VALS[j]}"
    local val="${OPT_VALS[j+1]}"
    local has_arg="${OPT_VALS[j+2]}"
    local meaning="${OPT_VALS[j+3]}"
    if [ "$has_arg" == "1" ]; then
      has_arg="ARG"
    else
      has_arg=""
    fi
    #Null opt
    if [ "$opt" == "_" ]; then
      opt=""
    fi

    if [ "$opt" != "DESCRIPTION" ]; then
      echo -e "\
          ${txtylw}${bold}${opt}${txtrst} ${txtylw}${has_arg}${txtrst} 
              $meaning
              "
    fi  
  done
}

######################################################

find_file_in_dirset () {
  #dirset delimiter is ':'
  local FILE=$1
  local DIRSET=$2
  local NDIRS=`echo $DIRSET | awk -F : '{print NF}'`
  for (( i=1;i <=$NDIRS;i++ )); do
    #get n-th dir in dirlist
    local dir=`echo $DIRSET:$i | awk -F : '{print $$NF}'`
    #find FILE in this dir
    local result=`find $dir -print 2>/dev/null | grep "$FILE$" 2>/dev/null | head -1`
    if [ "$result" != "" ]; then
      echo $result
      return
    fi
  done
}

