#!/bin/sh

function get_distro()
{
    which pacman  >/dev/null && { echo arch; return; }
    which yum &   >/dev/null && { echo redhat; return; }
    which apt-get >/dev/null && { echo debian; return; }
}

distro=`get_distro`

case $distro in
    arch)
        sudo pacman -S \
            make \
            cmake \
			gcc \
			sdl2 \
			sdl2_image \
			sdl2_ttf
        ;;
    redhat)
        sudo yum install -y \
            make \
            cmake \
			gcc \
			SDL2-devel \
			SDL2_image-devel \
			SDL2_ttf-devel
        ;;
	debian)
		sudo apt-get install -y \
			make \
			cmake \
			gcc \
			libsdl2-dev \
			libsdl2-image-dev \
			libsdl2-ttf-dev
		;;
    * )
        echo "Unsupported Distro"
        ;;
esac