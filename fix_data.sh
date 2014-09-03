#!/sbin/sh
# Fix data partition after bootloader unlock
# Format it if a filesystem is not present

data_partition=`busybox grep "/data" /etc/recovery.fstab | busybox cut -d " " -f 1`
sony_property=`busybox grep -q "^\s*ro\.semc\." /system/build.prop; busybox echo $?`

echo -n > /tmp/fix_data
if [ "`blkid $data_partition`" == "" ] && [ "$sony_property" == "0" ]; then
	echo -n "yes" > /tmp/fix_data
	exit 1
else
	echo -n "no" > /tmp/fix_data
	exit 0
fi
