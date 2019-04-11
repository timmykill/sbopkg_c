#!/bin/sh

SB_DIR='/opt/slackbuilds/'

if test "$1" != 'dl_source' \
	-a "$1" != 'info' \
	-a "$1" != 'build' \
	-a "$1" != 'all' \
	-a "$1" != 'x'
then
	echo "Usage: $0 [azione] [nome pacchetto]"
	echo "Azioni:"
	echo "info: mostra info pacchetto"
	echo "dl_source: scarica i sorgenti dal link fornito nel file info"
	exit
fi

all() {
	x $1
	info $1
	echo "Tutto ok?"
	read yn
	if test $yn == "y" -o $yn == "Y"; then
		dl_source $1
		#build $1
	else
		exit
	fi
}

build() {
	$SB_DIR/$1/$1.SlackBuild
}

x() {
	tar -zxf $1.tar.gz
}

dl_source() {
	wget $(cat $SB_DIR/$1/$1.info | grep DOWNLOAD | head -1 | cut -d'"' -f2) -P $1
}

info() {
	cat $SB_DIR/$1/$1.info
}

$@

