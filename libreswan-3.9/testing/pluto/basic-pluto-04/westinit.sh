/testing/guestbin/swan-prep
# confirm that the network is alive
ping -n -c 4 -I 192.0.1.254 192.0.2.254
# make sure that clear text does not get through
iptables -A INPUT -i eth1 -s 192.0.2.0/24 -j LOGDROP
# confirm with a ping
ping -n -c 4 -I 192.0.1.254 192.0.2.254
ipsec setup start
/testing/pluto/bin/wait-until-pluto-started
ipsec auto --add westnet-eastnet-aes128
ipsec auto --status
echo "initdone"